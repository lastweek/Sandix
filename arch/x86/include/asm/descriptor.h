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

/*
 * A segment descriptor is a data structure in a GDT or LDT that
 * provides the processor with the size and location of a segment,
 * as well as access control and status information.
 *
 * Segment descriptor is an ugly and complicated concept. Intel tried
 * to use descriptor to protect code/data, to distinguish segment.
 * Therefore, depends on its usage, these are many types:
 *
 * Segment descriptor
 *   |
 *   |--- Code segment   (S=1, Bit 11 = 1)
 *   |--- Data segment   (S=1, Bit 11 = 0)
 *   |--- System segment (S=0)
 *          |
 *          |--- System-segment descriptors (LDT, TSS segment)
 *          |--- Gate descriptors (Call, Interrupt and Trap gates)
 */

#ifndef _ASM_X86_DESCRIPTOR_H_
#define _ASM_X86_DESCRIPTOR_H_

#include <sandix/bug.h>
#include <sandix/types.h>
#include <sandix/string.h>
#include <sandix/bitops.h>
#include <sandix/compiler.h>

#include <asm/tss.h>
#include <asm/segment.h>
#include <asm/irq_vectors.h>

/*
 * 8-byte descriptor
 */
struct desc_struct {
	union {
		struct {
			unsigned int a;
			unsigned int b;
		};
		struct {
			unsigned short limit0;
			unsigned short base0;
			unsigned short base1:8, type:4, s:1, dpl:2, p:1;
			unsigned short limit1:4, avl:1, l:1, d:1, g:1, base2:8;
		};
	};
} __packed;

/*
 * 16-byte gate descriptor
 */
struct gate_struct64 {
	unsigned short offset_low;
	unsigned short segment;
	unsigned short ist:3, zero0:5, type:5, dpl:2, p:1;
	unsigned short offset_middle;
	unsigned int   offset_high;
	unsigned int   zero1;
} __packed;

#define PTR_LOW(x)	((unsigned long long)(x) & 0xFFFF)
#define PTR_MIDDLE(x)	(((unsigned long long)(x) >> 16) & 0xFFFF)
#define PTR_HIGH(x)	((unsigned long long)(x) >> 32)

/*
 * 16-byte LDT or TSS descriptor
 */
struct ldttss_desc64 {
	unsigned short limit0;
	unsigned short base0;
	unsigned short base1:8, type:5, dpl:2, p:1;
	unsigned short limit1:4, zero0:3, g:1, base2:8;
	unsigned int   base3;
	unsigned int   zero1;
} __packed;

/*
 * Sytem segment: System-Segment and Gate-Descriptor Types
 * (When the S (descriptor type) flag in a segment descriptor
 *  is clear, the descriptor type is a system descriptor.)
 */
enum DESC_TYPE {
	DESC_LDT  = 0x2,
	DESC_TSS  = 0x9,

	GATE_TASK = 0x5,
	GATE_CALL = 0xC,
	GATE_INTR = 0xE,
	GATE_TRAP = 0xF,

	NO_SYSSEG = 0x10	/* !system */
};

#ifdef CONFIG_X86_32
 typedef struct desc_struct	gate_desc;
 typedef struct desc_struct	ldt_desc;
 typedef struct desc_struct	tss_desc;
 #define gate_segment(g)	((g).a >> 16)
 #define gate_offset(g)		(((g).b & 0xffff0000) | ((g).a & 0x0000ffff))
#else
 typedef struct gate_struct64	gate_desc;
 typedef struct ldttss_desc64	ldt_desc;
 typedef struct ldttss_desc64	tss_desc;
 #define gate_segment(g)	((g).segment)
 #define gate_offset(g)		((g).offset_low | ((unsigned long)(g).offset_middle << 16) | \
				((unsigned long)(g).offset_high << 32))
#endif

/*
 * The weird data structure used by lgdt ect.
 * It would be 48 = 16 + 32 bit in i386,
 * It would be 80 = 16 + 64 bit in x86_64
 */
struct desc_ptr {
	unsigned short size;
	unsigned long address;
} __packed;

extern struct desc_struct idt_table[];
extern struct desc_struct gdt_table[];

