/* ddr.h - DDR devices parameters */

#ifndef _DDDR_H_
#define _DDDR_H_

#include <stdint.h>

#if defined(MICRON_MT47H32M16BN_3_171MHZ)
/* Micron MT47H32M16BN-3 @ 171 MHz settings:
 * TCK      = 5.85 nS -> 1 / 171MHz
 * T_REF    = 7.8 uS (varies with commercial vs industrial)
 * T_RFC    = 105 nS (varies with capacity)
 * T_RP     = 15 nS
 * T_RCD    = 15 nS
 * T_WR     = 15 nS
 * T_RAS    = 40 nS
 * T_RASMAX = 70 uS
 * T_RTP    = 7.5 nS
 * T_RC     = 55 nS
 * T_RRD    = 10 nS
 * T_WTR    = 7.5 nS
 * T_XSRD   = 200 nS
 * T_XSNR   = 115 nS -> T_RFC(MIN) + 10
 * T_CKE    = 3 TCK
 * T_XP     = 2 TCK
 */
static const uint8_t  DDR_IBANK = 2;    /* 4 banks. */
static const uint16_t DDR_RR    = 1336; /* DDRCLK * T_REF */
static const uint8_t  DDR_CL    = 3;
static const uint8_t  DDR_T_RFC = 17;  /* (T_RFC * DDRCLK) - 1 */
static const uint8_t  DDR_T_RP  = 2;   /* (T_RP  * DDRCLK) - 1 */
static const uint8_t  DDR_T_RCD = 2;   /* (T_RCD * DDRCLK) - 1 */
static const uint8_t  DDR_T_WR  = 2;   /* (T_WR  * DDRCLK) - 1 */
static const uint8_t  DDR_T_RAS = 6;   /* (T_RAS * DDRCLK) - 1 */
static const uint8_t  DDR_T_RC  = 9;   /* (T_RC  * DDRCLK) - 1 */
static const uint8_t  DDR_T_RRD = 1;   /* [((4 * T_RRD) + (2 * TCK)) / (4 * TCK)] - 1 */
/*
 * Only for 8 bank DDR2/mDDR memories. When interfacing to DDR2/mDDR memories
 * with less than 8 banks the T_RRD field should be calculated using:
 *   (T_RRD * DDRCLK) - 1.
 */
static const uint8_t  DDR_T_WTR    = 1;  /* (T_WTR  * DDRCLK) - 1 */
static const uint8_t  DDR_T_RASMAX = 8;  /*
					  * (T_RASMAX(uS) / T_REF) - 1
					  * Should be 17 but max. value
					  * is 15 (4 bits)
					  */
static const uint8_t  DDR_T_XP   = 2;   /*
					 * If T_XP > T_CKE then
					 *   T_XP = T_XP - 1
					 * else
					 *   T_XP = T_CKE - 1
					 */
static const uint8_t  DDR_T_XSNR = 19;  /* (T_XSNR * DDRCLK) - 1 */
static const uint8_t  DDR_T_XSRD = 199; /* T_XSRD - 1 */
static const uint8_t  DDR_T_RTP  = 1;   /* (T_RTP * DDRCLK) - 1 */
static const uint8_t  DDR_T_CKE  = 2;   /* T_CKE - 1 */
static const uint8_t  DDR_READ_Latency = 4; /* Board specific */

#elif defined(MICRON_MT47H32M16BN_3_162MHZ) /* SFFSDR */
/* Micron MT47H32M16BN-3 @ 162 MHz settings:
 * TCK      = 6.17 nS -> 1 / 162 MHz
 * T_REF    = 7.8 uS (varies with commercial vs industrial)
 * T_RFC    = 105 nS (varies with capacity)
 * T_RP     = 15 nS
 * T_RCD    = 15 nS
 * T_WR     = 15 nS
 * T_RAS    = 40 nS
 * T_RASMAX = 70 uS
 * T_RTP    = 7.5 nS
 * T_RC     = 55 nS
 * T_RRD    = 10 nS
 * T_WTR    = 7.5 nS
 * T_XSRD   = 200 nS
 * T_XSNR   = 115 nS -> T_RFC(MIN) + 10
 * T_CKE    = 3 TCK
 * T_XP     = 2 TCK
 */
