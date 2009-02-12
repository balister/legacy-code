/* --------------------------------------------------------------------------
    FILE        : dm644x.h
    PURPOSE     : Platform init header file
    PROJECT     : DaVinci User Boot-Loader and Flasher
    AUTHOR      : Daniel Allred
    DATE	    : Jan-22-2007  
 
    HISTORY
 	     v1.00 completion 							 						      
 	          Daniel Allred - Jan-22-2007
 	     v1.11 DJA - 7-Mar-2007 
 	        Added additional required defines for the LPSCs                                          
 ----------------------------------------------------------------------------- */

#ifndef _DM644X_H_
#define _DM644X_H_

#include <stdint.h> 

#include "tistdtypes.h"

/* -------------------------------------------------------------------------- *
 *    System Control Module register structure - See sprue14.pdf, Chapter 10  *
 *       for more details.                                                    *
 * -------------------------------------------------------------------------- */ 
typedef struct _sys_module_regs_
{
	volatile uint32_t PINMUX[2];         //0x00
	volatile uint32_t DSPBOOTADDR;       //0x08
	volatile uint32_t SUSPSRC;           //0x0C
	volatile uint32_t INTGEN;            //0x10
	volatile uint32_t BOOTCFG;           //0x14
	volatile uint8_t  RSVD0[16];         //0x18
	volatile uint32_t DEVICE_ID;         //0x28
	volatile uint8_t  RSVD1[8];          //0x2C
	volatile uint32_t USBPHY_CTL;        //0x34
	volatile uint32_t CHP_SHRTSW;        //0x38
	volatile uint32_t MSTPRI[2];         //0x3C
	volatile uint32_t VPSS_CLKCTL;       //0x44
	volatile uint32_t VDD3P3V_PWDN;      //0x48
	volatile uint32_t DDRVTPER;          //0x4C
	volatile uint8_t  RSVD2[32];         //0x50 
} sysModuleRegs;

#endif // End _DM644X_H_
