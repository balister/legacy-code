/* --------------------------------------------------------------------------
    FILE        : tistdtypes.h
    PURPOSE     : TI standard types header file
    PROJECT     : DaVinci User Boot-Loader and Flasher
    AUTHOR      : Vignesh LA, Daniel Allred
    DATE	    : Jan-22-2007
 
    HISTORY
        v1.00 completion
 	        Daniel Allred - Jan-22-2007
 ----------------------------------------------------------------------------- */

#ifndef _TISTDTYPES_H_
#define _TISTDTYPES_H_

#ifndef TRUE
typedef int Bool;
#define TRUE  ((Bool) 1)
#define FALSE ((Bool) 0)
#endif

#ifndef NULL
#define NULL  0
#endif

#endif /* _TISTDTYPES_H_ */
