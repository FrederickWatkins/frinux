.set REG_SIZE, 8     # 8 for RV64, 4 for RV32
.set FRAME_SIZE, 240 # 30 registers * 8 bytes

# Helper macro that iterates over the registers and applies the operation (ld or sd)
.macro DO_ALL_GPRS op
    .set _offset, 0
    .irp reg, ra, gp, tp, t0, t1, t2, s0, s1, a0, a1, a2, a3, a4, a5, a6, a7, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, t3, t4, t5, t6
        \op \reg, _offset(sp)
        .set _offset, _offset + REG_SIZE
    .endr
.endm

.macro SAVE_ALL_GPRS
    DO_ALL_GPRS sd
.endm

.macro LOAD_ALL_GPRS
    DO_ALL_GPRS ld
.endm