#!/bin/sh

##This script assumes that the image to be place on the SD card has been built and an SD card is
##in the card reader and has been mounted in the media folder.
##This script is intended for building an SD for the Hobart BCM board only.
##This script is to be started in the timesys root directory.
##TO DO: need to finish processing evt2 revision board 04032013.
##Aaron Swift 3/22/2013

PATH=/usr/bin:/bin:/usr/sbin:/sbin
floppy=/media/floppy
sdDev=/dev/sde
sdSizeGB=8
buildserver_IpAddress=wwbuildserver

# Make sure the build_dir variable is set to the location of the working copy on the build server
build_dir="/opt/builds/ht/trunk"

# Usually either "/media" or "/media/hobart" depending upon your Ubuntu image on VMWare 
mountPoint=/media/hobart

if [ "$(whoami)" != 'root' ];
then
    echo "You need to run this script as root."
    exit 1
fi

read -p "Enter SD card device name (Default: ${sdDev}): " deviceName

if [ "$deviceName" != "" ];
then
    sdDev=${deviceName}
fi

read -p "Enter SD card size [8=8GB, 16=16GB or 32=32GB] (Default: ${sdSizeGB}): " sdSizeEntered

if [ "$sdSizeEntered" != "" ];
then

    if [ "$sdSizeEntered" = "8" ] || [ "$sdSizeEntered" = "16" ] || [ "$sdSizeEntered" = "32" ]; then

        sdSizeGB=${sdSizeEntered}

    else

        echo "Invalid size entered for SD card"
        exit 1

    fi
fi

# Get the tarball from the build server
read -p "Get release from build system: (Y/N): " yn

if [ "$yn" = "Y" ] || [ "$yn" = "y" ]; then

    read -p "Enter release identifier X.X.X: " version

    rm -f hti-linux.tgz

    scp "buildmgr@${buildserver_IpAddress}:${build_dir}/hti-linux-usd-${version}-*.tgz" hti-linux.tgz
    if [ $? -ne 0 ]; then
        echo "Cannot get release build from the build server"
        exit 1
    fi
fi

## check that the SD card is present.
if [ -b ${sdDev}1 ]
then
    echo "SD card found at ${sdDev}1"
else
    echo "SD card is not present, please insert card into reader, mount and run script again."
    exit 1
fi

if [ -d ${mountPoint}/hobart ]
then
    echo "SD card is present unmount in progress..."
    umount ${mountPoint}/hobart
    umount ${mountPoint}/recovery
else
    echo "SD card search..."
    for file in ${mountPoint}/*; do
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

## remove partition table from SD card
dd if=/dev/zero of=${sdDev} bs=4096 count=1

## create new partition on SD card
echo "Formating the SD card"

case $sdSizeGB in

8)    (echo o; echo n; echo p; echo 1; echo; echo +6G;  echo n; echo p; echo 2; echo; echo +1350M;  echo w) | fdisk ${sdDev}
      ;;

16)    (echo o; echo n; echo p; echo 1; echo; echo +13G;  echo n; echo p; echo 2; echo; echo +1350M;  echo w) | fdisk ${sdDev}
       ;;

32)    (echo o; echo n; echo p; echo 1; echo; echo +27G;  echo n; echo p; echo 2; echo; echo +1350M;  echo w) | fdisk ${sdDev}
       ;;

esac

# refresh partition tables
partprobe ${sdDev}

## format the new partition with ext4 filesystem
echo "Applying EXT4 filesystem to primary partition "
mkfs.ext4 -F -O^64bit,^metadata_csum -Lhobart ${sdDev}1

echo "Applying EXT4 filesystem to recovery partition "
mkfs.ext4 -F -O^64bit,^metadata_csum -Lrecovery ${sdDev}2

# remount
udisksctl mount -b ${sdDev}1 --no-user-interaction
udisksctl mount -b ${sdDev}2 --no-user-interaction

mountPoint=/media/root

if [[ ! -d ${mountPoint}/hobart ]]; then
    echo "Error re-mounting hobart partition!"
    exit
fi

if [[ ! -d ${mountPoint}/recovery ]]; then
    echo "Error re-mounting recovery partition!"
    exit
fi

echo "SD card formated and mounted....."
echo "Applying root file system to SD card."
tar xvzf hti-linux.tgz -C ${mountPoint}

cd /

echo "Installing Office Viewer and PDF Viewer features ..."
tar xvzf ${mountPoint}/hobart/usr/local/hobart/preinstall/hti-linux-feature-office_viewer*.tgz -C ${mountPoint}/hobart
tar xvzf ${mountPoint}/hobart/usr/local/hobart/preinstall/hti-linux-feature-pdf_viewer*.tgz -C ${mountPoint}/hobart

touch ${mountPoint}/hobart/var/lib/dpkg/status

PYTHONPATH=$PYTHONPATH:${mountPoint}/hobart/usr/local/hobart/python python ${mountPoint}/hobart/usr/local/hobart/python/installOfficeViewer.py ${mountPoint}/hobart ""

rm -rf /tmp/Libre*
rm -f ${mountPoint}/hobart/usr/local/hobart/preinstall/hti-linux-feature-office_viewer*.* ${mountPoint}/hobart/usr/local/hobart/preinstall/hti-linux-feature-pdf_viewer*.*

sync

umount ${mountPoint}/hobart
umount ${mountPoint}/recovery

echo "SD card is ready for target....."
