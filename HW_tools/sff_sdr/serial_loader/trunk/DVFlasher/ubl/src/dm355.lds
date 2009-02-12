/* DM355 Linker command file for ubl. */

ENTRY(boot)
SECTIONS {
	EMIFStart  = 0x02000000;
	EMIFSize   = 0x02000000;
	IRAMStart  = 0x00000000;
	IRAMSize   = 0x00008000;
  	DRAMStart  = 0x00010000;
	DRAMSize   = 0x00008000;
	DDRStart   = 0x80000000;
	DDRSize    = 0x10000000;

	STACKStart = DRAMStart + DRAMSize;	
	__topstack = (DRAMStart + DRAMSize) - 0x4;
  
	. = EMIFStart;

	__CS2start = .;
	__NORFlash = .;
	__NANDFlash = .;

	.selfcopy : AT (0x0)
	{
		*(.selfcopy)		
		. = ALIGN(256);
	}
	
	__selfcopysrc = EMIFStart;
	__IVT = DRAMStart;
	__selfcopydest = DRAMStart + 0x00000020;

	. = 0x20;
	.fakeentry : AT ( LOADADDR(.selfcopy) + SIZEOF(.selfcopy))
	{
		*(.fakeentry)
		. = ALIGN(4);
	}
		
	.text		: AT ( LOADADDR(.fakeentry) + SIZEOF(.fakeentry) )
	{
		*(.text)
		. = ALIGN(4);
	}
	.boot		: AT ( LOADADDR(.text) + SIZEOF(.text))
	{
		*(.boot)
		. = ALIGN(4);
	}
	
        /* Move into DRAM for placing const and data sections */
        . += (DRAMStart - IRAMStart);

	.rodata		: AT ( LOADADDR(.boot) + SIZEOF(.boot) )
	{
		*(.rodata*)
		*(.rodata)
		. = ALIGN(4);
	}	

	.data		: AT ( LOADADDR(.rodata) + SIZEOF(.rodata) )
	{
		*(.data)
		. = ALIGN(4);
	}
			
	__selfcopydestend = __selfcopydest + SIZEOF(.fakeentry) +
						SIZEOF(.text) + SIZEOF(.boot) +
						SIZEOF(.data) + SIZEOF(.rodata);
	
	.bss		:
	{
		*(.bss) *(COMMON)
		. = ALIGN(4);
	}

	/* DDR2 */
   	. = DDRStart;
	.ddrram	:
	{
   		*(.ddrram)
	}
}

