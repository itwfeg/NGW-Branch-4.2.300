#!/bin/bash

###############################################################################
#
# This script will install configuration into a root filesystem.  Run this
# script in the root of the filesystem you wish to populate.
#
# Notes:
# 1) HOBART_APP must be defined
# 2) this script will install binaries if they are built
#
###############################################################################

export RFS=.
export HOME_DIR=$RFS/home

#
# create directory structure
#

mkdir $RFS/var
mkdir $RFS/var/www

mkdir $RFS/lib
mkdir $RFS/lib/firmware
mkdir $RFS/lib/firmware/hobart

mkdir $RFS/etc
mkdir $RFS/etc/hobart

mkdir $RFS/usr
mkdir $RFS/usr/share
mkdir $RFS/usr/share/themes
mkdir $RFS/usr/share/themes/Clearlooks
mkdir $RFS/usr/share/themes/Clearlooks/gtk-2.0

mkdir $RFS/usr/local
mkdir $RFS/usr/local/hobart
mkdir $RFS/usr/local/hobart/system
mkdir $RFS/usr/local/hobart/user
mkdir $RFS/usr/local/hobart/user/fonts
mkdir $RFS/usr/local/hobart/user/graphics

mkdir $RFS/usr/bin

mkdir $HOME_DIR
mkdir $HOME_DIR/Hobart
mkdir $HOME_DIR/Hobart/AccessScale
mkdir $HOME_DIR/Hobart/AccessScale/System
mkdir $HOME_DIR/Hobart/AccessScale/User
mkdir $HOME_DIR/Hobart/AccessScale/User/Fonts
mkdir $HOME_DIR/Hobart/AccessScale/User/Graphics

#
# populate directories
#

# web pages
cp -r $HOBART_APP/webPages/* $RFS/var/www

# firmware
cp $HOBART_APP/hlx_firmware_bins/* $RFS/lib/firmware/hobart
cp $HOBART_APP/ews_firmware_bins/* $RFS/lib/firmware/hobart
cp $HOBART_APP/aws_firmware_bins/* $RFS/lib/firmware/hobart

# database and ini files
cp -r $HOBART_APP/../Config_files/etc/hobart/* $RFS/etc/hobart

# access scale system data
cp -r $HOBART_APP/../Config_files/usr/local/hobart/system/* $RFS/usr/local/hobart/system
cp -r $HOBART_APP/../Config_files/usr/local/hobart/system/* $HOME_DIR/Hobart/AccessScale/System

# themes
cp $HOBART_APP/../Config_files/usr/share_themes/Clearlooks/* $RFS/usr/share/themes/Clearlooks/gtk-2.0

# scripts
cp $HOBART_APP/scripts/win_cmd.sh $RFS/usr/bin
cp $HOBART_APP/scripts/www_cmd.sh $RFS/usr/bin
cp $HOBART_APP/scripts/xvkbd.sh $RFS/usr/bin

chmod +x $RFS/usr/bin/win_cmd.sh
chmod +x $RFS/usr/bin/www_cmd.sh
chmod +x $RFS/usr/bin/xvkbd.sh

# binaries
cp $HOBART_APP/../3RDPartySoftware/goAheadWebServer/LINUX/webs $RFS/usr/bin
cp $HOBART_APP/source/app/chobart_d $RFS/usr/bin
cp $HOBART_APP/source/db/cbsql/cbsql $RFS/usr/bin

