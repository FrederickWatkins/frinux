#ifndef _ASM_PAGE_H
#define _ASM_PAGE_H

#include <kernel/types.h>

#if CONFIG_64BIT == 1
#include "page_64.h"
#else
#include "page_32.h"
#endif

#define KERNEL_PHYS_BASE 0x80200000
#define KERNEL_BOOT_PA _mk_paddr(_mk_ppn(_ppn2(_ppn(KERNEL_PHYS_BASE)), 0, 0), 0) 
#define KERNEL_OFFSET (KERNEL_VIRT_BASE - KERNEL_BOOT_PA)

#define _pa(addr) (unsigned long)((unsigned long)addr - KERNEL_OFFSET)
#define _va(addr) (unsigned long)((unsigned long)addr + KERNEL_OFFSET)

#endif
