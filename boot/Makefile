#	2015/04/02 Created by Shan Yizhou.
#	Real-Mode Makefile.

# Toolchain for build
AS = i386-elf-as
CC = i386-elf-gcc
LD = i386-elf-ld
OBJCOPY = i386-elf-objcopy
OBJDUMP = i386-elf-objdump
CATENATE = $(PWD)/build

# FIXME Tentative size: 4M
REALMODE_IMAGESIZE = 4

# 0. -march=i386 works???
# 1. Warn if a function is declared or defined without specifying the argument types.
# 2. Prevent gcc from keeping the stack 16 byte aligned. Taken from i386.
# 3. In a freestanding enviroment. No library, No builtin.
REALMODE_CFLAGS = -c -Wall -Wextra -pipe -D__RL__KERNEL__ -march=i386 \
		-Wstrict-prototypes  \
		-mpreferred-stack-boundary=2 \
		-ffreestanding \
		-I../include/

# 0. LD Output FILE name is: header.ldout
# 1. Entry point _start is in header.o
# 2. Default start address is 0x0, cause header lays at 0x9000:0000.
LDOFILE = header.ldout
LDFLAGS = -e _start -Ttext 0x0000 -o $(LDOFILE)

# FIXME: some more sections to reserve?
# 0. Reserve sections: .text .rodata
# 1. Generate raw binary file.
OBJCOPYFLAGS = -j .text -j .rodata -O binary

# 0. OBJDUMP flags for Real-Mode object files.
OBJDUMPFLAGS_REALMODE = -d -M i8086,att,suffix

# FIXME TOO UGLY, some better way?
# FIXME Does the order of files input to LD matters?
SOURCES_CC = header.S main.c tty.c bioscall.S regs.c string.c printf.c
OBJECTS_CC = header.o main.o tty.o bioscall.o regs.o string.o printf.o
SOURCES_LD = $(OBJECTS_CC)

# FIXME ADD DEPENDENCE.
s1:
	$(AS) -o bootsect.o bootsect.S
	$(CC) $(REALMODE_CFLAGS) $(SOURCES_CC)

	$(LD) $(LDFLAGS) $(SOURCES_LD)

	$(OBJCOPY) $(OBJCOPYFLAGS) $(LDOFILE) header
	$(OBJCOPY) $(OBJCOPYFLAGS) bootsect.o bootsect
	
	$(CATENATE) $(REALMODE_IMAGESIZE)
	mv bootsect bzimage

dump:
	$(OBJDUMP) $(OBJDUMPFLAGS_REALMODE) $(LDOFILE) > $(LDOFILE).dump
	ndisasm -b 16 bzimage > bzimage.dump
clean:
	rm -f *.o
	rm -f *.dump
	rm -f $(LDOFILE)
	rm -f bzimage
	rm -f header
