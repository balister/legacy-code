/* dm355.h - Platform init header file */

#ifndef _DM355_H_
#define _DM355_H_

#include <stdint.h>

#include "tistdtypes.h"

/* -------------------------------------------------------------------------- *
 *    System Control Module register structure - See sprue14.pdf, Chapter 10  *
 *       for more details.                                                    *
 * -------------------------------------------------------------------------- */ 
typedef struct _sys_module_regs_
{
  volatile uint32_t PINMUX[5];         //0x00
  volatile uint32_t BOOTCFG;           //0x14
  volatile uint32_t ARM_INTMUX;        //0x18
  volatile uint32_t EDMA_EVTMUX;       //0x1C
  volatile uint32_t DDR_SLEW;          //0x20
  volatile uint32_t CLKOUT;            //0x24
  volatile uint32_t DEVICE_ID;         //0x28
  volatile uint32_t VDAC_CONFIG;       //0x2C
  volatile uint32_t TIMER64_CTL;       //0x30
  volatile uint32_t USBPHY_CTL;        //0x34
  volatile uint32_t MISC;              //0x38
  volatile uint32_t MSTPRI[2];         //0x3C
  volatile uint32_t VPSS_CLKCTL;       //0x44
  volatile uint32_t DEEPSLEEP;         //0x48
  volatile uint8_t  RSVD0[4];          //0x4C
  volatile uint32_t DEBOUNCE[8];       //0x50
  volatile uint32_t VTPIOCR;           //0x70
} sysModuleRegs;

#endif // End _DM355_H_
