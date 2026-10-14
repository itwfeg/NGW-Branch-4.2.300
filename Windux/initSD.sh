#!/bin/sh

#---------------------------------------------------------------------
# This script re-initializes an SD card that has previously been used.
# It removes the hobart and recovery partitions on the SD card
# and re-creates them.
#
# Adapted from makeSD.sh
#---------------------------------------------------------------------

PATH=/usr/bin:/bin:/usr/sbin:/sbin
floppy=/media/floppy
sdDev=/dev/sdb

#if [ "$(whoami)" != 'root' ]; 
#then
#    echo "You need to run this script as root."
#    exit 1
#fi

read -p "Enter SD card device name (Default: ${sdDev}): " deviceName

if [ "$deviceName" != "" ];
then
    sdDev=${deviceName}
fi

## check that the SD card is present.
if [ -b ${sdDev}1 ]
then
    echo "SD card found at ${sdDev}1"
else
    echo "SD card is not present, please insert card into reader, mount and run script again."
    exit 1
fi

if [ -d /media/hobart ] 
then 
    echo "SD card is present unmount in progress..."
    umount /media/hobart
    umount /media/recovery
else
    echo "SD card search..."
    for file in /media/*; do
	if [ -d $file ]; then
	    if [ "$file" != "$floppy" ]; then
		echo "SD card found $file"
		umount "$file"					
            fi
	fi
    done
fi

## The BCM board currently is not boot straped to boot from serial flash. So we need to create a 10M unformatted partition 
## to place u-boot and kernel.

##remove partition table from SD card
sudo dd if=/dev/zero of=${sdDev} bs=4096 count=1

##create new partition on SD card
echo "--------------------------------------"
echo "Formating the SD card"
(echo o; echo n; echo p; echo 1; echo; echo +6G;  echo n; echo p; echo 2; echo; echo +1350M;  echo w) | sudo fdisk ${sdDev}

##
echo "--------------------------------------"
echo "Applying EXT4 filesystem to primary partition " 
echo "Formatting partition: " ${sdDev}1
##format the new partition with ext4 filesystem
sudo mkfs.ext4 -Lhobart ${sdDev}1

echo "--------------------------------------"
echo "Applying EXT4 filesystem to recovery partition " 
echo "Formatting partition: " ${sdDev}2
sudo mkfs.ext4 -Lrecovery ${sdDev}2 

#if the card was previously formatted check if it's present and remove.
if [ -d /media/hobart ]; then
	echo "recovery present from previous format. umount and remove directory...."
	umount /media/hobart
	umount /media/recovery
fi

echo "--------------------------------------"
echo "SD card has been re-inititalized."

