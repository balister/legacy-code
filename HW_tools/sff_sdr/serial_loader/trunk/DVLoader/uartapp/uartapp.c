/****************************************************************
 *  TI DVEVM Serial UART Application
 *  (C) 2006, Texas Instruments, Inc.                           
 *                                                              
 * Author:  Daniel Allred (d-allred@ti.com)                     
 * History: 11/13/2006 - Initial release
 *                                                              
 ****************************************************************/

#include "dm644x.h"
#include "uartapp.h"
#include "uart.h"

// Make DDRMem a pointer into DDR RAM with attribute
volatile unsigned int DDRMem[0] __attribute__((section(".ddrram")));     
     
unsigned int DDRTest()
{
     unsigned int pattLen = 7;
     unsigned int pattern[7] = {
                                 0xAAAA5555,
                                 0xCCCC3333,
                                 0x5555AAAA,
                                 0xC33C3CC3,
                                 0xA55A5AA5,
                                 0x96696996,
                                 0x99669966 };
     unsigned int numMemLocations = DDR_RAM_SIZE;
     
	unsigned int i = 0, j=0;
	unsigned int testValue;
	
	UARTSendData("\r\nFilling RAM");
     for (i = 0; i < (numMemLocations>>2); i++)
     {
          DDRMem[i] = pattern[j];
          if ( ( (i+1) & 0x3FFFFF ) == 0)
          {
               UARTSendData("\r\n\t...through 0x");
               UARTSendInt( ((i+1)<<2) + ((unsigned int) DDRMem));
          }
		if((++j)==pattLen) j = 0;
     }
	
     j=0;
     UARTSendData("\r\n\r\rChecking RAM");
     for (i = 0; i < (numMemLocations>>2); i++)
     {
          if ( ( (i+1) & 0x3FFFFF ) == 0)
          {
               UARTSendData("\r\n\t...through 0x");
               UARTSendInt( ((i+1)<<2) + ((unsigned int) DDRMem));
          }
          testValue = DDRMem[i];
          if (testValue != pattern[j])
          {    
               UARTSendData("\r\nAt Address: 0x");
               UARTSendInt((unsigned int)(DDRMem)+(i<<2));
               UARTSendData(", expected: 0x");
               UARTSendInt(pattern[j]);
               UARTSendData(", but received: 0x");
               UARTSendInt(testValue);
               return E_FAIL;
          }
          if((++j)==pattLen) j = 0;
     }
     return E_PASS;
}

void aemif_start()
{
	asm (" NOP");	//needed to prevent compiler optimizing away 
}

int uartapp_main()
{
	unsigned char inData[16];
	
	PlatformInit();
	
	while (UARTRecvData(1,inData) != E_PASS)
	{
		UARTSendData("\r\nPress any key to start!");
	}
				
	UARTSendData("\r\n");
	UARTSendData("\r\n");
	UARTSendData("\r\n");
	UARTSendData("\r\nApplication Menu:\r\n");
	UARTSendData("\t1) Print Hello World!.\r\n");
	UARTSendData("\t2) Initialize and test DDR RAM.\r\n");	
	UARTSendData("\t3) Boot out of NOR flash.\r\n");
	
	UARTSendData("\n\tEnter selection followed by <Enter>: ");
	while (UARTRecvData(2,inData) != E_PASS);
	
	switch (inData[0]-0x30)
	{
		case (1):
               UARTSendData("\r\nHello World!\r\n");
               break;
		case (2):
               if (DDRTest() == E_PASS)
	               UARTSendData("\r\nDDR Test Passed");
               else
	               UARTSendData("\r\nDDR Test Failed");
               break;
          case (3):
               aemif_start();
               break;
          default:
               UARTSendData("\r\nInvalid Selection!\r\n");
               break;
     }
     return 0;
}
