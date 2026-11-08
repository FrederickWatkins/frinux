#include <kernel/spinlock.h>
#include <kernel/kprint.h>
#include <kernel/console.h>
#include <stdarg.h>
#include <stdint.h>

#define F_LEFT  1
#define F_ZERO  2
#define F_UPPER 4

enum { LEN_INT, LEN_LONG, LEN_LLONG, LEN_SIZE };

/* Output sink: counts every char, stores only while there is room. */
struct out {
    char *buf;
    size_t size;    /* capacity including the NUL */
    size_t pos;    /* chars the full output would contain */
};

static void out_char(struct out *o, char c)
{
    if (o->size && o->pos < o->size - 1)
        o->buf[o->pos] = c;
    o->pos++;
}

static void out_pad(struct out *o, char c, int n)
{
    while (n-- > 0)
        out_char(o, c);
}

static void out_str(struct out *o, const char *s, int len, int flags, int width)
{
    int pad = width > len ? width - len : 0;

    if (!(flags & F_LEFT))
        out_pad(o, ' ', pad);
    for (int i = 0; i < len; i++)
        out_char(o, s[i]);
    if (flags & F_LEFT)
        out_pad(o, ' ', pad);
}

static void out_num(struct out *o, unsigned long long v, unsigned base,
        int neg, int flags, int width)
{
    const char *digits = (flags & F_UPPER) ? "0123456789ABCDEF" : "0123456789abcdef";
    char tmp[24];    /* 20 digits max for 64-bit decimal */
    int n = 0;

    do {
        tmp[n++] = digits[v % base];
        v /= base;
    } while (v);

    int pad = width - n - neg;
    if (pad < 0)
        pad = 0;

    if (flags & F_LEFT) {
        if (neg)
            out_char(o, '-');
        while (n)
            out_char(o, tmp[--n]);
        out_pad(o, ' ', pad);
    } else if (flags & F_ZERO) {
        if (neg)
            out_char(o, '-');
        out_pad(o, '0', pad);
        while (n)
            out_char(o, tmp[--n]);
    } else {
        out_pad(o, ' ', pad);
        if (neg)
            out_char(o, '-');
        while (n)
            out_char(o, tmp[--n]);
    }
}

static unsigned long long get_unsigned(va_list *ap, int len)
{
    switch (len) {
    case LEN_LLONG: return va_arg(*ap, unsigned long long);
    case LEN_LONG:  return va_arg(*ap, unsigned long);
    case LEN_SIZE:  return va_arg(*ap, size_t);
    default:        return va_arg(*ap, unsigned int);
    }
}

static long long get_signed(va_list *ap, int len)
{
    switch (len) {
    case LEN_LLONG: return va_arg(*ap, long long);
    case LEN_LONG:  return va_arg(*ap, long);
    case LEN_SIZE:  return (long long)va_arg(*ap, size_t);
    default:        return va_arg(*ap, int);
    }
}

static int vsnprintf(char *buf, size_t size, const char *fmt, va_list ap)
{
    struct out o = { .buf = buf, .size = size, .pos = 0 };

    for (; *fmt; fmt++) {
        if (*fmt != '%') {
            out_char(&o, *fmt);
            continue;
        }
        fmt++;


        int flags = 0, width = 0, len = LEN_INT;

        /* flags */
        for (;; fmt++) {
            if (*fmt == '-')
                flags |= F_LEFT;
            else if (*fmt == '0')
                flags |= F_ZERO;
            else
                break;
        }

        /* width */
        if (*fmt == '*') {
            width = va_arg(ap, int);
            if (width < 0) {
                flags |= F_LEFT;
                width = -width;
            }
            fmt++;
        } else {
            while (*fmt >= '0' && *fmt <= '9')
                width = width * 10 + (*fmt++ - '0');
        }

        /* length */
        if (*fmt == 'z') {
            len = LEN_SIZE;
            fmt++;
        } else {
            while (*fmt == 'l') {
                len = len == LEN_INT ? LEN_LONG : LEN_LLONG;
                fmt++;
            }
        }

        switch (*fmt) {
        case 'd':
        case 'i': {
            long long v = get_signed(&ap, len);
            int neg = v < 0;
            unsigned long long u = neg ? -(unsigned long long)v : (unsigned long long)v;
            out_num(&o, u, 10, neg, flags, width);
            break;
        }
        case 'u':
            out_num(&o, get_unsigned(&ap, len), 10, 0, flags, width);
            break;
        case 'X':
            flags |= F_UPPER;
            /* fall through */
        case 'x':
            out_num(&o, get_unsigned(&ap, len), 16, 0, flags, width);
            break;
        case 'p':
            /* always 0x + 16 zero-padded digits on rv64 */
            out_char(&o, '0');
            out_char(&o, 'x');
            out_num(&o, (uintptr_t)va_arg(ap, void *), 16, 0, F_ZERO, 16);
            break;
        case 'c': {
            char c = (char)va_arg(ap, int);
            out_str(&o, &c, 1, flags, width);
            break;
        }
        case 's': {
            const char *s = va_arg(ap, const char *);
            int n = 0;

            if (!s)
                s = "(null)";
            while (s[n])
                n++;
            out_str(&o, s, n, flags, width);
            break;
        }
        case '%':
            out_char(&o, '%');
            break;
        case '\0':
            fmt--;    /* lone '%' at end: let the outer loop terminate */
            break;
        default:    /* unknown conversion: print it verbatim */
            out_char(&o, '%');
            out_char(&o, *fmt);
            break;
        }
    }

    if (size)
        buf[o.pos < size ? o.pos : size - 1] = '\0';

    return (int)o.pos;
}

struct print_ctx {
    char *buf;
    int n;
};

static void print(struct console *c, struct print_ctx *ctx) {
    struct irq_global irq = spinlock_acquire_irqsave(c->lock);
    for(int i = 0; i < ctx->n; i++) {
        c->putc(ctx->buf[i], c->ctx);
    }
    spinlock_release_irqrestore(c->lock, irq);
}

static void print_adapter(struct console *c, void *ctx) {
    print(c, (struct print_ctx*)ctx);
}

int _kprint(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    char buf[256];
    int n = vsnprintf(buf, sizeof(buf), fmt, ap);
    struct print_ctx ctx = {
        .buf = buf,
        .n = n,
    };
    consoles_foreach(print_adapter, (void*) &ctx);
    va_end(ap);
    return n;
}

static void print_force(struct console *c, struct print_ctx *ctx) {
    struct irq_global irq = spinlock_acquire_force_irqsave(c->lock);
    for(int i = 0; i < ctx->n; i++) {
        c->putc(ctx->buf[i], c->ctx);
    }
    spinlock_release_irqrestore(c->lock, irq);
}

static void print_force_adapter(struct console *c, void *ctx) {
    print_force(c, (struct print_ctx*)ctx);
}

int _kprint_force(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    char buf[256];
    int n = vsnprintf(buf, sizeof(buf), fmt, ap);
    struct print_ctx ctx = {
        .buf = buf,
        .n = n,
    };
    consoles_foreach(print_force_adapter, (void*) &ctx);
    va_end(ap);
    return n;
}
