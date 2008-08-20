/* --------------------------------------------------------------------------
    FILE        : nand.h
    PURPOSE     : NAND header file
    PROJECT     : DaVinci User Boot-Loader and Flasher
    AUTHOR      : Daniel Allred
    DATE	    : Jan-22-2007

    HISTORY
        v1.00 completion
 	        Daniel Allred - Jan-22-2007
 	    v1.11 
 	        DJA - Mar-07-2007
 	            Added LPSC_1394 to power domain defines for use in setting the
 	            1394 power domain MDSTAT (Silicon workaround for reset from WDT)
 ----------------------------------------------------------------------------- */


#ifndef _NAND_H_
#define _NAND_H_

#include <stdint.h>

#include "ubl.h"
#include "tistdtypes.h"
#include "dm644x.h"
#include "nand.h"

// -------------- Constant define and macros --------------------

// BUS width defines
#define BUS_8BIT    0x01
#define BUS_16BIT   0x02
#define BUS_32BIT   0x04

// NAND flash addresses
#define NAND_DATA_OFFSET    (0x00)
#define NAND_ALE_OFFSET     (0x0B)
#define NAND_CLE_OFFSET     (0x10)

// NAND timeout 
//#define NAND_TIMEOUT    10240
#define NAND_TIMEOUT    20480 

// NAND flash commands
#define NAND_LO_PAGE        0x00
#define NAND_HI_PAGE        0x01
#define NAND_LOCK           0x2A
#define NAND_UNLOCK_START   0x23
#define NAND_UNLOCK_END     0x24
#define NAND_READ_30H       0x30
#define NAND_EXTRA_PAGE     0x50
#define	NAND_RDID           0x90
#define NAND_RDIDADD        0x00
#define	NAND_RESET          0xFF
#define	NAND_PGRM_START     0x80
#define NAND_PGRM_END       0x10
#define NAND_RDY            0x40
#define	NAND_PGM_FAIL       0x01
#define	NAND_BERASEC1       0x60
#define	NAND_BERASEC2       0xD0
#define	NAND_STATUS         0x70

// Flags to indicate what is being written to NAND
#define NAND_UBL_WRITE		0
#define NAND_APP_WRITE		1

// Defines for which blocks are valid for writing UBL and APP data
#define START_UBL_BLOCK_NUM     1
#define END_UBL_BLOCK_NUM       5
#define START_APP_BLOCK_NUM     6
#define END_APP_BLOCK_NUM       50

// Status Output
#define NAND_NANDFSR_READY		(0x01)
#define NAND_STATUS_WRITEREADY 	(0xC0)
#define NAND_STATUS_ERROR	 	(0x01)
#define NAND_STATUS_BUSY		(0x40)

#define UNKNOWN_NAND		    (0xFF)			// Unknown device id
#define MAX_PAGE_SIZE	        (2112)          // Including Spare Area

// Macro gets the page size in bytes without the spare bytes 
#define NANDFLASH_PAGESIZE(x) ( ( x >> 8 ) << 8 )


// ----------------- NAND device and information structures --------------------
// NAND_DEVICE_INFO structure
typedef struct _NAND_DEV_STRUCT_ {
    uint8_t   devID;              // DeviceID
    uint16_t  numBlocks;          // number of blocks in device
    uint8_t   pagesPerBlock;      // page count per block
    uint16_t  bytesPerPage;       // byte count per page (include spare)
} NAND_DEVICE_INFO, *PNAND_DEVICE_INFO;

// NAND_INFO structure 
typedef struct _NAND_MEDIA_STRUCT_ {
    uint32_t  flashBase;          // Base address of CS memory space where NAND is connected
	uint8_t   busWidth;           // NAND width 1->16 bits 0->8 bits
	uint8_t   devID;              // NAND_DevTable index
	uint16_t  numBlocks;          // block count per device
	uint8_t   pagesPerBlock;      // page count per block
	uint16_t  bytesPerPage;       // Number of bytes in a page
	uint8_t   numColAddrBytes;    // Number of Column address cycles
	uint8_t   numRowAddrBytes;    // Number of Row address cycles
	uint32_t  ECCMask;            // Mask for ECC register
	Bool    bigBlock;			// TRUE - Big block device, FALSE - small block device
	uint8_t   spareBytesPerPage;  // Number of bytes in spare byte area of each page   	
	uint8_t   blkShift;			// Number of bits by which block address is to be shifted
	uint8_t   pageShift;			// Number of bits by which page address is to be shifted
	uint8_t   CSOffset;           // 0 for CS2 space, 1 for CS3 space, 2 for CS4 space, 3 for CS5 space
} NAND_INFO, *PNAND_INFO;

typedef union {
	uint8_t c;
	uint16_t w;
	uint32_t l;
} FLASHData;

typedef union {
	volatile uint8_t *cp;
	volatile uint16_t *wp;
	volatile uint32_t *lp;
} FLASHPtr;

// ---------------- Prototypes of functions for NAND flash --------------------

// Generic NAND flash functions
volatile uint8_t *flash_make_addr (uint32_t baseAddr, uint32_t offset);
void flash_write_addr (PNAND_INFO pNandInfo, uint32_t addr);
void flash_write_cmd (PNAND_INFO pNandInfo, uint32_t cmd);
void flash_write_bytes (PNAND_INFO pNandInfo, void *pSrc, uint32_t numBytes);
void flash_write_addr_cycles(PNAND_INFO pNandInfo, uint32_t block, uint32_t page);
void flash_write_addr_bytes(PNAND_INFO pNandInfo, uint32_t numAddrBytes, uint32_t addr);
void flash_write_row_addr_bytes(PNAND_INFO pNandInfo, uint32_t block, uint32_t page);
void flash_write_data(PNAND_INFO pNandInfo, uint32_t offset, uint32_t data);
uint32_t flash_read_data (PNAND_INFO pNandInfo);
void flash_read_bytes(PNAND_INFO pNandInfo, void *pDest, uint32_t numBytes);
void flash_swap_data(PNAND_INFO pNandInfo, uint32_t* data);

//Initialize the NAND registers and structures
uint32_t NAND_Init();
uint32_t NAND_GetDetails();

// Page read and write functions
uint32_t NAND_ReadPage(uint32_t block, uint32_t page, uint8_t *dest);
uint32_t NAND_WritePage(uint32_t block, uint32_t page, uint8_t *src);
uint32_t NAND_VerifyPage(uint32_t block, uint32_t page, uint8_t *src, uint8_t* dest);

// Copy Application code from NAND to RAM (found in nandboot.c)
uint32_t NAND_Copy();

// Used to write NAND UBL or APP header and data to NAND
uint32_t NAND_WriteHeaderAndData(NAND_BOOT *nandBoot,uint8_t *srcBuf);

// Used to erase an entire NAND block
uint32_t NAND_EraseBlocks(uint32_t startBlkNum, uint32_t blkCount);

// Unprotect blocks encompassing specfied addresses */
uint32_t NAND_UnProtectBlocks(uint32_t startBlkNum,uint32_t endBlkNum);

// Protect blocks
void NAND_ProtectBlocks(void);

// Wait for ready signal seen at NANDFSCR
uint32_t NAND_WaitForRdy(uint32_t timeout);

// Wait for status result from device to read good */
uint32_t NAND_WaitForStatus(uint32_t timeout);

// Read ECC data and restart the ECC calculation
uint32_t NAND_ECCReadAndRestart (PNAND_INFO pNandInfo);

#endif //_NAND_H_
