#ifndef _ASM_ASM_H
#define _ASM_ASM_H

#if __riscv_xlen == 64
#define L_REG   ld
#define S_REG   sd
#define REG     .dword
#define SZREG   8
#define LGREG   3
#else
#define L_REG   lw
#define S_REG   sw
#define REG     .word
#define SZREG   4
#define LGREG   2
#endif

#endif /* _ASM_ASM_H */
