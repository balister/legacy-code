/* --------------------------------------------------------------------------
 * FILE        : nand.c
 * PURPOSE     : NAND driver file
 * PROJECT     : DaVinci User Boot-Loader and Flasher
 * AUTHOR      : Daniel Allred
 * DATE	       : Jan-22-2007
 *
 * HISTORY
 *     v1.0 completion
 *          Daniel Allred - Jan-22-2007
 *     v1.11 DJA - Mar-07-2007
 *          Fixed bug(s) for writing and reading Big BLock (2K) NAND devices.
 *     v1.12 DJA - Mar-14-2007
 *          Fixed bug for writing 256/512 block devices caused by v1.11 update
 *          (Thanks to Ivan Tonchev)
 * -------------------------------------------------------------------------- */

#include "ubl.h"
#include "davinci.h"
#include "uart.h"
#include "util.h"
#include "nand.h"

/* Define this to have more verbose debug messages */
#undef NAND_DEBUG

static uint8_t gNandTx[MAX_PAGE_SIZE] __attribute__((section(".ddrram")));
static uint8_t gNandRx[MAX_PAGE_SIZE] __attribute__((section(".ddrram")));

/* Symbol from linker script */
extern uint32_t __NANDFlash;

/* structure for holding details about the NAND device itself */
volatile NAND_INFO gNandInfo;

/* Table of ROM supported NAND devices */
static const NAND_DEVICE_INFO gNandDevInfo[] = {
	/* devID, numBlocks, pagesPerBlock, bytesPerPage */
	{0x6E,   256, 16, 256+8},   /*   1 MB */
	{0x68,   256, 16, 256+8},   /*   1 MB */
	{0xEC,   256, 16, 256+8},   /*   1 MB */
	{0xE8,   256, 16, 256+8},   /*   1 MB */
	{0xEA,   512, 16, 256+8},   /*   2 MB */
	{0xE3,   512, 16, 512+16},  /*   4 MB */
	{0xE5,   512, 16, 512+16},  /*   4 MB */
	{0xE6,  1024, 16, 512+16},  /*   8 MB */

	{0x39,  1024, 16, 512+16},  /*   8 MB */
	{0x6B,  1024, 16, 512+16},  /*   8 MB */
	{0x73,  1024, 32, 512+16},  /*  16 MB */
	{0x33,  1024, 32, 512+16},  /*  16 MB */
	{0x75,  2048, 32, 512+16},  /*  32 MB */
	{0x35,  2048, 32, 512+16},  /*  32 MB */

	{0x43,  1024, 32, 512+16},  /*  16 MB 0x1243 */
	{0x45,  2048, 32, 512+16},  /*  32 MB 0x1245 */
	{0x53,  1024, 32, 512+16},  /*  16 MB 0x1253 */
	{0x55,  2048, 32, 512+16},  /*  32 MB 0x1255 */
	{0x36,  4096, 32, 512+16},  /*  64 MB */
	{0x46,  4096, 32, 512+16},  /*  64 MB 0x1346 */
	{0x56,  4096, 32, 512+16},  /*  64 MB 0x1356 */

	{0x76,  4096, 32, 512+16},  /*  64 MB */

	{0x74,  8192, 32, 512+16},  /* 128 MB 0x1374 */
	{0x79,  8192, 32, 512+16},  /* 128 MB */
	{0x71, 16384, 32, 512+16},  /* 256 MB */
	{0xF1,  1024, 64, 2048+64}, /* 128 MB - Big Block */
	{0xA1,  1024, 64, 2048+64}, /* 128 MB - Big Block */
	{0xAA,  2048, 64, 2048+64}, /* 256 MB - Big Block */
	{0xDA,  2048, 64, 2048+64}, /* 256 MB - Big Block */
	{0xDC,  4096, 64, 2048+64}, /* 512 MB - Big Block */
	{0xAC,  4096, 64, 2048+64}, /* 512 MB - Big Block */
	{0xB1,  1024, 64, 2048+64}, /* 128 MB - Big Block 0x22B1 */
	{0xC1,  1024, 64, 2048+64}, /* 128 MB - Big Block 0x22C1 */
	{0xD3,  4096, 64, 2048+64}, // 512 MB - Big Block (4th ID byte will be checked)
	{0x00,	   0,  0,       0}  /* Null entry to indicate end of table */
};

