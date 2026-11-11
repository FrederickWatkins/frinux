# Architecture-independent build settings, driven by .config.
#
# Expects (from the top-level Makefile and arch/$(ARCH)/config.mk):
#   ARCH, CROSS, BUILD, TARGET, ARCH_CFLAGS, ARCH_LDFLAGS

TOOLCHAIN := $(patsubst "%",%,$(strip $(CONFIG_TOOLCHAIN)))
OPTIMIZE  := $(patsubst "%",%,$(strip $(CONFIG_OPTIMIZE)))

LDS := $(BUILD)/arch/$(ARCH)/linker.ld

# ---- toolchain selection ----
ifeq ($(TOOLCHAIN),LLVM)
CC                := clang
OBJCOPY           := llvm-objcopy
TOOLCHAIN_CFLAGS  := --target=$(TARGET) -Wno-unused-command-line-argument
TOOLCHAIN_LDFLAGS := -fuse-ld=lld
BUILTINS          := $(shell $(CC) $(TOOLCHAIN_CFLAGS) $(ARCH_CFLAGS) \
                       -rtlib=compiler-rt -print-libgcc-file-name)
else ifeq ($(TOOLCHAIN),GCC)
CC                := $(CROSS)gcc
OBJCOPY           := $(CROSS)objcopy
TOOLCHAIN_CFLAGS  :=
TOOLCHAIN_LDFLAGS :=
BUILTINS          := $(shell $(CC) $(ARCH_CFLAGS) -print-libgcc-file-name)
else
$(error CONFIG_TOOLCHAIN must be GCC or LLVM (got '$(TOOLCHAIN)'))
endif

# ---- common flags ----
COMMON_CFLAGS := $(OPTIMIZE) \
                 -Wall -Wextra -ffreestanding -fno-stack-protector \
                 -fno-pic -fno-asynchronous-unwind-tables -fno-unwind-tables \
                 -Iinclude -Iarch/$(ARCH)/include \
                 -include $(BUILD)/generated/autoconf.h \
                 -MMD -MP
COMMON_CFLAGS += $(if $(filter y,$(CONFIG_DEBUG_INFO)),-g)
COMMON_CFLAGS += $(if $(filter y,$(CONFIG_WERROR)),-Werror)

COMMON_LDFLAGS := -nostdlib -static -no-pie -Wl,--build-id=none \
                  -Wl,-T,$(LDS) -Wl,-Map=$(BUILD)/kernel.map

CFLAGS  := $(TOOLCHAIN_CFLAGS) $(ARCH_CFLAGS) $(COMMON_CFLAGS)
LDFLAGS := $(TOOLCHAIN_LDFLAGS) $(ARCH_LDFLAGS) $(COMMON_LDFLAGS)
