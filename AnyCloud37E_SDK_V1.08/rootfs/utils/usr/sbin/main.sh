#!/bin/sh

function get_partition_dev()
{
	mtdblockid=`cat /proc/mtd| grep "$1"| grep -E -o "mtd[0-9]+" | grep -E -o "[0-9]+"`
	if [ "$mtdblockid" != "" ];then
		echo "/dev/mtdblock$mtdblockid"
	else
		echo ""
	fi
}

#print kernel error default
echo 4 > /proc/sys/kernel/printk

#start ftp server, dir=root r/w, -t 600s(timeout)
/usr/bin/tcpsvd 0 21 ftpd -w / -t 600 &

echo "start telnet......"
/usr/sbin/telnetd &

echo "                     __                     "
echo "            _   ____ \/ ____  __            "
echo "     /\    | \ | |\ \  / /| |/ /    /\      "
echo "    /  \   |  \| | \ \/ / | ' /    /  \     "
echo "   / /\ \  | . ' |  \  /  |  <    / /\ \    "
echo "  / ____ \ | |\  |  |  |  | . \  / ____ \   "
echo " /_/    \_\|_| \_|  |__|  |_|\_\/_/    \_\  "
echo "                                            " 

#start syslogd & klogd, log rotated 3 files(200KB) to /var/log/messages
syslogd -D -n -O /var/log/messages -s 200 -b 3 & # -l prio
klogd -n & # -c prio

ifconfig lo 127.0.0.1


dmesg > /tmp/start_message

## set min free reserve bytes
echo 2048 > /proc/sys/vm/min_free_kbytes

echo "/tmp/core_%e_%p_%t" > /proc/sys/kernel/core_pattern

#insmod /usr/modules/ak_adc_key.ko
insmod /usr/modules/ak_rtc.ko
adjust_base_on_timer=`cat /sys/devices/platform/rtc/ak_rtc/adjust_base_on_timer`
echo 0 > /sys/devices/platform/rtc/ak_rtc/adjust_base_on_timer
hwclock -s
echo $adjust_base_on_timer > /sys/devices/platform/rtc/ak_rtc/adjust_base_on_timer

#insmod /usr/modules/ak_fb.ko lcd_ctl_force_init=1
#insmod /usr/modules/ak_pwm_char.ko
#insmod /usr/modules/ak_fb.ko
#insmod /usr/modules/ak_saradc.ko
insmod /usr/modules/ak_i2c.ko combined_format=1
insmod /usr/modules/ak_pcm.ko
#insmod /usr/modules/ak_gpio_keys.ko
#insmod /usr/modules/ak_gui.ko
insmod /usr/modules/ak_ion.ko
insmod /usr/modules/ak_leds.ko
#insmod /lib/modules/4.4.192V1.5/kernel/drivers/mmc/core/mmc_core.ko
#insmod /lib/modules/4.4.192V1.5/kernel/drivers/mmc/card/mmc_block.ko
#insmod /usr/modules/ak_mci.ko
insmod /usr/modules/ak_uio.ko
#insmod /usr/modules/exfat.ko

# start network
insmod /usr/modules/ak_eth.ko
ifconfig eth0 up
if test ! -f "/etc/config/mac.txt" ; then
    cat /sys/class/net/eth0/address > /etc/config/mac.txt
else
	cat /etc/config/mac.txt | xargs -i /sbin/ifconfig eth0 hw ether {}
fi

#/usr/sbin/screen.sh

# mount yaffs2 file-system.
# mount -t yaffs2 /dev/mtdblock9 /data
partition_data=`get_partition_dev DATA`
if [ "$partition_data" != "" ];then
	echo "/bin/mount -t yaffs2 $partition_data /data"
	/bin/mount -t yaffs2 $partition_data /data
fi

#start system service
/usr/sbin/service.sh start &



