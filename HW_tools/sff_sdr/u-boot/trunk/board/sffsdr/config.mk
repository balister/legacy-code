#
# (C) Copyright 2002
# Gary Jennejohn, DENX Software Engineering, <gj@denx.de>
# David Mueller, ELSOFT AG, <d.mueller@elsoft.ch>
#
# (C) Copyright 2003
# Texas Instruments, <www.ti.com>
# Swaminathan <swami.iyer@ti.com>
#
# Updated for Lyrtech SFF SDR board (ARM925EJS) cpu
# (C) Copyright 2007
# 
# Philip Balister <philip@opensdr.com>
#
# see http://www.ti.com/ for more information on Texas Instruments
#
# Lyrtech SFF SDR has 1 bank of 128 MB DDR RAM 
# Physical Address:
# 8000'0000 to 8800'0000
#
#
# Linux-Kernel is expected to be at 8000'8000, entry 8000'8000
# (mem base + reserved)
#
# we load ourself to 8100 '0000
#
#

#Provide a atleast 16MB spacing between us and the Linux Kernel image
TEXT_BASE = 0x81080000
