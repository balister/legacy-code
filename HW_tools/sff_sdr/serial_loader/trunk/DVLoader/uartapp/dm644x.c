/* -------------------------------------------------------------------------- *
 * dm644x.c - Implementation of platform initialization commands   				*
 * -------------------------------------------------------------------------- */

#include "dm644x.h"
#include "uart.h"
#include "uartapp.h"

extern volatile unsigned int DDRMem[0];

void boot()
{
	// Stack setup (__topstack symbol defined in linker script)
	extern int __topstack;
	register int* stackpointer asm ("sp");	
	stackpointer = &(__topstack);
	
	// Call to main code
	uartapp_main();
	
	// Infinite loop to catch fall through from the main 
	while(1);
}
	
void PlatformInit()
{
	// Mask all interrupts
	AINTC->INTCTL = 0x0;
	AINTC->EINT0 = 0x0;
	AINTC->EINT1 = 0x0;		
	
	// Put the C64x+ Core into reset (if it's on)
	PSC->MDCTL[39] &= (~0x00000100);
	PSC->PTCMD |= 0x00000002;
	while ((PSC->PTSTAT) & (0x00000002));
	while ((PSC->MDSTAT[39]) & (0x00000100));
	
	/******************* UART Setup **************************/
	UARTInit();
	
	/******************* System PLL Setup ********************/
	PLL1Init();
	
	/******************* DDR PLL Setup ***********************/	
	PLL2Init();

	/******************* DDR2 Timing Setup *******************/
	DDR2Init();
			
	/******************* AEMIF Setup *************************/
	AEMIFInit();
}

void PLL2Init()
{	
	// Set PLL2 clock input to external osc. 
	PLL2->PLLCTL &= (~0x00000100);
	
	// Clear PLLENSRC bit and clear PLLEN bit for Bypass mode 
	PLL2->PLLCTL &= (~0x00000021);

	// Wait for PLLEN mux to switch 
	waitloop(32*11);
	
	PLL2->PLLCTL &= (~0x00000008);          // Put PLL into reset
	PLL2->PLLCTL |= (0x00000010);           // Disable the PLL
	PLL2->PLLCTL &= (~0x00000002);          // Power-up the PLL
	PLL2->PLLCTL &= (~0x00000010);          // Enable the PLL
	
	// Set PLL multipliers and divisors 
	PLL2->PLLM = 23;        // 27  Mhz * (23+1) = 648 MHz 
	PLL2->PLLDIV1 = 8;      // 648 MHz / (8+1)  = 72  MHz (for VPSS)
	PLL2->PLLDIV2 = 1;      // 648 MHz / (1+1 ) = 324 MHz (the PHY DDR rate)
		
	PLL2->PLLDIV2 |= (0x00008000);          // Enable DDR divider	
	PLL2->PLLDIV1 |= (0x00008000);          // Enable VPBE divider	
	PLL2->PLLCMD |= 0x00000001;             // Tell PLL to do phase alignment
	while ((PLL2->PLLSTAT) & 0x1);          // Wait until done
	waitloop(256*11);

	PLL2->PLLCTL |= (0x00000008);           // Take PLL out of reset	
	waitloop(2000*11);                       // Wait for locking
	
	PLL2->PLLCTL |= (0x00000001);           // Switch out of bypass mode
}

void DDR2Init()
{
	int tempVTP;	
	
	// Set the DDR2 to enable
	while (PSC->PTSTAT & 0x00000001);
	PSC->MDCTL[13] = ((PSC->MDCTL[13]) & (0xFFFFFFE0)) | (0x00000003);
	PSC->PTCMD |= 0x00000001;
	while ((PSC->PTSTAT) & 0x00000001);
	while (((PSC->MDSTAT[13]) & 0x1F) != 0x00000003);	
	
	// For Micron MT47H32M16BN-3 @ 324 MHz
	// Setup the read latency (CAS Latency + 3 = 6 (but write 6-1=5))
	DDR->DDRPHYCR = 0x14001905;
	// Set TIMUNLOCK bit, CAS LAtency 3, 4 banks, 1024-word page size 
	DDR->SDBCR = 0x00138622;
	// Program timing registers 
	DDR->SDTIMR  = 0x45246452;
	DDR->SDTIMR2 = 0x004B8E82;
	// Clear the TIMUNLOCK bit 
	DDR->SDBCR &= (~0x00008000);
	// Set the refresh rate
	DDR->SDRCR = 0x000009DF;
	
	// Dummy write/read to apply timing settings
	DDRMem[0] = DDR_TEST_PATTERN;
	if (DDRMem[0] == DDR_TEST_PATTERN)
          UARTSendInt(DDRMem[0]);
	
	// Set the DDR2 to syncreset
	PSC->MDCTL[13] = ((PSC->MDCTL[13]) & (0xFFFFFFE0)) | (0x00000001);
	PSC->PTCMD |= 0x00000001;
	while ((PSC->PTSTAT) & 0x00000001);
	while (((PSC->MDSTAT[13]) & 0x1F) != 0x00000001);	

	// Set the DDR2 to enable
	PSC->MDCTL[13] = ((PSC->MDCTL[13]) & (0xFFFFFFE0)) | (0x00000003);
	PSC->PTCMD |= 0x00000001;
	while ((PSC->PTSTAT) & 0x00000001);
	while (((PSC->MDSTAT[13]) & 0x1F) != 0x00000003);
	
			 
	/***************** DDR2 VTP Calibration ****************/
	DDR->VTPIOCR = 0x201F;        // Clear calibration start bit
	DDR->VTPIOCR = 0xA01F;        // Set calibration start bit 
	
	waitloop(11*33);              // Wait for calibration to complete 
		 
	SYSTEM->DDRVTPER = 0x1;       // DDRVTPR Enable register
	
	tempVTP = 0x3FF & DDRVTPR;    // Read calibration data
	
	// Write calibration data to VTP Control register 
	DDR->VTPIOCR = ((DDR->VTPIOCR) & 0xFFFFFC00) | tempVTP; 
	
	// Clear calibration enable bit
	DDR->VTPIOCR = (DDR->VTPIOCR) & (~0x00002000);
	
	// DDRVTPR Enable register - disable DDRVTPR access 
	SYSTEM->DDRVTPER = 0x0;
	
}

