#!/bin/sh

install()
{
    insmod /usr/modules/ak_gpio_keys.ko
}

uninstall()
{
    rmmod ak_gpio_keys.ko
}

usage()
{
    echo "usage :" 
    echo "    ./key.sh <install/uninstall>"
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

