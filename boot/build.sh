#!	/bin/sh
#	2015/04/02 Created by Shan Yizhou.
#	Hope u are using gas.

as -o boot.o boot.s
objcopy -j .text -j .date -O binary boot.o boot
