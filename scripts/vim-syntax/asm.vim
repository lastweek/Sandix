"
"	asm.vim - VIM Colorful Syntax For Assembly
"	To Make Life Easier When Coding Sandix. ;)
"
"	Copyright (C) 2015-2016 Yizhou Shan <shan13@purdue.edu>
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

"	HOWTO:
"	1) Create ~/.vim/after/syntax/asm.vim
"	2) Run [:syntax enable] or [:syntax on] in VIM
"
"	Normally, every assembly code has labels like 1: 2:
"	In order to keep the color consistency of labels,
"	we choose to use decNumber color!

" See <sandix/linkage.h>
syn match decNumber	"ENTRY"
syn match decNumber	"END"
syn match decNumber	"ENDPROC"
syn match decNumber	"ALIGN"
syn match decNumber	"WEAK"
syn match decNumber	"GLOBAL"
syn match decNumber	"[a-zA-Z_][a-zA-Z0-9_]*:"he=e-1
