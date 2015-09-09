"
"	c.vim - VIM Colorful Syntax For Sandix
"	To Make Life Easier When Coding Sandix. ;)
"
"	Copyright (C) 2015 Yizhou Shan <shanyizhou@ict.ac.cn>
"
"	HOWTO:
"	1) Create ~/.vim/after/syntax/c.vim
"	2) Run [:syntax enable] or [:syntax on] in VIM
"
"	This program is free software; you can redistribute it and/or modify
"	it under the terms of the GNU General Public License as published by
"	the Free Software Foundation; either version 2 of the License, or
"	(at your option) any later version.
"
"	This program is distributed in the hope that it will be useful,
"	but WITHOUT ANY WARRANTY; without even the implied warranty of
"	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
"	GNU General Public License for more details.
"
"	You should have received a copy of the GNU General Public License along
"	with this program; if not, write to the Free Software Foundation, Inc.,
"	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
"

" See <asm/types.h>
syn keyword	cType		__u16	__s16
syn keyword	cType		__u32	__u32
syn keyword	cType		__u64	__u64
syn keyword	cType		u16	s16
syn keyword	cType		u32	s32
syn keyword	cType		u64	s64

" See <sandix/compiler.h>
syn keyword	cLabel		__section
syn keyword	cLabel		__init
syn keyword	cLabel		__initdata
syn keyword	cLabel		__read_mostly

" See <sandix/compiler.h>
syn keyword	cInclude	likely
syn keyword	cInclude	unlikely

" See <sandix/bug.h>
syn keyword	cInclude	BUG
syn keyword	cInclude	BUG_ON
syn keyword	cInclude	WARN_ON
syn keyword	cInclude	WARN_ON_ONCE

" See <sandix/compiler.h>
syn keyword	cStorageClass	__weak
syn keyword	cStorageClass	__pure
syn keyword	cStorageClass	__packed
syn keyword	cStorageClass	__noreturn
syn keyword	cStorageClass	__unused
syn keyword	cStorageClass	__used
syn keyword	cStorageClass	__maybe_unused
syn keyword	cStorageClass	__always_unused
syn keyword	cStorageClass	__attribute_const
syn keyword	cStorageClass	__aligned
syn keyword	cStorageClass	__scanf
syn keyword	cStorageClass	__printf
syn keyword	cStorageClass	__alias
syn keyword	cStorageClass	__noinline
syn keyword	cStorageClass	__noinline_for_stack
syn keyword	cStorageClass	__always_inline
syn keyword	cStorageClass	ALWAYS_INLINE
syn keyword	cStorageClass	INLINE
syn keyword	cStorageClass	__must_check
syn keyword	cStorageClass	__cold
syn keyword	cStorageClass	__visible
