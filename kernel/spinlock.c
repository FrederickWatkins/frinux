#include <kernel/spinlock.h>
#include <asm/irqflags.h>

void spinlock_init(struct spinlock *lock)
{
    atomic_flag_clear(&lock->flag);
}

void spinlock_acquire(struct spinlock *lock)
{
    while(atomic_flag_test_and_set_explicit(&lock->flag, memory_order_acquire));
}

struct irq_global spinlock_acquire_irqsave(struct spinlock *lock)
{
    struct irq_global irq = irq_save();
    spinlock_acquire(lock);
    return irq;
}

void spinlock_acquire_force(struct spinlock *lock)
{
    (void) atomic_flag_test_and_set_explicit(&lock->flag, memory_order_acquire);
}

struct irq_global spinlock_acquire_force_irqsave(struct spinlock *lock)
{
    struct irq_global irq = irq_save();
    spinlock_acquire_force(lock);
    return irq;
}

void spinlock_release(struct spinlock *lock)
{
    atomic_flag_clear_explicit(&lock->flag, memory_order_release);
}
void spinlock_release_irqrestore(struct spinlock *lock, struct irq_global irq)
{
    spinlock_release(lock);
    irq_restore(irq);
}
