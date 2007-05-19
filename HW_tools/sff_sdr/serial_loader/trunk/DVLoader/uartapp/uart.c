/* -------------------------------------------------------------------------- *
 * uart.c - Implementation of uart setup and use.                             *
 * -------------------------------------------------------------------------- */

#include "dm644x.h"
#include "uart.h"

// Send specified number of bytes 
unsigned int UARTSendData(unsigned char* seq)
{
	unsigned int i, numBytes, status = 0;
	unsigned int timerStatus = 1;
	
	numBytes = GetStringLen(seq);
	
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

unsigned int UARTSendInt(unsigned int value)
{
	char seq[9];
	unsigned int i,shift,temp;

	for( i = 0; i < 8; i++)
	{
		shift = ((7-i)*4);
		temp = ((value>>shift) & (0x0000000F));
		if (temp > 9)
		{
			temp = temp + 55;
		}
		else
		{
			temp = temp + 48;
		}
		seq[i] = temp;	
	}
	seq[8] = 0;
	return UARTSendData(seq);
}

// Get string length by finding null terminating char
unsigned int GetStringLen(unsigned char* seq)
{
	unsigned int i = 0;
	while ((seq[i] != 0) && (i<MAXSTRLEN)){ i++;}
	if (i == MAXSTRLEN)
		return 0;
	else
		return i;
}

// Receive data from UART 
unsigned int UARTRecvData(unsigned int numBytes, unsigned char* seq)
{
	unsigned int i, status = 0;
	unsigned int timerStatus = 1;
	
	for(i=0;i<numBytes;i++) {
		/* Enable Timer one time */
		TIMER0Start();
		do{
			status = (UART0->LSR)&(0x01);
			timerStatus = TIMER0Status();
		} while (!status && timerStatus);

		if(timerStatus == 0)
			return E_TIMEOUT;
		
		// Recv byte 
		seq[i] = UART0->RBR;

		// Echo byte back 
		UART0->THR = seq[i];
		do{
			status = (UART0->LSR)&(0x20);
		} while (!status);
	}
	return E_PASS;
}

