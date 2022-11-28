#!/bin/sh
DIR_RESERVE="/tmp/reserve"
mount_reserve_dir()
{
    HEXDUMP_ADDR=`cat /proc/ion | grep -E "^base-address" | grep -Eo '[0-9A-Fa-f]+$'`
    HEXDUMP_SIZE=`cat /proc/ion | grep -E "^size" | grep -Eo '[0-9A-Fa-f]+$'`
    HEX_MAX_SIZE=2000000                                                        #最大的内存保留区间大小32MB,此处为16进制

    DEC_HEXDUMP_SIZE=`echo "obase=10;ibase=16;$HEXDUMP_SIZE"|bc`
    DEC_MAX_SIZE=`echo "obase=10;ibase=16;$HEX_MAX_SIZE"|bc`

    if [ $DEC_HEXDUMP_SIZE -gt $DEC_MAX_SIZE ]; then                            #大于最大的内存保留分区大小则mount默认值
        MOUNT_SIZE=$HEX_MAX_SIZE
    else
        MOUNT_SIZE=$HEXDUMP_SIZE
    fi

    umount /dev/reserved_ram0
    rmmod ak_ramdisk
    INSMOD="insmod /usr/modules/ak_ramdisk.ko disk_addr=0x$HEXDUMP_ADDR disk_size=0x$MOUNT_SIZE"
    echo $INSMOD
    $INSMOD
    mkdir -p ${DIR_RESERVE}
    mount -t tmpfs /dev/reserved_ram0 ${DIR_RESERVE}
    df
}
mount_reserve_dir