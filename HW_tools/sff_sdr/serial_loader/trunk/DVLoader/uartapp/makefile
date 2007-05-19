#############################################################
# Makefile for uartapp project.                             #
#   Generates a binary file that can be sent over uart when #
#   the DM644x is set to UART boot mode.							#
#############################################################
# Auth: Daniel Allred (d-allred@ti.com)
# Vers: v1.0 on 08-Nov-2006
CROSSCOMPILE=arm_v5t_le-
CC=$(CROSSCOMPILE)gcc
OBJCOPY=$(CROSSCOMPILE)objcopy
OBJDUMP=$(CROSSCOMPILE)objdump

CFLAGS=-c -Os -Wall
LDFLAGS=-Wl,-T$(LINKERSCRIPT) -nostdlib
OBJCOPYFLAGS = -R .aemif -R .ddrram --gap-fill 0xFF --pad-to 0x3800 -S

SOURCES=uart.c dm644x.c uartapp.c
OBJECTS=$(SOURCES:.c=.o)

LINKERSCRIPT=uartapp.lds
EXECUTABLE=uartapp
BINARY=uartapp.bin

all: $(BINARY)

.PHONY : clean
clean:
		-rm -f -v $(OBJECTS) $(BINARY) $(EXECUTABLE)

$(BINARY): $(EXECUTABLE)
		$(OBJCOPY) $(OBJCOPYFLAGS) -O binary $< $@
		@echo -n "The entry point of the binary is 0x"
		@$(OBJDUMP) -t $(EXECUTABLE) | grep -E '[0-9a-f]{8}*[0-9a-f]{8} boot' | sed  's/\([0-9a-f]*\).*/\1/'


$(EXECUTABLE): $(OBJECTS)
		$(CC) $(LDFLAGS) $(OBJECTS) -o $@
		
.c.o :
		$(CC) $(CFLAGS) $< -o $@
		

