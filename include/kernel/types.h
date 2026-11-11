#ifndef KERNEL_TYPES_H
#define KERNEL_TYPES_H

#if defined(__LINKER__) || defined(__ASM__)
#define _UL(x) x
#else
#define _UL(x) x##UL
#endif

#endif
