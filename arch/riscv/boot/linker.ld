OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS
{
    . = 0x80200000;            /* RAM base 0x80000000 + 2MB text_offset */

    .head.text : { KEEP(*(.head.text)) }   /* image header must be first */
    .text      : { *(.text .text.*) }

    . = ALIGN(4096);
    .rodata    : { *(.rodata .rodata.*) }

    . = ALIGN(4096);
    .data      : { *(.data .data.*) *(.sdata .sdata.*) }

    . = ALIGN(4096);
    .bss (NOLOAD) : {
        __bss_start = .;
        *(.bss.stack)
        *(.sbss .sbss.*)
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(8);
        __bss_end = .;
    }
    _end = .;

    __image_size = _end - _start;   /* read by the header in head.c */

    /DISCARD/ : { *(.comment) *(.eh_frame) *(.note.*) }
}
