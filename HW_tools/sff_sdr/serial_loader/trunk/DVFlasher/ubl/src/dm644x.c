/* --------------------------------------------------------------------------
    FILE        : dm644x.c 				                             	 	        
    PURPOSE     : Platform initialization file
    PROJECT     : DaVinci User Boot-Loader and Flasher
    AUTHOR      : Daniel Allred
    DATE	    : Jan-22-2007  
 
    HISTORY
 	     v1.00 completion (with support for DM6441 and DM6441_LV)							 						      
 	          Daniel Allred - Jan-22-2007
 	     v1.11 DJA - Mar-07-2007
 	          Made all DDR and PLL global values as const variables.
 	          Added code to set 1394 MDSTAT, as per workaround from U-boot
 	          and code to init all power domains to known state.
 	     
 ----------------------------------------------------------------------------- */

#include "dm644x.h"
#include "ubl.h"
#include "util.h"
#include "uart.h"

extern volatile uint32_t DDRMem[0];
extern BootMode gBootMode;
extern uint32_t gEntryPoint;

// ---------------------------------------------------------------------------
// Global Memory Timing and PLL Settings
// ---------------------------------------------------------------------------
    // For Micron MT47H32M16BN-3 @ 324 MHz   
    const uint8_t DDR_NM = 0;
    const uint8_t DDR_CL = 3;
    const uint8_t DDR_IBANK = 2;
    const uint8_t DDR_PAGESIZE = 2;
    const uint8_t DDR_T_RFC = 34;
    const uint8_t DDR_T_RP = 4;
    const uint8_t DDR_T_RCD = 4;
    const uint8_t DDR_T_WR = 4;
    const uint8_t DDR_T_RAS = 12;
    const uint8_t DDR_T_RC = 17;
    const uint8_t DDR_T_RRD = 2;
    const uint8_t DDR_T_WTR = 2;
    const uint8_t DDR_T_XSNR = 37;
    const uint8_t DDR_T_XSRD = 199;
    const uint8_t DDR_T_RTP = 2;
    const uint8_t DDR_T_CKE = 2;
    const uint16_t DDR_RR = 2527;
    const uint8_t DDR_Board_Delay = 3;
    const uint8_t DDR_READ_Latency = 5;
    
    const uint32_t PLL2_Mult = 24;
    const uint32_t PLL2_Div1 = 12;
    const uint32_t PLL2_Div2 = 2;

// Set CPU clocks
    const uint32_t PLL1_Mult = 22;  // DSP=594 MHz ARM=297 MHz
    
// ---------------------------------------------------------
// End of global PLL and Memory settings
// ---------------------------------------------------------
    

void LPSCTransition(uint8_t module, uint8_t state)
{
    while (PSC->PTSTAT & 0x00000001);
	PSC->MDCTL[module] = ((PSC->MDCTL[module]) & (0xFFFFFFE0)) | (state);
	PSC->PTCMD |= 0x00000001;
	while ((PSC->PTSTAT) & 0x00000001);
	while (((PSC->MDSTAT[module]) & 0x1F) != state);	
}
	
void DM644xInit()
{
	// Mask all interrupts
	AINTC->INTCTL = 0x0;
	AINTC->EINT0 = 0x0;
	AINTC->EINT1 = 0x0;		
	
	/******************* UART Setup **************************/
	UARTInit();
	
	/******************* System PLL Setup ********************/
	PLL1Init();
	
	/******************* DDR PLL Setup ***********************/	
	PLL2Init();

	/******************* DDR2 Timing Setup *******************/
	DDR2Init();
			
	/******************* AEMIF Setup *************************/
	// Handled in NOR or NAND init
	//AEMIFInit();
    
    /******************* IRQ Vector Table Setup **************/
    IVTInit();
}

