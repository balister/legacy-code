/* --------------------------------------------------------------------------
    FILE        : ubl.h
    PURPOSE     : UBL main header file
    PROJECT     : DaVinci User Boot-Loader and Flasher
    AUTHOR      : Daniel Allred
    DATE	    : Jan-22-2007  
 
    HISTORY
        v1.00 completion
 	        Daniel Allred - Jan-22-2007
 	    v1.10 
 	        DJA - 1-Feb-2007 - Added the fakeentry point function prototype.
 	    v1.11
 	        DJA - 7-Mar-2007 - Version number update
 ----------------------------------------------------------------------------- */


#ifndef _UBL_H_
#define _UBL_H_

#include <stdint.h> 

#include "tistdtypes.h"

/* UBL version number */
#define UBL_VERSION_STRING "1.14.1"

/* Define this for bypassing the ECC check when reading from the NAND.
 * This is useful for debugging or during development. */
#define NAND_BYPASS_READ_PAGE_ECC_CHECK 1

// Define MagicNumber constants
#define MAGIC_NUMBER_VALID (0xA1ACED00)

/* Used by UBL and RBL */
#define UBL_MAGIC_SAFE				(0xA1ACED00)		/* Safe boot mode */

/* Used by RBL when doing NAND boot */
#define UBL_MAGIC_DMA				(0xA1ACED11)		/* DMA boot mode */
#define UBL_MAGIC_IC				(0xA1ACED22)		/* I Cache boot mode */
#define UBL_MAGIC_FAST				(0xA1ACED33)		/* Fast EMIF boot mode */
#define UBL_MAGIC_DMA_IC			(0xA1ACED44)		/* DMA + ICache boot mode */
#define UBL_MAGIC_DMA_IC_FAST		(0xA1ACED55)		/* DMA + ICache + Fast EMIF boot mode */

/* Used by UBL when doing UART boot, UBL Nor Boot, or NAND boot */
#define UBL_MAGIC_BIN_IMG			(0xA1ACED66)		/* Execute in place supported*/

/* Used by UBL when doing UART boot */
#define UBL_MAGIC_NOR_RESTORE		0xA1ACED77	/* Download via UART & Restore NOR with binary data */
#define UBL_MAGIC_NOR_SREC_BURN		0xA1ACED88	/* Download via UART & Burn NOR with UBL readable header and SREC data*/
#define UBL_MAGIC_NOR_BIN_BURN		0xA1ACED99	/* Download via UART & Burn NOR with UBL readable header and BIN data*/
#define UBL_MAGIC_NOR_GLOBAL_ERASE	0xA1ACEDAA	/* Download via UART & Global erase the NOR Flash*/
#define UBL_MAGIC_NAND_SREC_BURN	0xA1ACEDBB	/* Download via UART & Burn NAND - Image is S-record*/
#define UBL_MAGIC_NAND_BIN_BURN		0xA1ACEDCC	/* Download via UART & Burn NAND - Image is binary */
#define UBL_MAGIC_NAND_GLOBAL_ERASE	0xA1ACEDDD	/* Download via UART & Global erase the NAND Flash*/
#define UBL_MAGIC_DDR_TEST		0xA1ACEDEE	/* Test DDR2 memory. */

// Define UBL image size
#define UBL_IMAGE_SIZE      (0x00003800)

// Define maximum downloadable image size
#define MAX_IMAGE_SIZE		(0x00800000)

typedef struct {
	uint32_t magicNum;	/* Expected magic number */
	uint32_t entryPoint;	/* Entry point of the user application */
	uint32_t numPage;	/* Number of pages where boot loader is stored */
	uint32_t block;		/* starting block number where User boot loader is stored */
	uint32_t page;		/* starting page number where boot-loader is stored */
	uint32_t ldAddress;	/* Starting RAM address where image is to copied - XIP Mode */
} NAND_BOOT;

typedef struct {
	uint32_t magicNum;
	uint32_t entryPoint;	
	uint32_t appSize;
	uint32_t ldAddress;	/* Starting RAM address where image is to copied - XIP Mode */
} NOR_BOOT;


typedef enum BootMode_ {
	// Non secure NAND mode
	NON_SECURE_NAND = 0,
	// Non secure NOR mode
	NON_SECURE_NOR = 1,
		// Unknown mode
	UNKNOWN_MODE = 2,
	// Non secure UART mode
	NON_SECURE_UART = 3
} BootMode;

// ---------------- Function prototypes ----------------

/*
 * boot() has naked attribute (doesn't save registers since it is the entry point
 * out of boot and it doesn't have an exit point). This setup requires
 * that the gnu compiler uses the -nostdlib option. 
 */
void boot( void ) __attribute__((naked,section (".boot")));

/*
 * selfcopy() has naked attribute (doesn't save registers since it is the entry
 * point when the UBL is found at the base of the NOR Flash and then goes 
 * directly to the the boot() function, which is also naked). This setup 
 * requires that the gnu compiler uses the -nostdlib option. 
 */
void selfcopy( void ) __attribute__((naked,section (".selfcopy")));
void fake_entry( void ) __attribute__((naked,section (".fakeentry")));

int32_t main(void);
void (*APPEntry)(void);

#endif //_UBL_H_
