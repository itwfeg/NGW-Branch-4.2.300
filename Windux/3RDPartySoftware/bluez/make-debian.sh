#!/bin/bash

PACKAGE=bluez-5.19
VERSION=5.19

# remove output folder
if [ -d debian ];
then
	rm -rf debian
fi

###############################################################################
#
# compile bluez
#
###############################################################################

# remove existing folder
if [ -d ${PACKAGE} ];
then
	rm -rf ${PACKAGE}
fi

# perform checkout
tar -Jxvf ${PACKAGE}.tar.xz

if [ ! -d ${PACKAGE} ];
then
	echo "failed to extract ${PACKAGE}!"
	exit -1
fi

# configure and build
cd ${PACKAGE}

patch -p1 < ../0001-hciattach-add-QCA9377-Tuffello-support.patch
patch -p1 < ../0002-add-hciattach-rome.patch

# configure and build
PREFIX=${PWD}/../debian

./configure --host=armv7l-timesys-linux-gnueabihf --prefix=/usr --enable-tools --enable-test --enable-datafiles --with-systemdsystemunitdir=/lib/systemd/system --with-systemduserunitdir=/usr/lib/systemd/user --enable-library --enable-debug --with-dbussystembusdir=/usr/share/dbus-1/system-services --with-dbussessionbusdir=/usr/share/dbus-1/services
make -j16

# install
make install DESTDIR=${PWD}/../debian
#make install

cd ..

###############################################################################
#
# create a debian package.
#
###############################################################################

# build debian package
rm -f *.deb

mkdir -p ./debian/DEBIAN

sed "s/VERSION/${VERSION}/g" ./deb/control > ./debian/DEBIAN/control
cp ./deb/preinst ./debian/DEBIAN
cp ./deb/postinst ./debian/DEBIAN

# copy local files into package
mkdir -p ./debian/etc/init.d

cp init ./debian/etc/init.d/bluetooth
chmod +x ./debian/etc/init.d/bluetooth

# make package!
find ./debian -type d | xargs chmod 755
find ./debian -type d | xargs chmod g-s

# for Ubuntu 18.04 you need to add "-Zgzip" to the below command line
fakeroot dpkg-deb --build debian

mv debian.deb hobart-bluez_${VERSION}_hti_armhf.deb


