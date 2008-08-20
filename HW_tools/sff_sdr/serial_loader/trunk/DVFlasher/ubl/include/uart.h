/* --------------------------------------------------------------------------
    FILE        : uart.h
    PURPOSE     : UART header file
    PROJECT     : DaVinci User Boot-Loader and Flasher
    AUTHOR      : Daniel Allred
    DATE	    : Jan-22-2007  
 
    HISTORY
 	    v1.00 completion
 	        Daniel Allred - Jan-22-2007
 ----------------------------------------------------------------------------- */


#ifndef _UART_H_
#define _UART_H_

#include <stdint.h> 

#define MAXSTRLEN 256

typedef struct _UART_ACK_HEADER{
    uint32_t      magicNum;
    uint32_t      appStartAddr;
    uint32_t      srecByteCnt;
    uint32_t      srecAddr;
    uint32_t      binByteCnt;
    uint32_t      binAddr;
} UART_ACK_HEADER;

// ------ Function prototypes ------ 
// Main boot function 
void UART_Boot(void);

// Simple send/recv functions
uint32_t UARTSendData(uint8_t *seq, Bool includeNull);
uint32_t UARTSendInt(uint32_t value);
int32_t GetStringLen(uint8_t *seq);
uint32_t UARTRecvData(uint32_t numBytes, uint8_t *seq);

// Complex send/recv functions
uint32_t UARTCheckSequence(uint8_t *seq, Bool includeNull);
uint32_t UARTGetHexData(uint32_t numBytes, uint32_t* data);
uint32_t UARTGetCMD(uint32_t* bootCmd);
uint32_t UARTGetHeaderAndData(UART_ACK_HEADER* ackHeader);

#endif // End _UART_H_
