#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <kernel/irq.h>
#include <stdatomic.h>

struct spinlock {
    atomic_flag flag;
};

void spinlock_init(struct spinlock *lock);

void spinlock_acquire(struct spinlock *lock);
struct irq_global spinlock_acquire_irqsave(struct spinlock *lock);
void spinlock_acquire_force(struct spinlock *lock);
struct irq_global spinlock_acquire_force_irqsave(struct spinlock *lock);

void spinlock_release(struct spinlock *lock);
void spinlock_release_irqrestore(struct spinlock *lock, struct irq_global irq);

#endif /* SPINLOCK_H */
