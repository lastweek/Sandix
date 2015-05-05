#	2014/05/01 Created by Shan Yizhou.
#	Protected-Mode Kernel Image Makefile.

# Toolchain for build
AS = i386-elf-as
CC = i386-elf-gcc
LD = i386-elf-ld
OBJCOPY = i386-elf-objcopy
OBJDUMP = i386-elf-objdump

# FLAGS
CFLAGS = -Wall -Wextra -pipe -D__PM__KERNEL__ \
		-ffreestanding -fno-common -fno-pic -I../include/
LDFLAGS =

# FILES
SOURCES_CC = head.S
OBJECTS_CC = head.o

all:
	$(CC)
