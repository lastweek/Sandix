#!/bin/bash
#
# Link vmSandix
#
# System.map is generated to document addresses of all kernel symbols
#
# vmSandix is linked from the objects selected by $(KBUILD_VMLINUX_INIT) and
# $(KBUILD_VMLINUX_MAIN). Most are built-in.o files from top-level directories
# in the kernel tree, others are specified in arch/$(ARCH)/Makefile.
# Ordering when linking is important, and $(KBUILD_VMLINUX_INIT) must be first.
#
# vmSandix
#   ^
#   |
#   +-< $(KBUILD_VMLINUX_INIT)
#   |   +--< arch/${SRCARCH}/kernel/head.o + init/main.o + more
#   |
#   +--< $(KBUILD_VMLINUX_MAIN)
#   |    +--< drivers/built-in.o mm/built-in.o + more

set -e

# Beautiful output in kbuild format
# Will be suppressed by "make -s"
info()
{
	if [ "${quiet}" != "silent_" ]; then
		printf "  %-7s %s\n" ${1} ${2}
	fi
}

vmSandix_ld()
{
	local lds="${objtree}/${KBUILD_VMSANDIX_LDS}"

	${LD} -T ${lds} -o ${1}			\
		${KBUILD_VMSANDIX_INIT}		\
		--start-group			\
		${KBUILD_VMSANDIX_MAIN}		\
		--end-group
}

mksysmap()
{
	${NM} -n ${1} > ${2}
}

cleanup()
{
	rm -f .vmSandix.cmd
	rm -f System.map
	rm -f vmSandix
	rm -f arch/${SRCARCH}/boot/System.map
	rm -f arch/${SRCARCH}/boot/vmSandix
}

##
# Enable Debug Mode; Print commands.
#
if [ "${KBUILD_VERBOSE}" = "1" ]; then
	set -x;
fi

##
# Link vmSandix
#
if [ "$1" == "LD" ]; then
	vmSandix_ld vmSandix

	info SYSMAP System.map
	mksysmap vmSandix System.map
	
	##
	# Copy to make boot/Makefile happy
	#
	cp vmSandix arch/${SRCARCH}/boot/
	cp System.map arch/${SRCARCH}/boot/
fi

##
# Cleaning
#
if [ "$1" == "clean" ]; then
	info CLEAN vmSandix
	cleanup
	exit 0
fi

