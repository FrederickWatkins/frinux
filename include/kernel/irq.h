#ifndef KERNEL_IRQ_H
#define KERNEL_IRQ_H

#include <asm/irqflags.h>

struct irq_global irq_save();

void irq_restore(struct irq_global irq);

#endif /* KERNEL_IRQ_H */
