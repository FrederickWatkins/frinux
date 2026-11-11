# Top-level Makefile
#
#   make defconfig    copy arch/$(DEFCONFIG_ARCH)/defconfig to .config
#   make              build $(BUILD)/Image
#   make run          boot in QEMU
#   make debug        boot in QEMU, wait for gdb on :1234
#   make clean        remove build output
#   make distclean    also remove .config
#
# All build configuration lives in .config (see config.mk and
# arch/<arch>/config.mk for how the CONFIG_* options are turned into flags).

BUILD          := build
DEFCONFIG_ARCH ?= riscv

.DEFAULT_GOAL := all
.PHONY: all defconfig run debug clean distclean

# ---- targets that work without a .config ----
defconfig:
	cp arch/$(DEFCONFIG_ARCH)/defconfig .config

clean:
	rm -rf $(BUILD)

distclean: clean
	rm -f .config

# ---- everything else needs a .config ----
ifeq ($(wildcard .config),)

ifneq ($(filter-out defconfig clean distclean,$(or $(MAKECMDGOALS),all)),)
$(error No .config found. Run 'make defconfig' first)
endif

else

include .config

ARCH  := $(patsubst "%",%,$(strip $(CONFIG_ARCH)))
CROSS := $(patsubst "%",%,$(strip $(CONFIG_CROSS_COMPILE)))

include arch/$(ARCH)/config.mk   # TARGET, ARCH_CFLAGS, ARCH_LDFLAGS, QEMU
include config.mk                # CC, OBJCOPY, CFLAGS, LDFLAGS, LDS
-include qemu.mk

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

# ---- generated config header (CONFIG_* for C and assembly) ----
AUTOCONF    := $(BUILD)/generated/autoconf.h
CONFIG_DEPS := .config $(AUTOCONF)

# CONFIG_X=y        -> #define CONFIG_X 1
# CONFIG_X=n        -> (nothing)
# CONFIG_X="str"    -> #define CONFIG_X "str"
# CONFIG_X=VALUE    -> #define CONFIG_X_VALUE 1
$(AUTOCONF): .config
	@mkdir -p $(dir $@)
	sed -n \
	    -e 's/^\(CONFIG_[A-Za-z0-9_]*\)=y$$/#define \1 1/p;t' \
	    -e 's/^\(CONFIG_[A-Za-z0-9_]*\)=n$$//;t' \
	    -e 's/^\(CONFIG_[A-Za-z0-9_]*\)=\(".*"\)$$/#define \1 \2/p;t' \
	    -e 's/^\(CONFIG_[A-Za-z0-9_]*\)=\([A-Za-z0-9_][A-Za-z0-9_]*\)$$/#define \1_\2 1/p' \
	    $< > $@

# ---- rules ----
all: $(BUILD)/Image

$(BUILD)/%.o: %.c $(CONFIG_DEPS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: %.S $(CONFIG_DEPS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.ld:: %.ld.S $(CONFIG_DEPS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -E -P -x c -D__LINKER__ $< -o $@

$(BUILD)/kernel.elf: $(OBJS) $(LDS) $(BUILTINS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(BUILTINS) -o $@

$(BUILD)/Image: $(BUILD)/kernel.elf
	$(OBJCOPY) -O binary $< $@

run: $(BUILD)/Image
	$(QEMU) $(QEMU_EXTRA) -kernel $<

debug: $(BUILD)/Image
	$(QEMU) -kernel $< -s -S

-include $(OBJS:.o=.d)

endif
