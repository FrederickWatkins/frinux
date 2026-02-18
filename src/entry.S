.section .text.entry
.globl _start

_start:

    # 3. Setup the stack pointer
    la sp, __stack_top

    # 4. Jump to Rust 'kmain' (use 'tail' to avoid returning)
    tail _entry