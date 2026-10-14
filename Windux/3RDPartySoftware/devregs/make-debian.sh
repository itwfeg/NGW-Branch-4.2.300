#!/bin/bash

PACKAGE=devregs-master
VERSION=1.0-0

# remove output folder
if [ -d debian ];
then
	rm -rf debian
fi

###############################################################################
#
# compile valgrind
#
###############################################################################

# remove existing folder
if [ -d ${PACKAGE} ];
then
	rm -rf ${PACKAGE}
fi

# perform checkout
#git clone https://github.com/boundarydevices/devregs
unzip ${PACKAGE}.zip

if [ ! -d ${PACKAGE} ];
then
	echo "failed to extract ${PACKAGE}!"
	exit -1
fi

# configure and build
cd ${PACKAGE}

autoreconf -i -f

./configure --host=armv7l-timesys-linux-gnueabihf --exec-prefix=${PWD}/../debian/usr --sysconfdir=${PWD}/../debian/etc

# install
make -j16 install

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

# make package!
find ./debian -type d | xargs chmod 755
find ./debian -type d | xargs chmod g-s

# for Ubuntu 18.04 you need to add "-Zgzip" to the below command line
fakeroot dpkg-deb --build debian

mv debian.deb hobart-devregs_${VERSION}_hti_armhf.deb


