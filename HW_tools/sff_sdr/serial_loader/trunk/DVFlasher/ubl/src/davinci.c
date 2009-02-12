/* davinci.c - common platform initialization file */

#include "davinci.h"
#include "ubl.h"
#include "util.h"
#include "uart.h"

/* Uncomment and modify these definitions to turn ON the STATUS LED on your board. */
#define STATUS_LED                GPIO(71)
#define STATUS_LED_PINMUX_OFFSET  1
#define STATUS_LED_PINMUX_BITMASK (1 << 17)

extern BootMode gBootMode;

void pinmuxControl(uint32_t regOffset, uint32_t mask, uint32_t value)
{
	SYSTEM->PINMUX[regOffset] &= ~mask;
	SYSTEM->PINMUX[regOffset] |= (mask & value);
}

void LPSCTransition(uint8_t module, uint8_t domain, uint8_t state)
{
	// Wait for any outstanding transition to complete
	while ( (PSC->PTSTAT) & (0x00000001 << domain) );

	// If we are already in that state, just return
	if (((PSC->MDSTAT[module]) & 0x1F) == state) return;

	// Perform transition
	PSC->MDCTL[module] = ((PSC->MDCTL[module]) & (0xFFFFFFE0)) | (state);
	PSC->PTCMD |= (0x00000001 << domain);

	// Wait for transition to complete
	while ( (PSC->PTSTAT) & (0x00000001 << domain) );

	// Wait and verify the state
	while (((PSC->MDSTAT[module]) & 0x1F) != state);
}

static void IVTInit(void)
{
	volatile uint32_t *ivect;
	extern uint32_t __IVT;
    
	if (gBootMode == NON_SECURE_NOR) {
		ivect = &(__IVT);
		*ivect++ = 0xEAFFFFFE;  /* Reset @ 0x00*/
	} else {
		ivect = &(__IVT) + 4;
	}

	*ivect++ = 0xEAFFFFFE;  /* Undefined Address @ 0x04 */
	*ivect++ = 0xEAFFFFFE;  /* Software Interrupt @0x08 */
	*ivect++ = 0xEAFFFFFE;  /* Pre-Fetch Abort @ 0x0C */
	*ivect++ = 0xEAFFFFFE;  /* Data Abort @ 0x10 */
	*ivect++ = 0xEAFFFFFE;  /* Reserved @ 0x14 */
	*ivect++ = 0xEAFFFFFE;  /* IRQ @ 0x18 */
	*ivect   = 0xEAFFFFFE;	/* FIQ @ 0x1C */
}

#ifdef STATUS_LED
static void status_led_configure(void)
{
	volatile struct gpio_controller *g;
	uint32_t mask;
    
	g = gpio_to_controller(STATUS_LED);
	mask = gpio_mask(STATUS_LED);
	gpio_direction_out(STATUS_LED, 0); /* Initial value is 0 */
}

static void status_led_set(int state)
{
	volatile struct gpio_controller *g;
	uint32_t mask;
    
	g = gpio_to_controller(STATUS_LED);
	mask = gpio_mask(STATUS_LED);

	if (state)
		g->set_data = mask;
	else
		g->clr_data = mask;
}
#endif /* STATUS_LED */

