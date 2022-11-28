#!/bin/sh

install()
{
#    modprobe industrialio.ko
#    modprobe iio-trig-interrupt.ko
#    modprobe iio-trig-sysfs.ko
    insmod /usr/modules/ak_saradc.ko
}

uninstall()
{
    rmmod ak_saradc.ko

#    modprobe -r industrialio.ko
#    modprobe -r iio-trig-interrupt.ko
#    modprobe -r iio-trig-sysfs.ko
}

usage()
{
    echo "usage :" 
    echo "    ./sar-adc.sh <install/uninstall>"
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

