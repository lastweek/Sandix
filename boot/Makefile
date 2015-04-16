#	2015/04/02 Created by Shan Yizhou.
#	Real-Mode Makefile.

AS = i386-elf-as
CC = i386-elf-gcc
LD = i386-elf-ld
OBJCOPY = i386-elf-objcopy

# Something about -mregparm=3:
#	Real-Mode call instruction push 2 bytes IP into stack. When procedure AA
#	make a call to procedure BB, the first argument AA passes to BB is located
#	at 6(%ebp). However, GCC donot know this! In GCC's mind, the first argument
#	should be at 8(%ebp). Using -mregparam to pass arguments by registers avoids
#	such fatal problem. See code yourself.
# 1. Use eax edx ecx to pass parameters(Less instructions, more efficiency).
# 2. Warn if a function is declared or defined without specifying the argument types.
# 3. Prevent gcc from keeping the stack 16 byte aligned. Taken from i386.
# 4. In a freestanding enviroment. No library, No builtin.
REALMODE_CFLAGS = -S -Wall -Wextra -pipe -D__KERNEL__ -march=i386\
		-mregparm=3 \
		-Wstrict-prototypes  \
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

SOURCES_AS = bootsect.S header.S bioscall.S
SOURCES_CC = main.c tty.c regs.c string.c
OBJECTS_AS = $(SOURCES_AS:.S=.o)
OBJECTS_CC = $(SOURCES_CC:.c=.o)

# FIXME

make:
	$(CC) $(REALMODE_CFLAGS) -o main.o main.c
	$(CC) $(REALMODE_CFLAGS) -o tty.o tty.c
	$(CC) $(REALMODE_CFLAGS) -o regs.o regs.c
	$(CC) $(REALMODE_CFLAGS) -o string.o string.c
	
	$(AS) -o header.o header.S
	$(AS) -o bioscall.o bioscall.S
	$(AS) -o bootsect.o bootsect.S

	$(LD) $(LDFLAGS) header.o $(OBJECTS_CC) bioscall.o
	
	$(OBJCOPY) $(OBJCOPYFLAGS) header.out header
	$(OBJCOPY) $(OBJCOPYFLAGS) bootsect.o bootsect

	$(PWD)/build 4
	mv bootsect bzimage
test1:
	$(CC) $(REALMODE_CFLAGS) main.c
	$(CC) $(REALMODE_CFLAGS) tty.c
test2:
	$(AS) -o main.o main.s
	$(AS) -o tty.o tty.s
	$(AS) -o header.o header.S
	$(AS) -o bootsect.o bootsect.S

	$(LD) $(LDFLAGS) header.o main.o tty.o
	
	$(OBJCOPY) $(OBJCOPYFLAGS) header.out header
	$(OBJCOPY) $(OBJCOPYFLAGS) bootsect.o bootsect

	$(PWD)/build 4
	mv bootsect bzimage

clean:
	rm *.o
	rm *.s
	rm header.out
	rm header


