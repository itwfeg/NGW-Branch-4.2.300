#!/bin/bash

. /home/hobart/buildCombo.sh

PACKAGE=memtester-4.3.0
VERSION=4.3.0

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
tar -zxvf ${PACKAGE}.tar.gz

if [ ! -d ${PACKAGE} ];
then
	echo "failed to extract ${PACKAGE}!"
	exit -1
fi

# configure and build
cd ${PACKAGE}

sed -i 's/cc/armv7l-timesys-linux-gnueabihf-gcc/' conf-cc
sed -i 's/cc/armv7l-timesys-linux-gnueabihf-gcc/' conf-ld

sed -i 's=/usr/local=../debian/usr=' Makefile

# install
make -j16 all
make install

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

mv debian.deb hobart-memtester_${VERSION}_hti_armhf.deb


