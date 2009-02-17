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

#include "davinci.h"
#include "ddr.h"
#include "util.h"
#include "uart.h"

extern volatile uint32_t DDRMem[0];

/* Global Memory Timing and PLL Settings */
static const uint8_t  DDR_NM = 0;	/* 32-bit bus width by default. */
static const uint8_t  DDR_PAGESIZE = 2;	/* 1024-word page size. */

void PSCInit(void)
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
	PSC->MDCTL[LPSC_MMC_SD0]    |= 0x0203;
	PSC->MDCTL[LPSC_MEMSTK]     |= 0x0203;
	PSC->MDCTL[LPSC_ASP0]       |= 0x0203;
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
	PSC->MDCTL[LPSC_MMC_SD0]    &= 0x0003;
	PSC->MDCTL[LPSC_MEMSTK]     &= 0x0003;
	PSC->MDCTL[LPSC_ASP0]       &= 0x0003;
	PSC->MDCTL[LPSC_GPIO]       &= 0x0003;
	PSC->MDCTL[LPSC_IMCOP]      &= 0x0003;    
}

int DDR2Init(void)
{
	int32_t tempVTP;
	
	/* Enable DDR2 module. */
	LPSCTransition(LPSC_DDR2, PD0, PSC_ENABLE);
		
	/* Setup the read latency (CAS Latency + 3 = 6 (but write 6-1=5)) */
	DDR->DDRPHYCR = 0x14001900 | DDR_READ_Latency;

	/* Set TIMUNLOCK bit, CAS Latency, number of banks, page size */
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
	LPSCTransition(LPSC_DDR2, PD0, PSC_SYNCRESET);

	// Set the DDR2 to enable
	LPSCTransition(LPSC_DDR2, PD0, PSC_ENABLE);
	
			 
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

	return E_PASS;
}
