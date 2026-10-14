#!/bin/bash
#
# This script takes the USD tarball image from the build server and generates an SD Image suitable
# for burning directly to an SD card. This image is for the ARK board.
#
# Optionally this script will burn the image it generates to an SD card.
#
# Usage:
#   $ ./makeSD-ark.sh hti-linux-usd-tarball [/dev/sdX]
#
# Requirements:
#   findmnt
#   md5sum
#   mkfs
#   parted
#   sshpass
#
#
# The below table shows how the SD card is layed out:
#
# what       | offset | size | type | description
# -----------|--------|------|------|----------------------------------------------------------
# part table | 0      |      | n/a  | SD card partition table
# uboot      | 1K     |      | n/a  | raw UBOOT binary blob
# BOOT       | 1      | 32   | FAT  | BOOT partition; holds kernel, DTBs, splash screens
# recovery   | 33     | 2048 | ext4 | recovery partition
# hobart     | 2081   | 4096 | ext4 | live partition, expands to fill up SD card on first boot
#
# The sdcard.img image generated from this script may be burned via tools such as Win32DiskImager.
#

#set -x
#set -e

SDCARD_ALIGNMENT=1M
SDCARD=sdcard.img

BOOTFS_OFFSET=1
BOOTFS_SIZE=32
RECOVERYFS_SIZE=2048
ROOTFS_SIZE=4096

UBOOT=u-boot-ark.imx
KERNEL=uImage

USD=hti-linux.tgz
DEVICE=none

BUILDSERVER=wwbuildserver
BUILDSERVERDIR=/opt/builds/ht/trunk

# check permissions
if [ "$EUID" -ne 0 ]; then
    echo "Please run as root by using sudo"
    exit 1
fi

