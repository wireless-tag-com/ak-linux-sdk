#!/bin/bash

# 切换到os目录，准备编译内核和驱动
cd os

# 检查是否有残留的kernel目录
#if test -d kernel ; then
#	rm -rf kernel
#fi

# 解压缩内核，并进行编译
#tar xvfz linux.tar.gz
./build_kernel.sh
./build_kernel.sh -i

# 检查是否存在编译错误
if test $? -ne 0 ; then
	echo "Kernel build failed!"
    exit
fi

# 检查是否有残留的uboot目录
if test -d uboot ; then
	rm -rf uboot
fi

# 解压缩uboot，并进行编译
tar xvfz uboot.tar.gz
./build_uboot.sh
./build_uboot.sh -i

# 检查是否存在编译错误
if test $? -ne 0 ; then
	echo "Uboot build failed!"
    exit
fi

# 将驱动KO安装到文件系统中
cp -rf driver/* ../rootfs/ko/

# 制作文件系统
cd ../
./make_image.sh

# 安装镜像
cp -rf image/* tools/burntool