static volatile uint8_t *flash_make_addr(uint32_t baseAddr, uint32_t offset)
{
	return ((volatile uint8_t *) (baseAddr + offset));
}

static void flash_write_data(PNAND_INFO pNandInfo, uint32_t offset, uint32_t data)
{
	volatile FLASHPtr addr;
	FLASHData dataword;
	dataword.l = data;

	addr.cp = flash_make_addr(pNandInfo->flashBase, offset);
	switch (pNandInfo->busWidth) {
	case BUS_8BIT:
		*addr.cp = dataword.c;
		break;
	case BUS_16BIT:
		*addr.wp = dataword.w;
		break;
	}
}

static void flash_write_cmd(PNAND_INFO pNandInfo, uint32_t cmd)
{
	flash_write_data(pNandInfo, NAND_CLE_OFFSET, cmd);
}

static void flash_write_addr(PNAND_INFO pNandInfo, uint32_t addr)
{
	flash_write_data(pNandInfo, NAND_ALE_OFFSET, addr);
}

static void flash_write_bytes(PNAND_INFO pNandInfo, void *pSrc, uint32_t numBytes)
{
	volatile FLASHPtr destAddr, srcAddr;
	uint32_t i;

	srcAddr.cp = (volatile uint8_t*) pSrc;
	destAddr.cp = flash_make_addr(pNandInfo->flashBase, NAND_DATA_OFFSET);
	switch (pNandInfo->busWidth) {
	case BUS_8BIT:
		for (i = 0; i < numBytes; i++)
			*destAddr.cp = *srcAddr.cp++;
		break;
	case BUS_16BIT:
		for (i = 0; i < (numBytes >> 1); i++)
			*destAddr.wp = *srcAddr.wp++;
		break;
	}
}

static void flash_write_addr_bytes(PNAND_INFO pNandInfo, uint32_t numAddrBytes,
			    uint32_t addr)
{
	uint32_t i;
	for (i = 0; i < numAddrBytes; i++)
		flash_write_addr(pNandInfo, ((addr >> (8*i)) & 0xff));
}

static void flash_write_row_addr_bytes(PNAND_INFO pNandInfo, uint32_t block,
				uint32_t page)
{
	uint32_t row_addr;
	row_addr =
		(block << (pNandInfo->blkShift - pNandInfo->pageShift)) | page;
	flash_write_addr_bytes(pNandInfo, pNandInfo->numRowAddrBytes, row_addr);
}

static void flash_write_addr_cycles(PNAND_INFO pNandInfo, uint32_t block,
			     uint32_t page)
{
	flash_write_addr_bytes(pNandInfo, pNandInfo->numColAddrBytes,
			       0x00000000);
	flash_write_row_addr_bytes(pNandInfo, block, page);
}

static uint32_t flash_read_data(PNAND_INFO pNandInfo)
{
	volatile FLASHPtr addr;
	FLASHData cmdword;
	cmdword.l = 0x0;

	addr.cp = flash_make_addr(pNandInfo->flashBase, NAND_DATA_OFFSET);
	switch (gNandInfo.busWidth) {
	case BUS_8BIT:
		cmdword.c = *addr.cp;
		break;
	case BUS_16BIT:
		cmdword.w = *addr.wp;
		break;
	}
	return cmdword.l;
}