static const uint8_t  DDR_IBANK = 2;    /* 4 banks. */
static const uint16_t DDR_RR    = 1265; /* DDRCLK * T_REF */
static const uint8_t  DDR_CL    = 3;
static const uint8_t  DDR_T_RFC = 16;  /* (T_RFC * DDRCLK) - 1 */
static const uint8_t  DDR_T_RP  = 2;   /* (T_RP  * DDRCLK) - 1 */
static const uint8_t  DDR_T_RCD = 2;   /* (T_RCD * DDRCLK) - 1 */
static const uint8_t  DDR_T_WR  = 2;   /* (T_WR  * DDRCLK) - 1 */
static const uint8_t  DDR_T_RAS = 6;   /* (T_RAS * DDRCLK) - 1 */
static const uint8_t  DDR_T_RC  = 8;   /* (T_RC  * DDRCLK) - 1 */
static const uint8_t  DDR_T_RRD = 1;   /* [((4 * T_RRD) + (2 * TCK)) / (4 * TCK)] - 1 */
/*
 * Only for 8 bank DDR2/mDDR memories. When interfacing to DDR2/mDDR memories
 * with less than 8 banks the T_RRD field should be calculated using:
 *   (T_RRD * DDRCLK) - 1.
 */
static const uint8_t  DDR_T_WTR    = 1;  /* (T_WTR  * DDRCLK) - 1 */
static const uint8_t  DDR_T_RASMAX = 8;  /*
					  * (T_RASMAX(uS) / T_REF) - 1
					  * Should be 17 but max. value
					  * is 15 (4 bits)
					  */
static const uint8_t  DDR_T_XP   = 2;   /*
					 * If T_XP > T_CKE then
					 *   T_XP = T_XP - 1
					 * else
					 *   T_XP = T_CKE - 1
					 */
static const uint8_t  DDR_T_XSNR = 18;  /* (T_XSNR * DDRCLK) - 1 */
static const uint8_t  DDR_T_XSRD = 199; /* T_XSRD - 1 */
static const uint8_t  DDR_T_RTP  = 1;   /* (T_RTP * DDRCLK) - 1 */
static const uint8_t  DDR_T_CKE  = 2;   /* T_CKE - 1 */
static const uint8_t  DDR_READ_Latency = 4; /* Board specific */

#elif defined(MICRON_MT47H64M16BT_37E_171MHZ) /* EVM DM355 */
/* Micron MT47H64M16BT-37E @ 171 MHz */
static const uint8_t  DDR_IBANK = 3;	/* 8 banks. */
static const uint16_t DDR_RR = 1336;
static const uint8_t  DDR_CL = 3;
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
static const uint8_t  DDR_READ_Latency = 4; /* Board specific */

#elif defined(MICRON_MT47H64M16HR_3_162MHZ) /* DAS Commercial */
/* Micron MT47H64M16HR-3 @ 162 MHz settings:
 * TCK      = 6.17 nS -> 1 / 162 MHz
 * T_REF    = 7.8 uS (varies with commercial vs industrial)
 * T_RFC    = 127.5 nS (varies with capacity)
 * T_RP     = 15 nS
 * T_RCD    = 15 nS
 * T_WR     = 15 nS
 * T_RAS    = 40 nS
 * T_RASMAX = 70 uS
 * T_RTP    = 7.5 nS
 * T_RC     = 55 nS
 * T_RRD    = 10 nS
 * T_WTR    = 7.5 nS
 * T_XSRD   = 200 nS
 * T_XSNR   = 138 nS -> T_RFC(MIN) + 10
 * T_CKE    = 3 TCK
 * T_XP     = 2 TCK
 */
