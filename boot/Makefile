#	2015/04/02 Created by Shan Yizhou.
#	Real-Mode Kernel Image Makefile.

# Toolchain for build
AS = i386-elf-as
CC = i386-elf-gcc
LD = i386-elf-ld
OBJCOPY = i386-elf-objcopy
OBJDUMP = i386-elf-objdump
CATENATE = $(PWD)/build

# FLAGS
REALMODE_CFLAGS = -c -Wall -Wextra -pipe -D__RM__KERNEL__ -march=i386 \
		-Wstrict-prototypes -mpreferred-stack-boundary=2 -ffreestanding \
		-fno-common -fno-pic -I../include/

LDOFILE = header.ldout
LDFLAGS = --script=setup.ld -e _start -Ttext 0x0000 -o $(LDOFILE)

OBJCOPYFLAGS = -j .text -j .text32 -j .rodata -j .data -O binary

OBJDUMPFLAGS_REALMODE = -d -M i8086,att

# FIXME TOO UGLY, some better way?
SOURCES_CC = main.c tty.c regs.c string.c printf.c a20.c memory.c video.c pm.c
OBJECTS_CC = $(SOURCES_CC:.c=.o)

OBJECTS_CC := header.o $(OBJECTS_CC) bioscall.o pmjump.o
SOURCES_CC := header.S $(SOURCES_CC) bioscall.S pmjump.S

SOURCES_LD = $(OBJECTS_CC)


# Time for work
all: $(SOURCES_CC) cat

cat: objcp image
	$(CATENATE) 
	mv bootsect bzimage

objcp: ld bootsect
	$(OBJCOPY) $(OBJCOPYFLAGS) $(LDOFILE) header
	$(OBJCOPY) $(OBJCOPYFLAGS) bootsect.o bootsect

ld: $(SOURCES_LD)
	$(LD) $(LDFLAGS) $(SOURCES_LD)

$(OBJECTS_CC): boot.h string.h
	$(CC) $(REALMODE_CFLAGS) $(SOURCES_CC)

bootsect: bootsect.S
	$(AS) -o bootsect.o bootsect.S


# Dump the Object File
dump:
	$(OBJDUMP) $(OBJDUMPFLAGS_REALMODE) $(LDOFILE) > $(LDOFILE).dump
	ndisasm -b 16 bzimage > bzimage.dump


# Clean Everything.
clean: cleanobj cleandump

cleanobj:
	rm -f *.o
	rm -f $(LDOFILE)
	rm -f bzimage
	rm -f header

cleandump:
	rm -f *.dump
