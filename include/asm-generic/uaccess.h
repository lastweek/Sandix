/*
 *	Copyright (C) 2015-2016 Yizhou Shan <shan13@purdue.edu>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License along
 *	with this program; if not, write to the Free Software Foundation, Inc.,
 *	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _ASM_GENERIC_UACCESS_H_
#define _ASM_GENERIC_UACCESS_H_

#include <sandix/types.h>
#include <sandix/string.h>
#include <sandix/compiler.h>

/*
 * User space memory access functions, these should work
 * on any machine that has kernel and user data in the same
 * address space, e.g. all NOMMU machines.
 */

/*
 * Architectures with an MMU really should override all these
 * functions below.
 */

#ifndef __copy_from_user
static inline __must_check long __copy_from_user(void *to,
		const void __user *from, unsigned long n)
{
	if (__builtin_constant_p(n)) {
		switch(n) {
		case 1:
			*(u8 *)to = *(u8 *)from;
			return 0;
		case 2:
			*(u16 *)to = *(u16 *)from;
			return 0;
		case 4:
			*(u32 *)to = *(u32 *)from;
			return 0;
#ifdef CONFIG_64BIT
		case 8:
			*(u64 *)to = *(u64 *)from;
			return 0;
#endif
		default:
			break;
		}
	}

	memcpy((void *)to, (const void *)from, n);
	return 0;
}
#endif

#ifndef __copy_to_user
static inline __must_check long __copy_to_user(void __user *to,
		const void *from, unsigned long n)
{
	if (__builtin_constant_p(n)) {
		switch(n) {
		case 1:
			*(u8 *)to = *(u8 *)from;
			return 0;
		case 2:
			*(u16 *)to = *(u16 *)from;
			return 0;
		case 4:
			*(u32 *)to = *(u32 *)from;
			return 0;
#ifdef CONFIG_64BIT
		case 8:
			*(u64 *)to = *(u64 *)from;
			return 0;
#endif
		default:
			break;
		}
	}

	memcpy((void *)to, (const void *)from, n);
	return 0;
}
#endif

#define VERIFY_READ	0
#define VERIFY_WRITE	1

#define access_ok(type, addr, size) __access_ok((unsigned long)(addr),(size))

#ifndef __access_ok
static inline int __access_ok(unsigned long addr, unsigned long size)
{
	return 1;
}
#endif

#undef might_fault
#define might_fault()

static inline long copy_from_user(void *to,
		const void __user * from, unsigned long n)
{
	might_fault();
	if (access_ok(VERIFY_READ, from, n))
		return __copy_from_user(to, from, n);
	else
		return n;
}

static inline long copy_to_user(void __user *to,
		const void *from, unsigned long n)
{
	might_fault();
	if (access_ok(VERIFY_WRITE, to, n))
		return __copy_to_user(to, from, n);
	else
		return n;
}

#endif /* _ASM_GENERIC_UACCESS_H_ */
