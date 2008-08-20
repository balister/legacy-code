/* --------------------------------------------------------------------------
    FILE        : nandboot.c 				                             	 	        
    PURPOSE     : NAND user boot loader file
    PROJECT     : DaVinci User Boot-Loader and Flasher
    AUTHOR      : Daniel Allred
    DATE	    : Jan-22-2007
 
    HISTORY
 	     v1.00 completion 							 						      
 	          Daniel Allred - Jan-22-2007   
 	     v1.10 
 	          DJA - Feb-1-2007 - Added support for the magic number written by the
 	                             SDI CCS flashing tool.  They always write a the 
 	                             application as a binary image and use the 
 	                             UBL_MAGIC_DMA magic number in the application
 	                             header.
 ----------------------------------------------------------------------------- */

#ifdef UBL_NAND

#include "ubl.h"
#include "nand.h"
#include "uart.h"
#include "util.h"

// Structure with info about the NAND flash device
extern NAND_INFO gNandInfo;

// Entrypoint for application we are decoding out of flash
extern uint32_t gEntryPoint;

// structure for holding details about UBL stored in NAND
volatile NAND_BOOT	gNandBoot;

// Function to find out where the application is and copy to RAM
uint32_t NAND_Copy() {
	uint32_t count,blockNum;
	uint32_t i;
	uint32_t magicNum;
	uint8_t *rxBuf;		// RAM receive buffer
	uint32_t entryPoint2,temp;
	uint32_t block,page;
	uint32_t readError = E_FAIL;
	Bool failedOnceAlready = FALSE;

    // Maximum application size, in S-record form, is 16 MB
	rxBuf = (uint8_t*)ubl_alloc_mem((MAX_IMAGE_SIZE>>1));
	blockNum = START_APP_BLOCK_NUM;

	UARTSendStringCRLF("Starting NAND Copy...");
	
	// NAND Initialization
	if (NAND_Init() != E_PASS)
		return E_FAIL;
    
NAND_startAgain:
	if (blockNum > END_APP_BLOCK_NUM)
		return E_FAIL;  /* NAND boot failed and return fail to main */

	// Read data about Application starting at START_APP_BLOCK_NUM, Page 0
	// and possibly going until block END_APP_BLOCK_NUM, Page 0
	for(count=blockNum; count <= END_APP_BLOCK_NUM; count++)
	{		
		if(NAND_ReadPage(count,0,rxBuf) != E_PASS)
			continue;

		magicNum = ((uint32_t *)rxBuf)[0];

		/* Valid magic number found */
		if((magicNum & 0xFFFFFF00) == MAGIC_NUMBER_VALID)
		{
			UARTSendStringCRLF("Valid MagicNum found.");
			blockNum = count;
			break;
		}

	}

	// Never found valid header in any page 0 of any of searched blocks
	if (count > END_APP_BLOCK_NUM)
	{
		return E_FAIL;
	}

	// Fill in NandBoot header
	gNandBoot.entryPoint = *(((uint32_t *)(&rxBuf[4])));/* The first "long" is entry point for Application */
	gNandBoot.numPage = *(((uint32_t *)(&rxBuf[8])));	 /* The second "long" is the number of pages */
	gNandBoot.block = *(((uint32_t *)(&rxBuf[12])));	 /* The third "long" is the block where Application is stored in NAND */
	gNandBoot.page = *(((uint32_t *)(&rxBuf[16])));	 /* The fourth "long" is the page number where Application is stored in NAND */
	gNandBoot.ldAddress = *(((uint32_t *)(&rxBuf[20])));	 /* The fifth "long" is the Application load address */

	// If the application is already in binary format, then our 
	// received buffer can point to the specified load address
	// instead of the temp location used for storing an S-record
	// Checking for the UBL_MAGIC_DMA guarantees correct usage with the 
	// Spectrum Digital CCS flashing tool, flashwriter_nand.out
	if ((magicNum == UBL_MAGIC_BIN_IMG) || (magicNum == UBL_MAGIC_DMA))
	{
	    // Set the copy location to final run location
		rxBuf = (uint8_t *)gNandBoot.ldAddress;
		// Free temp memory rxBuf used to point to
		set_current_mem_loc(get_current_mem_loc() - (MAX_IMAGE_SIZE>>1));
	}

NAND_retry:
	/* initialize block and page number to be used for read */
	block = gNandBoot.block;
	page = gNandBoot.page;

    // Perform the actual copying of the application from NAND to RAM
	for(i=0;i<gNandBoot.numPage;i++) {
	    // if page goes beyond max number of pages increment block number and reset page number
		if(page >= gNandInfo.pagesPerBlock) {
			page = 0;
			block++;
		}
		readError = NAND_ReadPage(block,page++,(&rxBuf[i*gNandInfo.bytesPerPage]));	/* Copy the data */

		// We attempt to read the app data twice.  If we fail twice then we go look for a new
		// application header in the NAND flash at the next block.
		if(readError != E_PASS) {
			if(failedOnceAlready) {
				blockNum++;
				goto NAND_startAgain;
			}
		    else {
		        failedOnceAlready = TRUE;
				goto NAND_retry;
			}
		}
	}

	// Application was read correctly, so set entrypoint
	gEntryPoint = gNandBoot.entryPoint;

	/* Data is already copied to RAM, just set the entry point */
	if(magicNum == UBL_MAGIC_SAFE)
	{
		// Or do the decode of the S-record 
		if(SRecDecode( (uint8_t *)rxBuf, 
		               gNandBoot.numPage * gNandInfo.bytesPerPage,
					   (uint32_t *) &entryPoint2,
		               (uint32_t *) &temp ) != E_PASS)
		{
		    UARTSendString("S-record decode failure.");
			return E_FAIL;
		}
		
		if (gEntryPoint != entryPoint2)
		{
			UARTSendStringCRLF("WARNING: S-record entrypoint does not match header entrypoint.");
			UARTSendStringCRLF("WARNING: Using header entrypoint - results may be unexpected.");
		}
	}
	
	return E_PASS;
}

#endif