int davinci_platform_init(void)
{
	uint32_t status = E_PASS;

	/* Disable ARM interrupts */
	AINTC->INTCTL = 0x4;
	AINTC->EABASE = 0x0;
	AINTC->EINT0  = 0x0;
	AINTC->EINT1  = 0x0;

	AINTC->FIQ0 = 0xFFFFFFFF;
	AINTC->FIQ1 = 0xFFFFFFFF;
	AINTC->IRQ0 = 0xFFFFFFFF;
	AINTC->IRQ1 = 0xFFFFFFFF;

#ifdef DM6446
	pinmuxControl(1,0x00000001,0x00000001); /* Enable UART0 */
#endif
#ifdef DM355
	pinmuxControl(0,0xFFFFFFFF,0x00007F55);  // All Video Inputs
	pinmuxControl(1,0xFFFFFFFF,0x00145555);  // All Video Outputs
	pinmuxControl(2,0xFFFFFFFF,0x00000004);  // EMIFA
	pinmuxControl(3,0xFFFFFFFF,0x1BFF55FF);  // SPI0, SPI1, UART1, I2C, SD0, SD1, McBSP0, CLKOUTs
	pinmuxControl(4,0xFFFFFFFF,0x00000000);  // MMC/SD0 instead of MS, SPI0
#endif

	// System PLL setup
	if (status == E_PASS) status |= PLL1Init();

	// DDR PLL setup
	if (status == E_PASS) status |= PLL2Init();

	// DDR2 module setup
	if (status == E_PASS) status |= DDR2Init();

	// UART0 Setup
	if (status == E_PASS) status |= UART0Init();

	// TIMER0 Setup
	if (status == E_PASS) status |= TIMER0Init();

#ifdef STATUS_LED
	pinmuxControl(STATUS_LED_PINMUX_OFFSET, STATUS_LED_PINMUX_BITMASK, STATUS_LED_PINMUX_BITMASK);
	status_led_configure();
	status_led_set(1);
#endif /* STATUS_LED */

	/* IRQ Vector Table Setup */
	IVTInit();

	return status;
}

uint32_t TIMER0Init(void)
{
	TIMER0->TGCR  = 0x00000000; /* Reset timer */
	TIMER0->TCR   = 0x00000000; /* Disable timer */
	TIMER0->TIM12 = 0x00000000; /* Reset timer count to zero */

#ifdef DM6446
	// Set timer period (5 second timeout = (27000000 * 5) cycles = 0x080BEFC0) 
	TIMER0->PRD12 = 0x080BEFC0;
#endif
#ifdef DM355
	// Set timer period (5 second timeout = (24000000 * 5) cycles = 0x07270E00)
	TIMER0->PRD12 = 0x07270E00;
#endif

	return E_PASS;
}

void TIMER0Start(void)
{
       	AINTC->IRQ1  |= 0x00000001; /* Clear interrupt */
	TIMER0->TGCR  = 0x00000000; /* Reset timer */
	TIMER0->TIM12 = 0x00000000; /* Reset timer count to zero */
	TIMER0->TCR   = 0x00000040; /* Setup for one-shot mode */
	TIMER0->TGCR  = 0x00000005; /* Start TIMER12 in 32-bits mode. */
}

uint32_t TIMER0Status(void)
{
	return AINTC->IRQ1 & 0x1;
}

int UART0Init(void)
{		
	UART0->PWREMU_MGNT = 0; /* Reset UART TX & RX components */
	waitloop(100);

	/* Set DLAB bit - allows setting of clock divisors */
	UART0->LCR |= 0x80;
	
#ifdef DM6446
	/* divider = 27000000 / (16 * 115200) = 14.64 => 15 = 0x0F (2% error is OK) */
	UART0->DLL = 0x0F;
#endif
#ifdef DM355
	/* divider = 24000000 / (16 * 115200) = 13.02 => 13 = 0x0D */
	UART0->DLL = 0x0D;
#endif
	UART0->DLH = 0x00; 

	UART0->FCR = 0x0007; /* Clear UART TX & RX FIFOs */
	UART0->MCR = 0x0000; /* RTS & CTS disabled,
			      * Loopback mode disabled,
			      * Autoflow disabled
			      */

	UART0->LCR = 0x0003; /* Clear DLAB bit
			      * 8-bit words,
			      * 1 STOP bit generated,
			      * No Parity, No Stick paritiy,
			      * No Break control
			      */

	/* Enable receiver, transmitter, set to run.  */
	UART0->PWREMU_MGNT |= 0x6001;

	return E_PASS;
}
