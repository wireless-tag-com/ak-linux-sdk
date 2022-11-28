#!/bin/sh


install()
{
    echo "install tp9950 driver"
    /sbin/insmod /usr/modules/ak_image_capture.ko
    /sbin/insmod /usr/modules/sensor_tp9950.ko
}

uninstall()
{
    echo "uninstall tp9950 driver"
    /sbin/rmmod ak_image_capture.ko
    /sbin/rmmod sensor_tp9950.ko
}

usage()
{
    echo "usage :" 
    echo "    ./sensor.sh <install/uninstall>"
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
