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
#include <linux/string.h>
#include <linux/firmware.h>
#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/io.h>
#include <asm/arch/hardware.h>
#include <asm/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>

#include "common.h"
#include "bitstream_load.h"


/* FPGA physical EMIF register base address. */
#define FPGA_MEM_REGION_BASE_ADDR	(0x4000000)
#define FPGA_MEM_REGION_LENGTH		(0x2000000) /* 32M */

/* Size of the buffer holding the bitstream data. */
#define BITSTREAM_BUFFER_SIZE		(2*1024*1024) /* 2M */

/* Structure containing information specific to each FPGA. */
struct sffsdr_fpga_dev {
	enum {
		FPGA_DEV_INIT_START,
		FPGA_DEV_INIT_DEVICE_REGISTERED,
		FPGA_DEV_INIT_DONE,
	} init_state;
	struct miscdevice miscdev; /* Misc. device structure. */
	char *bitstream_name;
	u8 *bitstream_data;
	size_t bitstream_length;
	int bitstream_loaded; /* Set to TRUE when FPGA is loaded.
			       * FPGA1 can be loaded only if FPGA0
			       * has been loaded. */
};

/* Structure containing generic driver informations. */
struct sffsdr_fpga_private {
	enum {
		SFFSDR_FPGA_INIT_START,
		SFFSDR_FPGA_INIT_HAVE_IOMAPPING,
		SFFSDR_FPGA_INIT_HAVE_GPIO_PINS,
		SFFSDR_FPGA_INIT_DONE,
	} init_state;
	void *mmio_addr; /* Remapped I/O memory space */
	struct sffsdr_fpga_dev fpga[2]; /* FPGA0 on Base board.
					 * FPGA1 on conversion module. */
};

static struct sffsdr_fpga_private sffsdr_fpga;
static const char fpga0_device_name[] = "fpga0";
static const char fpga1_device_name[] = "fpga1";

/* Open method.
 *
 * return value:
 *   0 in case of success
 *   -ENOMEM if unable to allocate memory.
 */
int sffsdr_fpga_open(struct inode *inode, struct file *filp)
{
	int minor = iminor(inode);
	struct sffsdr_fpga_dev *fpgadev;

	if (minor == sffsdr_fpga.fpga[0].miscdev.minor)
		filp->private_data = &sffsdr_fpga.fpga[0];
	else if (minor == sffsdr_fpga.fpga[1].miscdev.minor)
		filp->private_data = &sffsdr_fpga.fpga[1];
	else {
		FAILMSG("Invalid device with minor number %d.", minor);
		return -EFAULT;
	}

	fpgadev = (struct sffsdr_fpga_dev *) filp->private_data;

	DBGMSG("Opening %s.", fpgadev->miscdev.name);

	fpgadev->bitstream_length = 0;
	fpgadev->bitstream_data = kmalloc(BITSTREAM_BUFFER_SIZE, GFP_KERNEL);

	if (!fpgadev->bitstream_data) {
		FAILMSG("Failed to allocate memory for bitstream.");
		return -ENOMEM;
	}

	return 0; /* success */
}

/* Write method. Fill buffer with bitstream data.
 */
ssize_t sffsdr_fpga_write(struct file *filp, const char __user *buff,
		  size_t count, loff_t *offp)
{
	struct sffsdr_fpga_dev *fpgadev =
		(struct sffsdr_fpga_dev *) filp->private_data;

	if ((fpgadev->bitstream_length + count) >= BITSTREAM_BUFFER_SIZE) {
		FAILMSG("Bitstream buffer size exceeded.");
		return -EFBIG;
	}
	
	if (copy_from_user(fpgadev->bitstream_data + fpgadev->bitstream_length,
			   (void __user *) buff, count))
		return -EFAULT;
	
	fpgadev->bitstream_length += count;
	
	return count;
}

/* Release method. This will initiate the FPGA programming.
 *                 Only SX35 is supported for the moment.
 *
 * return value: 0.
 */
int sffsdr_fpga_release(struct inode *inode, struct file *filp)
{
	struct sffsdr_fpga_dev *fpgadev =
		(struct sffsdr_fpga_dev *) filp->private_data;
	
	if (fpgadev->bitstream_data) {
		if (strcmp(fpgadev->miscdev.name, "fpga1") == 0) {
			DBGMSG("TODO: add support for loading SX25 FPGA.");
		}
		else {
			DBGMSG("Programming bitstream on %s.", fpgadev->miscdev.name);
			bitstream_load(sffsdr_fpga.mmio_addr, fpgadev->bitstream_data, fpgadev->bitstream_length);
		}
		kfree(fpgadev->bitstream_data);
	}		
	
	return 0; /* success */
}

static struct file_operations fops_fpga = {
	.owner = THIS_MODULE,
	.open = sffsdr_fpga_open,
	.write = sffsdr_fpga_write,
	.release = sffsdr_fpga_release
};

static void sffsdr_fpga_dev_cleanup(struct sffsdr_fpga_dev *fpgadev)
{
	DBGMSG("Removing %s.", fpgadev->miscdev.name);

	switch(fpgadev->init_state) {
	case FPGA_DEV_INIT_DONE:
		DBGMSG( "  FPGA_DEV_INIT_DONE" );
	case FPGA_DEV_INIT_DEVICE_REGISTERED:
		DBGMSG( "  FPGA_DEV_INIT_DEVICE_REGISTERED" );
		misc_deregister(&fpgadev->miscdev);
	case FPGA_DEV_INIT_START:
		; /* Nothing to do. */
	}
}

