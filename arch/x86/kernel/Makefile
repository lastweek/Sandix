# Note:
# Using $(extra-y) so Makefile.build and Makefile.clean know
# what they should do (This is part of extra-y's job, actually).

# $(head-y) part defined in arch/x86/Makefile
extra-y := head_32.o

# linker script
# it will be handled by Makefile.build
extra-y += vmSandix.ld

obj-y := entry.o
obj-y += setup.o
obj-y += traps.o
obj-y += e820.o
