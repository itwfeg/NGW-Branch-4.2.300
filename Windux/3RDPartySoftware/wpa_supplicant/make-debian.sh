#!/bin/bash

#PACKAGE=hostap
PACKAGE=wpa_supplicant-2.6
VERSION=2.6-1

# remove output folder
if [ -d debian ];
then
	rm -rf debian
fi

###############################################################################
#
# compile wpa_supplicant
#
###############################################################################

# remove existing folder
if [ -d ${PACKAGE} ];
then
	rm -rf ${PACKAGE}
fi

# perform checkout
#git clone git://w1.fi/srv/git/hostap.git
tar -zxvf ${PACKAGE}.tar.gz

if [ ! -d ${PACKAGE} ];
then
	echo "failed to extract ${PACKAGE}!"
	exit -1
fi

# configure and build
cd ${PACKAGE}

# Fixes 'krackattacks' protocol weakness (CVE-2017-13077 to 13088, 13084 is N/A)
patch -p1 < ../wpa_supplicant-2.6-upstream_fixes-1.patch

# build
cp ../wpa_supplicant_config wpa_supplicant/.config

cd wpa_supplicant
CC=armv7l-timesys-linux-gnueabihf-gcc CXX=armv7l-timesys-linux-gnueabihf-g++ make -j16

cd ../..

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

# install
mkdir -p ./debian/usr/sbin
install -v -m755 ${PACKAGE}/wpa_supplicant/wpa_{cli,passphrase,supplicant} ./debian/usr/sbin/

# make package!
find ./debian -type d | xargs chmod 755
find ./debian -type d | xargs chmod g-s

fakeroot dpkg-deb --build debian

mv debian.deb hobart-wpasupplicant_${VERSION}_hti_armhf.deb


