/*
 * (C) Copyright 2008
 * Lyrtech inc. <www.lyrtech.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef	_DVREGS_H
#define _DVREGS_H

#define DIR01		0x10	/* Direction control */
#define OUT_DATA01	0x14	/* Output data */
#define SET_DATA01	0x18	/* Set data */
#define CLR_DATA01	0x1C	/* Clear data */
#define IN_DATA01	0x20	/* Input data */
#define SET_RIS_TRIG01	0x24	/* Set rising edge intr */
#define CLR_RIS_TRIG01	0x28	/* Clear rising edge intr */
#define SET_FAL_TRIG01	0x2C	/* Set falling edge intr */
#define CLR_FAL_TRIG01	0x30	/* Clear falling edge intr */
#define INSTAT01	0x34	/* Intr status */

#define DIR23		0x38	/* Direction control */
#define OUT_DATA23	0x3C	/* Output data */
#define SET_DATA23	0x40	/* Set data */
#define CLR_DATA23	0x44	/* Clear data */
#define IN_DATA23	0x48	/* Input data */
#define SET_RIS_TRIG23	0x4C	/* Set rising edge intr */
#define CLR_RIS_TRIG23	0x50	/* Clear rising edge intr */
#define SET_FAL_TRIG23	0x54	/* Set falling edge intr */
#define CLR_FAL_TRIG23	0x58	/* Clear falling edge intr */
#define INSTAT23	0x5C	/* Intr status */

#define DIR4		0x60	/* Direction control */
#define OUT_DATA4	0x64	/* Output data */
#define SET_DATA4	0x68	/* Set data */
#define CLR_DATA4	0x6C	/* Clear data */
#define IN_DATA4	0x70	/* Input data */
#define SET_RIS_TRIG4	0x74	/* Set rising edge intr */
#define CLR_RIS_TRIG4	0x78	/* Clear rising edge intr */
#define SET_FAL_TRIG4	0x7C	/* Set falling edge intr */
#define CLR_FAL_TRIG4	0x80	/* Clear falling edge intr */
#define INSTAT4		0x84	/* Intr status */

#define GPIO_REG_BEGIN	DIR01
#define GPIO_REG_END	INSTAT4

#endif /* _DVREGS_H */