# check the usd image file passed in
if [ $# -ge 1 ]; then
    USD=$1
fi

USDCHECKSUM=${USD%.tgz}.md5

if [ ! -f ${USD} ]; then
    echo "USD image ${USD} is missing"
    echo

    read -p "Would you like to try and pull this USD image from the build server? (yes/no): " opt

    if [[ $opt == [yY] || $opt == [yY][eE][sS] ]]; then
        echo
    else
        exit 1
    fi

    # pull down usd image
    sshpass -p build1mgr scp -q -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null buildmgr@${BUILDSERVER}:${BUILDSERVERDIR}/${USD} ${USD} 

    result=$?
    if [ $result -ne  0 ]; then
        echo "Error pulling USD image from the build server"
        exit 1
    fi

    if [ ! -f ${USD} ]; then
        exit 1
    fi

    # attempt to pull down checksum
    sshpass -p build1mgr scp -q -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null buildmgr@${BUILDSERVER}:${BUILDSERVERDIR}/${USDCHECKSUM} ${USDCHECKSUM} 

fi

# checksum file
if [ -f ${USDCHECKSUM} ]; then

    MD5SUM=$(md5sum $USD | cut -c-32)

    grep -qs "$MD5SUM" ${USDCHECKSUM}  > /dev/null 2>&1
        
    result=$?
    if [ $result -ne  0 ]; then
        echo "Checksum mismatch, please try to pull the USD image again"
        exit 1
    fi
fi

# check the device passed in
if [ $# -ge 2 ]; then
    DEVICE=$2

    # disallow the use of sda
    if [[ $DEVICE == *sda* ]]; then
        echo "This script will not work on /dev/sda, which is usually a hard disk"
        exit 1
    fi

    if [ ! -e ${DEVICE} ]; then
        echo "The device ${DEVICE} is missing"
        exit 1
    fi

    udevadm info --query=property --name=${DEVICE} | grep 'ID_USB_DRIVER=usb-storage'  > /dev/null 2>&1

    result=$?
    if [ $result -ne  0 ]; then
        echo "Device ${DEVICE} does not report as 'usb storage' it is unlikely to be a SD card."
        echo "Being cautious and exiting..."
        exit 1
    fi
fi

##########################
# Generate SD filesystems
##########################

echo "Creating filesystem images..."

MNTDIR=$(mktemp -d /tmp/XXXXXXXX)

bootimg=${MNTDIR}/BOOT.img
recoveryimg=${MNTDIR}/recovery.img
hobartimg=${MNTDIR}/hobart.img

remove_filesystems () {
    if [ -f ${bootimg} ]; then
        rm ${bootimg}
    fi
    if [ -f ${recoveryimg} ]; then
        rm ${recoveryimg}
    fi
    if [ -f ${hobartimg} ]; then
        rm ${hobartimg}
    fi
    if [ -d ${MNTDIR} ]; then
        rmdir ${MNTDIR}
    fi
}

# $1 - image name
# $2 - image size in MB
# $3 - partition label
make_fat_part () {
    mkfs.vfat -n $3 -S 512 -F 32 -C $1 $(expr $2 \* 1024) > /dev/null 2>&1

    result=$?
    if [ $result -ne  0 ]; then
        remove_filesystems
        echo "Error: Make filesystem failed. Exiting!"
        exit 255
    fi
}

# $1 - image name
# $2 - image size in MB
# $3 - partition label
make_ext4_part () {
    dd if=/dev/zero of=$1 bs=${SDCARD_ALIGNMENT} oflag=sync conv=fsync count=$2 > /dev/null 2>&1

    result=$?
    if [ $result -ne  0 ]; then
        remove_filesystems
        echo "Error: Zeroing dd command failed. Exiting!"
        exit 255
    fi

    has_64bit=0
    has_metadata_csum=0

    if grep -qs "64bit" /etc/mke2fs.conf; then
        has_64bit=1
    fi

    if grep -qs "metadata_csum" /etc/mke2fs.conf; then
        has_metadata_csum=1
    fi

    if [ $has_64bit -ne 0 ] && [ $has_metadata_csum -ne 0 ]; then
        mkfs -t ext4 -L $3 -O ^64bit,^metadata_csum $1 $(expr $2 \* 1024) > /dev/null 2>&1
    elif [ $has_64bit -eq 0 ] && [ $has_metadata_csum -eq 0 ]; then
        mkfs -t ext4 -L $3 $1 $(expr $2 \* 1024) > /dev/null 2>&1
    else
        remove_filesystems
        echo "Error: Make filesystem failed with bad options. Exiting!"
        exit 255        
    fi

    result=$?
    if [ $result -ne  0 ]; then
        remove_filesystems
        echo "Error: Make filesystem failed. Exiting!"
        exit 255
    fi
}

make_fat_part ${bootimg} ${BOOTFS_SIZE} "BOOT"
make_ext4_part ${recoveryimg} ${RECOVERYFS_SIZE} "recovery"
make_ext4_part ${hobartimg} ${ROOTFS_SIZE} "hobart"

################
# Mount SD Card
################

echo "Mounting filesystem images..."

boot=${MNTDIR}/BOOT
recovery=${MNTDIR}/recovery
hobart=${MNTDIR}/hobart

umount_filesystems () {
    if [ -e ${boot} ]; then
        if [[ $(findmnt -M "$boot") ]]; then
            umount ${boot}
        fi
        rmdir ${boot}
    fi
    if [ -e ${recovery} ]; then
        if [[ $(findmnt -M "$recovery") ]]; then
            umount ${recovery}
        fi
        rmdir ${recovery}
    fi
    if [ -e ${hobart} ]; then
        if [[ $(findmnt -M "$hobart") ]]; then
            umount ${hobart}
        fi
        rmdir ${hobart}
    fi
}

# $1 - image name
# $2 - mount point
mount_filesystem () {
    mkdir $2
    mount -o loop $1 $2 > /dev/null 2>&1

    result=$?
    if [ $result -ne  0 ]; then
        umount_filesystems
        remove_filesystems
        echo "Error: Failed to mount partition. Exiting!"
        exit 255
    fi
}

mount_filesystem ${bootimg} ${boot}
mount_filesystem ${recoveryimg} ${recovery}
mount_filesystem ${hobartimg} ${hobart}

######################
# Extract Filesystems
######################

echo "Extracting USD Tarball..."

tar xf ${USD} -C ${MNTDIR}

result=$?
if [ $result -ne  0 ]; then
    umount_filesystems
    remove_filesystems
    echo "Error: Failed to extract filesystem. Exiting!"
    exit 255
fi

# validate
files=( \
    "${hobart}/${UBOOT}" \
    "${hobart}/boot/${KERNEL}" \
    "${hobart}/boot/imx6q-ark.dtb" \
    "${hobart}/boot/imx6q-ark-lcd.dtb" \
    "${hobart}/boot/imx6q-ark-hdmi.dtb" \
    "${hobart}/boot/imx6dl-ark.dtb" \
    "${hobart}/boot/imx6dl-ark-lcd.dtb" \
    "${hobart}/boot/imx6dl-ark-hdmi.dtb" \
    "${hobart}/uboot/uboot-version.txt" \
    "${hobart}/uboot/htm8.bmp" \
    "${hobart}/uboot/htx8.bmp" \
    "${hobart}/uboot/hti8.bmp" \
    "${hobart}/uboot/hts8.bmp" \
    "${hobart}/uboot/u-diagnostics.bmp" \
    "${hobart}/uboot/u-missing.bmp" \
    "${hobart}/uboot/u-version.bmp" \
    "${hobart}/uboot/slicer/hobart.bmp" \
    "${hobart}/uboot/slicer/u-diagnostics.bmp" \
    "${hobart}/uboot/slicer/u-error.bmp" )

for file in "${files[@]}"; do
    if [ ! -f ${file} ]; then
        umount_filesystems
        remove_filesystems
        echo "Error: Required file ${file} is missing. Exiting!"
        exit 255
    fi
done

# save off uboot for later use
if [ -f /tmp/${UBOOT} ]; then
    rm /tmp/${UBOOT}
fi

cp ${hobart}/${UBOOT} /tmp/${UBOOT}

############################################
# Install optional features from preinstall
############################################

echo "Installing Office Viewer and PDF Viewer..."

tar xf ${hobart}/usr/local/hobart/preinstall/hti-linux-feature-office_viewer*.tgz -C ${hobart}
tar xf ${hobart}/usr/local/hobart/preinstall/hti-linux-feature-pdf_viewer*.tgz -C ${hobart}

touch ${hobart}/var/lib/dpkg/status

PYTHONPATH=$PYTHONPATH:${hobart}/usr/local/hobart/python python ${hobart}/usr/local/hobart/python/installOfficeViewer.py ${hobart} ""  > /dev/null 2>&1

rm ${hobart}/usr/local/hobart/preinstall/hti-linux-feature-office_viewer*.*
rm ${hobart}/usr/local/hobart/preinstall/hti-linux-feature-pdf_viewer*.*
rm -rf /tmp/Libre*

#########################
# Install BOOT Partition
#########################

echo "Generating BOOT Partition..."

# setup tree
mkdir ${boot}/slicer

# copy files
cp ${hobart}/boot/${KERNEL} ${boot}

cp ${hobart}/boot/imx6q-ark*.dtb ${boot}
cp ${hobart}/boot/imx6dl-ark*.dtb ${boot}

cp ${hobart}/uboot/*.bmp ${boot}
cp ${hobart}/uboot/slicer/*.bmp ${boot}/slicer

cp ${hobart}/uboot/uboot-version.txt ${boot}

##################
# Unmount SD Card
##################

echo "Ejecting filesystem images..."

umount_filesystems

sync

####################
# Generate SD image
####################

echo "Creating SD Image..."

if [ -f ${SDCARD} ]; then
    rm ${SDCARD}
fi

dd if=/dev/zero of=${SDCARD} bs=${SDCARD_ALIGNMENT} oflag=sync conv=fsync count=$(expr 1 \+ ${BOOTFS_OFFSET} \+ ${BOOTFS_SIZE} \+ ${RECOVERYFS_SIZE} \+ ${ROOTFS_SIZE}) > /dev/null 2>&1

result=$?
if [ $result -ne  0 ]; then
    echo "Error: Zeroing dd command failed. Exiting!"
    exit 255
fi

echo "Partition SD Image..."
parted -s ${SDCARD} mklabel msdos
parted -s ${SDCARD} unit MiB mkpart primary fat32 ${BOOTFS_OFFSET} $(expr ${BOOTFS_OFFSET} \+ ${BOOTFS_SIZE})
parted -s ${SDCARD} unit MiB mkpart primary $(expr ${BOOTFS_OFFSET} \+ ${BOOTFS_SIZE}) $(expr ${BOOTFS_OFFSET} \+ ${BOOTFS_SIZE} \+ ${RECOVERYFS_SIZE})
parted -s ${SDCARD} unit MiB mkpart primary $(expr ${BOOTFS_OFFSET} \+ ${BOOTFS_SIZE} \+ ${RECOVERYFS_SIZE}) $(expr ${BOOTFS_OFFSET} \+ ${BOOTFS_SIZE} \+ ${RECOVERYFS_SIZE} \+ ${ROOTFS_SIZE})
#parted ${SDCARD} print

################
# Install UBOOT
################

echo "Installing UBOOT to SD Image..."

dd if=/tmp/${UBOOT} of=${SDCARD} conv=notrunc seek=1 bs=1K > /dev/null 2>&1

result=$?
if [ $result -ne  0 ]; then
    umount_filesystems
    remove_filesystems
    echo "Error: Copy dd command failed. Exiting!"
    exit 255
fi

#####################
# Install Filesystem
#####################

echo "Installing filesystem to SD Image..."

# $1 - image name
# $2 - image size in MB
# $3 - image offset in MB
install_image () {
    dd if=$1 of=${SDCARD} bs=${SDCARD_ALIGNMENT} oflag=sync conv=fsync count=$2 seek=$3 > /dev/null 2>&1

    result=$?
    if [ $result -ne  0 ]; then
        remove_filesystems
        echo "Error: Copy dd command failed. Exiting!"
        exit 255
    fi
}

install_image ${bootimg} ${BOOTFS_SIZE} ${BOOTFS_OFFSET}
install_image ${recoveryimg} ${RECOVERYFS_SIZE} $(expr ${BOOTFS_OFFSET} \+ ${BOOTFS_SIZE})
install_image ${hobartimg} ${ROOTFS_SIZE} $(expr ${BOOTFS_OFFSET} \+ ${BOOTFS_SIZE} \+ ${RECOVERYFS_SIZE})

remove_filesystems

sync

# check if we are done
if [ "$DEVICE" == "none" ]; then
    echo "Done!"
    exit 0
fi

##################
# Install SD Card
##################

echo "Copying SD image to SD card device..."

# when SD card is mounted lets make sure they want to umount and blow it up
if grep -qs "$DEVICE" /proc/mounts; then
    echo
    echo "!!! WARNING !!!"
    echo "The requested volume $DEVICE is already mounted."
    echo "Possibly this volume is a hard disk or some other important medium."
    echo
    echo "THIS SCRIPT IS ABOUT TO OVERWRITE $DEVICE DEVICE !!!"
    echo

    read -p "Are you sure you want to continue? (yes/no): " opt

    # they have to type yes...
    if [[ $opt == [yY][eE][sS] ]]; then
        echo
    else
        echo "Aborting... nothing was altered!"
        exit 0
    fi

    # umount
    ( umount $DEVICE* )  > /dev/null 2>&1

fi

dd if=$SDCARD of=$DEVICE bs=10M conv=fsync oflag=sync  > /dev/null 2>&1

result=$?
if [ $result -ne  0 ]; then
    echo "Error: Zeroing dd command failed. Exiting!"
    exit 255
fi

sync

################
# Mount SD Card
################

echo "Mounting SD card device..."

# refresh partition tables
partprobe ${DEVICE}

result=$?
if [ $result -ne  0 ]; then
    echo "Warning: Failed to probe device."
else

    # give OS a moment to catch up...
    sleep 8

    # remount
    udisksctl mount -b ${DEVICE}1 --no-user-interaction  > /dev/null 2>&1

    result=$?
    if [ $result -ne  0 ]; then
        echo "Warning: Failed to mount BOOT partition."
    fi

    udisksctl mount -b ${DEVICE}2 --no-user-interaction  > /dev/null 2>&1

    result=$?
    if [ $result -ne  0 ]; then
        echo "Warning: Failed to mount recovery partition."
    fi

    udisksctl mount -b ${DEVICE}3 --no-user-interaction  > /dev/null 2>&1

    result=$?
    if [ $result -ne  0 ]; then
        echo "Warning: Failed to mount hobart partition."
    fi

fi

echo "Done!"

