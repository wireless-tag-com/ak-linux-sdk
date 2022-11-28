#!/bin/sh

echo "now start to extract.sh"

if [ -d "rootfs" ];  then
   	echo "find rootfs dir"
	rm -rf "rootfs"
	mkdir "rootfs"
else
	echo "can't find rootfs dir"
	mkdir "rootfs"
fi

tar xvfz rootfs.tar.gz -C rootfs/
#tar xvfz resource.tar.gz
cp -rf resource/* rootfs
find rootfs -name .gitkeep | xargs rm -f
#rm -rf rootfs/bin/busybox
