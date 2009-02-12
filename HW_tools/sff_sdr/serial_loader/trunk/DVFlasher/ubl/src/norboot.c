/* --------------------------------------------------------------------------
    FILE        : norboot.c 				                             	 	        
    PURPOSE     : NOR user boot loader file
    PROJECT     : DaVinci User Boot-Loader and Flasher
    AUTHOR      : Daniel Allred
    DATE	    : Jan-22-2007  
 
    HISTORY
 	     v1.00 completion 							 						      
 	          Daniel Allred - Jan-22-2007                                              
 ----------------------------------------------------------------------------- */

#include "ubl.h"
#include "nor.h"
#include "util.h"
#include "uart.h"

extern uint32_t gEntryPoint;
extern NOR_INFO gNorInfo;

/* Function to find out where the Application is and copy to DRAM */
uint32_t NOR_Copy() {
	volatile NOR_BOOT *hdr = 0;
	volatile uint32_t *appStartAddr = 0;
	volatile uint32_t count = 0;
	volatile uint32_t *ramPtr = 0;
	uint32_t blkSize, blkAddress;

	UARTSendStringCRLF("Starting NOR Copy...");
	
	// Nor Initialization
	if (NOR_Init() != E_PASS)
	    return E_FAIL;
	    
	DiscoverBlockInfo( (gNorInfo.flashBase + UBL_IMAGE_SIZE), &blkSize, &blkAddress );
	
	hdr = (volatile NOR_BOOT *) (blkAddress + blkSize);

	/* Magic number found */
	if((hdr->magicNum & 0xFFFFFF00) != MAGIC_NUMBER_VALID)
	{
	 	return E_FAIL;/* Magic number not found */
	}

	/* Set the Start Address */
	appStartAddr = (uint32_t *)(((uint8_t*)hdr) + sizeof(NOR_BOOT));

	if(hdr->magicNum == UBL_MAGIC_BIN_IMG)
	{
		ramPtr = (uint32_t *) hdr->ldAddress;

		/* Copy data to RAM */
		for(count = 0; count < ((hdr->appSize + 3)/4); count ++)
		{
			ramPtr[count] = appStartAddr[count];
		}
		gEntryPoint = hdr->entryPoint;
		/* Since our entry point is set, just return success */
		return E_PASS;
	}

	if(SRecDecode((uint8_t *)appStartAddr, hdr->appSize, (uint32_t *)&gEntryPoint, (uint32_t *)&count ) != E_PASS)
	{
		return E_FAIL;
	}
 	return E_PASS;
}
