#!/bin/bash
#
# This script will stage Edgify installation to an SD card.
#
# Usage:
#   sudo ./stageEdgifyInstall.sh [mountpoint]
#
#   mountpoint     Mount point of root partition, for example:
#                  /media/hobart/root
#

# check permissions
if [ "$EUID" -ne 0 ]; then
    echo "Please run as root (by using sudo)"
    exit 1
fi

part=/media/hobart/root

if [ ! -d ${part} ]; then
    part=/media/root/root
fi

if [ $# -ge 1 ]; then
    part=$2
fi

if [ ! -d ${part} ]; then
    echo "Target mount point does not exist!"
    exit 1
fi

# copy files
folder=$(dirname $0)/distrib

cp ${folder}/rc.local ${part}/etc
cp ${folder}/edgify-*.rpm ${part}/home/root

chmod +x ${part}/etc/rc.local

sync

echo "Done!"
