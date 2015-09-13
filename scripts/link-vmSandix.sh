#!/bin/bash
#
#

set -e;

# Beautiful output in kbuild format
# Will be suppressed by "make -s"
info()
{
	if [ "${quiet}" != "silent_" ]; then
		printf "  %-7s %s\n" ${1} ${2}
	fi
}

cleanup()
{
	echo "Hello!!!!"
}

# Enable Debug Mode; Print commands.
if [ "${KBUILD_VERBOSE}" = "1" ]; then
	set -x;
fi

# Cleaning
if [ "$1" == "clean" ]; then
	info CLEAN vmSandix
	cleanup
	exit 0
fi
