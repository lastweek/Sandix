#
# Real-Mode Kernel Image Makefile
#

loader-y := bootsect.o

obj-y := a20.o bioscall.o header.o main.o
obj-y += memory.o pm.o pmjump.o printf.o regs.o
obj-y += string.o tty.o video.o

objcopy-y := bootsect.o built-in.o
objdump-y := bootsect.o built-in.o

M16_CFLAGS := $(call cc-option,-m16)

KBUILD_CFLAGS := $(M16_CFLAGS) -std=gnu89 -march=i386
KBUILD_CFLAGS += -pipe -Wall -Wundef -fomit-frame-pointer
KBUILD_CFLAGS += -fno-pic -fno-strict-aliasing -fno-common
KBUILD_CFLAGS += -Wdeclaration-after-statement
KBUILD_CFLAGS += -Werror=strict-prototypes
KBUILD_CFLAGS += -Werror=implicit-function-declaration
KBUILD_CFLAGS += -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx
KBUILD_CFLAGS += -ffreestanding

KBUILD_LDFLAGS := --script=$(src)/setup.ld -e _start -Ttext 0x0000