#define GDT_ENTRY_INIT(flags, base, limit)				\
{									\
	.a = ((limit) & 0xffff) | (((base) & 0xffff) << 16),		\
	.b = (((base) & 0xff0000) >> 16) | (((flags) & 0xf0ff) << 8) |	\
		((limit) & 0xf0000) | ((base) & 0xff000000),		\
}

/*
 * Note that:
 * IDT, GDT, LDT, TSS things are critical in para-virtualization.
 * Linux name the below functions with an addtional prefix: native,
 * which are used when paravirt is not enabled. If paravirt is enabled,
 * then everything is wrapped.
 */

static inline void write_idt_entry(gate_desc *idt, int entry, const gate_desc *gate)
{
	memcpy(&idt[entry], gate, sizeof(*gate));
}

static inline void write_ldt_entry(struct desc_struct *ldt, int entry, const void *desc)
{
	memcpy(&ldt[entry], desc, 8);
}

static inline void write_gdt_entry(struct desc_struct *gdt, int entry,
				   const void *desc, int type)
{
	unsigned int size;
	
	/*
	 * LDT and TSS descriptors are expanded to
	 * 16 bytes in IA-32e mode, occupying the
	 * space of two entries.
	 */
	switch (type) {
	case DESC_TSS:	size = sizeof(tss_desc);	break;
	case DESC_LDT:	size = sizeof(ldt_desc);	break;
	default:	size = sizeof(*gdt);		break;
	}
	memcpy(&gdt[entry], desc, size);
}

static inline void load_gdt(const struct desc_ptr *dtr)
{
	asm volatile("lgdt %0"::"m" (*dtr));
}

static inline void load_idt(const struct desc_ptr *dtr)
{
	asm volatile("lidt %0"::"m" (*dtr));
}

static inline void store_gdt(struct desc_ptr *dtr)
{
	asm volatile("sgdt %0":"=m" (*dtr));
}

static inline void store_idt(struct desc_ptr *dtr)
{
	asm volatile("sidt %0":"=m" (*dtr));
}

static inline void load_TR_desc(void)
{
	asm volatile("ltr %w0"::"q" (GDT_ENTRY_KERNEL_TSS * 8));
}

#ifdef CONFIG_X86_32
static inline void pack_gate(gate_desc *gate, unsigned char type,
			     unsigned long base, unsigned int dpl,
			     unsigned int flags, unsigned short seg)
{
	gate->a = (seg << 16) | (base & 0xffff);
	gate->b = (base & 0xffff0000) | (((0x80 | type | (dpl << 5)) & 0xff) << 8);
}
#else
static inline void pack_gate(gate_desc *gate, unsigned char type,
			     unsigned long func, unsigned int dpl,
			     unsigned int ist, unsigned short seg)
{
	gate->offset_low	= PTR_LOW(func);
	gate->segment		= __KERNEL_CS;
	gate->ist		= ist;
	gate->p			= 1;
	gate->dpl		= dpl;
	gate->zero0		= 0;
	gate->zero1		= 0;
	gate->type		= type;
	gate->offset_middle	= PTR_MIDDLE(func);
	gate->offset_high	= PTR_HIGH(func);
}
#endif

static inline void pack_descriptor(struct desc_struct *desc, unsigned long base,
				   unsigned long limit, unsigned char type,
				   unsigned int flags)
{
	desc->a = ((base & 0xffff) << 16) | (limit & 0xffff);
	desc->b = (base & 0xff000000) | ((base & 0xff0000) >> 16) |
		  (limit & 0x000f0000) | ((type & 0xff) << 8) |
		  ((flags & 0xf) << 20);
	desc->p = 1;
}

static inline void set_tssldt_descriptor(void *d, unsigned long addr, unsigned type, unsigned size)
{
#ifdef CONFIG_X86_64
	struct ldttss_desc64 *desc = d;

	memset(desc, 0, sizeof(*desc));

	desc->limit0		= size & 0xFFFF;
	desc->base0		= PTR_LOW(addr);
	desc->base1		= PTR_MIDDLE(addr) & 0xFF;
	desc->type		= type;
	desc->p			= 1;
	desc->limit1		= (size >> 16) & 0xF;
	desc->base2		= (PTR_MIDDLE(addr) >> 8) & 0xFF;
	desc->base3		= PTR_HIGH(addr);
#else
	pack_descriptor((struct desc_struct *)d, addr, size, 0x80 | type, 0);
#endif
}

