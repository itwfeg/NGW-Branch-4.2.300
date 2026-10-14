#!/bin/bash

PACKAGE=mosquitto-1.4.14
VERSION=1.4-14

# remove output folder
if [ -d debian ];
then
	rm -rf debian
fi

###############################################################################
#
# compile mosquitto
#
###############################################################################

# remove existing folder
if [ -d ${PACKAGE} ];
then
	rm -rf ${PACKAGE}
fi

# perform checkout
tar -zxvf ${PACKAGE}.tar.gz

if [ ! -d ${PACKAGE} ];
then
	echo "failed to extract ${PACKAGE}!"
	exit -1
fi

# configure and build
cd ${PACKAGE}

CROSS_COMPILE=armv7l-timesys-linux-gnueabihf- CC=gcc CXX=g++ make WITH_SRV=no

# install
make DESTDIR=${PWD}/../debian prefix=/usr install

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

cp mosquitto ./debian/etc/init.d
chmod +x ./debian/etc/init.d/mosquitto

# make package!
find ./debian -type d | xargs chmod 755
find ./debian -type d | xargs chmod g-s

dpkg-deb --build debian

mv debian.deb hobart-mosquitto_${VERSION}_hti_armhf.deb


