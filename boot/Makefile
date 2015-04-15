#	2015/04/02 Created by Shan Yizhou.
#	Real-Mode Makefile.

AS = i386-elf-as
CC = i386-elf-gcc
LD = i386-elf-ld
OBJCOPY = i386-elf-objcopy
CFLAGS = -c -Wall -mregparm=3
LDFLAGS = -e _start -Ttext 0x0000 -o header.out
OBJCOPYFLAGS = -j .text -j .rodata -O binary
SOURCES_AS = header.S bioscall.S
SOURCES_CC = main.c
OBJECTS = $(SOURCES_AS:.S=.o) $(SOURCES_CC:.c=.o)

make:
	$(AS) -o main.o main.s
	$(AS) -o bootsect.o bootsect.S
	$(AS) -o header.o header.S
	$(AS) -o bioscall.o bioscall.S

	$(LD) $(LDFLAGS) $(OBJECTS)
	
	$(OBJCOPY) $(OBJCOPYFLAGS) header.out header
	$(OBJCOPY) $(OBJCOPYFLAGS) bootsect.o bootsect
	
	$(PWD)/build 4

clean:
	rm *.o
	rm header.out
	mv bootsect bzimage
test:
	echo $(OBJECTS)
