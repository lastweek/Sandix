#	2015/05/01 Created by Shan Yizhou.
#	Top Makefile for Sandix.

# Toolchain for build
AS = i386-elf-as
CC = i386-elf-gcc
LD = i386-elf-ld
OBJCOPY = i386-elf-objcopy
OBJDUMP = i386-elf-objdump
MAKE = make
MAKEARGS = 
SUBDIRS = boot kernel

all-subdirs:
	-for dir in $(SUBDIRS); do \
		(cd $$dir; $(MAKE) $(MAKEARGS); cd ..); \
	done

clean:
	-for dir in $(SUBDIRS); do \
		(cd $$dir; $(MAKE) clean; cd ..); \
	done