static void flash_read_bytes(PNAND_INFO pNandInfo, void *pDest, uint32_t numBytes)
{
	volatile FLASHPtr destAddr, srcAddr;
	uint32_t i;

	destAddr.cp = (volatile uint8_t*) pDest;
	srcAddr.cp = flash_make_addr(pNandInfo->flashBase, NAND_DATA_OFFSET);
	switch (pNandInfo->busWidth) {
	case BUS_8BIT:
		for (i = 0; i < numBytes; i++)
			*destAddr.cp++ = *srcAddr.cp;
		break;
	case BUS_16BIT:
		for (i = 0; i < (numBytes >> 1); i++)
			*destAddr.wp++ = *srcAddr.wp;
		break;
	}
}

static void flash_swap_data(PNAND_INFO pNandInfo, uint32_t *data)
{
	uint32_t i, temp = *data;
	volatile FLASHPtr dataAddr, tempAddr;

	dataAddr.cp = flash_make_addr((uint32_t) data, 3);
	tempAddr.cp = flash_make_addr((uint32_t) &temp, 0);

	switch (gNandInfo.busWidth) {
	case BUS_8BIT:
		for (i = 0; i < 4; i++)
			*dataAddr.cp-- = *tempAddr.cp++;
		break;
	case BUS_16BIT:
		for (i = 0; i < 2; i++)
			*dataAddr.wp-- = *tempAddr.wp++;
		break;
	}
}

/* Poll bit of NANDFSR to indicate ready */
static uint32_t NAND_WaitForRdy(uint32_t timeout)
{
	volatile uint32_t cnt = timeout;
	uint32_t ready;

	waitloop(200);

	do {
		ready = AEMIF->NANDFSR & NAND_NANDFSR_READY;
		cnt--;
	} while ((cnt > 0) && !ready);

	if (cnt == 0) {
		UARTSendString("NANDWaitForRdy() Timeout!\n");
		return E_FAIL;
	}

#ifdef NAND_DEBUG
	UARTSendString("NANDWaitForRdy()Remaining time = ");
	UARTSendInt(cnt);
	UARTSendCRLF();
#endif

	return E_PASS;
}

/* Wait for the status to be ready in NAND register
 * There were some problems reported in DM320 with Ready/Busy pin
 * not working with all NANDs. So this check has also been added.
 */
static uint32_t NAND_WaitForStatus(uint32_t timeout)
{
	volatile uint32_t cnt;
	uint32_t status;
	cnt = timeout;

	do {
		flash_write_cmd((PNAND_INFO)&gNandInfo, NAND_STATUS);
		status = flash_read_data((PNAND_INFO)&gNandInfo) &
			(NAND_STATUS_ERROR | NAND_STATUS_BUSY);
		cnt--;
	}
	while ((cnt > 0) && !status);

	if (cnt == 0) {
		UARTSendString("NANDWaitForStatus() Timeout!\n");
		return E_FAIL;
	}

	return E_PASS;
}

/* Read the current ECC calculation and restart process */
static uint32_t NAND_ECCReadAndRestart(PNAND_INFO pNandInfo)
{
	uint32_t retval;
	/* Read and mask appropriate (based on CSn space flash is in)
	 * ECC register */
	retval = ((uint32_t *)(&(AEMIF->NANDF1ECC)))[pNandInfo->CSOffset] &
		pNandInfo->ECCMask;

	waitloop(5);

#ifdef NAND_DEBUG
	UARTSendString("Value read from ECC register = ");
	UARTSendInt(retval);
	UARTSendCRLF();
	UARTSendString(".");
#endif

	/* Write appropriate bit to start ECC calculations */
	AEMIF->NANDFCR |= (1<<(8 + (pNandInfo->CSOffset)));
	return retval;
}

/* Get details of the NAND flash used from the id and the table of NAND
 * devices. */
