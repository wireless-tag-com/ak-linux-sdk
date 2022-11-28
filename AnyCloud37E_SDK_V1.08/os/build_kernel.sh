#!/bin/bash

# 导入芯片类型
source ./config.mk

usage()
{
	echo "Usage : "
	echo "    Compile Kernel       : ./build_kernel.sh"
	echo "    Clean Kernel         : ./build_kernel.sh -c"
	echo "    Install Kernel Image : ./build_kernel.sh -i"
}

# 获取CPU核数，后续编译时用于提高编译效率
get_num_of_cpu_cores() 
{
    cpu_string=`cat /proc/cpuinfo | grep "cpu cores" | uniq`
    #num_of_cpu_cores=${cpu_string:0-1:1}
    num_of_cpu_cores=${cpu_string##*:}
}

get_config_info()
{
	CHIP_SERIES=`awk -F= '/^CHIP_SERIES/{gsub(" ","",$2); printf $2}' ../config.mk`
	chip_series=$(echo $CHIP_SERIES | tr '[A-Z]' '[a-z]')
}

get_toolchain()
{
	CROSS_COMPILE=`awk -F= '/^CROSS_COMPILE/{gsub(" ","",$2); printf $2}' ../rules.mk`
	echo "CROSS_COMPILE : $CROSS_COMPILE"
}

clean_kernel()
{
    # 检查编译过程目录是否存在，如果存在先清除掉上一次的编译
    if test -d $kernel_build_dir ; then
		echo "Build kernel dir($kernel_build_dir) , clean it"
        rm -rf $kernel_build_dir
    fi
}

install_kernel_image()
{
	# 安装镜像
	echo "Install kernel image!"
	find bd/ -name uImage | xargs -i cp {} $kernel_install_dir

	# 安装dtb文件
	echo "Install dtb file!"
	find bd/ -name *.dtb | xargs -i cp {} $kernel_install_dir
}

# 编译内核
build_kernel()
{
    # 创建编译的目标目录
	if test ! -d $kernel_build_dir ; then
    	mkdir $kernel_build_dir
	fi
    
    # 检查内核源码目录是否正常
    if test ! -d "kernel" ; then
        echo "Build Kernel failed, can't find kernel dir!"
		exit 1
    fi
   
    
    # 切换到源码目录，准备编译
    cd kernel
    
    # 根据芯片类型进行内核配置
	make O=$kernel_build_dir anycloud_ak37e_mini_defconfig CROSS_COMPILE=$CROSS_COMPILE

    # 检查配置是否正常
    if test $? -ne 0 ; then
        exit 1
    fi
    
    # 编译内核
	make O=$kernel_build_dir dtbs modules uImage -j$num_of_cpu_cores CROSS_COMPILE=$CROSS_COMPILE
    
    # 检查是否编译出错
    if test $? -ne 0 ; then
        exit 1
    fi

    # 安装内核模块
    make modules_install O=$kernel_build_dir INSTALL_MOD_PATH=$driver_install_dir/internal

    cd ..
}

option=$1

get_toolchain

if test ! -z $1 ; then
	if test $option = "-h" ; then
		usage
		exit 0
	fi
	
	if test $option = "-c" ; then
		clean_kernel
		exit 0
	fi
	
	if test $option = "-i" ; then
		install_kernel_image
		exit 0
	fi
fi

get_num_of_cpu_cores

get_config_info

build_kernel