static const uint8_t  DDR_IBANK = 3;   /* 8 banks. */
static const uint16_t DDR_RR    = 1265; /* DDRCLK * T_REF */
static const uint8_t  DDR_CL    = 3;
static const uint8_t  DDR_T_RFC = 20;  /* (T_RFC * DDRCLK) - 1 */
static const uint8_t  DDR_T_RP  = 2;   /* (T_RP  * DDRCLK) - 1 */
static const uint8_t  DDR_T_RCD = 2;   /* (T_RCD * DDRCLK) - 1 */
static const uint8_t  DDR_T_WR  = 2;   /* (T_WR  * DDRCLK) - 1 */
static const uint8_t  DDR_T_RAS = 6;   /* (T_RAS * DDRCLK) - 1 */
static const uint8_t  DDR_T_RC  = 8;   /* (T_RC  * DDRCLK) - 1 */
static const uint8_t  DDR_T_RRD = 2;   /* [((4 * T_RRD) + (2 * TCK)) / (4 * TCK)] - 1 */
/*
 * Only for 8 bank DDR2/mDDR memories. When interfacing to DDR2/mDDR memories
 * with less than 8 banks the T_RRD field should be calculated using:
 *   (T_RRD * DDRCLK) - 1.
 */
static const uint8_t  DDR_T_WTR    = 1;  /* (T_WTR  * DDRCLK) - 1 */
static const uint8_t  DDR_T_RASMAX = 8;  /*
					  * (T_RASMAX(uS) / T_REF) - 1
					  * Should be 17 but max. value
					  * is 15 (4 bits)
					  */
static const uint8_t  DDR_T_XP   = 2;   /*
					 * If T_XP > T_CKE then
					 *   T_XP = T_XP - 1
					 * else
					 *   T_XP = T_CKE - 1
					 */
static const uint8_t  DDR_T_XSNR = 21;  /* (T_XSNR * DDRCLK) - 1 */
static const uint8_t  DDR_T_XSRD = 199; /* T_XSRD - 1 */
static const uint8_t  DDR_T_RTP  = 1;   /* (T_RTP * DDRCLK) - 1 */
static const uint8_t  DDR_T_CKE  = 2;   /* T_CKE - 1 */
static const uint8_t  DDR_READ_Latency = 4; /* Board specific */

#elif defined(MICRON_MT47H64M16HR_3IT_162MHZ) /* DAS industrial */
/* Micron MT47H64M16HR-3IT @ 162 MHz settings:
 * TCK      = 6.17 nS -> 1 / 162 MHz
 * T_REF    = 3.9 uS (varies with commercial vs industrial)
 * T_RFC    = 127.5 nS (varies with capacity)
 * T_RP     = 15 nS
 * T_RCD    = 15 nS
 * T_WR     = 15 nS
 * T_RAS    = 40 nS
 * T_RASMAX = 70 uS
 * T_RTP    = 7.5 nS
 * T_RC     = 55 nS
 * T_RRD    = 10 nS
 * T_WTR    = 7.5 nS
 * T_XSRD   = 200 nS
 * T_XSNR   = 138 nS -> T_RFC(MIN) + 10
 * T_CKE    = 3 TCK
 * T_XP     = 2 TCK
 */
static const uint8_t  DDR_IBANK = 3;   /* 8 banks. */
static const uint16_t DDR_RR    = 635; /* DDRCLK * T_REF */
static const uint8_t  DDR_CL    = 3;
static const uint8_t  DDR_T_RFC = 20;  /* (T_RFC * DDRCLK) - 1 */
static const uint8_t  DDR_T_RP  = 2;   /* (T_RP  * DDRCLK) - 1 */
static const uint8_t  DDR_T_RCD = 2;   /* (T_RCD * DDRCLK) - 1 */
static const uint8_t  DDR_T_WR  = 2;   /* (T_WR  * DDRCLK) - 1 */
static const uint8_t  DDR_T_RAS = 6;   /* (T_RAS * DDRCLK) - 1 */
static const uint8_t  DDR_T_RC  = 8;   /* (T_RC  * DDRCLK) - 1 */
static const uint8_t  DDR_T_RRD = 2;   /* [((4 * T_RRD) + (2 * TCK)) / (4 * TCK)] - 1 */
/*
 * Only for 8 bank DDR2/mDDR memories. When interfacing to DDR2/mDDR memories
 * with less than 8 banks the T_RRD field should be calculated using:
 *   (T_RRD * DDRCLK) - 1.
 */
