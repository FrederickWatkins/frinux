#include <kernel/spinlock.h>
#include <kernel/kprint.h>
#include <kernel/console.h>
#include <kernel/libfdt.h>

static void indent(int depth)
{
    for (int i = 0; i < depth; i++)
        _kprint("    ");
}

/* True if the value looks like one or more NUL-terminated printable strings. */
static int looks_like_strings(const char *p, int len)
{
    if (len <= 0 || p[len - 1] != '\0')
        return 0;
    for (int i = 0; i < len - 1; i++) {
        unsigned char c = (unsigned char)p[i];
        if (c == '\0') {
            if (i == 0 || p[i - 1] == '\0')   /* empty string: not text */
                return 0;
        } else if (c < 0x20 || c > 0x7e) {
            return 0;
        }
    }
    return 1;
}

static void print_prop(const char *name, const void *val, int len)
{
    if (len == 0) {
        _kprint("%s;\n", name);
    } else if (looks_like_strings(val, len)) {
        const char *s = val;
        _kprint("%s = ", name);
        while (s < (const char *)val + len) {
            _kprint("\"%s\"", s);
            s += strlen(s) + 1;
            if (s < (const char *)val + len)
                _kprint(", ");
        }
        _kprint(";\n");
    } else if (len % 4 == 0) {
        const fdt32_t *cells = val;
        _kprint("%s = <", name);
        for (int i = 0; i < len / 4; i++)
            _kprint(i ? " 0x%x" : "0x%x", fdt32_to_cpu(cells[i]));
        _kprint(">;\n");
    } else {
        const unsigned char *b = val;
        _kprint("%s = [", name);
        for (int i = 0; i < len; i++)
            _kprint(i ? " %x" : "%x", b[i]);
        _kprint("];\n");
    }
}

static void dump_fdt(const void *fdt)
{
    int err = fdt_check_header(fdt);
    if (err) {
        kprint("Bad DTB: %s\n", fdt_strerror(err));
        return;
    }
    _kprint("DTB total size: %u bytes\n", fdt_totalsize(fdt));

    int depth = 0;
    for (int node = fdt_next_node(fdt, -1, &depth);
         node >= 0 && depth >= 0;
         node = fdt_next_node(fdt, node, &depth)) {

        const char *name = fdt_get_name(fdt, node, NULL);
        indent(depth);
        _kprint("%s {\n", (name && *name) ? name : "/");

        int prop;
        fdt_for_each_property_offset(prop, fdt, node) {
            const char *pname;
            int len;
            const void *val = fdt_getprop_by_offset(fdt, prop, &pname, &len);
            if (!val)
                continue;
            indent(depth + 1);
            print_prop(pname, val, len);
        }

        indent(depth);
        _kprint("};\n");
    }
}

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
        .ctx = (void*)0x10000000,
        .lock = &uart_lock,
    };
    register_console(&(struct console_node){.c=boot_uart});

    dump_fdt(dtb);

    kprint("Hello World!\n");
    kprint("Hart ID: %lu\n", hartid);
    kprint("DTB location: %p\n", dtb);
    kprint("Size of ptr: %zu\n", sizeof((void*) NULL));

    
    int chosen = fdt_path_offset(dtb, "/chosen");
    
    const char *stdout_path = fdt_getprop(dtb, chosen, "stdout-path", NULL);
    int stdout = fdt_path_offset(dtb, stdout_path);

    const char *stdout_name = fdt_get_name(dtb, stdout, NULL);

    kprint("Name of stdout: %s\n", stdout_name);
    const fdt32_t *stdout_reg = fdt_getprop(dtb, stdout, "reg", NULL);

    
    uint64_t address =
        ((uint64_t)fdt32_to_cpu(stdout_reg[0]) << 32) |
        fdt32_to_cpu(stdout_reg[1]);

    uint64_t size =
        ((uint64_t)fdt32_to_cpu(stdout_reg[2]) << 32) |
        fdt32_to_cpu(stdout_reg[3]);

    kprint("UART base address: 0x%lx\n", address);
    kprint("UART register size: 0x%lx\n", size);

    __asm__ volatile("wfi");
}
