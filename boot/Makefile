#	2015/04/02 Created by Shan Yizhou.
#	Real-Mode Makefile.

AS = i386-elf-as
CC = i386-elf-gcc
LD = i386-elf-ld
OBJCOPY = i386-elf-objcopy

# Use eax edx ecx to pass parameters(Less instructions, more efficiency).
# Prevent gcc from keeping the stack 16 byte aligned. Taken from i386.
# In a freestanding enviroment. No library, No builtin.
REALMODE_CFLAGS = -c -Wall -pipe \
		-mregparm=3 \
		-mpreferred-stack-boundary=2 \
		-ffreestanding

# Entry point _start in header.o
# Default start address 0x0.
LDFLAGS = -e _start \
		-Ttext 0x0000 \
		-o header.out

# Retain .text .rodata
# Generate raw binary file.
OBJCOPYFLAGS = -j .text -j .rodata -O binary

SOURCES_AS = header.S bioscall.S
SOURCES_CC = main.c
OBJECTS = $(SOURCES_AS:.S=.o) $(SOURCES_CC:.c=.o)

# FIXED
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
	echo $(REALMODE_CFLAGS)
