#include <kernel/spinlock.h>
#include <kernel/kprint.h>
#include <kernel/console.h>

#define UART0_BASE 0x10000000UL /* TEMPORARY: to be replaced by DTB lookup */

static void uart_putc(char c, void *base_addr)
{
    volatile unsigned char *uart = (volatile unsigned char *)base_addr;

    while (!(uart[5] & 0x20))
        ;
    uart[0] = (unsigned char)c;
}

void kmain(unsigned long hartid, void *dtb)
{
    struct spinlock uart_lock;
    spinlock_init(&uart_lock);
    struct console boot_uart = {
        .ident = "UART",
        .putc = uart_putc,
        .ctx = (void*)UART0_BASE,
        .lock = &uart_lock,
    };
    register_console(&(struct console_node){.c=boot_uart});

    kprint("Hello World!\n");
    kprint("Hart ID: %lu\n", hartid);
    kprint("DTB location: %p\n", dtb);
    kprint("Size of ptr: %zu\n", sizeof((void*) NULL));
    
    for (;;)
        __asm__ volatile("wfi");
}
