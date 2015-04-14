#!	/bin/sh
#	2015/04/02 Created by Shan Yizhou.

i386-elf-gcc -o main.o -c main.c
i386-elf-as -o bootsect.o bootsect.s
i386-elf-as -o header.o header.s

i386-elf-ld -e _start -Ttext 0x9000 -o header.out header.o main.o
i386-elf-objcopy -j .text  -O binary header.out header

i386-elf-objcopy -j .text  -O binary bootsect.o bootsect

# 4M byte
./build 4

# Clean and Rename
rm *.o
rm header.out
mv bootsect bzimage

