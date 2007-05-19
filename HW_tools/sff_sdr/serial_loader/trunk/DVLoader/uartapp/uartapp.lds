/*
 *  Desc: Linker command file ubl application program.
 *  Auth: Daniel Allred (d-allred@ti.com)
 *  Vers: 1.0 on 08-Nov-2006
 */

ENTRY(boot)
SECTIONS {
	. = 0x00008020;
	
	.rodata	: AT ( 0x0 )
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
	
	. -= 0x8000;
	.text		: AT ( LOADADDR(.data) + SIZEOF(.data) )
	{
		*(.text)
		. = ALIGN(4);
	}
	.boot		: AT ( LOADADDR(.text) + SIZEOF(.text) )
	{
		*(.boot)
		. = ALIGN(4);
	}

	. += 0x8000;
	.bss		:
	{
		*(.bss) *(COMMON)
		. = ALIGN(4);
	}
	
	__topstack = 0xC000 - 0x4;
	
	. = 0x02000000;
   .aemif 	:
   {
   	*(.aemif)
   }

   . = 0x80000000;
   .ddrram	:
   {
   	*(.ddrram)
   }					
}

