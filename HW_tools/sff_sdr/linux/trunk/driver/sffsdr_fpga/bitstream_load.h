/*
 * FPGA bitstream load header file.
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

#ifndef BITSTREAM_LOAD_H
#define BITSTREAM_LOAD_H 1

/* Return codes. */
#define FPGA_LOAD_SUCCESS	0
#define FPGA_LOAD_ERROR		-1
#define FPGA_LOAD_INIT_ERROR	-2
#define FPGA_LOAD_TIMEOUT	-3
#define FPGA_LOAD_INVALID_BOARD_TYPE	-4

/* Board types. */
#define BOARD_TYPE_SFFSDR		0
#define BOARD_TYPE_FEMTO_BASE_STATION	1

int
bitstream_load( int board_type, void *mmio_addr, u8 *data, size_t size );

#endif /* BITSTREAM_LOAD_H */
