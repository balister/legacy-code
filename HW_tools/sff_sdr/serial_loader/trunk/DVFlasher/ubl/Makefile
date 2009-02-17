all:
	@echo "You must select a board"; exit 1

evmdm6446:
	$(MAKE) -C src BOARD=$(MAKECMDGOALS)

sffsdr:
	$(MAKE) -C src BOARD=$(MAKECMDGOALS)

das:
	$(MAKE) -C src BOARD=$(MAKECMDGOALS)

evmdm355:
	$(MAKE) -C src BOARD=$(MAKECMDGOALS)

minidas:
	$(MAKE) -C src BOARD=$(MAKECMDGOALS)

afeusb:
	$(MAKE) -C src BOARD=$(MAKECMDGOALS)

clean:
	$(MAKE) -C src FLASH=nand clean
	$(MAKE) -C src FLASH=nor clean
