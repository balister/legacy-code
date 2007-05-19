/* -------------------------------------------------------------------------- *
 * uartapp.h - Header file for main function code                                 *
 * -------------------------------------------------------------------------- */

#ifndef _UARTAPP_H_
#define _UARTAPP_H_

//********************* Function Declarations **********************

// uboot has naked attribute since it's a dummy function, and also it is
// placed in aemif section (defined in linker script ubl.cmd) which is located in
// CS2 AEMIF memory space 
void aemif_start( void ) __attribute__((naked,section(".aemif")));

unsigned int DDRTest(void);

int uartapp_main( void );

#endif
