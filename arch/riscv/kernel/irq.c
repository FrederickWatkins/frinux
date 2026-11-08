#include <asm/irqflags.h>
#include <asm/csr.h>
#include <kernel/irq.h>

struct irq_global irq_save()
{
    unsigned long output;
    unsigned long mask = SSTATUS_SIE;
    __asm__ __volatile__ (
        "csrrc %0, sstatus, %1"
        : "=r"(output)
        : "r"(mask)
        : "memory"
    );
    return (struct irq_global){.sie = (output & SSTATUS_SIE) > 0};
}

void irq_restore(struct irq_global irq)
{
    unsigned long mask = irq.sie? SSTATUS_SIE : 0;
    __asm__ __volatile__ (
        "csrrs x0, sstatus, %0"
        :
        : "r"(mask)
        : "memory"
    );
}

