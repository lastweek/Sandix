/*
 *	Copyright (C) 2015-2016 Yizhou Shan <shan13@purdue.edu>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 */

#include <sandix/kernel.h>

void panic(const char *fmt, ...) {
	char buf[1024];
	va_list args;

	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);
	
	pr_emerg("----------------[ cut here ] ----------------\n\r"
		 "Kernel panic: %s", buf);

	for(;;);
}
EXPORT_SYMBOL(panic);
