#include <kernel/console.h>
#include <stddef.h>

static struct console_node *c_head = NULL;

void register_console(struct console_node *c)
{
    c->next = c_head;
    c_head = c;
}

void unregister_console(struct console_node *c)
{
    struct console_node **next = &c_head;
    while(next != NULL) {
        if (*next == c) {
            *next = c->next;
            return;
        }
        next = &(*next)->next;
    }
}

void consoles_foreach(void (*fn)(struct console *c, void *ctx), void *ctx)
{
    struct console_node *next = c_head;
    while(next != NULL) {
        struct console* c = &next->c;
        fn(c, ctx);
        next = next->next;
    }
}
