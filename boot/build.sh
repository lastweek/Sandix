#!	/bin/sh
#	2015/04/02 Created by Shan Yizhou.

i386-elf-as -o bootsect.o bootsect.s
i386-elf-objcopy -j .text -j .data -O binary bootsect.o bootsect

./fill_MBR bootsect
