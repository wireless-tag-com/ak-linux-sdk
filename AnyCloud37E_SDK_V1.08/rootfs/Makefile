image:
	@if [ $(FLASH_TYPE) = "NOR" ] ; then ./create_squashfs.sh; ./create_jffs2fs.sh; fi;
	@if [ $(FLASH_TYPE) = "NAND" ] ; then ./create_yaffs2fs.sh; fi;
