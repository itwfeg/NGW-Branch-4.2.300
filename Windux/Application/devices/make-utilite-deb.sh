#!/bin/bash

# There is a nice tutorial for DEBs at:
# http://tldp.org/HOWTO/html_single/Debian-Binary-Package-Building-HOWTO/

#
# build data.tar.gz
#

mkdir -p ./debian/usr/local/hobart/images
mkdir -p ./debian/usr/local/hobart/playlists
mkdir -p ./debian/usr/local/hobart/nowServing
mkdir -p ./debian/usr/local/hobart/nowServing/res

cp ./res/* ./debian/usr/local/hobart/nowServing/res

cp ./utilite/icon.png ./debian/usr/local/hobart/nowServing
cp ./nowServing.config ./debian/usr/local/hobart/nowServing

while ! [ -f ./debian/usr/local/hobart/nowServing/nowServing ] ; do

    read -p "Binary is missing from image, please copy and press ENTER to continue... "

done

mkdir -p ./debian/home/utilite/.config/autostart

cp ./utilite/nowServing.desktop ./debian/home/utilite/.config/autostart

mkdir -p ./debian/etc/lightdm

cp ./utilite/lightdm.conf ./debian/etc/lightdm

#
# build control.tar.gz
#

mkdir -p ./debian/DEBIAN

find ./debian -type d | xargs chmod 755

cp ./utilite/control ./debian/DEBIAN

#
# make package!
#

dpkg-deb --build debian

mv debian.deb hobart-nowserving_1.0-1_utilite_armel.deb
