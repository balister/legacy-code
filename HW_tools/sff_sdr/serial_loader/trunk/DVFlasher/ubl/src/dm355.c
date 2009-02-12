/* dm355.c - Platform initialization file */

#include "davinci.h"
#include "ubl.h"
#include "util.h"
#include "uart.h"

#define DEVICE_PLLCTL_PLLEN_MASK    (0x00000001)
#define DEVICE_PLLCTL_PLLPWRDN_MASK (0x00000002)
#define DEVICE_PLLCTL_PLLRST_MASK   (0x00000008)
#define DEVICE_PLLCTL_PLLDIS_MASK   (0x00000010)
#define DEVICE_PLLCTL_PLLENSRC_MASK (0x00000020)
#define DEVICE_PLLCTL_CLKMODE_MASK  (0x00000100)

#define DEVICE_PLLCMD_GOSET_MASK    (0x00000001)
#define DEVICE_PLLSTAT_GOSTAT_MASK  (0x00000001)
#define DEVICE_PLLDIV_EN_MASK       (0x00008000)
#define DEVICE_PLLSTAT_LOCK_MASK    (0x00000002)

#define DEVICE_VTPIOCR_PWRDN_MASK       (0x00000040)
#define DEVICE_VTPIOCR_LOCK_MASK        (0x00000080)
#define DEVICE_VTPIOCR_PWRSAVE_MASK     (0x00000100)
#define DEVICE_VTPIOCR_CLR_MASK         (0x00002000)
#define DEVICE_VTPIOCR_VTPIOREADY_MASK  (0x00004000)
#define DEVICE_VTPIOCR_READY_MASK       (0x00008000)

#define DEVICE_MISC_PLL1POSTDIV_MASK    (0x00000002)
#define DEVICE_MISC_AIMWAITST_MASK      (0x00000001)
#define DEVICE_MISC_TIMER2WDT_MASK      (0x00000010)


// ---------------------------------------------------------------------------
// Global Memory Timing and PLL Settings
// ---------------------------------------------------------------------------
// For Micron MT47H64M16BT-37E @ 171 MHz
static const uint8_t  DDR_NM = 1;      // Must be set to 1 on DM35x (only 16-bit buswidth is supported)
static const uint8_t  DDR_CL = 3;
static const uint8_t  DDR_IBANK = 3;
static const uint8_t  DDR_PAGESIZE = 2;
static const uint8_t  DDR_T_RFC = 21;
static const uint8_t  DDR_T_RP = 2;
static const uint8_t  DDR_T_RCD = 2;
static const uint8_t  DDR_T_WR = 2;
static const uint8_t  DDR_T_RAS = 6;
static const uint8_t  DDR_T_RC = 9;
static const uint8_t  DDR_T_RRD = 1;
static const uint8_t  DDR_T_WTR = 1;
static const uint8_t  DDR_T_RASMAX = 7;
static const uint8_t  DDR_T_XP = 2;
static const uint8_t  DDR_T_XSNR = 23;
static const uint8_t  DDR_T_XSRD = 199;
static const uint8_t  DDR_T_RTP = 3;
static const uint8_t  DDR_T_CKE = 3;
static const uint16_t DDR_RR = 1336;
static const uint8_t  DDR_READ_Latency = 4;

static const uint32_t PLL1_Mult = 144u; /* 216 MHz */
static const uint32_t PLL2_Mult = 114u;


