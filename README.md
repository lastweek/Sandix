#			SANDIX KERNEL PROJECT
## You do not really understand something unless you can explain it to your grandmother.
		Copyright (C) 2015 Yizhou Shan <shanyizhou@ict.ac.cn>

WHAT IS SANDIX?

	- Sandix is a clone of the operating system Unix, written based
	  on Linux kernel(Mimic everything in a simple implementation).
	  Sandix is the abbreviation for Sandy Unix (www.sandix.cn).

WHO IS DEVELOPING SANDIX?

	- Yizhou Shan <shanyizhou@ict.ac.cn>
	- Architecture Contributors are very welcomed.
	- For more information, Email me. ;)

BUILD INSTRUCTIONS:
	
	Hope you have cross-compiler and cross-binutils:
		i386-elf-gcc
		i386-elf-as
		i386-elf-ld
		i386-elf-nm
		i386-elf-objcopy
		i386-elf-objdump

	- 1) Config Kernel
		make config
	    or  make menuconfig
	
	- 2) Build Kernel
		make
	    or  make all

	- 3) Cleanup
		make mrproper
		make clean

	- 4) Invoke make using mixed targets, e.g.:
		make config all
		make defconfig all

CAVEAT:

	- THE KERNEL IS STILL UNDER DEVELOPMENT. NOT USABLE.

LICENSE:

	- GNU GENERAL PUBLIC LICENSE Version 2

