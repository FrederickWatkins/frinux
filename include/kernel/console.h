#ifndef KERNEL_CONSOLE_H
#define KERNEL_CONSOLE_H

#include <kernel/spinlock.h>

struct console {
    char *ident;
    struct spinlock *lock;
    void (*putc)(char, void* ctx);
    void *ctx;
};

struct console_node {
    struct console c;
    struct console_node *next;
};

void register_console(struct console_node *c);

void unregister_console(struct console_node *c);

void consoles_foreach(void (*fn)(struct console *c, void *ctx), void *ctx);

#endif /* KERNEL_CONSOLE_H */