void PSCInit()
{
    uint32_t i;

    // Put the C64x+ Core into reset (if it's on)
	PSC->MDCTL[LPSC_DSP] &= (~0x00000100);
	PSC->PTCMD |= 0x00000002;
	while ((PSC->PTSTAT) & (0x00000002));
	while ((PSC->MDSTAT[LPSC_DSP]) & (0x00000100));
	
    for( i = LPSC_VPSS_MAST ; i < LPSC_1394 ; i++ )
        PSC->MDCTL[i] |= 0x03; // Enable

	// Do this for enabling a WDT initiated reset this is a workaround
	// for a chip bug.  Not required under normal situations 
	// Copied from U-boot boards/DaVinci/platform.S and convereted to C
	//      LDR R6, P1394
	//      MOV R10, #0x0	
	//      STR R10, [R6]        
    PSC->MDCTL[LPSC_1394] = 0x0;
    
    for( i = LPSC_USB ; i < LPSC_DSP ; i++ )
        PSC->MDCTL[i] |= 0x03; // Enable
    
    PSC->MDCTL[LPSC_IMCOP] = 0x03;

    // Set EMURSTIE to 1 on the following
    PSC->MDCTL[LPSC_VPSS_SLV]   |= 0x0203;
    PSC->MDCTL[LPSC_EMAC0]      |= 0x0203;
    PSC->MDCTL[LPSC_EMAC1]      |= 0x0203;
    PSC->MDCTL[LPSC_MDIO]       |= 0x0203;
    PSC->MDCTL[LPSC_USB]        |= 0x0203;
    PSC->MDCTL[LPSC_ATA]        |= 0x0203;
    PSC->MDCTL[LPSC_VLYNQ]      |= 0x0203;
    PSC->MDCTL[LPSC_HPI]        |= 0x0203;
    PSC->MDCTL[LPSC_DDR2]       |= 0x0203;
    PSC->MDCTL[LPSC_AEMIF]      |= 0x0203;
    PSC->MDCTL[LPSC_MMCSD]      |= 0x0203;
    PSC->MDCTL[LPSC_MEMSTK]     |= 0x0203;
    PSC->MDCTL[LPSC_ASP]        |= 0x0203;
    PSC->MDCTL[LPSC_GPIO]       |= 0x0203;
    PSC->MDCTL[LPSC_IMCOP]      |= 0x0203;

    // Do Always-On Power Domain Transitions
    PSC->PTCMD |= 0x00000001;
    while ((PSC->PTSTAT) & 0x00000001);
    
    // DO DSP Power Domain Transitions
    PSC->PTCMD |= 0x00000002;
	while ((PSC->PTSTAT) & (0x00000002));
	
	
	// Clear EMURSTIE to 0 on the following
	PSC->MDCTL[LPSC_VPSS_SLV]   &= 0x0003;
    PSC->MDCTL[LPSC_EMAC0]      &= 0x0003;
    PSC->MDCTL[LPSC_EMAC1]      &= 0x0003;
    PSC->MDCTL[LPSC_MDIO]       &= 0x0003;
    PSC->MDCTL[LPSC_USB]        &= 0x0003;
    PSC->MDCTL[LPSC_ATA]        &= 0x0003;
    PSC->MDCTL[LPSC_VLYNQ]      &= 0x0003;
    PSC->MDCTL[LPSC_HPI]        &= 0x0003;
    PSC->MDCTL[LPSC_DDR2]       &= 0x0003;
    PSC->MDCTL[LPSC_AEMIF]      &= 0x0003;
    PSC->MDCTL[LPSC_MMCSD]      &= 0x0003;
    PSC->MDCTL[LPSC_MEMSTK]     &= 0x0003;
    PSC->MDCTL[LPSC_ASP]        &= 0x0003;
    PSC->MDCTL[LPSC_GPIO]       &= 0x0003;
    PSC->MDCTL[LPSC_IMCOP]      &= 0x0003;    
}

void PLL2Init()
{	
        
	// Set PLL2 clock input to external osc. 
	PLL2->PLLCTL &= (~0x00000100);
	
	// Clear PLLENSRC bit and clear PLLEN bit for Bypass mode 
	PLL2->PLLCTL &= (~0x00000021);

	// Wait for PLLEN mux to switch 
	waitloop(32*(PLL1_Mult/2));
	
	PLL2->PLLCTL &= (~0x00000008);          // Put PLL into reset
	PLL2->PLLCTL |= (0x00000010);           // Disable the PLL
	PLL2->PLLCTL &= (~0x00000002);          // Power-up the PLL
	PLL2->PLLCTL &= (~0x00000010);          // Enable the PLL
	
	// Set PLL multipliers and divisors 
	PLL2->PLLM      = PLL2_Mult-1;     // 27  Mhz * (23+1) = 648 MHz 
	PLL2->PLLDIV1   = PLL2_Div1-1;     // 648 MHz / (11+1) = 54  MHz
	PLL2->PLLDIV2   = PLL2_Div2-1;     // 648 MHz / (1+1 ) = 324 MHz (the PHY DDR rate)
		
	PLL2->PLLDIV2 |= (0x00008000);          // Enable DDR divider	
	PLL2->PLLDIV1 |= (0x00008000);          // Enable VPBE divider	
	PLL2->PLLCMD |= 0x00000001;             // Tell PLL to do phase alignment
	while ((PLL2->PLLSTAT) & 0x1);          // Wait until done
	waitloop(256*(PLL1_Mult/2));

	PLL2->PLLCTL |= (0x00000008);           // Take PLL out of reset	
	waitloop(2000*(PLL1_Mult/2));                       // Wait for locking
	
	PLL2->PLLCTL |= (0x00000001);           // Switch out of bypass mode
}

