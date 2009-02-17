#############################################################
# Makefile for TI UBL project.                              #
#   Generates the binary UBL file can be used as the        #
#	secondary bootloader for the DM644x in NOR, NAND,   #
#	and UART boot modes.                                #
#############################################################
# Auth: Daniel Allred (d-allred@ti.com)
# Vers: v1.1 on 1-Feb-2007
# 		v1.11 on 7-Mar-2007
#		v1.12 on 14-Mar-2007

ifndef CROSS_COMPILE
CROSS_COMPILE=arm-linux-
endif

CC=$(CROSS_COMPILE)gcc
OBJCOPY=$(CROSS_COMPILE)objcopy
OBJDUMP=$(CROSS_COMPILE)objdump
INCLUDEDIR=../include

CFLAGS:=-c -Os -Wall -I$(INCLUDEDIR)

LDFLAGS=-Wl,-T$(LINKERSCRIPT) -nostdlib 
OBJCOPYFLAGS = -R .ddrram -R .ddrram2 --gap-fill 0xFF --pad-to 0x3800 -S

SOURCES := davinci.c uart.c uartboot.c ubl.c util.c gpio.c

# Board setup
ifeq ($(BOARD),evmdm6446)
	DEVICE   := DM6446
	FLASH    := nand
	DDR_TYPE := MICRON_MT47H64M16BT_3_162MHZ
	DDR_SIZE := DDR_SIZE_256MB
endif
ifeq ($(BOARD),sffsdr)
	DEVICE   := DM6446
	FLASH    := nand
	DDR_TYPE := MICRON_MT47H32M16BN_3_162MHZ
	DDR_SIZE := DDR_SIZE_128MB
endif
ifeq ($(BOARD),das)
	DEVICE   := DM6446
	FLASH    := nand
	DDR_TYPE := MICRON_MT47H64M16HR_3_162MHZ
	DDR_SIZE := DDR_SIZE_256MB
endif
ifeq ($(BOARD),minidas)
	DEVICE   := DM355
	FLASH    := nand
	DDR_TYPE := MICRON_MT47H128M16HG_3IT_171MHZ
	DDR_SIZE := DDR_SIZE_256MB
endif
ifeq ($(BOARD),afeusb)
	DEVICE   := DM355
	FLASH    := nand
	DDR_TYPE := MICRON_MT47H32M16BN_3_171MHZ
	DDR_SIZE := DDR_SIZE_64MB
endif
ifeq ($(BOARD),evmdm355)
	DEVICE   := DM355
	FLASH    := nand
	DDR_TYPE := MICRON_MT47H64M16BT_37E_171MHZ
	DDR_SIZE := DDR_SIZE_128MB
endif

CFLAGS += -D$(DDR_TYPE) -D$(DDR_SIZE)

# Processor setup
ifeq ($(DEVICE),DM6441)
	CFLAGS += -DDM6441
	SOURCES += dm644x.c
	LINKERSCRIPT = dm644x.lds
endif
ifeq ($(DEVICE),DM6441_LV)
	CFLAGS += -DDM6441_LV
	SOURCES += dm644x.c
	LINKERSCRIPT = dm644x.lds
endif
ifeq ($(DEVICE),DM6446)
	CFLAGS += -DDM6446
	SOURCES += dm644x.c
	LINKERSCRIPT = dm644x.lds
endif
ifeq ($(DEVICE),DM355)
	CFLAGS += -DDM355
	SOURCES += dm355.c
	LINKERSCRIPT = dm355.lds
endif

# NAND flash setup
ifeq ($(FLASH),nand)
	CFLAGS+= -DUBL_NAND
	SOURCES += nandboot.c nand.c
endif
ifeq ($(FLASH),nor)
	CFLAGS+= -DUBL_NOR
	SOURCES += norboot.c nor.c
endif

OBJECTS:=$(patsubst %.c,%_$(FLASH).o,$(SOURCES))
EXECUTABLE:=ubl_davinci_$(FLASH)
BINARY:=../$(EXECUTABLE).bin
START:=../$(EXECUTABLE)_start_addr.txt

all: $(BINARY) $(START)

.PHONY : clean objclean
clean:
	-rm -f -v *.o $(BINARY) $(EXECUTABLE) $(START)
	-rm -f -v *~

objclean:
	-rm -f -v $(OBJECTS)

$(START): $(EXECUTABLE)
	@$(OBJDUMP) -t $< | grep -E '[0-9a-f]{8}*[0-9a-f]{8} boot' | sed  's/^\([0-9a-f]\{4\}\)\([0-9a-f]\{4\}\).*/\2/' > $@

$(BINARY): $(EXECUTABLE)
	$(OBJCOPY) $(OBJCOPYFLAGS) -O binary $< $@
	@echo -n "The entry point of the binary is 0x"
	@$(OBJDUMP) -t $< | grep -E '[0-9a-f]{8}*[0-9a-f]{8} boot' | sed  's/^\([0-9a-f]\{4\}\)\([0-9a-f]\{4\}\).*/\2/'

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%_$(FLASH).o : %.c $(wildcard *.h)
	$(CC) $(CFLAGS) $< -o $@

vpath %.h $(INCLUDEDIR)
