
# Cross Tools and libs
CROSS_PATH      ?= /opt/arm-anykav500-crosstool/usr
CROSS_COMPILE    ?= arm-anykav500-linux-uclibcgnueabi-
ARM_LIBC_PATH	?=$(CROSS_PATH)/arm-anykav500-linux-uclibcgnueabi/sysroot/usr/lib
ARM_LIBGCC_PATH ?=$(CROSS_PATH)/lib/gcc/arm-anykav500-linux-uclibcgnueabi/4.8.5

# Tools
CC           = $(CROSS_COMPILE)gcc
CXX          = $(CROSS_COMPILE)g++
AS           = $(CROSS_COMPILE)as
AR           = $(CROSS_COMPILE)ar
LD           = $(CROSS_COMPILE)ld
RM           = rm -rf
MKDIR        = mkdir
OBJDUMP      = $(CROSS_COMPILE)objdump
OBJCOPY	     = $(CROSS_COMPILE)objcopy
STRIP        = $(CROSS_COMPILE)strip

ifeq ($(CHIP_SERIES), AK37D)
CFLAGS += -D__CHIP_AK37D_SERIES
endif

ifeq ($(CHIP_SERIES), AK39EV33X)
CFLAGS += -D__CHIP_AK39EV33X_SERIES
endif

ifeq ($(CHIP_SERIES), AK37E)
CFLAGS += -D__CHIP_AK37E_SERIES
endif

CFLAGS += -Werror -D_GNU_SOURCE -std=c99 -mlittle-endian  -fno-builtin -nostdlib -O2 -mlong-calls $(DEFINE) $(INCLUDE) $(GLB_INCLUDE)
ASFLAGS += -mlittle-endian -x assembler-with-cpp -O2 

export CLIB := $(ARM_LIBC_PATH)/libm.a $(ARM_LIBC_PATH)/libc.a  $(ARM_LIBGCC_PATH)/libgcc.a


# Rules


# --------------------------- s -> o
%.o:%.s
	@echo ---------------------[build $<]----------------------------------
	$(CC) -c $(ASFLAGS) $(CFLAGS) -o $@ $<

# ----------------------------- c -> o
%.o:%.c
	@echo ---------------------[build $<]----------------------------------
	$(CC) -c $(CFLAGS) $(INCLUDE) -o $@ $<

