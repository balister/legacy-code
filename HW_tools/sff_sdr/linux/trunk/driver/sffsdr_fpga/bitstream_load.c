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

#define FPGA_DONE_TIMEOUT	100000

#define BITSTREAM_MODE_UNKNOWN	0
#define BITSTREAM_MODE_FULL	1
#define BITSTREAM_MODE_PARTIAL	2

#define FPGA_FULL_RESET_VAL	3
#define FPGA_PARTIAL_RESET_VAL	2

#define BITSTREAM_SYNC_BYTE1		(0xAA)
#define BITSTREAM_SYNC_BYTE2		(0x99)
#define BITSTREAM_SYNC_BYTE3		(0x55)
#define BITSTREAM_SYNC_BYTE4 		(0x66)

#define BITSTREAM_PACKET_HEADER_TYPE1	(1)
#define BITSTREAM_PACKET_HEADER_TYPE2	(2)

#define BITSTREAM_TYPE1_OPCODE_WRITE	(2)

#define BITSTREAM_TYPE1_REG_ADDR_FDRI	(2)

/* Access of FPGA register through EMIF (mmio). */
#define FPGA_ACCESS_REG(x)	(*((volatile u8 *)(x)))

/* The Virtex-4 device only drives BUSY during readback.
 * Define this to check the state of thwe BUSY pin before each write. */
#undef SFFSDR_FPGA_CHECK_BUSY_PIN

/* Structure of a TYPE1 packet. */
struct type1_packet_t {
	uint32_t word_count:11;
	uint32_t reserved2:2;
	uint32_t address:5;
	uint32_t reserved1:9;
	uint32_t opcode:2;
	uint32_t header:3;
};

/* Structure of a TYPE2 packet. */
struct type2_packet_t {
	uint32_t word_count:27;
	uint32_t opcode:2; /* Reserved. */
	uint32_t header:3;
};

static void *fpga_mmio_addr;
static uint8_t fpga_program_b_gpio;
static uint8_t fpga_done_gpio;
static uint8_t fpga_init_gpio;
static uint8_t fpga_busy_gpio;

/* Reset the inside logic of the FPGA according to the
 * bitstream mode. This is done when the bitstream has
 * been programmed and is Lyrtech SFF-SDR specific. */
