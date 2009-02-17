/* dm644x.h - Platform init header file */

#ifndef _DM644X_H_
#define _DM644X_H_

#include <stdint.h> 

#include "tistdtypes.h"

#define SYSTEM_CLK_HZ 27000000

#define PLL1_Mult 22 /* DSP=594 MHz ARM=297 MHz */

#define PLL2_Mult 24 /* DDRPHY=324 MHz DDRCLK=162 MHz */
#define PLL2_Div1 12
#define PLL2_Div2  2

#endif /* _DM644X_H_ */