void PSCInit(void)
{
	uint32_t i;

	//***************************************
	// Do always on power domain transitions
	//***************************************
	while ((PSC->PTSTAT) & 0x00000001);

	for( i = LPSC_VPSS_MAST ; i <= LPSC_ARM ; i++ )
		PSC->MDCTL[i] = (PSC->MDCTL[i] & 0xFFFFFFE0) | PSC_ENABLE;

	PSC->MDCTL[LPSC_VPSS_DAC] = (PSC->MDCTL[LPSC_VPSS_DAC] & 0xFFFFFFE0) | PSC_ENABLE;

	// Set EMURSTIE to 1 on the following
	PSC->MDCTL[LPSC_VPSS_MAST]    |= EMURSTIE_MASK;
	PSC->MDCTL[LPSC_VPSS_SLV]     |= EMURSTIE_MASK;
	PSC->MDCTL[LPSC_TIMER3]       |= EMURSTIE_MASK;
	PSC->MDCTL[LPSC_SPI1]         |= EMURSTIE_MASK;
	PSC->MDCTL[LPSC_USB]          |= EMURSTIE_MASK;
	PSC->MDCTL[LPSC_PWM3]         |= EMURSTIE_MASK;
	PSC->MDCTL[LPSC_DDR2]         |= EMURSTIE_MASK;
	PSC->MDCTL[LPSC_AEMIF]        |= EMURSTIE_MASK;
	PSC->MDCTL[LPSC_MMC_SD0]      |= EMURSTIE_MASK;
	PSC->MDCTL[LPSC_ASP0]         |= EMURSTIE_MASK;
	PSC->MDCTL[LPSC_GPIO]         |= EMURSTIE_MASK;
	PSC->MDCTL[LPSC_VPSS_DAC]     |= EMURSTIE_MASK;

	// Do Always-On Power Domain Transitions
	PSC->PTCMD |= 0x00000001;
	while ((PSC->PTSTAT) & 0x00000001);

	// Clear EMURSTIE to 0 on the following
	PSC->MDCTL[LPSC_VPSS_MAST]    &= (~EMURSTIE_MASK);
	PSC->MDCTL[LPSC_VPSS_SLV]     &= (~EMURSTIE_MASK);
	PSC->MDCTL[LPSC_TIMER3]       &= (~EMURSTIE_MASK);
	PSC->MDCTL[LPSC_SPI1]         &= (~EMURSTIE_MASK);
	PSC->MDCTL[LPSC_USB]          &= (~EMURSTIE_MASK);
	PSC->MDCTL[LPSC_PWM3]         &= (~EMURSTIE_MASK);
	PSC->MDCTL[LPSC_DDR2]         &= (~EMURSTIE_MASK);
	PSC->MDCTL[LPSC_AEMIF]        &= (~EMURSTIE_MASK);
	PSC->MDCTL[LPSC_MMC_SD0]      &= (~EMURSTIE_MASK);
	PSC->MDCTL[LPSC_ASP0]         &= (~EMURSTIE_MASK);
	PSC->MDCTL[LPSC_GPIO]         &= (~EMURSTIE_MASK);
	PSC->MDCTL[LPSC_VPSS_DAC]     &= (~EMURSTIE_MASK);
}

int PLL2Init(void)
{	
	Bool PllIsPoweredUp = ( PLL2->PLLCTL & 0x0002 ) >> 1;

	// Step 1 - Set PLL2 clock input to internal osc.
	PLL2->PLLCTL &= ~(DEVICE_PLLCTL_CLKMODE_MASK);

	// Step 2 - Set PLL to bypass, then wait for PLL to stabilize
	PLL2->PLLCTL &= ~(DEVICE_PLLCTL_PLLENSRC_MASK);
	PLL2->PLLCTL &= ~(DEVICE_PLLCTL_PLLEN_MASK);

	waitloop( 4*(PLL1_Mult>>3) );
	//waitloop( 150 );

	// Step 3 - Reset PLL
	PLL2->PLLCTL |= DEVICE_PLLCTL_PLLRST_MASK;

	// Step 4 - Disable PLL
	// Step 5 - Powerup PLL
	// Step 6 - Enable PLL
	if ( PllIsPoweredUp )
	{
		PLL2->PLLCTL |= DEVICE_PLLCTL_PLLDIS_MASK;         // Disable PLL
		PLL2->PLLCTL &= ~(DEVICE_PLLCTL_PLLPWRDN_MASK);      // Power up PLL
		PLL2->PLLCTL &= ~(DEVICE_PLLCTL_PLLDIS_MASK);        // Enable PLL
	}
	else
	{
		PLL2->PLLCTL &= ~(DEVICE_PLLCTL_PLLDIS_MASK);        // Enable PLL
	}
	// Step 7 - Wait for PLL to stabilize
	waitloop( 150 );

	// Step 8 - Load PLL multiplier and divisors
	PLL2->PLLM = (PLL2_Mult-1) & 0xFF;              // PLL2 fixed at 342 MHz
	PLL2->PLLDIV1 |= DEVICE_PLLDIV_EN_MASK;         // SYSCLK1 = 342MHz /1 = 342 MHz (PHY rate) = 171 MHZ DDR2 rate

	// Step 9 - Start PLL changes
	PLL2->PLLCMD |= DEVICE_PLLCMD_GOSET_MASK;

	// Step 10 - Wait for phase alignment
	while( ( PLL2->PLLSTAT & DEVICE_PLLSTAT_GOSTAT_MASK ) );

	// Step 11 - Wait for PLL to reset ( ~5 usec )
	waitloop( 24*5*(PLL1_Mult>>3) );
	//waitloop( 2000 );

	// Step 12 - Release from reset
	PLL2->PLLCTL &= ~(DEVICE_PLLCTL_PLLRST_MASK);

	// Step 13 - Wait for PLL to re-lock ( 8000 ref clk cycles )
	waitloop( 8000*(PLL1_Mult>>3) );
	//waitloop( 2000 );

	// Step 13 - Switch out of BYPASS mode
	PLL2->PLLCTL |= DEVICE_PLLCTL_PLLEN_MASK;

	return E_PASS;
}

