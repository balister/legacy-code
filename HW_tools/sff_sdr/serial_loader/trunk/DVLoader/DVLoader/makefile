#############################################################
# Makefile for uartapp project.                             #
#   Generates a binary file that can be sent over uart when #
#   the DM644x is set to UART boot mode.					#
#############################################################
# Auth: Daniel Allred (d-allred@ti.com)
# Vers: v1.0 on 13-Nov-2006

MONOCOMPILE=gmcs
DOTNETCOMPILE=csc

SOURCES=DVLoader.cs

EXECUTABLE=DVLoader.exe

.PHONY : all clean mono dotnet

all: $(SOURCES)
ifeq ($(shell uname),Linux)
		$(MONOCOMPILE) -t:exe -out:$(EXECUTABLE) $(SOURCES)
else
		$(DOTNETCOMPILE) /t:exe /out:$(EXECUTABLE) $(SOURCES)
endif

clean:
		-rm -f -v $(EXECUTABLE)

mono: $(SOURCES)
		$(MONOCOMPILE)  -t:exe -out:$(EXECUTABLE) $(SOURCES)
dotnet: $(SOURCES)
		$(DOTNETCOMPILE) /t:exe /out:$(EXECUTABLE) $(SOURCES)