static void sffsdr_fpga_cleanup(void)
{
	sffsdr_fpga_dev_cleanup(&sffsdr_fpga.fpga[1]);
	sffsdr_fpga_dev_cleanup(&sffsdr_fpga.fpga[0]);
	
	switch(sffsdr_fpga.init_state) {
	case SFFSDR_FPGA_INIT_DONE:
		DBGMSG( "  SFFSDR_FPGA_INIT_DONE" );
	case SFFSDR_FPGA_INIT_HAVE_GPIO_PINS:
		DBGMSG( "  SFFSDR_FPGA_INIT_HAVE_GPIO_PINS" );
		select_map_release_gpio_pins();
	case SFFSDR_FPGA_INIT_HAVE_IOMAPPING:
		DBGMSG( "  SFFSDR_FPGA_INIT_HAVE_IOMAPPING" );
		iounmap(sffsdr_fpga.mmio_addr);
	case SFFSDR_FPGA_INIT_START:
		; /* Nothing to do. */
	}
}

static int sffsdr_fpga_init_device(struct sffsdr_fpga_dev *fpgadev)
{
	const struct firmware *fw_entry;
	int retval;

	fpgadev->miscdev.minor = MISC_DYNAMIC_MINOR;
	fpgadev->miscdev.fops = &fops_fpga;

	DBGMSG("Initializing %s.", fpgadev->miscdev.name);

	retval = misc_register(&fpgadev->miscdev);
	if (retval < 0) {
		FAILMSG("misc_register() failed (%d).", retval);
		goto error;
	}
	fpgadev->init_state = FPGA_DEV_INIT_DEVICE_REGISTERED;

	/* Try to load firmware through hotplug if available. */
	retval = request_firmware(&fw_entry, fpgadev->bitstream_name, fpgadev->miscdev.this_device);
	if (retval < 0) {
		DBGMSG("Firmware not available.");
	}
	else {
		DBGMSG("fw_entry->size = %d kb", fw_entry->size / 1024 );
		
		retval = bitstream_load(sffsdr_fpga.mmio_addr, fw_entry->data, fw_entry->size);
		release_firmware(fw_entry);
		if (retval < 0) {
			FAILMSG("Firmware not loaded in FPGA.");
			goto error;
		}
	}
	
	fpgadev->init_state = FPGA_DEV_INIT_DONE;
	return 0;

error:
	sffsdr_fpga_dev_cleanup(fpgadev);
	return retval;
}

int __init sffsdr_fpga_init( void )
{
	int retval = -1;

	DBGMSG("sffsdr_fpga_init() called");

	sffsdr_fpga.init_state = SFFSDR_FPGA_INIT_START;

	/* Assign virtual addresses to I/O memory regions. */
	sffsdr_fpga.mmio_addr = ioremap(FPGA_MEM_REGION_BASE_ADDR, FPGA_MEM_REGION_LENGTH);
	if( sffsdr_fpga.mmio_addr == NULL ) {
		FAILMSG("Failed to remap registers." );
		goto error;
	}

	sffsdr_fpga.init_state = SFFSDR_FPGA_INIT_HAVE_IOMAPPING;

	/* Initialize DSP to FPGA control pins (GPIOs).
	 * Hardcoded to SFFSDR board type for now. */
	retval = select_map_init_gpio_pins(BOARD_TYPE_SFFSDR);
	if (retval < 0)
		goto error;
	
	sffsdr_fpga.init_state = SFFSDR_FPGA_INIT_HAVE_GPIO_PINS;

	/* Initializing FPGA0. */
	sffsdr_fpga.fpga[0].init_state = FPGA_DEV_INIT_START;
	sffsdr_fpga.fpga[0].bitstream_name = "sffsdr_fpga0.bit";
	sffsdr_fpga.fpga[0].miscdev.name = fpga0_device_name;
	retval = sffsdr_fpga_init_device(&sffsdr_fpga.fpga[0]);
	if (retval < 0) {
		goto error;
	}

	/* Initializing FPGA1. */
	sffsdr_fpga.fpga[1].init_state = FPGA_DEV_INIT_START;
	sffsdr_fpga.fpga[1].bitstream_name = "sffsdr_fpga1.bit";
	sffsdr_fpga.fpga[1].miscdev.name = fpga1_device_name;
	retval = sffsdr_fpga_init_device(&sffsdr_fpga.fpga[1]);
	if (retval < 0) {
		goto error;
	}
	
	sffsdr_fpga.init_state = SFFSDR_FPGA_INIT_DONE;

	return 0;

error:
	sffsdr_fpga_cleanup();
	return retval;
}

void __exit sffsdr_fpga_exit( void )
{
	DBGMSG( "sffsdr_fpga_exit() called");

	sffsdr_fpga_cleanup();
}

/***********************************
 * General module initialization   *
 ***********************************/
MODULE_AUTHOR("Hugo Villeneuve <hvilleneuve@lyrtech.com>");
MODULE_DESCRIPTION("Lyrtech SFFSDR FPGA1 driver");
MODULE_LICENSE("GPL");

module_init(sffsdr_fpga_init);
module_exit(sffsdr_fpga_exit);
