CROSS   ?= riscv64-unknown-elf-
ARCH    ?= riscv
CC      := $(CROSS)gcc
OBJCOPY := $(CROSS)objcopy
BUILD   := build

CFLAGS  := -march=rv64imac_zicsr_zifencei -mabi=lp64 -mcmodel=medany -mno-relax \
           -O2 -g -Wall -Wextra -ffreestanding -fno-stack-protector \
           -fno-pic -fno-asynchronous-unwind-tables -fno-unwind-tables \
           -Iinclude -Iarch/$(ARCH)/include -MMD -MP
LDS     := arch/$(ARCH)/boot/linker.ld
LDFLAGS := -nostdlib -static -no-pie -Wl,--no-relax -Wl,--build-id=none \
           -Wl,-T,$(LDS) -Wl,-Map,$(BUILD)/kernel.map

# ---- collect objects from per-directory Makefiles ----
objs :=

# $(1) = directory. Reads $(1)/Makefile, records its objects, then descends.
define add_dir
obj-y :=
subdir-y :=
include $(1)/Makefile
objs += $$(addprefix $(1)/,$$(obj-y))
$$(foreach d,$$(subdir-y),$$(eval $$(call add_dir,$(1)/$$d)))
endef

TOPDIRS := arch/$(ARCH) init drivers kernel lib
$(foreach d,$(wildcard $(TOPDIRS)),$(eval $(call add_dir,$d)))

OBJS := $(addprefix $(BUILD)/,$(objs))

# ---- rules ----
.PHONY: all run debug clean
all: $(BUILD)/Image

$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/kernel.elf: $(OBJS) $(LDS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $@

$(BUILD)/Image: $(BUILD)/kernel.elf
	$(OBJCOPY) -O binary $< $@

QEMU := qemu-system-riscv64 -machine virt -nographic -bios default
run: $(BUILD)/Image
	$(QEMU) -kernel $<
debug: $(BUILD)/Image
	$(QEMU) -kernel $< -s -S
clean:
	rm -r $(BUILD)

-include $(OBJS:.o=.d)
