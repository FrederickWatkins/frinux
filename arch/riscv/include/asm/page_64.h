#ifndef _ASM_PAGE_64_H
#define _ASM_PAGE_64_H

#include <kernel/types.h>

#define _SEXT_BIT    (_UL(1) << (VADDR_WIDTH - 1))

#define _sext_va(x) \
    ((((x) & MASK_BITS(VADDR_WIDTH)) ^ _SEXT_BIT) - _SEXT_BIT)

#define MASK_BITS(bits) ((_UL(1) << (bits)) - _UL(1))

#define KERNEL_VIRT_BASE 0xFFFFFFFF00000000

#define PTE_V      (_UL(1) << 0)   /* valid */
#define PTE_R      (_UL(1) << 1)   /* readable */
#define PTE_W      (_UL(1) << 2)   /* writable (requires R) */
#define PTE_X      (_UL(1) << 3)   /* executable */
#define PTE_U      (_UL(1) << 4)   /* user-accessible */
#define PTE_G      (_UL(1) << 5)   /* global */
#define PTE_A      (_UL(1) << 6)   /* accessed */
#define PTE_D      (_UL(1) << 7)   /* dirty */

#define OFFSET_WIDTH _UL(12)
#define OFFSET_MASK MASK_BITS(OFFSET_WIDTH)

#define INDEX_WIDTH _UL(9)
#define INDEX_MASK MASK_BITS(INDEX_WIDTH)

#define PAGE_DEPTH _UL(3)

#define PPN_WIDTH (PAGE_DEPTH * INDEX_WIDTH)
#define PPN_MASK MASK_BITS(PPN_WIDTH)

#define PHYS_ADDR_WIDTH (PPN_WIDTH + OFFSET_WIDTH)

#define PTE_FLAGS_WIDTH _UL(10)
#define PTE_FLAGS_MASK MASK_BITS(PTE_FLAGS_WIDTH)

#define SATP_MODE_SV39 (_UL(8)  << 60)
#define SATP_MODE_SV48 (_UL(9)  << 60)
#define SATP_MODE_SV57 (_UL(10) << 60)

#define ROOT_PPN_WIDTH (PPN_WIDTH - (INDEX_WIDTH * (PAGE_DEPTH - _UL(1))))

#define _ppn(addr) ((addr) >> OFFSET_WIDTH)

#define _ppn0(ppn) ((ppn) & INDEX_MASK)
#define _ppn1(ppn) (((ppn) >> INDEX_WIDTH) & INDEX_MASK)
#define _ppn2(ppn) ((ppn) >> (INDEX_WIDTH * 2))

#define _mk_ppn(ppn2, ppn1, ppn0) \
    ( \
        ppn0 | \
        (ppn1 << INDEX_WIDTH) | \
        (ppn2 << (INDEX_WIDTH * 2)) \
    )

#define _mk_paddr(ppn, offset) (((ppn) << OFFSET_WIDTH) | (offset))

#define _vpn(addr) ((addr) >> OFFSET_WIDTH)

#define _vpn0(vpn) ((vpn) & INDEX_MASK)
#define _vpn1(vpn) (((vpn) >> INDEX_WIDTH) & INDEX_MASK)
#define _vpn2(vpn) (((vpn) >> (INDEX_WIDTH * 2)) & INDEX_MASK)

#define _mk_vpn(vpn2, vpn1, vpn0) \
    ( \
        vpn0 | \
        (vpn1 << INDEX_WIDTH) | \
        (vpn2 << (INDEX_WIDTH * 2)) \
    )

#define _mk_vaddr(vpn, offset) \
    _sext_va((((vpn) << OFFSET_WIDTH) | (offset)))

#define _pte_flags(pte) \
    ((pte) & ((_UL(1) << PTE_FLAGS_WIDTH) - _UL(1)))

#define _pte_ppn(pte) \
    ((pte) >> PTE_FLAGS_WIDTH)

#define _mk_pte(ppn, flags) \
    (((ppn) << PTE_FLAGS_WIDTH) | (flags))

#endif
