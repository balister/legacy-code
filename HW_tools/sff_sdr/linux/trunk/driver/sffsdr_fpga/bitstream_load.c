/*
 * FPGA bitstream loader on the
 * SFF-SDR development board.
 *
 * Copyright (C) 2008 Lyrtech <www.lyrtech.com>
 *
 * The FPGA is loaded using the SelectMAP mode through
 * the EMIF interface and some dedicated control signals:
 *
 *   FPGA          DM6446
 *   --------------------
 *   PROGRAM_B     GPIO37
 *   DONE          GPIO39
 *   INIT          GPIO40
 *   DOUT_BUSY     GPIO42
 *   CS_B          EMIF_A13 OR CS3n
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <asm/irq.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <asm/hardware.h>
#include <linux/delay.h>

#include "common.h"
#include "bitstream_load.h"
#include "dvregs.h"


#define BITSTREAM_LENGTH_SX35	(4*426810) /* Length in bytes of a
					    * full bitstream. */
#define FPGA_DONE_TIMEOUT	100000

#define FPGA_DONE_MASK		0x80
#define FPGA_BUSY_MASK		0x400
#define FPGA_PROG_B_MASK	0x20
#define FPGA_INIT_MASK		0x100

#define GPIO_FPGA_PROGRAM_B	GPIO(37)
#define GPIO_FPGA_DONE		GPIO(39)
#define GPIO_FPGA_INIT		GPIO(40)
#define GPIO_FPGA_DOUT_BUSY	GPIO(42)

#define BITSTREAM_MODE_UNKNOWN	0
#define BITSTREAM_MODE_FULL	1
#define BITSTREAM_MODE_PARTIAL	2

#define FPGA_FULL_RESET_VAL	3
#define FPGA_PARTIAL_RESET_VAL	2

/* Access of FPGA register through EMIF (mmio). */
#define FPGA_ACCESS_REG(x)	(*((volatile u8 *)(x)))

/* The Virtex-4 device only drives BUSY during readback.
 * Define this to check the state of thwe BUSY pin before each write. */
#undef SFFSDR_FPGA_CHECK_BUSY_PIN

static void *fpga_mmio_addr;

/* Reset the inside logic of the FPGA according to the
 * bitstream mode. This is done when the bitstream has
 * been programmed and is Lyrtech SFF-SDR specific. */
static void
fpga_reset( int bitstream_mode )
{
	u32 value;

	if (bitstream_mode == BITSTREAM_MODE_FULL) {
		value = FPGA_FULL_RESET_VAL;
	}
	else {
		value = FPGA_PARTIAL_RESET_VAL;
	}

	FPGA_ACCESS_REG(fpga_mmio_addr + FPGA_RESET_REG_OFFSET) = value;
	FPGA_ACCESS_REG(fpga_mmio_addr + FPGA_RESET_REG_OFFSET) = 0;
}

/* Swap bits inside a byte. */
static u8
swapbits( u8 c) 
{
	int i;
	u8 result = 0;
	
        for( i = 0; i < 8; ++i ) {
		result = result << 1; 
                result |= (c & 1); 
                c = c >> 1; 
        }
	
        return result; 
}

/*
 * This function writes a byte of data to the FPGA SelectMAP
 * interface. The FPGA_SELECT_MAP_REG address is within
 * the FPGA address space (CS3), and when we write a byte
 * to that address, the CCLK line will be toggled.
 */
static void
select_map_write_byte( u8 data )
{
#ifdef SFFSDR_FPGA_CHECK_BUSY_PIN
	/* Making sure BUSY is high. */
	if (gpio_get_value(GPIO_FPGA_DOUT_BUSY) == 0) {
		FAILMSG("Error: FPGA BUSY.");
	}	
#endif

	/* We have to swap the bits */
	FPGA_ACCESS_REG(fpga_mmio_addr + FPGA_SELECT_MAP_REG_OFFSET) = swapbits(data);
}

/*
 * This function toggles the CCLK line on the select map
 * interface the number of times specified by <cycles>.
 */
static void
select_map_make_clock( int cycles )
{
	int k;
	
	for (k = 0; k < cycles; k++) {
		select_map_write_byte(0);
	}
}

/*
 * Return value:
 *   0: Error
 *   1: Full bitstream
 *   2: Partial bitstream
 */
