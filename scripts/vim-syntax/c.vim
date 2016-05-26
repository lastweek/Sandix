"	c.vim - VIM Colorful Syntax For C Language
"	To Make Life Easier When Coding Sandix. ;)
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

syn keyword	cInclude	likely
syn keyword	cInclude	unlikely

syn keyword	cInclude	BUG
syn keyword	cInclude	BUG_ON
syn keyword	cInclude	WARN
syn keyword	cInclude	WARN_ON
syn keyword	cInclude	WARN_ON_ONCE
syn keyword	cInclude	WARN_ONCE
syn keyword	cInclude	BUILD_BUG_ON
syn keyword	cInclude	DSNVM_BUG_ON
syn keyword	cInclude	DSNVM_WARN_ON

syn keyword	cInclude	READ_ONCE
syn keyword	cInclude	WRITE_ONCE
syn keyword	cInclude	ACCESS_ONCE

syn keyword	cLabel		__section
syn keyword	cLabel		__init
syn keyword	cLabel		__initdata
syn keyword	cLabel		__read_mostly
syn keyword	cLabel		__cacheline_aligned
syn keyword	cLabel		__init_task_data
syn keyword	cLabel		__lockfunc

syn keyword	cType		__u8	__s8
syn keyword	cType		__u16	__s16
syn keyword	cType		__u32	__s32
syn keyword	cType		__u64	__s64
syn keyword	cType		u8	s8
syn keyword	cType		u16	s16
syn keyword	cType		u32	s32
syn keyword	cType		u64	s64
syn keyword	cType		__be16	__le16
syn keyword	cType		__be32	__le32
syn keyword	cType		__be64	__le64
syn keyword	cType		atomic_t
syn keyword	cType		cpumask_t
syn keyword	cType		spinlock_t
syn keyword	cType		phys_addr_t

syn keyword	cLabel		asmlinkage
syn keyword	cLabel		dotraplinkage

syn keyword	cLabel		__weak
syn keyword	cLabel		__pure
syn keyword	cLabel		__packed
syn keyword	cLabel		__noreturn
syn keyword	cLabel		__unused
syn keyword	cLabel		__used
syn keyword	cLabel		__maybe_unused
syn keyword	cLabel		__always_unused
syn keyword	cLabel		__attribute_const
syn keyword	cLabel		__aligned
syn keyword	cLabel		__scanf
syn keyword	cLabel		__printf
syn keyword	cLabel		__alias
syn keyword	cLabel		__noinline
syn keyword	cLabel		__noinline_for_stack
syn keyword	cLabel		__always_inline
syn keyword	cLabel		ALWAYS_INLINE
syn keyword	cLabel		INLINE
syn keyword	cLabel		__must_check
syn keyword	cLabel		__cold
syn keyword	cLabel		__visible

syn keyword	cLabel		__user
syn keyword	cLabel		__kernel
syn keyword	cLabel		__safe
syn keyword	cLabel		__force
syn keyword	cLabel		__nocast
syn keyword	cLabel		__iomem
syn keyword	cLabel		__must_hold
syn keyword	cLabel		__acquires
syn keyword	cLabel		__releases
syn keyword	cLabel		__acquire
syn keyword	cLabel		__release
syn keyword	cLabel		__cond_lock
syn keyword	cLabel		__percpu
syn keyword	cLabel		__pmem
syn keyword	cLabel		__rcu
syn keyword	cLabel		__pmem

syn keyword	cType		LIST_HEAD
syn keyword	cType		DEFINE_MUTEX
syn keyword	cType		DEFINE_BITMAP
syn keyword	cType		DECLARE_BITMAP
syn keyword	cType		EXPORT_SYMBOL

syn keyword	cError		__MUST_TODO__

syn keyword	cConstant	SIGBUS
syn keyword	cConstant	SIGSTKFLT
syn keyword	cConstant	SIGURG
syn keyword	cConstant	SIGXCPU
syn keyword	cConstant	SIGXFSZ
syn keyword	cConstant	SIGVTALRM
syn keyword	cConstant	SIGPROF

syn keyword	cLabel		for_each_set_bit
syn keyword	cLabel		for_each_clear_bit
syn keyword	cLabel		list_for_each_entry
syn keyword	cLabel		list_for_each_entry_safe
syn keyword	cLabel		list_for_each_entry_reverse
syn keyword	cLabel		list_for_each
