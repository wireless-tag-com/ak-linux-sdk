#!/bin/bash

boot_type=0                # -b参数
spl_partion_size=0         # -s参数
uboot_partion_size=0       # -u参数
env_partion_size=0         # -e参数
dtb_partion_size=0         # -d参数
block_size=0
block_unit=0               # -B参数
version="V1.0.00"

usage() {
    echo "Usage : ./mkuboot.sh <options>, such as:./mkuboot.sh -b 1 -u 200 -d 48 -e 4 -B 4"
	echo "  -b : Boottype, 1 --> uboot, 2 --> spl"
    echo "  -s : Spl partion size(unit:KB)"
    echo "  -u : Uboot partion size(unit:KB)"	
	echo "  -d : dtb partion size(unit:KB)"
	echo "  -e : env partion size(unit:KB)"
	echo "  -B : block size(unit:KB, nor flash: (4-default, 32, 64), nand flash: (128-default))"
	exit 1
}

print_parameter() {
	echo "boot_type="$boot_type
	echo "spl_partion_size="$spl_partion_size
	echo "uboot_partion_size="$uboot_partion_size
	echo "env_partion_size="$env_partion_size
	echo "dtb_partion_size="$dtb_partion_size
}

verify_parameter() {
	if test $boot_type -ne 1 -a $boot_type -ne 2 ; then
		echo "Boottype parameter error! Please check!"
		usage
	fi

	if test $boot_type -eq 1 ; then
		if test $block_unit -ne 4 -a $block_unit -ne 32 -a $block_unit -ne 64 ; then
			echo "block size error! Please check!"
			usage
		fi
	else
		if test $block_unit -ne 128 ; then
			echo "block size error! Please check!"
			usage
		fi
	fi
}

verify_file() {
    if test ! -e ./u-boot.bin ; then
		echo "Can't find u-boot.bin file!"
	    exit 2
	fi

	if test ! -e ./env.img ; then
		echo "Can't find env.img file!"
		exit 2
	fi

	if test ! -e ./cloudOS.dtb ; then
		echo "Can't find cloudOS.dtb file!"
		exit 2
	fi

	if test $boot_type -eq 2 ; then
		if test ! -e ./u-boot-spl.bin ; then
			echo "Can't find u-boot-spl.bin file!"
			exit 2
		fi
	fi
}

mkuboot() {
    if test $boot_type = "1" ; then
		if test $block_unit -eq 0 ; then
            block_unit=4
	    fi

		block_size=$(($block_unit*1024))
		if test $(($uboot_partion_size % $block_unit)) -ne 0 -o $(($env_partion_size % $block_unit)) -ne 0 -o $(($dtb_partion_size % $block_unit)) -ne 0 ; then
			echo "Align Error..."
			usage
		fi
		tr '\000' '\377' < /dev/zero | dd of=./uboot.img bs=1k count=$(($uboot_partion_size + $env_partion_size * 2 + $dtb_partion_size))
        dd if=./u-boot.bin of=./uboot.img bs=$block_size conv=fsync conv=notrunc
        dd if=./env.img of=./uboot.img bs=$block_size seek=$(($uboot_partion_size/$block_unit)) conv=fsync conv=notrunc
        dd if=./env.img of=./uboot.img bs=$block_size seek=$(($(($uboot_partion_size + $env_partion_size))/$block_unit)) conv=fsync conv=notrunc
		dd if=./cloudOS.dtb of=./uboot.img bs=$block_size seek=$(($(($uboot_partion_size + $env_partion_size * 2))/$block_unit)) conv=fsync conv=notrunc
		echo "Make uboot image finish! Success!"
	elif test $boot_type = "2" ; then
		if test $block_unit -eq 0 ; then
            block_unit=128
	    fi

		block_size=$(($block_unit*1024))
		if test $(($spl_partion_size % $block_unit)) -ne 0 -o $(($uboot_partion_size % $block_unit)) -ne 0 -o $(($env_partion_size % $block_unit)) -ne 0 -o $(($dtb_partion_size % $block_unit)) -ne 0 ; then
			echo "Align Error..."
			usage
		fi
		tr '\000' '\377' < /dev/zero | dd of=./uboot.img bs=1k count=$(($spl_partion_size + $uboot_partion_size + $env_partion_size* 2 + $dtb_partion_size))
        dd if=./u-boot-spl.bin of=./uboot.img bs=$block_size conv=sync conv=notrunc
        dd if=./env.img of=./uboot.img bs=$block_size seek=$(($spl_partion_size/$block_unit)) conv=fsync conv=notrunc
        dd if=./env.img of=./uboot.img bs=$block_size seek=$(($(($spl_partion_size + $env_partion_size))/$block_unit)) conv=fsync conv=notrunc
		dd if=./cloudOS.dtb of=./uboot.img bs=$block_size seek=$(($(($spl_partion_size + $env_partion_size * 2))/$block_unit)) conv=fsync conv=notrunc
		dd if=./u-boot.bin of=./uboot.img bs=$block_size seek=$(($(($spl_partion_size + $env_partion_size * 2 + $dtb_partion_size))/$block_unit)) conv=fsync conv=notrunc
		echo "Make uboot image finish! Success!"
	else
        echo "Parameter error, please check!"
		usage
	fi
}

echo "Version Information : "$version
if test $# -eq 0 ; then
	usage
elif test $# -ne 10 -a $# -ne 8 ; then
	usage
fi

while getopts "b:s:u:e:d:B:" arg
do
	case $arg in
		b)
			boot_type=$OPTARG
			;;
		s)
			spl_partion_size=$OPTARG
			;;
		u)
			uboot_partion_size=$OPTARG
			;;
		e)
			env_partion_size=$OPTARG
			;;
		d)
			dtb_partion_size=$OPTARG
			;;
		B)
			block_unit=$OPTARG
			;;
		?)
			echo "unknow argument"
			usage
			;;
	esac
done


verify_parameter

verify_file

mkuboot

#print_parameter
