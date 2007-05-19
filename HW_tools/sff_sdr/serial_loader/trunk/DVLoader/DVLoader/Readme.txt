/****************************************************************
 *  TI DVEVM Serial Application Loader                     
 *  (C) 2006, Texas Instruments, Inc.                           
 *                                                              
 * Author:  Daniel Allred (d-allred@ti.com)                     
 * History: 11/16/2006 - Initial release
 *                                                              
 ****************************************************************/
 
Running
-------

This application can be run from the commandline under Windows with 
the .Net Framework 2.0 or later installed.  

	DVLoader.exe [Hex string of start Address] <Binary App File>

Example:
	DVLoader.exe 0100 uartapp.bin

This example will download uartapp.bin to the DM644x and attempt to 
begin execution at the memory address 0x0100.


It can also be run on a Linux machine with the latest open-source Mono
Framework installed.

	mono DVLoader.exe [Hex string of start Address] <Binary App File>


Note that the start address is optional and if not provided, 0x0100 will
be assumed.
 
 
 
Compiling
---------

A makefile is included for compiling the application if needed.  

Under Windows - 
The currently supported method is to use the Cygwin enviroment (such as
the one provided with the Montavista Linux tools for Windows) and put the 
C sharp compiler's install location in the user's path.  This compiler
comes with the .NET Framework installation and can usually be found in 
C:\WINDOWS\Microsoft.NET\Framework\<version number>.  
	
	make
	-or-
	make dotnet

If you have the Mono framework installed and in the path (where the mcs 
and gmcs executables can be found), you can specify that the mono 
compiler be used instead.
	
	make mono


Under Linux -
WIth the Mono Framework installed

	make
	-or-
	make mono





