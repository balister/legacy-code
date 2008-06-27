/*
 *  Header file with ioctl definitions.
 *
 *  The declarations here have to be in a header file, because
 *  they need to be known both to the kernel module
 *  (in chardev.c) and the user process calling ioctl.
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

#ifndef CHARDEV_H
#define CHARDEV_H 1

#include <linux/ioctl.h>

/* Use 'h' as magic number */
#define SFFSDR_IOCTL_MAGIC_NUMBER 'h'

/*
 * _IOR means that we're creating an ioctl command number for passing
 *      information from the kernel module to a user process.
 * _IOW means that we're creating an ioctl command number for passing
 *      information from a user process to the kernel module.
 *
 * First argument:  SFFSDR_FPGA_DEVICE_MAJOR is the major device number we're using.
 * Second argument: The number of the command (there could be several with
 *                  different meanings).
 * Third argument:  The type we want to get from the process to the kernel, or
 *                  vice-versa.
 */
#define IOCTL_SFFSDR_FPGA_READ_REG    _IOW(SFFSDR_IOCTL_MAGIC_NUMBER, 0, int32_t *)
#define IOCTL_SFFSDR_FPGA_WRITE_REG   _IOW(SFFSDR_IOCTL_MAGIC_NUMBER, 1, int32_t *)

#endif /* CHARDEV_H */
