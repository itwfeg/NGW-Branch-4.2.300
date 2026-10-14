#!/bin/bash

# Looks like an IPK is just a DEB in disguise (except using gzip instead of tar).
# http://inportb.com/2010/10/19/making-an-opkg-package/
# http://tldp.org/HOWTO/html_single/Debian-Binary-Package-Building-HOWTO/

#
# build data.tar.gz
#

mkdir -p ./debian/usr/local/hobart
mkdir -p ./debian/usr/local/hobart/python

cp ../../../../ScaleManagement/smBackend/microSmBackend.py ./debian/usr/local/hobart/python
cp ../../shared/sessionHandler/python/file_io.py ./debian/usr/local/hobart/python
cp ../../shared/sessionHandler/python/getHostname.py ./debian/usr/local/hobart/python
cp ../../shared/sessionHandler/python/getMacAddress.py ./debian/usr/local/hobart/python
cp ../../shared/sessionHandler/python/osUtils.py ./debian/usr/local/hobart/python
cp ../../shared/sessionHandler/python/shellCommand.py ./debian/usr/local/hobart/python

mkdir -p ./debian/etc/init.d

cp ./asura/smBackend ./debian/etc/init.d

mkdir -p ./debian/extension/apps/hobart
mkdir -p ./debian/extension/apps/hobart/res

cp ./res/* ./debian/extension/apps/hobart/res

cp ./asura/icon.png ./debian/extension/apps/hobart
cp ./asura/applications.conf ./debian/extension/apps/hobart
cp ./nowServing.config ./debian/extension/apps/hobart

while ! [ -f ./debian/extension/apps/hobart/nowServing ] ; do

    read -p "Binary is missing from image, please copy and press ENTER to continue... "

done

#
# build control.tar.gz
#

mkdir -p ./debian/DEBIAN

find ./debian -type d | xargs chmod 755

cp ./asura/control ./debian/DEBIAN
cp ./asura/postinst ./debian/DEBIAN

#
# make package!
#

dpkg-deb -Zgzip --build debian

mv debian.deb hobart-nowserving_1.0-1_asura_arm.ipk
