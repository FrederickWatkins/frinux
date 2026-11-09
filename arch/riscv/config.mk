# RISC-V specific build settings.
#
# Inputs (from .config):
#   CONFIG_64BIT, CONFIG_RISCV_ISA_{M,A,F,D,C}
# Outputs (used by the top-level config.mk):
#   TARGET, ARCH_CFLAGS, ARCH_LDFLAGS, QEMU

ifeq ($(filter y,$(CONFIG_RISCV_ISA_D)),y)
ifneq ($(filter y,$(CONFIG_RISCV_ISA_F)),y)
$(error CONFIG_RISCV_ISA_D requires CONFIG_RISCV_ISA_F)
endif
endif

riscv_xlen := $(if $(filter y,$(CONFIG_64BIT)),64,32)

# ISA string, in canonical order
riscv_isa := rv$(riscv_xlen)i
riscv_isa := $(riscv_isa)$(if $(filter y,$(CONFIG_RISCV_ISA_M)),m)
riscv_isa := $(riscv_isa)$(if $(filter y,$(CONFIG_RISCV_ISA_A)),a)
riscv_isa := $(riscv_isa)$(if $(filter y,$(CONFIG_RISCV_ISA_C)),c)
riscv_isa := $(riscv_isa)_zicsr_zifencei

# ABI: lp64/ilp32, plus f/d suffix when hardware FP is enabled
riscv_abi := $(if $(filter y,$(CONFIG_64BIT)),lp64,ilp32)
riscv_abi := $(riscv_abi)$(if $(filter y,$(CONFIG_RISCV_ISA_D)),d,$(if $(filter y,$(CONFIG_RISCV_ISA_F)),f))

TARGET       := riscv$(riscv_xlen)-unknown-elf
ARCH_CFLAGS  := -march=$(riscv_isa) -mabi=$(riscv_abi) -mcmodel=medany -mno-relax
ARCH_LDFLAGS := -Wl,--no-relax

QEMU := qemu-system-riscv$(riscv_xlen) -machine virt -nographic -bios default

# Required config for RISCV
CONFIG_LIBFDT := y
