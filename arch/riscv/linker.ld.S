#include <asm/page.h>

OUTPUT_ARCH(riscv)
ENTRY(_start)

PHDRS
{
    boottext  PT_LOAD FLAGS(5);      /* R X  low, MMU-off code + rodata */
    bootdata  PT_LOAD FLAGS(6);      /* R W  low, boot data + page tables */
    text      PT_LOAD FLAGS(5);      /* R X  high */
    rodata    PT_LOAD FLAGS(4);      /* R    high */
    data      PT_LOAD FLAGS(6);      /* R W  high */
}

SECTIONS
{
    . = KERNEL_PHYS_BASE;

    /* ---- Low part (VMA == LMA): runs with the MMU off ---- */
    .boot.text : {
        KEEP(*(.head.text))                  /* image header must be first */
        *?/arch/riscv/boot/?*.o(.text .text.?* .rodata .rodata.?* .srodata .srodata.?*)
    } :boottext

    . = ALIGN(4096);
    .boot.data : {
        ?*/arch/riscv/boot/?*.o(.data .data.?* .sdata .sdata.?*)
    } :bootdata

    .boot.bss (NOLOAD) : {                   /* early page tables + boot stack */
        __boot_bss_start = .;
        ?*/arch/riscv/boot/?*.o(.bss .bss.?* .sbss .sbss.?* COMMON)
        . = ALIGN(4096);
        __boot_bss_end = .;
    } :bootdata
    __boot_end = .;

    /* ---- High part: VMA = LMA + KOFF ---- */
    . = ALIGN(4096) + KERNEL_OFFSET;
    __kernel_virt_start = .;
    __kernel_phys_start = . - KERNEL_OFFSET;


    .text : AT(ADDR(.text) - KERNEL_OFFSET)
    {
        KEEP(*(.kmain.text))
        *(.text .text.*)
    } :text

    . = ALIGN(4096);
    .rodata : AT(ADDR(.rodata) - KERNEL_OFFSET) { *(.rodata .rodata.* .srodata .srodata.*) } :rodata

    . = ALIGN(4096);
    .data : AT(ADDR(.data) - KERNEL_OFFSET) { *(.data .data.*) *(.sdata .sdata.*) } :data

    . = ALIGN(4096);
    .bss (NOLOAD) : AT(ADDR(.bss) - KERNEL_OFFSET) {
        __bss_start = .;
        *(.bss.stack)
        *(.sbss .sbss.*)
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(8);
        __bss_end = .;
    } :data
    _end = .;
    __kernel_phys_end = _end - KERNEL_OFFSET;

    /* header reads this; physical-space size, not a high VMA */
    __image_size = __kernel_phys_end - _start;

    /DISCARD/ : { *(.comment) *(.eh_frame) *(.note.*) }
}
