#!	/bin/sh
#	2015/04/02 Created by Shan Yizhou.

# Debug stage: use main.s instead main.c to generate main.o
i386-elf-as -o main.o main.s
i386-elf-as -o bootsect.o bootsect.s
i386-elf-as -o header.o header.s

# -Ttext have to be configured? Should know more about ld...
i386-elf-ld -e _start -Ttext 0x0000 -o header.out header.o main.o
i386-elf-objcopy -j .text -j .rodata -O binary header.out header

i386-elf-objcopy -j .text  -O binary bootsect.o bootsect

# 4M byte
./build 4

# Clean and Rename
rm *.o
rm header.out
mv bootsect bzimage

