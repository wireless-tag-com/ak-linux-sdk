#!/bin/sh

install()
{
#    modprobe media.ko
#    modprobe videobuf2_core.ko
#    modprobe videobuf2_memops.ko
#    modprobe videodev.ko
#    modprobe videobuf2_dma_contig.ko
#    modprobe v4l2_common.ko
#    modprobe videobuf2_v4l2.ko
    
    insmod /usr/modules/ak_isp.ko
    insmod /usr/modules/sensor_gc2063.ko

    if test ! -f /etc/config/isp_gc2063_mipi_2lane_h3b.conf ; then
        rm /etc/config/isp_*.conf
        cp /etc/isp_gc2063_mipi_2lane_h3b.conf /etc/config/
    fi

}

uninstall()
{
    rmmod /usr/modules/sensor_gc2063.ko
    rmmod /usr/modules/ak_isp.ko

#    modprobe -r media.ko
#    modprobe -r videobuf2_core.ko
#    modprobe -r videobuf2_memops.ko
#    modprobe -r videodev.ko
#    modprobe -r videobuf2_dma_contig.ko
#    modprobe -r v4l2_common.ko
#    modprobe -r videobuf2_v4l2.ko
    
}

usage()
{
    echo "usage :" 
    echo "    ./gc2063.sh <install/uninstall>"
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