static const uint8_t  DDR_T_WTR    = 1;  /* (T_WTR  * DDRCLK) - 1 */
static const uint8_t  DDR_T_RASMAX = 15; /*
					  * (T_RASMAX(uS) / T_REF) - 1
					  * Should be 17 but max. value
					  * is 15 (4 bits)
					  */
static const uint8_t  DDR_T_XP   = 2;   /*
					 * If T_XP > T_CKE then
					 *   T_XP = T_XP - 1
					 * else
					 *   T_XP = T_CKE - 1
					 */
static const uint8_t  DDR_T_XSNR = 21;  /* (T_XSNR * DDRCLK) - 1 */
static const uint8_t  DDR_T_XSRD = 199; /* T_XSRD - 1 */
static const uint8_t  DDR_T_RTP  = 1;   /* (T_RTP * DDRCLK) - 1 */
static const uint8_t  DDR_T_CKE  = 2;   /* T_CKE - 1 */
static const uint8_t  DDR_READ_Latency = 4; /* Board specific */

#elif defined(MICRON_MT47H64M16BT_3_162MHZ)
/* Micron MT47H64M16HR-3IT @ 162 MHz settings:
 * TCK      = 5.85 nS -> 1 / 162 MHz
 * T_REF    = 3.9 uS (varies with commercial vs industrial)
 * T_RFC    = 198 nS (varies with capacity)
 * T_RP     = 15 nS
 * T_RCD    = 15 nS
 * T_WR     = 15 nS
 * T_RAS    = 40 nS
 * T_RASMAX = 70 uS
 * T_RTP    = 7.5 nS
 * T_RC     = 55 nS
 * T_RRD    = 10 nS
 * T_WTR    = 7.5 nS
 * T_XSRD   = 200 nS
 * T_XSNR   = 208 nS -> T_RFC(MIN) + 10
 * T_CKE    = 3 TCK
 * T_XP     = 2 TCK
 */
static const uint8_t  DDR_IBANK = 3;   /* 8 banks. */
static const uint16_t DDR_RR    = 667; /* DDRCLK * T_REF */
static const uint8_t  DDR_CL    = 3;
static const uint8_t  DDR_T_RFC = 33;  /* (T_RFC * DDRCLK) - 1 */
static const uint8_t  DDR_T_RP  = 2;   /* (T_RP  * DDRCLK) - 1 */
static const uint8_t  DDR_T_RCD = 2;   /* (T_RCD * DDRCLK) - 1 */
static const uint8_t  DDR_T_WR  = 2;   /* (T_WR  * DDRCLK) - 1 */
static const uint8_t  DDR_T_RAS = 6;   /* (T_RAS * DDRCLK) - 1 */
static const uint8_t  DDR_T_RC  = 9;   /* (T_RC  * DDRCLK) - 1 */
static const uint8_t  DDR_T_RRD = 1;   /* [((4 * T_RRD) + (2 * TCK)) / (4 * TCK)] - 1 */
/*
 * Only for 8 bank DDR2/mDDR memories. When interfacing to DDR2/mDDR memories
 * with less than 8 banks the T_RRD field should be calculated using:
 *   (T_RRD * DDRCLK) - 1.
 */
static const uint8_t  DDR_T_WTR    = 1;  /* (T_WTR  * DDRCLK) - 1 */
static const uint8_t  DDR_T_RASMAX = 15; /*
					  * (T_RASMAX(uS) / T_REF) - 1
					  * Should be 17 but max. value
					  * is 15 (4 bits)
					  */
