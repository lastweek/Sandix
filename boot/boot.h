/*
 *	2015/04/02 Created by Shan Yizhou.
 *
 *	boot.h: Header file for real-mode boot process.
 */

#ifndef BOOT_BOOT_H
#define BOOT_BOOT_H

typedef unsigned int	u32;
typedef unsigned short	u16;
typedef unsigned char	u8;
typedef unsigned int	size_t;

static inline u16 ds(void)
{
	u16 seg;
	__asm__("movw %%ds,%0" : "=rm" (seg));
	return seg;
}

static inline void set_fs(u16 seg)
{
	__asm__ volatile("movw %0,%%fs" : : "rm" (seg));
}
static inline u16 fs(void)
{
	u16 seg;
	__asm__ volatile("movw %%fs,%0" : "=rm" (seg));
	return seg;
}

static inline void set_gs(u16 seg)
{
	__asm__ volatile("movw %0,%%gs" : : "rm" (seg));
}
static inline u16 gs(void)
{
	u16 seg;
	__asm__ volatile("movw %%gs,%0" : "=rm" (seg));
	return seg;
}

/*
pusha:-->
 Push(EAX);
 Push(ECX);
 Push(EDX);
 Push(EBX);
 Push(ESP);
 Push(EBP);
 Push(ESI);
 Push(EDI);
*/

/*---------------------------------*/
/* bioscall.S                      */
/*---------------------------------*/
struct biosregs {
	union {
		struct {
			u32 edi;
			u32 esi;
			u32 ebp;
			u32 _esp;
			u32 ebx;
			u32 edx;
			u32 ecx;
			u32 eax;
			u32 _fsgs;
			u32 _dses;
			u32 eflags;
		};
		struct {
			u16 di, hdi;
			u16 si, hsi;
			u16 bp, hbp;
			u16 _sp, _hsp;
			u16 bx, hbx;
			u16 dx, hdx;
			u16 cx, hcx;
			u16 ax, hax;
			u16 gs, fs;
			u16 es, ds;
			u16 flags, hflags;
		};
		struct {
			u8 dil, dih, edi2, edi3;
			u8 sil, sih, esi2, esi3;
			u8 bpl, bph, ebp2, ebp3;
			u8 _spl, _sph, _esp2, _esp3;
			u8 bl, bh, ebx2, ebx3;
			u8 dl, dh, edx2, edx3;
			u8 cl, ch, ecx2, ecx3;
			u8 al, ah, eax2, eax3;
		};
	};
};
void intcall(unsigned char int_no, const struct biosregs *ireg, struct biosregs *oreg);
 
/*---------------------------------*/
/* regs.c                          */
/*---------------------------------*/
void initregs(struct biosregs *regs);

#endif /* BOOT_BOOT_H */
