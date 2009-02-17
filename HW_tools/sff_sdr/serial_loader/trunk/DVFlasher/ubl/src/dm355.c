/* dm355.c - Platform initialization file */

#include "davinci.h"
#include "ddr.h"
#include "util.h"
#include "uart.h"

#define DEVICE_VTPIOCR_PWRDN_MASK       (0x00000040)
#define DEVICE_VTPIOCR_LOCK_MASK        (0x00000080)
#define DEVICE_VTPIOCR_PWRSAVE_MASK     (0x00000100)
#define DEVICE_VTPIOCR_CLR_MASK         (0x00002000)
#define DEVICE_VTPIOCR_VTPIOREADY_MASK  (0x00004000)
#define DEVICE_VTPIOCR_READY_MASK       (0x00008000)

/* Global Memory Timing and PLL Settings */
static const uint8_t  DDR_NM = 1;	/* 16-bit bus width only on DM35x. */
static const uint8_t  DDR_PAGESIZE = 2;	/* 1024-word page size. */

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

	// Set bit LOCK(bit 7) and PWRSAVE (bit 8)
	SYSTEM->VTPIOCR |= (DEVICE_VTPIOCR_PWRSAVE_MASK | DEVICE_VTPIOCR_LOCK_MASK);

	// Powerdown VTP as it is locked (bit 6)
	SYSTEM->VTPIOCR |= DEVICE_VTPIOCR_PWRDN_MASK;

	// Wait for calibration to complete
	waitloop( 150 );

	// Set the DDR2 to synreset, then enable it again
	LPSCTransition(LPSC_DDR2,PD0,PSC_SYNCRESET);
	LPSCTransition(LPSC_DDR2,PD0,PSC_ENABLE);

	/* DDR Timing Setup. */

	/* The configuration of DDRPHYCR1 is not dependent on the DDR2 device
	 * specification but rather on the board layout.
	 * Setup the read latency, external DQS gating enabled */
	DDR->DDRPHYCR = 0x50006400 | (DDR_READ_Latency & 0xFF);

	/* Due to an internal DM35x hardware issue, you must not accept the
	 * default bit field value (0xFF) for PR_OLD_COUNT. */
	DDR->PBBPR = 0x000000FE;

	/* Set the refresh rate */
	DDR->SDRCR &= (0xFFFF0000 | DDR_RR);

	// Set TIMUNLOCK bit, CAS LAtency, banks, 1024-word page size
	DDR->SDBCR =  0x00008000    |
		(DDR_NM << 14)      |
		(DDR_CL << 9)       |
		(DDR_IBANK << 4)    |
		(DDR_PAGESIZE << 0);

	// Program timing registers
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
