#!/bin/sh

#---------------------------------------------------------------------------------------------------------
# This version of makeSD has been modified for the Transcend "Industrial" 16GB SD cards.  These were found
# to be approximately 600MB smaller than the normal Transcend 16GB SD cards.  To account for this and
# still have the same size recovery partition, the size of the main hobart partition has been reduced
# to 12.5GB (12,500MB).
#---------------------------------------------------------------------------------------------------------

##This script assumes that the image to be place on the SD card has been built and an SD card is
##in the card reader and has been mounted in the media folder.
##This script is intended for building an SD for the Hobart BCM board only.
##This script is to be started in the timesys root directory.
##TO DO: need to finish processing evt2 revision board 04032013.
##Aaron Swift 3/22/2013

PATH=/usr/bin:/bin:/usr/sbin:/sbin
floppy=/media/floppy
sdDev=/dev/sdc
sdSizeGB=8
buildserver_IpAddress=10.3.176.4

# Make sure the build_dir variable is set to the location of the working copy on the build server
build_dir="/home2/branchBuildSystem/hte/trunk-test-build"

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
sudo dd if=/dev/zero of=${sdDev} bs=4096 count=1

## create new partition on SD card
echo "Formating the SD card"

case $sdSizeGB in

8)    (echo o; echo n; echo p; echo 1; echo; echo +6G;  echo n; echo p; echo 2; echo; echo +1350M;  echo w) | sudo fdisk ${sdDev}
      ;;

16)    (echo o; echo n; echo p; echo 1; echo; echo +12500M;  echo n; echo p; echo 2; echo; echo +1350M;  echo w) | sudo fdisk ${sdDev}
       ;;

32)    (echo o; echo n; echo p; echo 1; echo; echo +28G;  echo n; echo p; echo 2; echo; echo +1350M;  echo w) | sudo fdisk ${sdDev}
       ;;

esac


##
echo "Applying EXT4 filesystem to primary partition "

## format the new partition with ext4 filesystem
sudo mkfs.ext4 -Lhobart ${sdDev}1

echo "Applying EXT4 filesystem to recovery partition "
sudo mkfs.ext4 -Lrecovery ${sdDev}2

# if the card was previously formatted check if it's present and remove.
if [ -d ${mountPoint}/hobart ]; then
    echo "recovery present from previous format. umount and remove directory...."
    umount ${mountPoint}/hobart
    umount ${mountPoint}/recovery
fi

echo "Eject SD card from device and reinsert into reader/writer device"
while true; do
    read -p "Press <Enter> after card re-inserted and it mounts in your VM" yn
    if [ -d ${mountPoint}/hobart ]
    then
    break
    else
    echo "SD card not mounted yet, re-insert"
    fi
done

echo "SD card formated and mounted....."
echo "Applying root file system to SD card."
tar xvzf hti-linux.tgz -C ${mountPoint}

cd /

echo "Installing Office Viewer and PDF Viewer features ..."
tar xvzf ${mountPoint}/hobart/usr/local/hobart/preinstall/hti-linux-feature-office_viewer*.tgz -C ${mountPoint}/hobart
tar xvzf ${mountPoint}/hobart/usr/local/hobart/preinstall/hti-linux-feature-pdf_viewer*.tgz -C ${mountPoint}/hobart

sudo touch ${mountPoint}/hobart/var/lib/dpkg/status

sudo PYTHONPATH=$PYTHONPATH:${mountPoint}/hobart/usr/local/hobart/python python ${mountPoint}/hobart/usr/local/hobart/python/installOfficeViewer.py ${mountPoint}/hobart ""

sudo rm -rf /tmp/Libre*
sudo rm -rf ${mountPoint}/hobart/usr/local/hobart/preinstall

sync

umount ${mountPoint}/hobart
umount ${mountPoint}/recovery

echo "SD card is ready for target....."
