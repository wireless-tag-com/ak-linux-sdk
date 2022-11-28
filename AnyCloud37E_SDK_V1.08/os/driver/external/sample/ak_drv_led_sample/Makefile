####################################################################################################
#	@Copyright (C) 	Anyka 2019-12-16
#	@brief					Create Linux4.4.x linux driver sample
#	@Version				V1.0.0 
#####################################GCC Compile Flags #############################################
DEBUG := n
ifeq ($(DEBUG), y)
	CFLAGS := -g -O0
	CFLAGS += -DAK_MEM_DEBUG=1
	CXXFLAGS := -g -O0
else
	CFLAGS := -fno-strict-aliasing -Os
	CXXFLAGS := -fno-strict-aliasing -Os
endif

CFLAGS += -Wall -Werror -D_GNU_SOURCE -std=c99 -fms-extensions

#####################################GCC Objects ###################################################
#定义当前的工作目录
CUR_DIR=$(shell pwd)  
SRC_DIR=$(CUR_DIR)

#链接共享库库
#LOCAL_SHARED_LIB:= akaudiofilter

#链接静态库
#LOCAL_STATIC_LIB:= plat_ai plat_thread

#定义链接库的路径
#LOCAL_LDFLAGS := -L$(PLAT_LIB_DIR)
#LOCAL_LDFLAGS += $(addprefix $(PLAT_LIB_DIR)/,$(patsubst %,lib%.a,$(LOCAL_STATIC_LIB)))

#添加链接库路径到LDFLAGS
#LDFLAGS := $(LOCAL_LDFLAGS)
#LDFLAGS += $(addprefix -l,$(patsubst lib%,%,$(LOCAL_SHARED_LIB)))

#添加标准库参数到LDFLAGS
STD_LDFLAGS = -lrt -lpthread -ldl -Wl,-rpath=/mnt -rdynamic
LDFLAGS += $(STD_LDFLAGS)

#定义当前的工作目录头文件路径
#INC_DIR :=$(CUR_DIR)/include
INC_DIR := $(CUR_DIR)/
LOCAL_INCLUDES := $(INC_DIR)
CFLAGS += $(addprefix -I,$(LOCAL_INCLUDES))

#找到当前模块目录下的所有.c的源码文件
SRCS		:=	${wildcard *.c}

#将当前的所有源码文件，替换成不带目录路径的.o目标文件
OBJS		:=	${patsubst %.c, %.o, ${SRCS}}

#定义编译输出的模块镜像名称
TARGET := ak_drv_led_sample

#定义编译镜像的输出路径
BUILDPATH  := ./output
WORKDIR:=$(shell pwd)
INSTALL_BIN_DIR ?= $(WORKDIR)/output

#####################################Platform Related Compile Toolchain#############################
#定义工具链compile toolchain
CROSS_COMPILE ?= arm-anykav500-linux-uclibcgnueabi-
CC :=      $(CROSS_COMPILE)gcc
AS :=      $(CROSS_COMPILE)as
AR :=      $(CROSS_COMPILE)ar
#LD :=     $(CROSS_COMPILE)ld
LD :=      $(CC)
NM :=      $(CROSS_COMPILE)nm
OBJCOPY := $(CROSS_COMPILE)objcopy
OBJDUMP := $(CROSS_COMPILE)objdump
STRIP :=  $(CROSS_COMPILE)strip
CAT=     cat
RM :=     rm -rf
CP := 		cp -f
MKDIR=   mkdir
MV =     mv

#定义编译输出镜像的名称可以调整
#USER_MODULE_NAME ?= 
#ifneq ($(USER_MODULE_NAME),)
#MODULE_NAME := $(USER_MODULE_NAME)
#endif

#####################################Make Compile Module ###########################################
#定义编译链接过程
all:$(TARGET)
	@echo "make all first step!"
	
$(TARGET):$(OBJS)
	@echo "make modules!"
	@echo $(OBJS)
	$(LD) $^ $(LDFLAGS) -o $@
	$(STRIP) $@

#定义c代码编译输出.o代码		
%.o : %.c
	$(CC) $(CFLAGS) -c $^ -o $@
	@echo ""

#.PHONY: all modules install clean test
.PHONY: all $(TARGET) install clean test

install:
	$(CP) $(TARGET) $(INSTALL)
	@echo "install to " $(INSTALL)

clean:
	$(RM) *.o *.bak
	$(RM) $(TARGET)

test:
	@echo $(CUR_DIR)
	@echo $(SRCS)
	@echo $(OBJS)