static inline void set_tss_desc(void *addr)
{
	struct desc_struct *gdt = gdt_table;
	tss_desc tss;

	/*
	 * sizeof(unsigned long) coming from an extra "long" at the end
	 * of the iobitmap. See tss_struct definition in processor.h
	 *
	 * -1? seg base+limit should be pointing to the address of the
	 * last valid byte
	 */
	set_tssldt_descriptor(&tss, (unsigned long)addr, DESC_TSS,
			      IO_BITMAP_OFFSET + IO_BITMAP_BYTES +
			      sizeof(unsigned long) - 1);
	write_gdt_entry(gdt, GDT_ENTRY_KERNEL_TSS, &tss, DESC_TSS);
}

/*
 * (a)
 *
 * The processor check the DPL of the interrupt or trap gate
 * only if the interrupt or exception is generated by INT n.
 * 
 * The CPL must be less than or equal to the DPL of the gate.
 * That's why system call gate use 0x3 as its DPL.
 *
 * For hardware-generated interrutps and processor-deteced
 * exceptions, the processor ignores the DPL of the gates.
 *
 * (b)
 *
 * The only difference between trap and interrupt gates is the
 * way the processor handles the IF flags in EFLAGS register.
 *
 * Through a [trap gate], it does not affect the IF flag.
 *
 * Through an [interrupt gate], the processor clears the IF flag
 * to prevent other interrupts from interfering with the current
 * interrupt handler. A subsequent IRET instruction restores
 * the IF flag to its value in the saved contents of the EFLAGS
 * register on the stack. (The IF flag does not affect the
 * generation of exceptions or NMI interrupts).
 */

static inline void __set_gate(int gate, unsigned char type, void *addr,
			unsigned int dpl, unsigned int ist, unsigned short seg)
{
	gate_desc s;

	pack_gate(&s, type, (unsigned long)addr, dpl, ist, seg);
	write_idt_entry(idt_table, gate, &s);
}

/*
 * The following two routines:
 *	set_system_intr_gate
 *	set_system_trap_gate
 * are used to set intr/trap gate with 0x3 DPL, those gates can be called
 * via INT instruction from userspace.
 */

static inline void set_system_intr_gate(unsigned int gate, void *addr)
{
	BUG_ON(gate > 0xFF);
	__set_gate(gate, GATE_INTR, addr, 0x3, 0, __KERNEL_CS);
}

static inline void set_system_trap_gate(unsigned int gate, void *addr)
{
	BUG_ON(gate > 0xFF);
	__set_gate(gate, GATE_TRAP, addr, 0x3, 0, __KERNEL_CS);
}

/*
 * The following two routines:
 *	set_intr_gate
 *	set_trap_gate
 * are used to set intr/trap gate with 0x0 DPL, those gates can NOT be called
 * via INT instruction from userspace
 */

static inline void set_intr_gate(unsigned int gate, void *addr)
{
	BUG_ON(gate > 0xFF);
	__set_gate(gate, GATE_INTR, addr, 0, 0, __KERNEL_CS);
}

static inline void set_trap_gate(unsigned int gate, void *addr)
{
	BUG_ON(gate > 0xFF);
	__set_gate(gate, GATE_TRAP, addr, 0, 0, __KERNEL_CS);
}

static inline void set_task_gate(unsigned int gate, unsigned int gdt_entry)
{
	BUG_ON(gate > 0xFF);
	__set_gate(gate, GATE_TASK, (void *)0, 0, 0, (gdt_entry << 3));
}

/* traps.c */
extern int first_system_vector;
DECLARE_BITMAP(used_vectors, NR_VECTORS);

static inline void alloc_system_vector(int vector)
{
	if (!test_bit(vector, used_vectors)) {
		set_bit(vector, used_vectors);
		if (first_system_vector > vector)
			first_system_vector = vector;
	} else {
		BUG();
	}
}

static inline void alloc_intr_gate(unsigned int n, void *addr)
{
	alloc_system_vector(n);
	set_intr_gate(n, addr);
}

/* Save a segment register away */
#define savesegment(seg, value)		\
	asm (				\
		"mov %%" #seg ",%0"	\
		:"=r" (value)		\
		:			\
		: "memory"		\
	)

#endif /* _ASM_X86_DESCRIPTOR_H_*/
