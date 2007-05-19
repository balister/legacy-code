****************************************************************
 *  TI DVEVM Serial UART Application
 *  (C) 2006, Texas Instruments, Inc.                           
 *                                                              
 * Author:  Daniel Allred (d-allred@ti.com)                     
 * History: 11/13/2006 - Initial release
 *                                                              
 ****************************************************************/
 
Running
-------

This application is intended to be downloaded to the DM644x during the 
UART boot process. This is done with the assistance of the DVSerial host 
application.
 
 
Compiling
---------

A makefile has been included to compile the application using the 
MontaVista ARM cross-compile tools.  These can be run natively under 
Linux or under Cygwin on the Windows platform.  In either case, run

make

to build the binary uartapp.bin.  The last line output by the makefile 
gives the entry point address that needs to be provided to the DVSerial
application for the UART boot process to succeed.


You can also run 

make clean

to clear out the partial build files and start over.