"
"	c.vim - VIM Colorful Syntax For C Language
"	To Make Life Easier When Coding Sandix. ;)
"
"	Copyright (C) 2015 Yizhou Shan <shanyizhou@ict.ac.cn>
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

"
"	HOWTO:
"	1) Create ~/.vim/after/syntax/c.vim
"	2) Run [:syntax enable] or [:syntax on] in VIM
"
"	Actually, we only concern about the colors, not the syntax groups.
"	Hence the additional colorful words should not mess up the code. In my
"	opinion, the principle of color choosing is: Reduce Cross-Color Code!
"	Of course, feel free to choose your own favorite color schemes.
"

" See <sandix/compiler.h>
syn keyword	cInclude	likely
syn keyword	cInclude	unlikely

" See <sandix/bug.h>
syn keyword	cInclude	BUG
syn keyword	cInclude	BUG_ON
syn keyword	cInclude	WARN
syn keyword	cInclude	WARN_ON
syn keyword	cInclude	WARN_ON_ONCE

" See <sandix/compiler.h>
" Special labels for special sections.
" Use cLabel to distinguish with those __weak attribute things.
syn keyword	cLabel		__section
syn keyword	cLabel		__init
syn keyword	cLabel		__initdata
syn keyword	cLabel		__read_mostly

" See <asm/types.h>
" Conform with variable types.
syn keyword	cType		__u8	__s8
syn keyword	cType		__u16	__s16
syn keyword	cType		__u32	__u32
syn keyword	cType		__u64	__u64
syn keyword	cType		u8	s8
syn keyword	cType		u16	s16
syn keyword	cType		u32	s32
syn keyword	cType		u64	s64

" See <sandix/linkage.h>
" Conform with function return types.
syn keyword	cType		asmlinkage

" See <sandix/export.h>
" Conform with function return types.
syn keyword	cType		EXPORT_SYMBOL

" See <sandix/compiler.h>
" Conform with function return types, mostly.
syn keyword	cType		__weak
syn keyword	cType		__pure
syn keyword	cType		__packed
syn keyword	cType		__noreturn
syn keyword	cType		__unused
syn keyword	cType		__used
syn keyword	cType		__maybe_unused
syn keyword	cType		__always_unused
syn keyword	cType		__attribute_const
syn keyword	cType		__aligned
syn keyword	cType		__scanf
syn keyword	cType		__printf
syn keyword	cType		__alias
syn keyword	cType		__noinline
syn keyword	cType		__noinline_for_stack
syn keyword	cType		__always_inline
syn keyword	cType		ALWAYS_INLINE
syn keyword	cType		INLINE
syn keyword	cType		__must_check
syn keyword	cType		__cold
syn keyword	cType		__visible
