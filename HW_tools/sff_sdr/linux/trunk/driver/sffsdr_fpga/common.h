/*
 * Common macros/definitions.
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

#ifndef COMMON_H
#define COMMON_H 1

#define MODULE_NAME "sffsdr_fpga"

/* Define this to have verbose debug messages. */
#define SFFSDR_DEBUG 1

#ifdef SFFSDR_DEBUG
#define DBGMSG(fmt, args...) \
printk(KERN_INFO "%s: "fmt"\n" , MODULE_NAME, ## args)
#define FAILMSG(fmt, args...) \
printk(KERN_ERR "%s: "fmt"\n" , MODULE_NAME, ## args)
#else
#define DBGMSG(fmt, args...)
#define FAILMSG(fmt, args...)
#endif

/* Select MAP register address. */
#define FPGA_SELECT_MAP_REG_OFFSET	0x0000
/* Lyrtech specific register inside FPGA. */
#define FPGA_RESET_REG_OFFSET		0x8040

#endif /* COMMON_H */
