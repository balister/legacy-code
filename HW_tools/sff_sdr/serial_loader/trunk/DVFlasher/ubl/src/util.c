/* --------------------------------------------------------------------------
    FILE        : util.c 				                             	 	        
    PURPOSE     : Utility and misc. file
    PROJECT     : DaVinci User Boot-Loader and Flasher
    AUTHOR      : Daniel Allred
    DATE	    : Jan-22-2007  
 
    HISTORY
 	     v1.00 completion 							 						      
 	          Daniel Allred - Jan-22-2007                                              
 ----------------------------------------------------------------------------- */

#include <stdint.h>

#include "ubl.h"
#include "dm644x.h"
#include "uart.h"
#include "util.h"

// Memory allocation stuff
static volatile uint32_t current_mem_loc;
volatile uint8_t DDRMem[0] __attribute__((section (".ddrram")));;

// DDR Memory allocation routines (for storing large data)
uint32_t get_current_mem_loc()
{
	return current_mem_loc;
}

void set_current_mem_loc(uint32_t value)
{
	current_mem_loc = value;
}

void *ubl_alloc_mem (uint32_t size)
{
	void *cPtr;
	uint32_t size_temp;

	// Ensure word boundaries
	size_temp = ((size + 4) >> 2 ) << 2;
	
	if((current_mem_loc + size_temp) > (MAX_IMAGE_SIZE))
	{
		return 0;
	}

	cPtr = (void *) (DDRMem + current_mem_loc);
	current_mem_loc += size_temp;

	return cPtr;
}


// S-record Decode stuff
uint32_t GetHexData(uint8_t *src, uint32_t numBytes, uint8_t* seq)
{
	uint32_t i;
	uint8_t temp[2];
	uint32_t checksum = 0;

	for(i=0;i<numBytes;i++)
	{
		/* Converting ascii to Hex*/
		temp[0] = *src++ - 48;
		temp[1] = *src++ - 48;
		if(temp[0] > 22)/* To support lower case a,b,c,d,e,f*/
		   temp[0] = temp[0]-39;
		else if(temp[0]>9)/* To support Upper case A,B,C,D,E,F*/
		   temp[0] = temp[0]-7;
		if(temp[1] > 22)/* To support lower case a,b,c,d,e,f*/
		   temp[1] = temp[1]-39;
		else if(temp[1]>9)/* To support Upper case A,B,C,D,E,F*/
		   temp[1] = temp[1]-7;
	
		seq[i] = (temp[0] << 4) | temp[1];
		checksum += seq[i];

	}
	return checksum;
}

uint32_t GetHexAddr(uint8_t *src, uint32_t* addr)
{
	uint32_t checksum;
	checksum = GetHexData(src,4,(uint8_t *)addr);
	*addr = ENDIAN_SWAP(*addr);
	return checksum;
}

uint32_t SRecDecode(uint8_t *srecAddr, uint32_t srecByteCnt, uint32_t *binAddr, uint32_t *binByteCnt)
{
	volatile uint32_t		index;
	uint8_t		cnt;
	uint32_t		dstAddr;
	uint32_t		totalCnt;
	uint32_t		checksum;

	totalCnt = 0;
	index = 0;

	// Look for S0 (starting/title) record
	if((srecAddr[index] != 'S')||(srecAddr[index+1] != '0'))
	{
		return E_FAIL;
	}
	
	index += 2;
	// Read the length of S0 record & Ignore that many bytes
	GetHexData (&srecAddr[index], 1, &cnt);
	index += (cnt * 2) + 2;     // Ignore the count and checksum

	while(index <= srecByteCnt)
	{
	    // Check for S3 (data) record
		if(( srecAddr[index] == 'S' )&&( srecAddr[index+1] == '3'))
		{
			index += 2;
		
			checksum = GetHexData (srecAddr+index, 1, &cnt);
			index += 2;

			checksum += GetHexAddr(srecAddr+index, &dstAddr);
			index += 8;
			
			// Eliminate the Address 4 Bytes and checksum
			cnt -=5;	

			// Read cnt bytes to the to the determined destination address
			checksum += GetHexData (srecAddr+index, cnt, (uint8_t *)dstAddr);
			index += (cnt << 1);
			dstAddr += cnt;
			totalCnt += cnt;

			/* Skip Checksum */
			GetHexData (srecAddr+index, 1, &cnt);
			index += 2;
			if ( cnt != (uint8_t)((~checksum) & 0xFF) )
			{
				UARTSendStringCRLF("S-record decode checksum failure.");
				return E_FAIL;
			}
		}
		
		// Check for S7 (terminating) record
		if ( ( srecAddr[index] == 'S' ) && ( srecAddr[index + 1] == '7' ) )
		{
			index += 2;

			GetHexData (&srecAddr[index], 1, (uint8_t *)&cnt);
			index += 2;

			if (cnt == 5)
			{
				GetHexAddr (&srecAddr[index], binAddr);
				index +=8;

				// Skip over the checksum
				index += 2;
				// We are at the end of the S-record, so we can exit while loop
				break;
			}
			else 
			{
				return E_FAIL;
			}
		}

		// Ignore all spaces and returns
		if( ( srecAddr[index] == ' '  ) ||
			( srecAddr[index] == '\n' ) ||
			( srecAddr[index] == ','  ) ||
			( srecAddr[index] == '\r' ) )
		{
			/**** Do nothing ****/
			index ++;
		}

	}	//end while
	*binByteCnt = totalCnt;
	return E_PASS;
}


// Simple wait loop - comes in handy.
void waitloop(int32_t loopcnt)
{
	for (; loopcnt > 0; loopcnt--)
	{
		asm("   NOP");
	}
}