static uint32_t NAND_GetDetails()
{
	uint32_t deviceID, i, j;

	/* Issue device read ID command. */
	flash_write_cmd((PNAND_INFO)&gNandInfo, NAND_RDID);
	flash_write_addr((PNAND_INFO)&gNandInfo, NAND_RDIDADD);

	/* Read ID bytes */
	j        = flash_read_data((PNAND_INFO)&gNandInfo) & 0xFF;
	deviceID = flash_read_data((PNAND_INFO)&gNandInfo) & 0xFF;
	j        = flash_read_data((PNAND_INFO)&gNandInfo) & 0xFF;
	j        = flash_read_data((PNAND_INFO)&gNandInfo) & 0xFF;

	UARTSendString("  Device ID = ");
	UARTSendInt(deviceID);
	UARTSendCRLF();

	if (gNandInfo.busWidth == BUS_16BIT)
		UARTSendString("  Bus width = 16 bits\r\n");
	else
		UARTSendString("  Bus width = 8 bits\r\n");

	i = 0;
	while (gNandDevInfo[i].devID != 0x00) {
		if (deviceID == gNandDevInfo[i].devID) {
			gNandInfo.devID = (uint8_t) gNandDevInfo[i].devID;
			gNandInfo.pagesPerBlock = gNandDevInfo[i].pagesPerBlock;
			gNandInfo.numBlocks = gNandDevInfo[i].numBlocks;
			gNandInfo.bytesPerPage =  NANDFLASH_PAGESIZE(
				gNandDevInfo[i].bytesPerPage);

			UARTSendString("  Bytes per page = ");
			UARTSendInt(gNandInfo.bytesPerPage);
			UARTSendCRLF();

			gNandInfo.spareBytesPerPage =
				gNandDevInfo[i].bytesPerPage -
				gNandInfo.bytesPerPage;

			/* Assign the big_block flag */
			gNandInfo.bigBlock =
				(gNandInfo.bytesPerPage == 2048)?TRUE:FALSE;

			/* Setup address shift values */
			j = 0;
			while ((gNandInfo.pagesPerBlock >> j) > 1)
				j++;

			gNandInfo.blkShift = j;
			gNandInfo.pageShift = (gNandInfo.bigBlock)?16:8;

#ifdef NAND_DEBUG
			UARTSendString("  Page shift = ");
			UARTSendInt(gNandInfo.pageShift);
			UARTSendCRLF();
#endif

			gNandInfo.blkShift += gNandInfo.pageShift;

#ifdef NAND_DEBUG
			UARTSendString("  Block shift = ");
			UARTSendInt(gNandInfo.blkShift);
			UARTSendCRLF();
#endif

			/* Set number of column address bytes needed */
			gNandInfo.numColAddrBytes = gNandInfo.pageShift >> 3;

#ifdef NAND_DEBUG
			UARTSendString("  Number of column address bytes = ");
			UARTSendInt(gNandInfo.numColAddrBytes);
			UARTSendCRLF();
#endif

			j = 0;
			while ((gNandInfo.numBlocks >> j) > 1)
				j++;

			/* Set number of row address bytes needed */
			if ((gNandInfo.blkShift + j) <= 24)
				gNandInfo.numRowAddrBytes = 3 -
					gNandInfo.numColAddrBytes;
			else if ((gNandInfo.blkShift + j) <= 32)
				gNandInfo.numRowAddrBytes = 4 -
					gNandInfo.numColAddrBytes;
			else
				gNandInfo.numRowAddrBytes = 5 -
					gNandInfo.numColAddrBytes;

#ifdef NAND_DEBUG
			UARTSendString("  Number of row address bytes = ");
			UARTSendInt(gNandInfo.numRowAddrBytes);
			UARTSendCRLF();
#endif

			/* Set the ECC bit mask */
			if (gNandInfo.bytesPerPage < 512)
				gNandInfo.ECCMask = 0x07FF07FF;
			else
				gNandInfo.ECCMask = 0x0FFF0FFF;

#ifdef NAND_DEBUG
			UARTSendString("  ECCMask = ");
			UARTSendInt(gNandInfo.ECCMask);
			UARTSendCRLF();
#endif

			return E_PASS;
		}
		i++;
	}
	/* No match was found for the device ID */
	return E_FAIL;
}

