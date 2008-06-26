/*
 * sffsdr_fpga driver
 *
 * Copyright (C) 2008 Lyrtech <www.lyrtech.com>
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
#include <linux/device.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/firmware.h>
#include <linux/mm.h>
#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/io.h>
#include <asm/arch/hardware.h>

#include "common.h"
#include "bitstream_load.h"


/* FPGA physical EMIF register base address. */
#define FPGA_MEM_REGION_BASE_ADDR	0x4000000
#define FPGA_MEM_REGION_LENGTH		0x2000000 /* 32M */

/* Future char driver. */
#undef USE_CHARDEV_DRIVER

struct sffsdr_fpga_private {
	enum {
		SFFSDR_FPGA_INIT_START,
		SFFSDR_FPGA_INIT_DEVICE_REGISTERED,
		SFFSDR_FPGA_INIT_HAVE_IOMAPPING,
		SFFSDR_FPGA_INIT_FIRMWARE_LOADED,
		SFFSDR_FPGA_INIT_DONE,
	} init_state;
	void *mmio_addr; /* Remapped I/O memory space */
	int minor; /* chardev minor number. */
};

static struct sffsdr_fpga_private sffsdr_fpga;

static void fpga1_cleanup(struct device *dev)
{
	switch(sffsdr_fpga.init_state) {
	case SFFSDR_FPGA_INIT_DONE:
		DBGMSG( "  SFFSDR_FPGA_INIT_DONE" );
#ifdef USE_CHARDEV_DRIVER
		status = sffsdr_fpga_chardev_unregister(0);
		if( status < 0 ) {
			DBGMSG( "  Error, sffsdr_fpga_chardev_unregister() returned %d", status );
		}
#endif
	case SFFSDR_FPGA_INIT_FIRMWARE_LOADED:
		DBGMSG( "  SFFSDR_FPGA_INIT_FIRMWARE_LOADED" );
	case SFFSDR_FPGA_INIT_HAVE_IOMAPPING:
		DBGMSG( "  SFFSDR_FPGA_INIT_HAVE_IOMAPPING" );
		iounmap(sffsdr_fpga.mmio_addr);
	case SFFSDR_FPGA_INIT_DEVICE_REGISTERED:
		DBGMSG( "  SFFSDR_FPGA_INIT_DEVICE_REGISTERED" );
		device_unregister(dev);
	case SFFSDR_FPGA_INIT_START:
		; /* Nothing to do. */
	}
}

static void fpga1_release(struct device *dev)
{
	DBGMSG("fpga1_release() called");
}

static struct device fpga1_device = {
	.bus_id = "fpga1",
	.release = fpga1_release
};

int __init
sffsdr_fpga_init( void )
{
	const struct firmware *fw_entry;
	int retval;

	DBGMSG("sffsdr_fpga_init() called");

	sffsdr_fpga.init_state = SFFSDR_FPGA_INIT_START;

	retval = device_register(&fpga1_device);
	if (retval < 0) {
		FAILMSG("device_register() failed.");
		goto error;
	}
	sffsdr_fpga.init_state = SFFSDR_FPGA_INIT_DEVICE_REGISTERED;

	/* Assign virtual addresses to I/O memory regions. */
	sffsdr_fpga.mmio_addr = ioremap(FPGA_MEM_REGION_BASE_ADDR, FPGA_MEM_REGION_LENGTH);
	if( sffsdr_fpga.mmio_addr == NULL ) {
		FAILMSG("Failed to remap registers." );
		goto error;
	}

	sffsdr_fpga.init_state = SFFSDR_FPGA_INIT_HAVE_IOMAPPING;

	retval = request_firmware(&fw_entry, "sffsdr_fpga1.bit", &fpga1_device);
	if (retval < 0) {
		FAILMSG("Firmware not available.");
		goto error;
	}

	DBGMSG("fw_entry->size = %d kb", fw_entry->size / 1024 );

	retval = bitstream_load(BOARD_TYPE_SFFSDR, sffsdr_fpga.mmio_addr, fw_entry->data, fw_entry->size);
	release_firmware(fw_entry);
	if (retval < 0) {
		FAILMSG("Firmware not loaded in FPGA.");
		goto error;
	}
	
	sffsdr_fpga.init_state = SFFSDR_FPGA_INIT_FIRMWARE_LOADED;

	return 0;

error:
	fpga1_cleanup(&fpga1_device);
	return retval;
}

void __exit
sffsdr_fpga_exit( void )
{
	DBGMSG( "sffsdr_fpga_exit() called");

	fpga1_cleanup(&fpga1_device);
}

/***********************************
 * General module initialization   *
 ***********************************/
MODULE_AUTHOR("Hugo Villeneuve <hvilleneuve@lyrtech.com>");
MODULE_DESCRIPTION ("Lyrtech SFFSDR FPGA1 driver");
MODULE_LICENSE("GPL");

module_init(sffsdr_fpga_init);
module_exit(sffsdr_fpga_exit);
