ld ra, FRAME_SIZE(sp) # Load stored user sp
csrrw zero, sscratch, ra # Write user stack pointer to sscratch
LOAD_ALL_GPRS
addi sp, sp, FRAME_SIZE + REG_SIZE
csrrw sp, sscratch, sp # Swap user and kernel sp
bnez sp, 2f
1:
csrrw sp, sscratch, sp # Put zero back into sscratch and kernel sp into sp

2:
# Do nothing, user sp is already in sp and kernel sp already in sscratch