/* Initialize NAND interface and find the details of the NAND used */
uint32_t NAND_Init(void)
{
	uint32_t width;
	uint32_t *CSRegs;

	UARTSendString("Initializing NAND flash:\r\n");

#ifdef NAND_BYPASS_READ_PAGE_ECC_CHECK
	UARTSendString("  Bypassing ECC check on page reads.\r\n");
#endif /* NAND_BYPASS_READ_PAGE_ECC_CHECK */

	/* Set NAND flash base address */
	gNandInfo.flashBase = (uint32_t) &(__NANDFlash);

	/* Get the CSOffset (can be 0 through 3 - corresponds with CS2 through
	 * CS5) */
	gNandInfo.CSOffset = (gNandInfo.flashBase >> 25) - 1;

	/* Setting the nand_width = 0(8 bit NAND) or 1(16 bit NAND). AEMIF CS2
	 *  bus Width is given by the BOOTCFG(bit no.5). */
	width = (((SYSTEM->BOOTCFG) & 0x20) >> 5);
	gNandInfo.busWidth = (width)?BUS_16BIT:BUS_8BIT;

	/* Setup AEMIF registers for NAND     */
	CSRegs = (uint32_t *) &(AEMIF->A1CR);

	/* Set correct AxCR reg */
	CSRegs[gNandInfo.CSOffset] = 0x3FFFFFFC | width;

	/* NAND enable for CSx. */
	AEMIF->NANDFCR |= (0x1 << (gNandInfo.CSOffset));
	NAND_ECCReadAndRestart((PNAND_INFO)&gNandInfo);

	/* Send reset command to NAND */
	flash_write_cmd((PNAND_INFO)&gNandInfo, NAND_RESET);

	if (NAND_WaitForRdy(NAND_TIMEOUT) != E_PASS)
		return E_FAIL;

	return NAND_GetDetails();
}

/* Routine to read a page from NAND */
uint32_t NAND_ReadPage(uint32_t block, uint32_t page, uint8_t *dest)
{
#ifndef NAND_BYPASS_READ_PAGE_ECC_CHECK
	uint32_t eccValue[4];
	uint32_t spareValue[4], tempSpareValue;
#endif
	uint8_t numReads, i;

	/* Setup numReads */
	numReads = (gNandInfo.bytesPerPage >> 9);
	if (numReads == 0)
		numReads++;

	/* Write read command */
	flash_write_cmd((PNAND_INFO)&gNandInfo, NAND_LO_PAGE);

	/* Write address bytes */
	flash_write_addr_cycles((PNAND_INFO)&gNandInfo, block, page);

	/* Additional confirm command for big_block devices */
	if (gNandInfo.bigBlock)
		flash_write_cmd((PNAND_INFO)&gNandInfo, NAND_READ_30H);

	/* Wait for data to be available */
	if (NAND_WaitForRdy(NAND_TIMEOUT) != E_PASS)
		return E_FAIL;

#ifndef NAND_BYPASS_READ_PAGE_ECC_CHECK
	/* Starting the ECC in the NANDFCR register for CS2(bit no.8) */
	NAND_ECCReadAndRestart((PNAND_INFO)&gNandInfo);
#endif

	/* Read the page data */
	for (i = 0; i < numReads; i++) {
		/* Actually read bytes */
		if (gNandInfo.bigBlock) {
			flash_read_bytes((PNAND_INFO)&gNandInfo, (void *)(dest),
					 512);
		} else {
			flash_read_bytes((PNAND_INFO)&gNandInfo, (void *)(dest),
					 gNandInfo.bytesPerPage);
		}

#ifndef NAND_BYPASS_READ_PAGE_ECC_CHECK
		/* Get the ECC Value */
		eccValue[i] = NAND_ECCReadAndRestart((PNAND_INFO)&gNandInfo);
#endif

		/* Increment pointer */
		if (gNandInfo.bigBlock)
			dest += 512;
		else
			dest += gNandInfo.bytesPerPage;
	}

#ifndef NAND_BYPASS_READ_PAGE_ECC_CHECK
	/* Read the stored ECC value(s) */
	for (i = 0; i < numReads; i++) {
		if (gNandInfo.bytesPerPage == 256)
			flash_read_bytes((PNAND_INFO)&gNandInfo,
					 (void *)(spareValue), 8);
		else
			flash_read_bytes((PNAND_INFO)&gNandInfo,
					 (void *)(spareValue), 16);

		if (gNandInfo.bigBlock) {
			flash_swap_data((PNAND_INFO)&gNandInfo,
					((uint32_t *) (spareValue)+2));
			tempSpareValue = spareValue[2];
		} else {
			flash_swap_data((PNAND_INFO)&gNandInfo,
					(uint32_t *)(spareValue));
			tempSpareValue = spareValue[0];
		}

#ifdef NAND_DEBUG
		UARTSendString("ECCValue = ");
		UARTSendInt(eccValue[i]);
		UARTSendString(", ECC from spare RAM = ");
		UARTSendInt(tempSpareValue);
		UARTSendCRLF();
#endif

		/* Verify ECC values */
		if (eccValue[i] != tempSpareValue) {
			UARTSendString("NAND ECC failure!\r\n");
			UARTSendString("eccValue[i] = ");
			UARTSendInt(eccValue[i]);
			UARTSendCRLF();
			UARTSendString("tempSpareValue = ");
			UARTSendInt(tempSpareValue);
			UARTSendCRLF();
			UARTSendString("i = ");
			UARTSendInt(i);
			UARTSendCRLF();
			return E_FAIL;
		}
	}
#endif /* NAND_BYPASS_READ_PAGE_ECC_CHECK */

#ifdef NAND_DEBUG
	UARTSendString("\r\nDone reading a page from NAND flash\r\n");
#endif

	/* Return status check result */
	return NAND_WaitForStatus(NAND_TIMEOUT);
}

