#!/bin/sh

install()
{
    modprobe usb-common
    modprobe usbcore
    insmod /usr/modules/ak_hcd.ko
}

uninstall()
{
    rmmod ak_hcd.ko
    modprobe -r usbcore
    modprobe -r usb-common
}

usage()
{
    echo "usage :" 
    echo "    ./usb_hcd.sh <install/uninstall>"
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

