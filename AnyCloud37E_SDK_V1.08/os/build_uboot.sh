#!/bin/bash

# 包含编译配置选项
source ./config.mk

usage()
{
	echo "Usage : "
	echo "    Compile Uboot       : ./build_uboot.sh"
	echo "    Clean Uboot         : ./build_uboot.sh -c"
	echo "    Install Uboot Image : ./build_uboot.sh -i"
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
	FLASH_TYPE=`awk -F= '/^FLASH_TYPE/{gsub(" ","",$2); printf $2}' ../config.mk`
	CHIP_SERIES=`awk -F= '/^CHIP_SERIES/{gsub(" ","",$2); printf $2}' ../config.mk`
	SCREEN_TYPE=`awk -F= '/^SCREEN_TYPE/{gsub(" ","",$2); printf $2}' ../config.mk`
	DRAM=`awk -F= '/^DRAM/{gsub(" ","",$2); printf $2}' ../config.mk`
	SPI_BUS_WIDTH=`awk -F= '/^SPI_BUS_WIDTH/{gsub(" ","",$2); printf $2}' ../config.mk`

	chip_series=$(echo $CHIP_SERIES | tr '[A-Z]' '[a-z]')
	echo "FLASH_TYPE="$FLASH_TYPE", CHIP_SERIES="$CHIP_SERIES
}

get_toolchain()
{
	CROSS_COMPILE=`awk -F= '/^CROSS_COMPILE/{gsub(" ","",$2); printf $2}' ../rules.mk`
	echo "CROSS_COMPILE : $CROSS_COMPILE"
}

modify_uboot_config()
{
	sed -i -e 's/^\#define CONFIG_RTT_DTB_ADDR/\/\*\#define CONFIG_RTT_DTB_ADDR\*\//' include/configs/ak_board.h
	
	# 根据芯片类型及屏幕类型修改ak_board.h文件中的屏幕相关的宏定义
	if test $SCREEN_TYPE = "RGB" ; then
	    sed -i -e "s/^.*_CORE_BOARD.*/#define CONFIG_RGB_CORE_BOARD/" include/configs/ak_board.h
	    echo "screen RGB"
	elif test $SCREEN_TYPE = "MPU" ; then
	    sed -i -e "s/^.*_CORE_BOARD.*/#define CONFIG_RGB_CORE_BOARD/" include/configs/ak_board.h
	    echo "screen MPU"
	else
	    sed -i -e "s/^.*_CORE_BOARD.*/#define CONFIG_MIPI_CORE_BOARD/" include/configs/ak_board.h
	    echo "screen MIPI"
	fi
	
	# 根据芯片类型修改ak_board.h文件中的DRAM相关的宏定义
	if test $DRAM = "64M" ; then
	    sed -i -e "s/^.*CONFIG_DRAM.*/#define CONFIG_DRAM_64M/" include/configs/ak_board.h
	    echo "DRAM 64M"
	elif test $DRAM = "128M" ; then
	    sed -i -e "s/^.*CONFIG_DRAM.*/#define CONFIG_DRAM_128M/" include/configs/ak_board.h
	    echo "DRAM 128M"
	fi
	
	# 根据FLASH类型修改ak_board.h中flash相关的宏定义
	if test $FLASH_TYPE = "NOR" ; then
	    count=`sed -n -e '/\/\*#define CONFIG_SPINAND_FLASH\*\//p' include/configs/ak_board.h | wc -l`
	    echo "count="$count
	    if test $count -ne 1; then
	        sed -i -e 's/\#define CONFIG_SPINAND_FLASH/\/\*\#define CONFIG_SPINAND_FLASH\*\//' include/configs/ak_board.h
	    fi
	
		count=`sed -n -e '/\/\*#define CONFIG_SPL_BOOT\*\//p' include/configs/ak_board.h | wc -l`
	    echo "count="$count
	    if test $count -ne 1; then
	        sed -i -e 's/\#define CONFIG_SPL_BOOT/\/\*\#define CONFIG_SPL_BOOT\*\//' include/configs/ak_board.h
	    fi
	
	    echo "spi nor flash"
	else
	    sed -i -e 's/\/\*\#define CONFIG_SPINAND_FLASH\*\//\#define CONFIG_SPINAND_FLASH/' include/configs/ak_board.h
	    sed -i -e 's/\/\*\#define CONFIG_SPL_BOOT\*\//\#define CONFIG_SPL_BOOT/' include/configs/ak_board.h
	    echo "spi nand flash"
	fi
	
	# 根据芯片类型确定spi flash是一线还是4线
	if test $SPI_BUS_WIDTH = "1x" ; then
	    sed -i -e 's/^\#define CONFIG_SPI_FLASH_BUSWIDTH_4X/\/\*\#define CONFIG_SPI_FLASH_BUSWIDTH_4X\*\//' include/configs/ak_board.h
	elif test $SPI_BUS_WIDTH = "4x" ; then
	    sed -i -e 's/^\/\*\#define CONFIG_SPI_FLASH_BUSWIDTH_4X\*\//\#define CONFIG_SPI_FLASH_BUSWIDTH_4X/' include/configs/ak_board.h
	fi
}

clean_uboot()
{
	cd uboot

	# 先进行编译前的清理
	make distclean
	make clean

	cd -
}

install_uboot_image()
{
	cd uboot

	# 按配置的目录安装编译好的镜像
	if test $FLASH_TYPE = "NAND" ; then
		cp -f spl/u-boot-spl.bin $uboot_install_dir
	    cp -f u-boot.img $uboot_install_dir
	else
	    cp -f u-boot.bin $uboot_install_dir
	fi

	cd -
}

build_uboot()
{
	cd uboot

	# 根据配置项修改头文件
	modify_uboot_config
	
	# 根据芯片类型配置uboot
	make anycloud_ak37e_config CROSS_COMPILE=$CROSS_COMPILE
	
	# 检测配置过程是否存在错误
	if test $? -ne 0 ; then
	    exit 1
	fi
	
	# 编译uboot
	make all -j$num_of_cpu_cores CROSS_COMPILE=$CROSS_COMPILE
	
	# 检测编译过程是否存在错误
	if test $? -ne 0 ; then
	    exit 1
	fi

	cd -
}

option=$1

get_toolchain

get_config_info

if test ! -z $1 ; then
	if test $option = "-h" ; then
		usage
		exit 0
	fi
	
	if test $option = "-c" ; then
		clean_uboot
		exit 0
	fi
	
	if test $option = "-i" ; then
		install_uboot_image
		exit 0
	fi
fi

get_num_of_cpu_cores 

# 检测源码是否已导入
if test ! -d "uboot" ; then
    echo "Build u-boot failed, can't find uboot src dir!"
	exit 1
fi

build_uboot
