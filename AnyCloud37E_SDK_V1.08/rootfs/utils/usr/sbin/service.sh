#!/bin/sh
udhcpc&
if [ -f "/data/app.sh" ]
then
	echo "/data/app.sh"
	/data/app.sh
elif [ -f "/etc/config/app.sh" ]
then
	echo "/etc/config/app.sh"
	/etc/config/app.sh
fi