void DDR2Init()
{
	int32_t tempVTP;
	
	// Set the DDR2 to enable
	LPSCTransition(LPSC_DDR2, PSC_ENABLE);
		
	// For Micron MT47H64M16BT-37E @ 162 MHz
	// Setup the read latency (CAS Latency + 3 = 6 (but write 6-1=5))
	DDR->DDRPHYCR = 0x14001900 | DDR_READ_Latency;

	// Set TIMUNLOCK bit, CAS LAtency 3, 8 banks, 1024-word page size 
	DDR->SDBCR = 0x00138000 |
	             (DDR_NM << 14)   |
	             (DDR_CL << 9)    |
	             (DDR_IBANK << 4) |
	             (DDR_PAGESIZE <<0);
	
	// Program timing registers 
	DDR->SDTIMR = (DDR_T_RFC << 25) |              
                  (DDR_T_RP << 22)  |
                  (DDR_T_RCD << 19) |
                  (DDR_T_WR << 16)  |
                  (DDR_T_RAS << 11) |
                  (DDR_T_RC << 6)   |
                  (DDR_T_RRD << 3)  |
                  (DDR_T_WTR << 0);
                  
	DDR->SDTIMR2 = (DDR_T_XSNR << 16) |
                   (DDR_T_XSRD << 8)  |
                   (DDR_T_RTP << 5)   |
                   (DDR_T_CKE << 0);
    
    
    // Clear the TIMUNLOCK bit 
	DDR->SDBCR &= (~0x00008000);
	
	// Set the refresh rate
	DDR->SDRCR = DDR_RR;
	
	// Dummy write/read to apply timing settings
	DDRMem[0] = DDR_TEST_PATTERN;
	if (DDRMem[0] == DDR_TEST_PATTERN)
          UARTSendInt(DDRMem[0]);
	
	// Set the DDR2 to syncreset
	LPSCTransition(LPSC_DDR2, PSC_SYNCRESET);

	// Set the DDR2 to enable
	LPSCTransition(LPSC_DDR2, PSC_ENABLE);
	
			 
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
	PLL1->PLLM = PLL1_Mult - 1;        // 27Mhz * (21+1) = 594 MHz 
			
	PLL1->PLLCMD |= 0x00000001;		// Tell PLL to do phase alignment
	while ((PLL1->PLLSTAT) & 0x1);	// Wait until done
	
	waitloop(256);
	PLL1->PLLCTL |= (0x00000008);		//	Take PLL out of reset	
	waitloop(2000);				// Wait for locking
	
	PLL1->PLLCTL |= (0x00000001);		// Switch out of bypass mode
}
/* 
void AEMIFInit()
{     
   AEMIF->AWCCR     = 0x00000000;
   AEMIF->AB1CR     = 0x3FFFFFFD;
   AEMIF->AB2CR     = 0x3FFFFFFD;
   AEMIF->AB3CR     = 0x3FFFFFFD;
   AEMIF->AB4CR     = 0x3FFFFFFD;
   AEMIF->NANDFCR   = 0x00000000;
}*/
 
void UARTInit()
{		
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
	//UART0->PWREMU_MGNT |= 0x8001;

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

void IVTInit()
{
	volatile uint32_t *ivect;
    extern uint32_t __IVT;
    
	if (gBootMode == NON_SECURE_NOR)
	{
		ivect = &(__IVT);
		*ivect++ = 0xEAFFFFFE;  /* Reset @ 0x00*/
	}
	else
	{
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

