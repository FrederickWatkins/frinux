#ifndef KERNEL_LIBFDT_ENV_H
#define KERNEL_LIBFDT_ENV_H

#include <stddef.h>
#include <stdint.h>
#include <kernel/string.h>
#include <limits.h>

typedef uint16_t fdt16_t;
typedef uint32_t fdt32_t;
typedef uint64_t fdt64_t;

static inline uint16_t fdt16_to_cpu(fdt16_t x) { return __builtin_bswap16(x); }
static inline uint32_t fdt32_to_cpu(fdt32_t x) { return __builtin_bswap32(x); }
static inline uint64_t fdt64_to_cpu(fdt64_t x) { return __builtin_bswap64(x); }

static inline fdt16_t cpu_to_fdt16(uint16_t x) { return __builtin_bswap16(x); }
static inline fdt32_t cpu_to_fdt32(uint32_t x) { return __builtin_bswap32(x); }
static inline fdt64_t cpu_to_fdt64(uint64_t x) { return __builtin_bswap64(x); }

#endif /* KERNEL_LIBFDT_ENV_H */
