#ifndef _SANDIX_IRQ_H_
#define _SANDIX_IRQ_H_

#include <asm/irq.h>

#define irq_disable()	arch_irq_disable()
#define irq_enable()	arch_irq_enable()

#endif /* _SANDIX_IRQ_H_ */