void PLL1Init()
{
	// Set PLL2 clock input to internal osc. 
	PLL1->PLLCTL &= (~0x00000100);	
	
	// Clear PLLENSRC bit and clear PLLEN bit for Bypass mode 
	PLL1->PLLCTL &= (~0x00000021);

	// Wait for PLLEN mux to switch 
	waitloop(32);
	
	PLL1->PLLCTL &= (~0x00000008);     // Put PLL into reset
	PLL1->PLLCTL |= (0x00000010);      // Disable the PLL
	PLL1->PLLCTL &= (~0x00000002);     // Power-up the PLL
	PLL1->PLLCTL &= (~0x00000010);     // Enable the PLL
	
	// Set PLL multipliers and divisors 
	PLL1->PLLM = 21;                   // 27Mhz * (21+1) = 594 MHz 
			
	PLL1->PLLCMD |= 0x00000001;		// Tell PLL to do phase alignment
	while ((PLL1->PLLSTAT) & 0x1);	// Wait until done
	
	waitloop(256);
	PLL1->PLLCTL |= (0x00000008);		//	Take PLL out of reset	
	waitloop(2000);				// Wait for locking
	
	PLL1->PLLCTL |= (0x00000001);		// Switch out of bypass mode
}
 
void AEMIFInit()
{     
   AEMIF->AWCCR     = 0x00000000;
   AEMIF->AB1CR     = 0x3FFFFFFD;
   AEMIF->AB2CR     = 0x3FFFFFFD;
   AEMIF->AB3CR     = 0x3FFFFFFD;
   AEMIF->AB4CR     = 0x3FFFFFFD;
   AEMIF->NANDFCR   = 0x00000000;
}
 
void UARTInit() {

     // Wait until the RBL is done using the UART. 
     while((UART0->LSR & 0x40) == 0 );

	// The DM644x pin muxing registers must be set for UART0 use. 
	SYSTEM->PINMUX[1] |=  1;
	
	// Set DLAB bit - allows setting of clock divisors 
	UART0->LCR |= 0x80;
	
	//divider = 27000000/(16*115200) = 14.64 => 15 = 0x0F (2% error is OK)
	UART0->DLL = 0x0F;
	UART0->DLH = 0x00; 

    // Enable, clear and reset FIFOs	
	UART0->FCR = 0x07;
	
	// Disable autoflow control 
	UART0->MCR = 0x00;
	
	// Enable receiver, transmitter, st to run. 
	UART0->PWREMU_MGNT |= 0x6001;

	// Set word length to 8 bits, clear DLAB bit 
	UART0->LCR = 0x03;

	// Disable the timer 
	TIMER0->TCR = 0x00000000;
	// Set to 64-bit GP Timer mode, enable TIMER12 & TIMER34
	TIMER0->TGCR = 0x00000003;

	// Reset timers to zero 
	TIMER0->TIM34 = 0x00000000;
	TIMER0->TIM12 = 0x00000000;
	
	// Set timer period (5 second timeout = (27000000 * 5) cycles = 0x080BEFC0) 
	TIMER0->PRD34 = 0x00000000;
	TIMER0->PRD12 = 0x080BEFC0;
}

void waitloop(unsigned int loopcnt)
{
	for (loopcnt = 0; loopcnt<1000; loopcnt++)
	{
		asm("   NOP");
	}
}

