# Project name

PROJ = ak_venc_sample

LIBS = $(addprefix -l, $(LIB_DEP))

TARGET = ak_venc_sample

ifneq ($(OS_TYPE), 0)
    TARGET = libak_venc_sample.a
    LIB_DEP = plat_common  plat_mem plat_thread pthread  plat_log plat_osal mpi_venc akv_encode plat_dbg
else
    TARGET = ak_venc_sample
    LIB_DEP = plat_common  plat_mem plat_thread pthread  plat_log plat_osal mpi_venc akv_encode plat_dbg
    
    ifeq ($(VI), y)
    	LIB_DEP += plat_vi
    endif

    ifeq ($(ISPSDK), y)
        LIB_DEP += plat_isp_sdk
    endif

    ifeq ($(VPSS), y)
    	LIB_DEP += plat_vpss
    endif

endif

TOPDIR 	?= $(shell pwd)/../..

# Flags
ifneq ($(VI), n)
	CFLAGS 	+= -DVI_ENABLE
endif

INCLUDE    =	-I. \
		-I../../include_inner \
		-I../../include \
		-I../include \
		-I$(EXT_LIB_DIR)/include \

#Locations

SRCDIR += .

CSRCS += $(foreach d, $(SRCDIR), $(wildcard $d/*.c))

COBJS += $(patsubst %.c, %.o, $(CSRCS))

LIB_DIR = $(TOPDIR)/lib

LD_FLAGS = -Xlinker "-(" $(LIBS) -Xlinker "-)"  -L$(LIB_DIR)

ifeq ($(OS_TYPE), 0)
LD_FLAGS += -L$(EXT_LIB_DIR)
endif

#build rule
.PHONY: all target install clean

all: $(COBJS)
ifneq ($(OS_TYPE), 0)
	$(AR) -rsv $(TARGET) $(COBJS)
	@cp -r $(TARGET) $(TOPDIR)/lib
else
	$(CC) $(COBJS) $(LD_FLAGS) -o $(TARGET)
	$(STRIP) $(TARGET)
endif

install:
ifeq ($(OS_TYPE), 0)
	@cp -f $(TARGET) $(SAMPLE_INSTALL_DIR)
else
	@cp -f $(TARGET) $(PLATFORM_INSTALL_DIR)/lib
endif

clean : 
	$(RM) $(COBJS) $(TARGET)

ifndef RULES_MK_FILE
RULES_MK_FILE=$(TOPDIR)/../../rules.mk
endif


# Rules
include $(RULES_MK_FILE)
