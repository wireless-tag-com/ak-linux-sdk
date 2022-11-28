# Project name

PROJ = ak_mux_sample

LIB_DEP := plat_common  plat_mem plat_thread plat_log plat_ai mpi_aenc plat_osal mpi_venc plat_vqe mpi_mux
LIB_DEP += akaudiofilter akaudiocodec akv_encode akmedia plat_dbg

ifeq ($(VI), y)
	LIB_DEP += plat_vi
endif

ifeq ($(ISPSDK), y)
    LIB_DEP += plat_isp_sdk
endif

ifeq ($(VPSS), y)
	LIB_DEP += plat_vpss
endif
    
ifneq ($(OS_TYPE), 0)
    TARGET = lib$(PROJ).a
else
    TARGET = $(PROJ)
    LIB_DEP += pthread
endif

LIBS = $(addprefix -l, $(LIB_DEP))

TOPDIR 	?= $(shell pwd)/../..

# Flags
ifneq ($(VI), n)
	CFLAGS 	+= -DVI_ENABLE
endif

# Flags


INCLUDE    =	-I. \
		-I../../include_inner \
		-I../../include \
		-I../../src/include \
		-I../include \
		-I$(EXT_LIB_DIR)/include \

#Locations

SRCDIR += .

CSRCS += $(foreach d, $(SRCDIR), $(wildcard $d/*.c))

COBJS += $(patsubst %.c, %.o, $(CSRCS))

LIB_DIR = $(TOPDIR)/lib
RTSP_LIB_DIR := $(TOPDIR)/lib/rtsp_lib

LD_FLAGS = -Xlinker "-(" $(LIBS) -Xlinker "-)" -L$(LIB_DIR) -L$(RTSP_LIB_DIR)

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
