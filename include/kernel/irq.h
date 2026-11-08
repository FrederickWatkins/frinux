#ifndef IRQ_H
#define IRQ_H

#include <asm/irqflags.h>

struct irq_global irq_save();

void irq_restore(struct irq_global irq);

#endif /* IRQ_H */
