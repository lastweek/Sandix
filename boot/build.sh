#!	/bin/sh
#	2015/04/02 Created by Shan Yizhou.

i386-elf-as -o boot.o boot.s
i386-elf-objcopy -j .text -j .data -O binary boot.o boot

./fill_MBR boot