static int
fpga_bitstream_parse_header( u8 *buffer, size_t length )
{
	int index = 0;
	size_t payload_size;
	
	/* Search and skip Xilinx header. */
	while ( ( buffer[0+index] != 0xFF ||
		  buffer[1+index] != 0xFF ||
		  buffer[2+index] != 0xFF ||
		  buffer[3+index] != 0xFF ||
		  buffer[4+index] != 0xAA ||
		  buffer[5+index] != 0x99 ||
		  buffer[6+index] != 0x55 ||
		  buffer[7+index] != 0x66 ) &&
		(index < length)) {
		index++;
	}
	
	if (index == length) {
		/* Error, Xilinx header not found in the buffer. */
		return BITSTREAM_MODE_UNKNOWN;
	}
	
	/* Now find the length of the payload data by
	 * searching for Packet Type 1 : 0x30004000 (FDRI) and
	 * beginning of packet 2. */
	while ( ( buffer[0+index]!=0x30 ||
		  buffer[1+index]!=0x00 ||
		  buffer[2+index]!=0x40 ||
		  buffer[3+index]!=0x00 ||
		  ( ( buffer[4+index] & 0xE0 ) != 0x40 ) ) &&
		(index < length)) {
		index++;
	}
	
	if (index == length) {
		/* Error, payload size not found in the buffer. */
		return BITSTREAM_MODE_UNKNOWN;
	}

	payload_size =
		(buffer[4+index] & 0x03 << 24) |
		(buffer[5+index] << 16) |
		(buffer[6+index] << 8) |
		(buffer[7+index]); /* Length in u32. */
	payload_size *= 4; /* Length in bytes. */

	DBGMSG("Bitstream payload size: %d kbytes", payload_size / 1024 );
	
	/* Is it a full of a partial bitstream? */
	if (payload_size == BITSTREAM_LENGTH_SX35) {
		DBGMSG("Bitstream type: FULL");
		return BITSTREAM_MODE_FULL;
	}
	else {
		DBGMSG("Bitstream type: PARTIAL");
		return BITSTREAM_MODE_PARTIAL;
	}
}

/* Init DaVinci GPIO to FPGA control pins for the Select MAP mode. */
static void
dv_init_control_pins( void )
{
	u32 value;

	gpio_set_value(GPIO_FPGA_PROGRAM_B, 1); /* FPGA_PROGRAM_B is HIGH. */

	value = davinci_readl(DAVINCI_GPIO_BASE + DIR23);

	/* Configure input pins. */
	value |= FPGA_DONE_MASK | FPGA_BUSY_MASK | FPGA_INIT_MASK;
	/* Configure output pins. */
	value &= ~FPGA_PROG_B_MASK;

	davinci_writel(value, DAVINCI_GPIO_BASE + DIR23);
}

/*
 * mode: Full or partial.
 * full bitstream that supports partial must be generated with option Perist = true.
 */
int
bitstream_load( void *mmio_addr, u8 *data, size_t size )
{
	int k;
	int timeout_counter = 0;
	int bitstream_mode;

	fpga_mmio_addr = mmio_addr;

	/* Initialize DSP to FPGA control pins (GPIOs). */
	dv_init_control_pins();

	bitstream_mode = fpga_bitstream_parse_header( data, size );
	if( bitstream_mode == BITSTREAM_MODE_UNKNOWN ) {
		FAILMSG("Error: Unknown bitstream mode.");
		return FPGA_LOAD_ERROR;
	}

	if (bitstream_mode == BITSTREAM_MODE_FULL) {
		/* Toggle PROG_B Pin and wait at least 300nS before proceeding. */
		gpio_set_value(GPIO_FPGA_PROGRAM_B, 0); /* FPGA_PROGRAM_B is LOW. */
		udelay(1);
	}
	
	/* For partial bitstream, PROGRAM_B is already high. */
	select_map_make_clock(3);
	gpio_set_value(GPIO_FPGA_PROGRAM_B, 1); /* FPGA_PROGRAM_B is HIGH. */
	
	/* Wait for INIT pin to go high. */
	while (gpio_get_value(GPIO_FPGA_INIT) == 0) {
		select_map_make_clock(3);
	}
	
	/* Send actual bitstream data to FPGA one byte at a time. */
	for ( k = 0; k < size; k++) {
		select_map_write_byte(data[k]);
		
		if (gpio_get_value(GPIO_FPGA_INIT) == 0) {
			/* Error if INIT goes low during programming. */
			FAILMSG("Error: INIT LOW during programming.");
			return FPGA_LOAD_INIT_ERROR;
		}
    	}
	
	/* Pulse the clock line ten times at the end. */
	select_map_make_clock(10);
	
	/* FPGA DONE pin must go high. */
	while ((gpio_get_value(GPIO_FPGA_DONE) == 0) && (timeout_counter < FPGA_DONE_TIMEOUT)) {
		timeout_counter ++;
	}
    
	if (gpio_get_value(GPIO_FPGA_DONE) == 0) {
		/* Timeout error. */
		FAILMSG("Error: timeout while waiting for DONE to go HIGH.");
		return FPGA_LOAD_TIMEOUT;
	}    
	else {
		/* Success. */
		fpga_reset(bitstream_mode);
		return FPGA_LOAD_SUCCESS;
	}
}
