csrrw sp, sscratch, sp # Swap stack pointer with sscratch
bnez sp, 2f # If sscratch was zero then already using kernel stack
1:
csrrw sp, sscratch, sp
addi sp, sp, -8 # Leave room for user mode sp on stack
sd zero, 0(sp) # Zero out user mode sp frame so restore context knows to return to kernel
j 3f

2:
addi sp, sp, -16
sd ra, 0(sp) # Free up ra so we can use it for sscratch
csrrw ra, sscratch, zero # Zero out sscratch and put user mode sp in ra
sd ra, 8(sp) # Store user mode sp at top of stack frame
lw ra, 0(sp) # Load saved ra back to register for save_context
addi sp, sp, 8

3:
addi sp, sp, -FRAME_SIZE
SAVE_ALL_GPRS
