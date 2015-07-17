/*
 * Recently, i have been working on ccNUMA system.
 * Through the experience of progamming linux smp
 * code and using linux smp APIs, i have gained some
 * common understanding about smp.
 *
 * The smp code in linux locate in x86/kernel/smp.c & smpboot.c,
 * and some definitions about IRQ number in asm/irq_vectors.h,
 * and some IDT initializations in x86/kernel/irqinit.c
 * They are used to send/receive IPIs between different cores.
 *
 * How linux get information about SMP topology? Through the
 * MP description table prepared by BIOS or detect the whole
 * system by himself? x86/kernel/mpparse.c is used to parse
 * MP table. So that's it?
 *
 * Also, the SDM is very helpful and descriptive. Read Chap.10
 * in SDM3 about APIC which depicts the fundamentals of Intel
 * APIC system. In addition, read Intel MP Spec, which is old.
 *
 * This file is touched in Tue Jul 14 23:46:56 CST 2015, Sandix
 * should support SMP architecture!
 */
/*
 * From SDM:
 * When the low double-word of the ICR is written to, the local
 * APIC creates an IPI message from the information contained
 * in the ICR and sends the message out on the bus.
 */