int DDR2Init(void)
{
	LPSCTransition(LPSC_DDR2,PD0,PSC_ENABLE);

	// DO VTP calibration
	SYSTEM->VTPIOCR &= ~( DEVICE_VTPIOCR_PWRDN_MASK |
			      DEVICE_VTPIOCR_LOCK_MASK |
			      DEVICE_VTPIOCR_CLR_MASK );  // Clear CLR & PWRDN & LOCK bits

	// Set CLR (bit 13)
	SYSTEM->VTPIOCR |= DEVICE_VTPIOCR_CLR_MASK;

	// Wait for ready
	while( !(SYSTEM->VTPIOCR & DEVICE_VTPIOCR_READY_MASK) );

	// Set bit VTP_IO_READY(bit 14)
	SYSTEM->VTPIOCR |= DEVICE_VTPIOCR_VTPIOREADY_MASK;

	// Set bit LOCK(bit 7) and  PWRSAVE  (bit 8)
	SYSTEM->VTPIOCR |= (DEVICE_VTPIOCR_PWRSAVE_MASK | DEVICE_VTPIOCR_LOCK_MASK);

	// Powerdown VTP as it is locked  (bit 6)
	SYSTEM->VTPIOCR |= DEVICE_VTPIOCR_PWRDN_MASK;

	// Wait for calibration to complete
	waitloop( 150 );

	// Set the DDR2 to synreset, then enable it again
	LPSCTransition(LPSC_DDR2,PD0,PSC_SYNCRESET);
	LPSCTransition(LPSC_DDR2,PD0,PSC_ENABLE);

	// DDR Timing Setup for Micron MT47H64M16BT-37E @ 171 MHz

	// Setup the read latency
	//External DQS gating enabled
	//DDR->DDRPHYCR = 0x50006494;
	DDR->DDRPHYCR = 0x50006400 | (0xFF & DDR_READ_Latency);

	DDR->PBBPR = 0x000000FE;

	// Set the refresh rate
	//DDR->SDRCR = 0x00000535;
	DDR->SDRCR &= (0xFFFF0000 | DDR_RR);

	// Set TIMUNLOCK bit, CAS LAtency, banks, 1024-word page size
	//DDR->SDBCR = 0x0000C632;
	DDR->SDBCR =  0x00008000          |
		(DDR_NM << 14)      |
		(DDR_CL << 9)       |
		(DDR_IBANK << 4)    |
		(DDR_PAGESIZE <<0);

	// Program timing registers
	//DDR->SDTIMR = 0x2A923249;
	DDR->SDTIMR =   (DDR_T_RFC << 25) |
		(DDR_T_RP << 22)  |
		(DDR_T_RCD << 19) |
		(DDR_T_WR << 16)  |
		(DDR_T_RAS << 11) |
		(DDR_T_RC << 6)   |
		(DDR_T_RRD << 3)  |
		(DDR_T_WTR << 0);

	//DDR->SDTIMR2 = 0x3C17C763;
	DDR->SDTIMR2 =  (DDR_T_RASMAX << 27)  |
		(DDR_T_XP << 25)      |
		(DDR_T_XSNR << 16)    |
		(DDR_T_XSRD << 8)     |
		(DDR_T_RTP << 5)      |
		(DDR_T_CKE << 0);


	// Clear the TIMUNLOCK bit
	//DDR->SDBCR = 0x00004632;
	DDR->SDBCR =  0x00000000          |
		(DDR_NM << 14)      |
		(DDR_CL << 9)       |
		(DDR_IBANK << 4)    |
		(DDR_PAGESIZE <<0);

	return E_PASS;
}

