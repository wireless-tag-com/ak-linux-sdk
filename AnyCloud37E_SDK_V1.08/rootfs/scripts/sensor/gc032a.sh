#!/bin/sh


install()
{
    echo "install gc032a driver"
    /sbin/insmod /usr/modules/ak_image_capture.ko
    /sbin/insmod /usr/modules/sensor_gc032a.ko
}

uninstall()
{
    echo "uninstall gc032a driver"
    /sbin/rmmod ak_image_capture.ko
    /sbin/rmmod sensor_gc032a.ko
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

