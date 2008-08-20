/* --------------------------------------------------------------------------
    FILE        : util.h
    PURPOSE     : Misc. utility header file
    PROJECT     : DaVinci User Boot-Loader and Flasher
    AUTHOR      : Daniel Allred
    DATE	    : Jan-22-2007

    HISTORY
 	    v1.00 completion
 	        Daniel Allred - Jan-22-2007
 ----------------------------------------------------------------------------- */

#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdint.h>

#define ENDIAN_SWAP(a) (((a&0xFF)<<24)|((a&0xFF0000)>>8)|((a&0xFF00)<<8)|((a&0xFF000000)>>24))

// Memory Allocation for decoing the s-record
void *ubl_alloc_mem (uint32_t size);
uint32_t get_current_mem_loc(void);
void set_current_mem_loc(uint32_t value);

// Routines to decode the S-record
uint32_t GetHexData(uint8_t *src, uint32_t numBytes, uint8_t *seq);
uint32_t GetHexAddr(uint8_t *src, uint32_t* addr);
uint32_t SRecDecode(uint8_t *srecAddr, uint32_t srecByteCnt, uint32_t *binAddr, uint32_t *binByteCnt);

// NOP wait loop 
void waitloop(int32_t loopcnt);

#endif //_UTIL_H_
