/* --------------------------------------------------------------------------
    FILE        : uart.c 				                             	 	        
    PURPOSE     : UART driver file
    PROJECT     : DaVinci User Boot-Loader and Flasher
    AUTHOR      : Daniel Allred
    DATE	    : Jan-22-2007
 
    HISTORY
 	     v1.00 completion 							 						      
 	          Daniel Allred - Jan-22-2007                                              
 ----------------------------------------------------------------------------- */
#include "dm644x.h"
#include "uart.h"
#include "util.h"
#include "ubl.h"

extern volatile uint32_t gMagicFlag,gBootCmd;
extern volatile uint32_t gDecodedByteCount,gSrecByteCount;

// Send specified number of bytes 
uint32_t UARTSendData(uint8_t* seq, Bool includeNull)
{
	uint32_t status = 0;
    int32_t i,numBytes;
	uint32_t timerStatus = 1;
	
	numBytes = includeNull?(GetStringLen(seq)+1):(GetStringLen(seq));
	
	for(i=0;i<numBytes;i++) {
		/* Enable Timer one time */
		TIMER0Start();
		do{
			status = (UART0->LSR)&(0x20);
			timerStatus = TIMER0Status();
		} while (!status && timerStatus);

		if(timerStatus == 0)
			return E_TIMEOUT;
		
		// Send byte 
		(UART0->THR) = seq[i];
	}
	return E_PASS;
}

uint32_t UARTSendInt(uint32_t value)
{
	char seq[9];
	uint32_t i,shift,temp;

	for( i = 0; i < 8; i++)
	{
		shift = ((7-i)*4);
		temp = ((value>>shift) & (0x0000000F));
		if (temp > 9)
		{
			temp = temp + 7;
        }
		seq[i] = temp + 48;	
	}
	seq[8] = 0;
	return UARTSendData((uint8_t *)seq, FALSE);
}

// Get string length by finding null terminating char
int32_t GetStringLen(uint8_t* seq)
{
	int32_t i = 0;
	while ((seq[i] != 0) && (i<MAXSTRLEN)){ i++;}
	if (i == MAXSTRLEN)
		return -1;
	else
		return i;
}

// Receive data from UART 
uint32_t UARTRecvData(uint32_t numBytes, uint8_t* seq)
{
	uint32_t i, status = 0;
	uint32_t timerStatus = 1;
	
	for(i=0;i<numBytes;i++) {
		
        // Enable timer one time
		TIMER0Start();
		do{
			status = (UART0->LSR)&(0x01);
			timerStatus = TIMER0Status();
		} while (!status && timerStatus);

		if(timerStatus == 0)
			return E_TIMEOUT;
		
		// Receive byte 
		seq[i] = (UART0->RBR) & 0xFF;
        
        // Check status for errors
        if( ( (UART0->LSR)&(0x1C) ) != 0 )
            return E_FAIL;

	}
	return E_PASS;
}

// More complex send / receive functions
uint32_t UARTCheckSequence(uint8_t* seq, Bool includeNull)
{
    int32_t i, numBytes;
    uint32_t  status = 0,timerStatus = 1;
    
    numBytes = includeNull?(GetStringLen(seq)+1):(GetStringLen(seq));
    
    for(i=0;i<numBytes;i++) {
        /* Enable Timer one time */
        TIMER0Start();
        do{
            status = (UART0->LSR)&(0x01);
            timerStatus = TIMER0Status();
        } while (!status && timerStatus);

        if(timerStatus == 0)
            return E_TIMEOUT;

        if( ( (UART0->RBR)&0xFF) != seq[i] )
            return E_FAIL;
    }
    return E_PASS;
}

uint32_t UARTGetHexData(uint32_t numBytes, uint32_t* data) {
    
    uint32_t i,j;
    uint32_t temp[8];
    uint32_t timerStatus = 1, status = 0;
    uint32_t numLongs, numAsciiChar, shift;
    
    if(numBytes == 2) {
        numLongs = 1;
        numAsciiChar = 4;
        shift = 12;
    } else {
        numLongs = numBytes/4;
        numAsciiChar = 8;
        shift = 28;
    }

    for(i=0;i<numLongs;i++) {
        data[i] = 0;
        for(j=0;j<numAsciiChar;j++) {
            /* Enable Timer one time */
            TIMER0Start();
            do{
                status = (UART0->LSR)&(0x01);
                timerStatus = TIMER0Status();
            } while (!status && timerStatus);

            if(timerStatus == 0)
                return E_TIMEOUT;

            /* Converting ascii to Hex*/
            temp[j] = ((UART0->RBR)&0xFF)-48;
            if(temp[j] > 22)/* To support lower case a,b,c,d,e,f*/
               temp[j] = temp[j]-39;
            else if(temp[j]>9)/* To support Upper case A,B,C,D,E,F*/
               temp[j] = temp[j]-7;

            /* Checking for bit 1,2,3,4 for reception Error *///1E->1C
            if( ( (UART0->LSR)&(0x1C) ) != 0)
                return E_FAIL;

            data[i] |= (temp[j]<<(shift-(j*4)));
        }
    }
    return E_PASS;
}

