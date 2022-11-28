#!/bin/sh

install()
{
    modprobe mmc_core
    modprobe mmc_block
    insmod /usr/modules/ak_mci.ko
}

uninstall()
{
    rmmod /usr/modules/ak_mci.ko
    modprobe -r mmc_block
    modprobe -r mmc_core
}

usage()
{
    echo "usage :" 
    echo "    ./tf_card.sh <install/uninstall>"
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

