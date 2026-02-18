OUTPUT_ARCH(riscv)
ENTRY(_start)

BASE_ADDRESS = 0x80200000;

SECTIONS
{
    . = BASE_ADDRESS;
    skernel = .;

    .text : {
        *(.text.entry) /* Ensure entry.S comes first */
        *(.text .text.*)
    }

    .rodata : {
        *(.rodata .rodata.*)
    }

    .data : {
        *(.data .data.*)
    }

    .bss : {
        *(.bss .bss.*)
        *(COMMON)
    }

    /* Define a 16KB stack area per CPU core (adjust as needed) */
    .stack : {
        . = ALIGN(16);
        __stack_start = .;
        . += 0x4000; 
        __stack_top = .;
    }

    ekernel = .;
}