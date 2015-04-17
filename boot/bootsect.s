#########################################################################
# 2015/04/01. Created by Shan Yizhou.
#
# bootsect.s: Bootloader for Sandix.
#
#########################################################################
# bootsect.s is the conventional MBR sector of disk, which is loaded
# by BIOS. The tiny bootloader will load the 16-bit setup image and
# 32-bit kernel image to memory. After a successful migration from
# memory, it just transfer control to 16-bit setup image.(2015/04/11)
#########################################################################	


BOOTSEG = 0x07c0    # bootsect.s segment.
INITSEG = 0x9000    # 16-bit setup image segment.
SYSSEG  = 0X1000    # 32-bit kernel image segment.
SETUP_OFFSET = 512  # offset of entry point in setup image.
SECTORS = 6         # sectors of setup image need to be load
#KERNELS = ?        # sectors of kernel image

	.code16
	
	.text
	.globl bs_start_msg
bs_start_msg:
	movw $BOOTSEG, %ax # indeed
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss
	
	movb $0x03, %ah	# read cursor pos
	xor %bh, %bh
	int $0x10
	
	movw $(setup_fail_msg-start_msg), %cx # length of msg.
	movw $0x0007, %bx # page 0, attribute 7.
	movw $start_msg, %bp # pointer to msg.
	movw $0x1301, %ax # write msg, move cursor.
	int $0x10

bs_load_setup:
	xor %ah, %ah	# reset FDC
	xor %dl, %dl
	int $0x13
	
	# Load setup image
	mov $INITSEG, %ax
	mov %ax, %es
	xor %bx, %bx	# dest mem-->es:bx

	mov $0x02, %ah		# service 2
	mov $SECTORS, %al	# nr of sectors
	xor %dx, %dx		# drive 0, head 0
	mov $0x80, %dl
	mov $2, %cl			# sector 2, track 0
	int $0x13
	jc bs_setup_fail	# CF set on error, go die

	ljmp $INITSEG, $SETUP_OFFSET

bs_load_kernel:
	xor %ah, %ah	# reset FDC
	xor %dl, %dl
	int $0x13
	
	# Load kernel image.
	# FIXME
	
	#############################
	# That's all for bootloader!#
	# Go to 16bit setup!		#
	#############################
bs_success_go:
	ljmp $INITSEG, $SETUP_OFFSET

bs_setup_fail:
	movw $BOOTSEG, %ax # why have to do this???
	movw %ax, %ds
	movw %ax, %es
	
	movb $0x03, %ah	
	xor %bh, %bh
	int $0x10	# read cursor pos
	movw $(kernel_fail_msg-setup_fail_msg), %cx
	movw $0x0007, %bx
	movw $setup_fail_msg, %bp
	movw $0x1301, %ax 
	int $0x10	# write string
	jmp bs_die

bs_kernel_fail:
	movw $BOOTSEG, %ax # why have to do this???
	movw %ax, %ds
	movw %ax, %es
	
	movb $0x03, %ah	
	xor %bh, %bh
	int $0x10	# read cursor pos
	movw $(bs_die-kernel_fail_msg), %cx
	movw $0x0007, %bx
	movw $kernel_fail_msg, %bp
	movw $0x1301, %ax 
	int $0x10	# write string
	jmp bs_die
	
start_msg:
	.ascii "\n\rMBR: Loading System..."
setup_fail_msg:
	.ascii "\n\rMBR: Loading setup fail..."
kernel_fail_msg:
	.ascii "\n\rMBR: Loading kernel fail..."
bs_die:
	hlt
	jmp bs_die
bs_end:
	nop
