#!	/bin/sh
#	2015/04/02 Created by Shan Yizhou.

i386-elf-as -o bootsect.o bootsect.s
i386-elf-as -o header.o header.s
i386-elf-objcopy -j .text  -O binary bootsect.o bootsect
i386-elf-objcopy -j .text  -O binary header.o header

rm *.o

./build 4