static void fpga_reset(int bitstream_mode)
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
static u8 swapbits(u8 c) 
{
	int i;
	u8 result = 0;
	
        for (i = 0; i < 8; ++i) {
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
static void select_map_write_byte(u8 data)
{
#ifdef SFFSDR_FPGA_CHECK_BUSY_PIN
	/* Making sure BUSY is high. */
	if (gpio_get_value(fpga_busy_gpio) == 0) {
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
static void select_map_make_clock(int cycles)
{
	int k;
	
	for (k = 0; k < cycles; k++) {
		select_map_write_byte(0);
	}
}

static int select_map_select_gpio_pins(int board_type)
{
	switch (board_type) {
	case BOARD_TYPE_SFFSDR:
		fpga_program_b_gpio = GPIO(37);
		fpga_done_gpio      = GPIO(39);
		fpga_init_gpio      = GPIO(40);
		fpga_busy_gpio      = GPIO(42);
		break;
	default:
		FAILMSG("Error: Unknown board type.");
		return FPGA_LOAD_INVALID_BOARD_TYPE;
	}

	return 0;
}

/* Init DaVinci GPIO to FPGA control pins for the Select MAP mode.
 * NOTE: This code should be eventually be moved to the board init section. */
int select_map_init_gpio_pins(int board_type)
{
	int retval;

	retval = select_map_select_gpio_pins(board_type);
	if (retval != 0)
		goto error;

	/* Configure FPGA PROGRAM_B GPIO. */
        retval = gpio_request(fpga_program_b_gpio, "fpga_program_b");
	if (retval == 0)
		retval = gpio_direction_output(fpga_program_b_gpio, 1); /* FPGA_PROGRAM_B is initially HIGH. */
	if (retval != 0)
		goto error;
	
	/* Configure FPGA INIT GPIO. */
        retval = gpio_request(fpga_init_gpio, "fpga_init");
	if (retval == 0)
		retval = gpio_direction_input(fpga_init_gpio);
	if (retval != 0)
		goto error;

	/* Configure FPGA DONE GPIO. */
        retval = gpio_request(fpga_done_gpio, "fpga_done");
	if (retval == 0)
		retval = gpio_direction_input(fpga_done_gpio);
	if (retval != 0)
		goto error;

	/* Configure FPGA BUSY GPIO. */
        retval = gpio_request(fpga_busy_gpio, "fpga_busy");
	if (retval == 0)
		retval = gpio_direction_input(fpga_busy_gpio);
	if (retval != 0)
		goto error;

	return 0;

error:
	FAILMSG("Error configuring GPIO pins.");
	return -1;
}

/* Release DaVinci GPIO to FPGA control pins for the Select MAP mode.
 * NOTE: This code should be eventually be moved to the board init section. */
void select_map_release_gpio_pins(void)
{
	gpio_free(fpga_busy_gpio);
	gpio_free(fpga_done_gpio);
	gpio_free(fpga_init_gpio);
	gpio_free(fpga_program_b_gpio);
}

/*
 * Return value:
 *   0: Error
 *   1: Full bitstream
 *   2: Partial bitstream
 */
static int bitstream_parse_header(u8 *buffer, size_t length)
{
	int index = 0;
	int found;
	size_t payload_size = 0;

	/* Search for bitstream sync word. */
	found = false;
	while ((index < length) && (found == false)) {
		if ((buffer[index + 0] == BITSTREAM_SYNC_BYTE1) &&
		    (buffer[index + 1] == BITSTREAM_SYNC_BYTE2) &&
		    (buffer[index + 2] == BITSTREAM_SYNC_BYTE3) &&
		    (buffer[index + 3] == BITSTREAM_SYNC_BYTE4)) {
			found = true;
		}
		else
			index++;
	}
	
	if (found == false) {
		DBGMSG("Error, Xilinx header not found in bitstream file.");
		return BITSTREAM_MODE_UNKNOWN;
	}
	
	/* Find the payload size. */
	while (index < length) {
		u32 temp = ntohl(*((u32 *) &buffer[index]));
		struct type1_packet_t *type1_packet = (struct type1_packet_t *) &temp;

		/* Search for type 1 packet header. */
		if ((type1_packet->header == BITSTREAM_PACKET_HEADER_TYPE1) &&
		    (type1_packet->opcode == BITSTREAM_TYPE1_OPCODE_WRITE) &&
		    (type1_packet->address == BITSTREAM_TYPE1_REG_ADDR_FDRI)) {
			if (type1_packet->word_count != 0) {
				payload_size = type1_packet->word_count;
				break;
			}
			else {
				u32 temp2 = ntohl(*((u32 *) &buffer[index+4]));
				struct type2_packet_t *type2_packet = (struct type2_packet_t *) &temp2;
				
				/* Search for type 2 packet header just after type1 packet. */
				if ((type2_packet->header == BITSTREAM_PACKET_HEADER_TYPE2)) {
					payload_size = type2_packet->word_count;
					break;
				}
			}
		}
		
		index += 4; /* u32 aligned when sync word has been found. */
	}
	
	if (index >= length) {
		DBGMSG("Error, payload size not found in bitstream file.");
		return BITSTREAM_MODE_UNKNOWN;
	}

	payload_size *= 4; /* Length in bytes. */

	DBGMSG("Bitstream payload size: %d bytes", payload_size);
	
	/* Is it a full or a partial bitstream? */
	if (payload_size == BITSTREAM_LENGTH_SX35) {
		DBGMSG("Bitstream type: FULL");
		return BITSTREAM_MODE_FULL;
	}
	else {
		DBGMSG("Bitstream type: PARTIAL");
		return BITSTREAM_MODE_PARTIAL;
	}
}

/*
 * mode: Full or partial.
 * full bitstream that supports partial must be generated with option Perist = true.
 */
int bitstream_load(void *mmio_addr, u8 *data, size_t size)
{
	int k;
	int timeout_counter = 0;
	int bitstream_mode;

	fpga_mmio_addr = mmio_addr;

	bitstream_mode = bitstream_parse_header(data, size);
	switch (bitstream_mode) {
	case BITSTREAM_MODE_UNKNOWN:
		FAILMSG("Error: Unknown bitstream mode.");
		return FPGA_LOAD_ERROR;
		break;
	case BITSTREAM_MODE_FULL:
		/* Toggle PROG_B Pin and wait at least 300nS before proceeding. */
		gpio_set_value(fpga_program_b_gpio, 0); /* FPGA_PROGRAM_B is LOW. */
		udelay(1);
		break;
	case BITSTREAM_MODE_PARTIAL:
		break;
	}
	
	/* For partial bitstream, PROGRAM_B is already high. */
	select_map_make_clock(3);
	gpio_set_value(fpga_program_b_gpio, 1); /* FPGA_PROGRAM_B is HIGH. */
	
	/* Wait for INIT pin to go high. */
	while (gpio_get_value(fpga_init_gpio) == 0) {
		select_map_make_clock(3);
	}
	
	/* Send actual bitstream data to FPGA one byte at a time. */
	for (k = 0; k < size; k++) {
		select_map_write_byte(data[k]);
		
		if (gpio_get_value(fpga_init_gpio) == 0) {
			/* Error if INIT goes low during programming. */
			FAILMSG("Error: INIT LOW during programming.");
			return FPGA_LOAD_INIT_ERROR;
		}
    	}
	
	/* Pulse the clock line ten times at the end. */
	select_map_make_clock(10);
	
	/* FPGA DONE pin must go high. */
	while ((gpio_get_value(fpga_done_gpio) == 0) && (timeout_counter < FPGA_DONE_TIMEOUT)) {
		timeout_counter ++;
	}
    
	if (gpio_get_value(fpga_done_gpio) == 0) {
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