int PLL1Init(void)
{
	uint32_t PLLDiv3_ratio = 0, PLLDiv4_ratio = 0;

	// Calculate PLL divder ratio for divider 3 (feeds VPBE)
	while ( (PLLDiv3_ratio*27000000) < (24000000*(PLL1_Mult>>3)) )
	{
		PLLDiv3_ratio++;
	}

	// Check to make sure we can supply 27 MHz VPBE clock
	if ((PLLDiv3_ratio * 27000000) != (24000000*(PLL1_Mult>>3)))
		return E_FAIL;

	// See the device datasheet for more info (must be 2 or 4)
	PLLDiv4_ratio = 4;

	// Step 1 - Set PLL1 clock input to internal osc.
	PLL1->PLLCTL &= ~(DEVICE_PLLCTL_CLKMODE_MASK);

	// Step 2 - Set PLL to bypass, then wait for PLL to stabilize
	PLL1->PLLCTL &= ~(DEVICE_PLLCTL_PLLENSRC_MASK);
	PLL1->PLLCTL &= ~(DEVICE_PLLCTL_PLLEN_MASK);
	waitloop( 150 );

	// Step 3 - Reset PLL
	PLL1->PLLCTL |= DEVICE_PLLCTL_PLLRST_MASK;

	// Step 4 - Disable PLL
	// Step 5 - Powerup PLL
	// Step 6 - Enable PLL
	PLL1->PLLCTL |= DEVICE_PLLCTL_PLLDIS_MASK;         // Disable PLL
	PLL1->PLLCTL &= ~(DEVICE_PLLCTL_PLLPWRDN_MASK);    // Power up PLL
	PLL1->PLLCTL &= ~(DEVICE_PLLCTL_PLLDIS_MASK);      // Enable PLL

	// Step 7 - Wait for PLL to stabilize
	waitloop( 150 );

	// Step 8 - Load PLL multiplier and dividers, enable dividers as needed
	PLL1->PLLM = (PLL1_Mult - 1) & 0xff;                // 432MHz or 540MHz (depends on PLL1_Mult)
	PLL1->PLLDIV1 |= DEVICE_PLLDIV_EN_MASK;           // SYSCLK1 (ARM) fixed at 216MHz or 270MHz
	PLL1->PLLDIV2 |= DEVICE_PLLDIV_EN_MASK;           // SYSCLK2 (Peripherals) fixed at 432MHz/4 = 108MHz
	PLL1->PLLDIV3 = DEVICE_PLLDIV_EN_MASK | (PLLDiv3_ratio-1);   // SYSCLK3 at 27MHz (VPBE)
	PLL1->PLLDIV4 = DEVICE_PLLDIV_EN_MASK | (PLLDiv4_ratio-1);   // SYSCLK4 (VPSS) at 432MHz/4 = 108MHz
	waitloop( 150 );

	// Step 9 - Set the processor AIM wait state to 1 and PLL1 post-divider equal to 1
	SYSTEM->MISC &= ~(DEVICE_MISC_PLL1POSTDIV_MASK | DEVICE_MISC_AIMWAITST_MASK);

	// Step 10 - Bring PLL Out of Reset
	PLL1->PLLCMD |= DEVICE_PLLCMD_GOSET_MASK;

	// Step 10 - Wait for phase alignment
	while( ( PLL1->PLLSTAT & DEVICE_PLLSTAT_GOSTAT_MASK ) );

	// Step 11 - Wait for PLL to reset ( ~5 usec )
	waitloop( 24*5*(PLL1_Mult>>3) );
	//waitloop(2000);

	// Step 12 - Release from reset
	PLL1->PLLCTL &= ~(DEVICE_PLLCTL_PLLRST_MASK);

	// Step 13 - Wait for PLL to re-lock ( 8000 ref clk cycles )
	waitloop( 8000 );

	// Step 13 - Switch out of BYPASS mode
	PLL1->PLLCTL |= DEVICE_PLLCTL_PLLEN_MASK;

	return E_PASS;
}
