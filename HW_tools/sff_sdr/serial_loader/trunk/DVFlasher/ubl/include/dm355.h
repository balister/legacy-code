/* dm355.h - Platform init header file */

#ifndef _DM355_H_
#define _DM355_H_

#include <stdint.h>

#include "tistdtypes.h"

#define SYSTEM_CLK_HZ 24000000

#define VPBE_CLK_HZ   27000000

#define PLL1_Mult 144 /* 216 MHz */

#define PLL2_Mult 114
#define PLL2_Div1   2 /* Fixed */
#define PLL2_Div2  -1 /* PLLDIV2 not used */

#define DEVICE_MISC_PLL1POSTDIV_MASK	0x00000002
#define DEVICE_MISC_AIMWAITST_MASK	0x00000001
#define DEVICE_MISC_TIMER2WDT_MASK	0x00000010

#endif /* _DM355_H_ */
