#	2015/04/02 Created by Shan Yizhou.
#	Real-Mode Makefile.

AS = i386-elf-as
CC = i386-elf-gcc
LD = i386-elf-ld
OBJCOPY = i386-elf-objcopy

# 1. Use eax edx ecx to pass parameters(Less instructions, more efficiency).
# 2. Warn if a function is declared or defined without specifying the argument types.
# 3. Prevent gcc from keeping the stack 16 byte aligned. Taken from i386.
# 4. In a freestanding enviroment. No library, No builtin.
REALMODE_CFLAGS = -c -Wall -pipe -D__KERNEL__ -march=i386\
		-mregparm=3 \
		-Wstrict-prototypes \
		-mpreferred-stack-boundary=2 \
		-ffreestanding \
		-I../include/

# Entry point _start in header.o
# Default start address 0x0.
LDFLAGS = -e _start \
		-Ttext 0x0000 \
		-o header.out

# Retain .text .rodata
# Generate raw binary file.
OBJCOPYFLAGS = -j .text -j .rodata -O binary

SOURCES_AS = header.S 
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
	
	$(PWD)/tools/build 4
	
	mv bootsect bzimage

clean:
	rm *.o
	rm header.out
test:
	echo $(OBJECTS)
	echo $(REALMODE_CFLAGS)