/* Generic routine to write a page of data to NAND */
uint32_t NAND_WritePage(uint32_t block, uint32_t page, uint8_t *src)
{
	uint32_t eccValue[4];
	uint32_t tempSpareValue[4];
	uint8_t numWrites, i;

	/* Setup numReads */
	numWrites = (gNandInfo.bytesPerPage >> 9);
	if (numWrites == 0)
		numWrites++;

	/* Write program command */
	flash_write_cmd((PNAND_INFO)&gNandInfo, NAND_PGRM_START);

	/* Write address bytes */
	flash_write_addr_cycles((PNAND_INFO)&gNandInfo, block, page);

	/* Starting the ECC in the NANDFCR register for CS2(bit no.8) */
	NAND_ECCReadAndRestart((PNAND_INFO)&gNandInfo);

	/* Write data */
	for (i = 0; i < numWrites; i++) {
		/* Write data to page */
		if (gNandInfo.bigBlock)
			flash_write_bytes((PNAND_INFO)&gNandInfo, (void *) src,
					  512);
		else
			flash_write_bytes((PNAND_INFO)&gNandInfo, (void *) src,
					  gNandInfo.bytesPerPage);

		/* Read the ECC value */
		eccValue[i] = NAND_ECCReadAndRestart((PNAND_INFO)&gNandInfo);

		/* Increment the pointer */
		if (gNandInfo.bigBlock)
			src += 512;
		else
			src += gNandInfo.bytesPerPage;
	}

	/* Write spare bytes */
	for (i = 0; i < numWrites; i++) {
		flash_swap_data((PNAND_INFO)&gNandInfo, &(eccValue[i]));

		/* Place the ECC values where the ROM read routine
		 * expects them */
		if (gNandInfo.bigBlock) {
			tempSpareValue[0] = 0xFFFFFFFF;
			tempSpareValue[1] = 0xFFFFFFFF;
			tempSpareValue[2] = eccValue[i];
			tempSpareValue[3] = 0xFFFFFFFF;
		} else {
			tempSpareValue[0] = eccValue[i];
			tempSpareValue[1] = 0xFFFFFFFF;
			tempSpareValue[2] = 0xFFFFFFFF;
			tempSpareValue[3] = 0xFFFFFFFF;
		}
		if (gNandInfo.bytesPerPage == 256)
			flash_write_bytes((PNAND_INFO)&gNandInfo,
					  (void *)(tempSpareValue), 8);
		else
			flash_write_bytes((PNAND_INFO)&gNandInfo,
					  (void *)(tempSpareValue), 16);
	}

	/* Write program end command */
	flash_write_cmd((PNAND_INFO)&gNandInfo, NAND_PGRM_END);

	/* Wait for the device to be ready */
	if (NAND_WaitForRdy(NAND_TIMEOUT) != E_PASS)
		return E_FAIL;

	/* Return status check result */
	return NAND_WaitForStatus(NAND_TIMEOUT);
}

