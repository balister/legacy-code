/* -------------------------------------------------------------------------- *
 * uart.h - header file to define necessary registers for uart setup and use. *
 *    See sprue33.pdf for more details.                                       *
 * -------------------------------------------------------------------------- */

#ifndef _UART_H_
#define _UART_H_

#define MAXSTRLEN 256

// Function prototypes 
unsigned int UARTSendData(unsigned char* seq);
unsigned int UARTSendInt(unsigned int value);
unsigned int GetStringLen(unsigned char* seq);
unsigned int UARTRecvData(unsigned int numBytes, unsigned char* seq);

#endif // End _UART_H_
