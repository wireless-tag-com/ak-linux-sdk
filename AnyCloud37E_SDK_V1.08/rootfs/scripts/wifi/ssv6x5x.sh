#! /bin/sh
### BEGIN INIT INFO
# File:             ssv6x5x.sh
# Provides:         
# Required-Start:   
# Required-Stop:
# Default-Start:     
# Default-Stop:
# Short-Description: 
# Author:            
# Email:             
# Date:                2021-02-20
### END INIT INFO

PATH=$PATH:/bin:/sbin:/usr/bin:/usr/sbin
MODE=$1
usage()
{
    echo "Usage: $0 MODE"
    echo "       MODE:   ap | sta | stop "
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
    rmmod ssv6x5x
    rmmod ak_hcd
    rmmod usbcore
    rmmod usb-common
	rmmod mac80211
    rmmod cfg80211
}

driver_install()
{
    wifi_stop
	modprobe cfg80211.ko
	modprobe mac80211.ko
  
	modprobe mmc_core.ko
    insmod /usr/modules/ak_mci.ko  

    modprobe usb-common.ko
    modprobe usbcore.ko
	
    insmod 	/usr/modules/ak_hcd.ko 
    insmod 	/usr/modules/ssv6x5x.ko 	stacfgpath=/etc/config/ssv6x5x-wifi.cfg
    sleep 2
}

####### main
case "$MODE" in
    ap)
        driver_install
        ifconfig wlan0  172.14.10.1
        mkdir -p /var/lib/misc/
        touch /var/lib/misc/udhcpd.leases
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

