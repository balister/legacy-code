/* davinci.c - common platform initialization file */

#include "davinci.h"
#include "ubl.h"
#include "util.h"
#include "uart.h"

/*
 * TODO: define the GPIO used for the STAUS LED based on the board type.
 * Uncomment and modify these definitions to turn ON the STATUS LED on your board.
 */
/* #define STATUS_LED                GPIO(71) */
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

static uint32_t TIMER0Init(void)
{
	TIMER0->TGCR  = 0x00000000; /* Reset timer */
	TIMER0->TCR   = 0x00000000; /* Disable timer */
	TIMER0->TIM12 = 0x00000000; /* Reset timer count to zero */

	/* Set timer period (5 seconds timeout) */
	TIMER0->PRD12 = SYSTEM_CLK_HZ * 5;

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

static int UART0Init(void)
{		
	UART0->PWREMU_MGNT = 0; /* Reset UART TX & RX components */
	waitloop(100);

	/* Set DLAB bit - allows setting of clock divisors */
	UART0->LCR |= 0x80;

	/*
	 * Compute divisor value. Normally, we should simply return:
	 *   SYSTEM_CLK_HZ / (16 * baudrate)
	 * but we need to round that value by adding 0.5.
	 * Rounding is especially important at high baud rates.
	 */
	UART0->DLL = (SYSTEM_CLK_HZ + (UART_BAUDRATE * (UART_BCLK_RATIO / 2))) /
		(UART_BCLK_RATIO * UART_BAUDRATE);
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

static int pll_init(PLLRegs *pll, int pll_mult, int plldiv_ratio[5])
{
	int k;
	volatile uint32_t *plldiv_reg[5];
	int pll_is_powered_up = (pll->PLLCTL & DEVICE_PLLCTL_PLLPWRDN_MASK) >> 1;

	plldiv_reg[0] = &pll->PLLDIV1;
	plldiv_reg[1] = &pll->PLLDIV2;
	plldiv_reg[2] = &pll->PLLDIV3;
	plldiv_reg[3] = &pll->PLLDIV4;
	plldiv_reg[4] = &pll->PLLDIV5;
	
	/* Set PLL clock input to internal osc. */
	pll->PLLCTL &= ~(DEVICE_PLLCTL_CLKMODE_MASK);

	/* Set PLL to bypass, then wait for PLL to stabilize */
	pll->PLLCTL &= ~(DEVICE_PLLCTL_PLLENSRC_MASK |
			 DEVICE_PLLCTL_PLLEN_MASK);
	waitloop(150);

	/* Reset PLL: Warning, bit state is inverted for DM6446 vs DM355. */
#if defined(DM6446)
	pll->PLLCTL &= ~DEVICE_PLLCTL_PLLRST_MASK;
#elif defined(DM355)
	pll->PLLCTL |= DEVICE_PLLCTL_PLLRST_MASK;
#endif

	if (pll_is_powered_up) {
		/* Disable PLL */
		pll->PLLCTL |= DEVICE_PLLCTL_PLLDIS_MASK;

		/* Powerup PLL */
		pll->PLLCTL &= ~(DEVICE_PLLCTL_PLLPWRDN_MASK);
	}

	/* Enable PLL */
	pll->PLLCTL &= ~(DEVICE_PLLCTL_PLLDIS_MASK);

	/* Wait for PLL to stabilize */
	waitloop(150);

	/* Load PLL multiplier. */
	pll->PLLM = (pll_mult - 1) & 0xff;

	/* Set and enable dividers as needed. */
	for (k = 0; k < 5; k++) {
		if (plldiv_ratio[k] > 0) {

#ifdef PLL_DEBUG
			UARTSendString("Enabling PLLDIV ");
			UARTSendInt(k + 1);
			UARTSendCRLF();
			UARTSendString("  PLLDIV (before) = ");
			UARTSendInt(*(plldiv_reg[k]));
			UARTSendCRLF();
#endif
			*(plldiv_reg[k]) |= DEVICE_PLLDIV_EN_MASK | (plldiv_ratio[k] - 1);
#ifdef PLL_DEBUG
			UARTSendString("  PLLDIV (after)  = ");
			UARTSendInt(*(plldiv_reg[k]));
			UARTSendCRLF();
#endif
		}
	}

#if defined(DM355)
	/* Set the processor AIM wait state to 1 and PLL1 post-divider equal to 1 */
	SYSTEM->MISC &= ~(DEVICE_MISC_PLL1POSTDIV_MASK | DEVICE_MISC_AIMWAITST_MASK);
#endif

	/* Set the GOSET bit in PLLCMD to 1 to initiate a new divider transition. */
	pll->PLLCMD |= DEVICE_PLLCMD_GOSET_MASK;

	/* Wait for the GOSTAT bit in PLLSTAT to clear to 0
	 * (completion of phase alignment). */
	while ((pll->PLLSTAT & DEVICE_PLLSTAT_GOSTAT_MASK))
		;

	/* Wait for PLL to reset ( ~5 usec ) */
	waitloop(5000);

	/* Release PLL from reset */

	/* Reset PLL: Warning, bit state is inverted for DM6446 vs DM355. */
#if defined(DM6446)
	pll->PLLCTL |= DEVICE_PLLCTL_PLLRST_MASK;
#elif defined(DM355)
	pll->PLLCTL &= ~DEVICE_PLLCTL_PLLRST_MASK;
#endif

	/* Wait for PLL to re-lock:
	 * DM644z: 2000P
	 * DM35x:  8000P
	 */
	waitloop(8000);

	/* Switch out of BYPASS mode */
	pll->PLLCTL |= DEVICE_PLLCTL_PLLEN_MASK;

	return E_PASS;
}

static int pll1_init(void)
{
	int plldiv_ratio[5];

#ifdef PLL_DEBUG
	UARTSendString("Configuring PLL1\n");
#endif

#if defined(DM6446)
	plldiv_ratio[0] =  1; /* PLLDIV1 fixed */
	plldiv_ratio[1] =  2; /* PLLDIV2 fixed */
	plldiv_ratio[2] =  3; /* PLLDIV3 fixed */
	plldiv_ratio[3] = -1; /* PLLDIV4 not used */
	plldiv_ratio[4] =  6; /* PLLDIV5 fixed */
#elif defined(DM355)
	plldiv_ratio[0] =  2; /* PLLDIV1 fixed */
	plldiv_ratio[1] =  4; /* PLLDIV2 fixed */

	/* Calculate PLL divider ratio for divider 3 (feeds VPBE) */
	plldiv_ratio[2] = 0;
	while ((plldiv_ratio[2] * VPBE_CLK_HZ) < (SYSTEM_CLK_HZ * (PLL1_Mult >> 3)))
		plldiv_ratio[2]++;

	/* Check to make sure we can supply accurate VPBE clock */
	if ((plldiv_ratio[2] * VPBE_CLK_HZ) != (SYSTEM_CLK_HZ * (PLL1_Mult >> 3)))
		return E_FAIL;

	/* See the device datasheet for more info (must be 2 or 4) */
	plldiv_ratio[3] =  4;
	plldiv_ratio[4] = -1; /* PLLDIV5 not used */
#endif

	return pll_init(PLL1, PLL1_Mult, plldiv_ratio);
}

static int pll2_init(void)
{	
	int plldiv_ratio[5];

#ifdef PLL_DEBUG
	UARTSendString("Configuring PLL2\n");
#endif

	plldiv_ratio[0] = PLL2_Div1;
	plldiv_ratio[1] = PLL2_Div2;
	plldiv_ratio[2] = -1; /* PLLDIV3 not used */
	plldiv_ratio[3] = -1; /* PLLDIV4 not used */
	plldiv_ratio[4] = -1; /* PLLDIV5 not used */

	return pll_init(PLL2, PLL2_Mult, plldiv_ratio);
}

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

#if defined(DM6446)
	pinmuxControl(1,0x00000001,0x00000001); /* Enable UART0 */
#elif defined(DM355)
	pinmuxControl(0,0xFFFFFFFF,0x00007F55);  // All Video Inputs
	pinmuxControl(1,0xFFFFFFFF,0x00145555);  // All Video Outputs
	pinmuxControl(2,0xFFFFFFFF,0x00000004);  // EMIFA
	pinmuxControl(3,0xFFFFFFFF,0x1BFF55FF);  // SPI0, SPI1, UART1, I2C, SD0, SD1, McBSP0, CLKOUTs
	pinmuxControl(4,0xFFFFFFFF,0x00000000);  // MMC/SD0 instead of MS, SPI0
#endif

	// System PLL setup
	if (status == E_PASS) status |= pll1_init();

	// DDR PLL setup
	if (status == E_PASS) status |= pll2_init();

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
