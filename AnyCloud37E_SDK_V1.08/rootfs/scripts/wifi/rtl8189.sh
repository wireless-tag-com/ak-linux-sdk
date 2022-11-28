#! /bin/sh
### BEGIN INIT INFO
# File:                rtl8189.sh
# Provides:         driver install/uninstall, operate as station/ap 
# Required-Start:   $
# Required-Stop:
# Default-Start:     
# Default-Stop:
# Short-Description: rtl8189 wifi script
# Author:            
# Email:             
# Date:                2020-02-28
### END INIT INFO

PATH=$PATH:/bin:/sbin:/usr/bin:/usr/sbin
MODE=$1
usage()
{
    echo "Usage: $0 MODE"
    echo "       MODE:    ap | sta | stop "
    echo "               ap         Start wifi as ap role"
    echo "               sta        Start wifi as station role"
    echo "               stop        Stop  wifi "
}

wifi_stop()
{
    killall wpa_supplicant
    killall udhcpc
    killall udhcpd
    killall hostapd
    rmmod rtl8189ftv
    rmmod cfg80211
}

driver_install()
{
    wifi_stop
    modprobe cfg80211
    modprobe mmc_core
    insmod /usr/modules/ak_mci.ko
    insmod /usr/modules/rtl8189ftv.ko
    sleep 2
}

####### main
case "$MODE" in
    ap)
        driver_install
        ifconfig wlan0  172.14.10.1
        mkdir -p /var/lib/misc/
        touch /var/lib/misc/udhcpd.leases
	rm -f /dev/random
	ln -s  /dev/urandom  /dev/random
        udhcpd /etc/config/udhcpd.conf -S
        hostapd /etc/config/hostapd.conf -B
        ;;
    sta)
        driver_install
        wpa_supplicant -Dnl80211 -i wlan0 -c /etc/config/wpa_supplicant.conf -B
        sleep 1
        udhcpc -i wlan0
        ;;
    stop)
        wifi_stop
        ;;
    *)
        usage
        ;;
esac
exit 0

