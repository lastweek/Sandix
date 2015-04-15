#	2015/04/02 Created by Shan Yizhou.
#	Real-Mode Makefile.

AS = i386-elf-as
CC = i386-elf-gcc
LD = i386-elf-ld
OBJCOPY = i386-elf-objcopy
CFLAGS =
LDFLAGS = 

make:
	$(AS) -o main.o main.s
	$(AS) -o bootsect.o bootsect.s
	$(AS) -o header.o header.s

	$(LD) -e _start -Ttext 0x0000 -o header.out header.o main.o
	
	$(OBJCOPY) -j .text -j .rodata -O binary header.out header
	$(OBJCOPY) -j .text  -O binary bootsect.o bootsect
	
	$(PWD)/build 4

clean:
	rm *.o
	rm header.out
	mv bootsect bzimage

