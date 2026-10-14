#!/bin/sh
# Erase, format, and partition an already mounted usd card device based on specified
# mount point and size of filesystem to format
# also verify that the mount point of uSD card is not the running linux OS uSD card
# and if so exit

PATH=/usr/bin:/bin:/usr/sbin:/sbin

# lsof | grep media
# stop udev so it does not try to automount external uSD card while we are
# reformatting it
/etc/init.d/udevd stop

if [ "$1" != "" ]
then
    mount_point=$1
    root_device=`readlink /dev/root`
    # if the mount point specified is the root device or uSD card with
    # root linux partition on it then block formatting it
    if [ "/media/${root_device}" == "${mount_point}" ]
    then
        echo "Can not format root device at %{mount_point}.  This is the uSD with Linux root partition on it."
        exit 1
    fi
else
    echo "missing mount point of device to reformat"
    exit 1
fi

if [ "$2" != "" ]
then
    if [ "$2" == "8" ] || [ "$2" == "16" ] || [ "$2" == "32" ]
    then
        usd_size_in_gb=$2
    else
        echo "Invalid size entered for SD card, must be 8, 16, or 32"
        exit 1
    fi
else
    echo "missing size argument in GB for the filesystem size for the format"
    exit 1
fi

usd_device_with_partition_number=`findmnt -n -o SOURCE --target ${mount_point}`
#usd_device_with_partition_number=`/dev/mmcblk0p1`

# remove partition number which is last two characters
usd_device=${usd_device_with_partition_number%??}

if [ -b ${usd_device} ]
then
    echo "uSD card found at ${usd_device}"
else
    echo "uSD card is not present, insert uSD card into outboard slot"
fi

if [ -d ${mount_point} ]
then
    echo "SD card is present unmounting device to partition and reformat as ext4"
    umount -f ${mount_point}
fi

## remove partition table from SD card
dd if=/dev/zero of=${usd_device} bs=4096 count=1

case $usd_size_in_gb in

8)    (echo o; echo n; echo p; echo 1; echo; echo; echo w) | fdisk ${usd_device}
      ;;

16)    (echo o; echo n; echo p; echo 1; echo; echo; echo w) | fdisk ${usd_device}
       ;;

32)    (echo o; echo n; echo p; echo 1; echo; echo; echo w) | fdisk ${usd_device}
       ;;

esac

# refresh partition tables
partprobe ${usd_device}

sleep 5

## format the new partition with ext4 filesystem
echo "Applying EXT4 filesystem to primary partition "
mkfs.ext4 -F -Litw ${usd_device_with_partition_number}

# remount
mount -t ext4 ${usd_device_with_partition_number} ${mount_point}

if [ ! -d ${mount_point} ]
then
    echo "Error re-mounting primary partition!"
    exit
fi

/etc/init.d/udevd start

sync
