all:
		$(MAKE) -C src FLASH=nand DEVICE=DM6446
		$(MAKE) -C src FLASH=nor DEVICE=DM6446

DM6441:
		$(MAKE) -C src FLASH=nand DEVICE=DM6441
		$(MAKE) -C src FLASH=nor DEVICE=DM6441

DM6441_LV:
		$(MAKE) -C src FLASH=nand DEVICE=DM6441_LV
		$(MAKE) -C src FLASH=nor DEVICE=DM6441_LV

dm355:
		$(MAKE) -C src FLASH=nand DEVICE=DM355
		$(MAKE) -C src FLASH=nor DEVICE=DM355

clean:
		$(MAKE) -C src FLASH=nand clean
		$(MAKE) -C src FLASH=nor clean
%::
		$(MAKE) -C src FLASH=nand $@
		$(MAKE) -C src FLASH=nor $@
