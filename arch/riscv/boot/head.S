/*
 * RISC-V boot header and entry.
 *
 * Boot contract (Documentation/arch/riscv/boot.rst):
 *   S-mode, MMU off, a0 = hartid, a1 = DTB physical address.
 */

    .section .head.text, "ax"
    .global _start
_start:
    /* 64-byte Image header. Force 4-byte instructions so the layout is fixed. */
    .option push
    .option norvc
    j _start_kernel
    nop
    .option pop
    .dword 0x200000    /* text_offset: RAM base + 2MB */
    .dword __image_size    /* image_size, including BSS (from linker.ld) */
    .dword 0    /* flags: little-endian */
    .word 0x2    /* version 0.2 */
    .word 0    /* res1 */
    .dword 0    /* res2 */
    .dword 0x5643534952    /* magic:  "RISCV\0\0\0" */
    .word 0x05435352    /* magic2: "RSC\x05" */
    .word 0    /* res3 */

_start_kernel:
    /* a0/a1 must survive until the call: only t0/t1 are used below */
    la t0, __bss_start
    la t1, __bss_end
1:
    bgeu t0, t1, 2f
    sd zero, 0(t0)
    addi t0, t0, 8
    j 1b
2:
    la sp, _stack_top
    call kmain
3:
    wfi /* kmain must not return */
    j 3b

    .section .bss.stack, "aw", @nobits
    .balign 16
_stack_bottom:
    .skip 16384
    .global _stack_top
_stack_top:
