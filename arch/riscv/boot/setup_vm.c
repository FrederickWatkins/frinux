#include <asm/page.h>

static unsigned long early_l2_pg[512] __attribute__((aligned(4096)));

void _setup_vm() {
    early_l2_pg[0] = _mk_pte(_ppn(0UL), PTE_V|PTE_R|PTE_W|PTE_A|PTE_D|PTE_G);
    // Identity mapping
    unsigned long kernel_identity = _vpn2(_vpn(KERNEL_BOOT_PA));
    early_l2_pg[kernel_identity] = _mk_pte(_ppn(KERNEL_BOOT_PA), PTE_V|PTE_R|PTE_W|PTE_X|PTE_A|PTE_D|PTE_G);
    // High mapping
    unsigned long kernel_high = _vpn2(_vpn(KERNEL_VIRT_BASE));
    early_l2_pg[kernel_high] = _mk_pte(_ppn(KERNEL_BOOT_PA), PTE_V|PTE_R|PTE_W|PTE_X|PTE_A|PTE_D|PTE_G);

    
    unsigned long l2_ppn = _ppn((unsigned long)&early_l2_pg);
    unsigned long satp = l2_ppn | SATP_MODE_SV39;
    asm volatile(
        "csrw satp, %0\n"
        "sfence.vma zero, zero\n"
        :: "r"(satp) : "memory"
    );
}
