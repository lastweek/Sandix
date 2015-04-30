#	2015/04/02 Created by Shan Yizhou.
#	Real-Mode Makefile.

# Toolchain for build
AS = i386-elf-as
CC = i386-elf-gcc
LD = i386-elf-ld
OBJCOPY = i386-elf-objcopy
OBJDUMP = i386-elf-objdump
CATENATE = $(PWD)/build

# FLAGS
REALMODE_CFLAGS = -c -Wall -Wextra -pipe -D__RL__KERNEL__ -march=i386 \
		-Wstrict-prototypes -mpreferred-stack-boundary=2 -ffreestanding \
		-fno-common -fno-pic -I../include/

LDOFILE = header.ldout
LDFLAGS =  --script=setup.ld \
		-e _start -Ttext 0x0000 -o $(LDOFILE)

OBJCOPYFLAGS = -j .text -j .text32 -j .rodata -j .data -O binary

OBJDUMPFLAGS_REALMODE = -d -M i8086,att

# FIXME TOO UGLY, some better way?
SOURCES_CC = header.S main.c tty.c bioscall.S regs.c string.c printf.c \
			 a20.c memory.c video.c pm.c pmjump.S
OBJECTS_CC = header.o main.o tty.o bioscall.o regs.o string.o printf.o \
			 a20.o memory.o video.o pm.o pmjump.o
SOURCES_LD = $(OBJECTS_CC)

# FIXME ADD DEPENDENCE.
s1:
	#
	# STEP1 ---> COMPILE
	#
	$(AS) -o bootsect.o bootsect.S
	$(CC) $(REALMODE_CFLAGS) $(SOURCES_CC)
	#
	# STEP2 ---> LINK
	#
	$(LD) $(LDFLAGS) $(SOURCES_LD)
	#
	# STEP3 ---> OBJCOPY
	#
	$(OBJCOPY) $(OBJCOPYFLAGS) $(LDOFILE) header
	$(OBJCOPY) $(OBJCOPYFLAGS) bootsect.o bootsect
	#
	# STEP4 ---> CATENATE
	#
	$(CATENATE) 
	mv bootsect bzimage

dump:
	#
	# STEP ---> DUMP 
	#
	$(OBJDUMP) $(OBJDUMPFLAGS_REALMODE) $(LDOFILE) > $(LDOFILE).dump
	ndisasm -b 16 bzimage > bzimage.dump
	
clean:
	rm -f *.o
	rm -f *.dump
	rm -f $(LDOFILE)
	rm -f bzimage
	rm -f header