/* Verify data written by reading and comparing byte for byte */
uint32_t NAND_VerifyPage(uint32_t block, uint32_t page, uint8_t *src,
			 uint8_t *dest)
{
	uint32_t i;

#ifdef NAND_DEBUG
	UARTSendString("In NAND_VerifyPage\r\n");
#endif

	if (NAND_ReadPage(block, page, dest) != E_PASS)
		return E_FAIL;

	for (i = 0; i < gNandInfo.bytesPerPage; i++) {

#ifdef NAND_DEBUG
		if (i < 20 && ((i % 4) == 0)) {
			UARTSendString("Data read from flash = ");
			UARTSendInt(*((uint32_t *) &dest[i]));
			UARTSendCRLF();
		}
#endif

		/* Check for data read errors */
		if (src[i] != dest[i]) {
			UARTSendString("NAND verification failed at block ");
			UARTSendInt(block);
			UARTSendString(", page ");
			UARTSendInt(page);
			UARTSendCRLF();
			return E_FAIL;
		}
	}
	return E_PASS;
}

/* NAND Flash erase block function */
uint32_t NAND_EraseBlocks(uint32_t startBlkNum, uint32_t blkCnt)
{
	uint32_t i;

	/* Do bounds checking */
	if ((startBlkNum + blkCnt - 1) >= gNandInfo.numBlocks)
		return E_FAIL;

	/* Output info about what we are doing */
	UARTSendString("Erasing blocks ");
	UARTSendInt(startBlkNum);
	UARTSendString(" through ");
	UARTSendInt(startBlkNum + blkCnt - 1);
	UARTSendString(".\r\n");

	for (i = 0; i < blkCnt; i++) {
		/* Start erase command */
		flash_write_cmd((PNAND_INFO)&gNandInfo, NAND_BERASEC1);

		/* Write the row addr bytes only */
		flash_write_row_addr_bytes((PNAND_INFO)&gNandInfo,
					   (startBlkNum+i), 0);

		/* Confirm erase command */
		flash_write_cmd((PNAND_INFO)&gNandInfo, NAND_BERASEC2);

		/* Wait for the device to be ready */
		if (NAND_WaitForRdy(NAND_TIMEOUT) != E_PASS)
			return E_FAIL;

		/* Verify the op succeeded by reading status from flash */
		if (NAND_WaitForStatus(NAND_TIMEOUT) != E_PASS)
			return E_FAIL;
	}

	return E_PASS;
}

/* NAND Flash unprotect command */
uint32_t NAND_UnProtectBlocks(uint32_t startBlkNum, uint32_t blkCnt)
{
	uint32_t endBlkNum;
	endBlkNum = startBlkNum + blkCnt - 1;

	/* Do bounds checking */
	if (endBlkNum >= gNandInfo.numBlocks)
		return E_FAIL;

	UARTSendString("Unprotecting blocks ");
	UARTSendInt(startBlkNum);
	UARTSendString(" through ");
	UARTSendInt(endBlkNum);
	UARTSendString(".\n");

	flash_write_cmd((PNAND_INFO)&gNandInfo, NAND_UNLOCK_START);
	flash_write_row_addr_bytes((PNAND_INFO)&gNandInfo, startBlkNum, 0);

	flash_write_cmd((PNAND_INFO)&gNandInfo, NAND_UNLOCK_END);
	flash_write_row_addr_bytes((PNAND_INFO)&gNandInfo, endBlkNum, 0);

	return E_PASS;
}

