/*
 *	2015/04/29 Created by Shan Yizhou.
 *	init.c: Init everything.
 */

#include <sandix/const.h>
#include <sandix/types.h>
#include <sandix/screen_info.h>
#include <sandix/e820.h>
#include <sandix/bootparam.h>

/*
 *	We got %esi points to the real-mode boot_param struct.
 *	Get information about system from boot_param, and then
 *	pass them to corresponding initial function.
 */

void kernel_init(void)
{
	
}
