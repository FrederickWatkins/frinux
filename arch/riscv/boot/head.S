/*
 * RISC-V boot header and entry.
 *
 * Boot contract (Documentation/arch/riscv/boot.rst):
 *   S-mode, MMU off, a0 = hartid, a1 = DTB physical address.
 */

#include <asm/asm.h>

    .section .head.text, "ax"
    .global _start
_start:
    /* 64-byte Image header. Force 4-byte instructions so the layout is fixed. */
    .option push
    .option norvc
    j _start_kernel
    nop
    .option pop
#if __riscv_xlen == 64
    .dword 0x200000        /* text_offset: RAM base + 2MB */
#else
    .dword 0x400000        /* text_offset: RAM base + 4MB */
#endif
    .dword __image_size    /* image_size, including BSS (from linker.ld) */
    .dword 0    /* flags: little-endian */
    .word 0x2    /* version 0.2 */
    .word 0    /* res1 */
    .dword 0    /* res2 */
    .dword 0x5643534952    /* magic:  "RISCV\0\0\0" */
    .word 0x05435352    /* magic2: "RSC\x05" */
    .word 0    /* res3 */

_start_kernel:
    mv s0, a0
    mv s1, a1

    la t0, __boot_bss_start
    la t1, __boot_bss_end
1:
    bgeu t0, t1, 2f
    S_REG zero, 0(t0)
    addi t0, t0, SZREG
    j 1b
2:
    la sp, _stack_top
    call _setup_vm

    /* Debug: print 'J' on the QEMU virt UART (ns16550a at 0x10000000) */
    li t1, 0x10000000
3:  lbu t2, 5(t1)             /* LSR */
    andi t2, t2, 0x20          /* THRE: transmit holding register empty */
    beqz t2, 3b
    sb t2, 0(t1)               /* THR */

    mv a0, s0
    mv a1, s1
    ld t0, _high_entry
    jr t0 /* Jump to kernel virtual start */

    .balign 8
_high_entry:
    .dword __kernel_virt_start
    
    .section .bss.stack, "aw", @nobits
    .balign 16
_stack_bottom:
    .skip 16384
    .global _stack_top
_stack_top:
