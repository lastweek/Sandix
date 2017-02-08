`I'm not developing this project anymore. Instead, now I'm coding another
kernel project from scratch for Wuklab. I spent a lof of time in Sandix,
and suffered enormous pain alone. But I love it, I learned a lot, truly.
Thank you and good bye.`

##			SANDIX KERNEL PROJECT
###	Copyright (C) 2015-2016 Yizhou Shan <shan13@purdue.edu>

WHAT IS SANDIX?

	- Sandix is a clone of the operating system Unix, written based
	  on Linux kernel(Mimic everything in a simple implementation).
	  Sandix is the abbreviation for Sandy Unix

	- Just mimic everything, no innovative things about OS maybe. :)
	  But I do hope I could bring something new when I learning and
	  coding Sandix.

WHO IS DEVELOPING SANDIX?

	- Yizhou Shan <shan13@purdue.edu>
	- Architecture Contributors are very welcomed.
	- For more information, email me. ;)
	- Personal site: http://qiuqiuxu.com

BUILD INSTRUCTIONS:
	
	Hope you have cross-compiler and cross-binutils if you are
	building the kernel at platforms besides 32-bit x86:
		$(CROSS_COMPILE)-gcc
		$(CROSS_COMPILE)-as
		$(CROSS_COMPILE)-ld
		$(CROSS_COMPILE)-nm
		$(CROSS_COMPILE)-objcopy
		$(CROSS_COMPILE)-objdump

	- 1) Config Kernel
		make config
	
	- 2) Build Kernel
		make
	
	- 3) Install Kernel
		make install

	- 4) Clean
		make mrproper
		make clean

CAVEAT:

	- THE KERNEL IS STILL UNDER DEVELOPMENT. NOT USABLE.

LICENSE:

	- GNU GENERAL PUBLIC LICENSE Version 2