/* NAND Flash protect command */
void NAND_ProtectBlocks(void)
{
	UARTSendString("Protecting the entire NAND flash.\n");
	flash_write_cmd((PNAND_INFO)&gNandInfo, NAND_LOCK);
}


/* Generic function to write a UBL or Application header and the
 * associated data */
uint32_t NAND_WriteHeaderAndData(NAND_BOOT *nandBoot, uint8_t *srcBuf)
{
	uint32_t endBlockNum;
	uint32_t *ptr;
	uint32_t blockNum;
	uint32_t count;
	uint32_t countMask;
	uint32_t numBlks;

	/* Get total number of blocks needed */
	numBlks = 0;
	while ((numBlks * gNandInfo.pagesPerBlock)  < (nandBoot->numPage + 1))
		numBlks++;

	UARTSendString("Number of blocks needed for header and data: ");
	UARTSendInt(numBlks);
	UARTSendCRLF();

	UARTSendString("Pages to write = ");
	UARTSendInt(nandBoot->numPage);
	UARTSendCRLF();

	/* Check whether writing UBL or APP (based on destination block) */
	blockNum = nandBoot->block;
	if (blockNum == START_UBL_BLOCK_NUM)
		endBlockNum = END_UBL_BLOCK_NUM;
	else if (blockNum == START_APP_BLOCK_NUM)
		endBlockNum = END_APP_BLOCK_NUM;
	else
		return E_FAIL; /* Block number is out of range */

NAND_WRITE_RETRY:
	if (blockNum > endBlockNum)
		return E_FAIL;

	UARTSendString("Attempting to start in block number ");
	UARTSendInt(blockNum);
	UARTSendCRLF();

	/* Unprotect all needed blocks of the Flash */
	if (NAND_UnProtectBlocks(blockNum, numBlks) != E_PASS) {
		blockNum++;
		UARTSendString("Unprotect failed\n");
		goto NAND_WRITE_RETRY;
	}

	/* Erase the block where the header goes and the data starts */
	if (NAND_EraseBlocks(blockNum, numBlks) != E_PASS) {
		blockNum++;
		UARTSendString("Erase failed\n");
		goto NAND_WRITE_RETRY;
	}

	/* Setup header to be written */
	ptr = (uint32_t *) gNandTx;
	ptr[0] = nandBoot->magicNum;
	ptr[1] = nandBoot->entryPoint;
	ptr[2] = nandBoot->numPage;
	ptr[3] = blockNum; 	/* Always start data in current block */
	ptr[4] = 1;		/* Always start data in page 1
				 * (this header goes in page 0) */
	ptr[5] = nandBoot->ldAddress;

	/* Write the header to page 0 of the current blockNum */
	UARTSendString("Writing header...\n");
	if (NAND_WritePage(blockNum, 0, gNandTx) != E_PASS)
		return E_FAIL;

	waitloop(200);

	/* Verify the page just written */
	if (NAND_VerifyPage(blockNum, 0, gNandTx, gNandRx) != E_PASS)
		return E_FAIL;

	count = 1;

	/* The following assumes power of 2 page_cnt -  *should* always be
	 * valid. */
	countMask = (uint32_t)gNandInfo.pagesPerBlock - 1;
	UARTSendString("Writing data...\n");
	do {
		/* Write the UBL or APP data on a per page basis */
		if (NAND_WritePage(blockNum,
				   (count & countMask), srcBuf) != E_PASS)
			return E_FAIL;

		waitloop(200);

		/* Verify the page just written */
		if (NAND_VerifyPage(blockNum,
				    (count & countMask), srcBuf,
				    gNandRx) != E_PASS)
			return E_FAIL;

		count++;
		srcBuf += gNandInfo.bytesPerPage;
		if (!(count & countMask))
			blockNum++;
	} while (count <= nandBoot->numPage);

	NAND_ProtectBlocks();

	return E_PASS;
}