static const uint8_t  DDR_T_XP   = 2;   /*
					 * If T_XP > T_CKE then
					 *   T_XP = T_XP - 1
					 * else
					 *   T_XP = T_CKE - 1
					 */
static const uint8_t  DDR_T_XSNR = 34;  /* (T_XSNR * DDRCLK) - 1 */
static const uint8_t  DDR_T_XSRD = 199; /* T_XSRD - 1 */
static const uint8_t  DDR_T_RTP  = 1;   /* (T_RTP * DDRCLK) - 1 */
static const uint8_t  DDR_T_CKE  = 2;   /* T_CKE - 1 */
static const uint8_t  DDR_READ_Latency = 4; /* Board specific */

#elif defined(MICRON_MT47H128M16HG_3IT_171MHZ)
/* Micron MT47H128M16HG-3IT @ 171 MHz settings:
 * TCK      = 5.85 nS -> 1 / 171MHz
 * T_REF    = 3.9 uS (varies with commercial vs industrial)
 * T_RFC    = 198 nS (varies with capacity)
 * T_RP     = 15 nS
 * T_RCD    = 15 nS
 * T_WR     = 15 nS
 * T_RAS    = 40 nS
 * T_RASMAX = 70 uS
 * T_RTP    = 7.5 nS
 * T_RC     = 55 nS
 * T_RRD    = 10 nS
 * T_WTR    = 7.5 nS
 * T_XSRD   = 200 nS
 * T_XSNR   = 208 nS -> T_RFC(MIN) + 10
 * T_CKE    = 3 TCK
 * T_XP     = 2 TCK
 */
static const uint8_t  DDR_IBANK = 3;   /* 8 banks. */
static const uint16_t DDR_RR    = 667; /* DDRCLK * T_REF */
static const uint8_t  DDR_CL    = 3;
static const uint8_t  DDR_T_RFC = 33;  /* (T_RFC * DDRCLK) - 1 */
static const uint8_t  DDR_T_RP  = 2;   /* (T_RP  * DDRCLK) - 1 */
static const uint8_t  DDR_T_RCD = 2;   /* (T_RCD * DDRCLK) - 1 */
static const uint8_t  DDR_T_WR  = 2;   /* (T_WR  * DDRCLK) - 1 */
static const uint8_t  DDR_T_RAS = 6;   /* (T_RAS * DDRCLK) - 1 */
static const uint8_t  DDR_T_RC  = 9;   /* (T_RC  * DDRCLK) - 1 */
static const uint8_t  DDR_T_RRD = 1;   /* [((4 * T_RRD) + (2 * TCK)) / (4 * TCK)] - 1 */
/*
 * Only for 8 bank DDR2/mDDR memories. When interfacing to DDR2/mDDR memories
 * with less than 8 banks the T_RRD field should be calculated using:
 *   (T_RRD * DDRCLK) - 1.
 */
static const uint8_t  DDR_T_WTR    = 1;  /* (T_WTR  * DDRCLK) - 1 */
static const uint8_t  DDR_T_RASMAX = 15; /*
					  * (T_RASMAX(uS) / T_REF) - 1
					  * Should be 17 but max. value
					  * is 15 (4 bits)
					  */
static const uint8_t  DDR_T_XP   = 2;   /*
					 * If T_XP > T_CKE then
					 *   T_XP = T_XP - 1
					 * else
					 *   T_XP = T_CKE - 1
					 */
static const uint8_t  DDR_T_XSNR = 34;  /* (T_XSNR * DDRCLK) - 1 */
static const uint8_t  DDR_T_XSRD = 199; /* T_XSRD - 1 */
static const uint8_t  DDR_T_RTP  = 1;   /* (T_RTP * DDRCLK) - 1 */
static const uint8_t  DDR_T_CKE  = 2;   /* T_CKE - 1 */
static const uint8_t  DDR_READ_Latency = 4; /* Board specific */

#endif

#endif /* _DDDR_H_ */