uint32_t UARTGetCMD(uint32_t* bootCmd)
{
    if(UARTCheckSequence((uint8_t*)"    CMD", TRUE) != E_PASS)
    {
        return E_FAIL;
    }

    if(UARTGetHexData(4,bootCmd) != E_PASS)
    {
        return E_FAIL;
    }

    return E_PASS;
}

uint32_t UARTGetHeaderAndData(UART_ACK_HEADER* ackHeader)
{
    uint32_t error = E_FAIL;

    // Send ACK command
    error = UARTCheckSequence((uint8_t*)"    ACK", TRUE);
    if(error != E_PASS)
    {
        return E_FAIL;
    }

    // Get the ACK header elements
    error =  UARTGetHexData( 4, (uint32_t *) &(ackHeader->magicNum)     );
    error |= UARTGetHexData( 4, (uint32_t *) &(ackHeader->appStartAddr) );
    error |= UARTGetHexData( 4, (uint32_t *) &(ackHeader->srecByteCnt)  );
    error |= UARTCheckSequence((uint8_t*)"0000", FALSE);
    if(error != E_PASS)
    {
        return E_FAIL;
    }

    UARTSendData((uint8_t*) "Magic Number = ",FALSE);
    UARTSendInt(ackHeader->magicNum);
    UARTSendData((uint8_t*) "\r\nApplication start address = ",FALSE);
    UARTSendInt(ackHeader->appStartAddr);
    UARTSendData((uint8_t*) "\r\nSREC Byte Count = ",FALSE);
    UARTSendInt(ackHeader->srecByteCnt);
    UARTSendData((uint8_t*) "\r\n",FALSE);

    // Verify that the S-record's size is appropriate
    if((ackHeader->srecByteCnt == 0) || (ackHeader->srecByteCnt > MAX_IMAGE_SIZE))
    {
        UARTSendData((uint8_t*)" BADCNT", TRUE);/*trailing /0 will come along*/
        return E_FAIL;
    }

    // Verify application start address is in RAM (lower 16bit of appStartAddr also used 
    // to hold UBL entry point if this header describes a UBL)
    if( (ackHeader->appStartAddr < RAM_START_ADDR) || (ackHeader->appStartAddr > RAM_END_ADDR) )
    {
        UARTSendData((uint8_t*)"BADADDR", TRUE);/*trailing /0 will come along*/
        return E_FAIL;
    }

    // Allocate storage for S-record
    ackHeader->srecAddr = (uint32_t) ubl_alloc_mem(ackHeader->srecByteCnt);

    // Send BEGIN command
    if ( UARTSendData((uint8_t*)"  BEGIN", TRUE) != E_PASS )
        return E_FAIL;

    // Receive the data over UART
    if ( UARTRecvData(ackHeader->srecByteCnt, (uint8_t*)(ackHeader->srecAddr)) != E_PASS )
    {
        UARTSendData((uint8_t*)"\r\nUART Receive Error\r\n", FALSE);
        return E_FAIL;
    }

    // Return DONE when all data arrives
    if ( UARTSendData((uint8_t*)"   DONE", TRUE) != E_PASS )
        return E_FAIL;

    // Now decode the S-record
    if ( SRecDecode(    (uint8_t *)(ackHeader->srecAddr),
                        ackHeader->srecByteCnt,
                        &(ackHeader->binAddr),
                        &(ackHeader->binByteCnt) ) != E_PASS )
    {
        UARTSendData((uint8_t*)"\r\nS-record Decode Failed.\r\n", FALSE);
        return E_FAIL;
    }

    if ( UARTSendData((uint8_t*)"   DONE", TRUE) != E_PASS )
        return E_FAIL;


    return E_PASS;
}   
