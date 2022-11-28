#!/bin/sh

install()
{
	modprobe usb-common
	modprobe udc-core
    insmod /usr/modules/ak_udc.ko
#    modprobe configfs
    modprobe libcomposite
    modprobe usb_f_mass_storage
    modprobe g_mass_storage file=/dev/mmcblk0p1 stall=0 removable=1
    modprobe g_mass_storage file=/dev/mmcblk0 stall=0 removable=1
}

uninstall()
{
#    modprobe -r configfs
    modprobe -r g_mass_storage
    modprobe -r usb_f_mass_storage
    modprobe -r libcomposite

    rmmod ak_udc
	modprobe -r udc-core
	modprobe -r usb-common
}

usage()
{
    echo "usage :" 
    echo "    ./usb_udc.sh <install/uninstall>"
    echo "    default : install"

}

if test $# -gt 1 ; then
    usage
elif test $# -eq 0 ; then
    install
elif test $# -eq 1 ; then
    if test $1 = "install" ; then
        install
    elif test $1 = "uninstall" ; then
        uninstall
    else
        usage
    fi
fi

