#
# Real-Mode Kernel Image Makefile
#

loader-y := bootsect.o

# header.o main.o MUST come first
obj-y := header.o
obj-y += main.o
obj-y += a20.o
obj-y += bioscall.o
obj-y += string.o
obj-y += tty.o
obj-y += video.o
obj-y += memory.o
obj-y += printf.o
obj-y += regs.o
obj-y += pm.o
obj-y += pmjump.o 

objcopy-y := bootsect.o
#objdump-y := bootsect.o built-in.o

M16_CFLAGS := $(call cc-option,-m16)

KBUILD_CFLAGS := $(M16_CFLAGS) -std=gnu89 -march=i386
KBUILD_CFLAGS += -pipe -Wall -Wundef -fomit-frame-pointer
KBUILD_CFLAGS += -fno-pic -fno-strict-aliasing -fno-common
KBUILD_CFLAGS += -Wdeclaration-after-statement
KBUILD_CFLAGS += -Werror=strict-prototypes
KBUILD_CFLAGS += -Werror=implicit-function-declaration
KBUILD_CFLAGS += -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx
KBUILD_CFLAGS += -ffreestanding
