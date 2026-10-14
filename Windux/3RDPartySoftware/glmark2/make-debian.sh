#!/bin/bash

PACKAGE=glmark2-master
VERSION=2017.07-0

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
#git clone https://github.com/glmark2/glmark2.git 
unzip ${PACKAGE}.zip

if [ ! -d ${PACKAGE} ];
then
	echo "failed to extract ${PACKAGE}!"
	exit -1
fi

# configure and build
cd ${PACKAGE}

CC=armv7l-timesys-linux-gnueabihf-gcc \
CXX=armv7l-timesys-linux-gnueabihf-g++ \
	./waf configure --with-flavors=x11-gl,x11-glesv2,drm-gl,drm-glesv2 --prefix=/usr

./waf build -j16

# install
./waf install --destdir=${PWD}/../debian

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

mv debian.deb hobart-glmark2_${VERSION}_hti_armhf.deb


