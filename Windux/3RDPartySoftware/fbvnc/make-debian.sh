#!/bin/bash

# remove output folder
if [ -d debian ];
then
	rm -rf debian
fi

###############################################################################
#
# compile libvncserver
#
###############################################################################

# remove existing folder
if [ -d libvncserver ];
then
	rm -rf libvncserver
fi

# perform checkout
#git clone https://github.com/LibVNC/libvncserver
tar -zxvf libvncserver-1.0.0.tar.gz

if [ ! -d libvncserver ];
then
	echo "failed to clone libvncserver!"
	exit -1
fi

# configure and build
cd libvncserver

./autogen.sh
./configure --host=armv7l-timesys-linux-gnueabihf --prefix=${PWD}/../debian/usr --with-sdl-config=no
make -j16

# install
make install

cd ..

###############################################################################
#
# compile libvncserver
#
###############################################################################

# remove existing folder
if [ -d framebuffer-vncserver ];
then
	rm -rf framebuffer-vncserver
fi

# perform checkout
#git clone https://github.com/ponty/framebuffer-vncserver
#tar czf framebuffer-vncserver-1.0.X.tar.gz framebuffer-vncserver
tar -zxvf framebuffer-vncserver-1.0.1.tar.gz

if [ ! -d framebuffer-vncserver ];
then
	echo "failed to clone framebuffer-vncserver!"
	exit -1
fi

# patch source
patch -p0 < fix-libvncserver-include.patch
patch -p0 < fix-libvncserver-bpp.patch

# configure and build
cd framebuffer-vncserver

mkdir build
cd build

qmake ../framebuffer-vncserver.pro
make

if [ ! -f framebuffer-vncserver ];
then
	echo "failed to build framebuffer-vncserver!"
	exit -1
fi

cp framebuffer-vncserver ../../debian/usr/bin

cd ../..

###############################################################################
#
# create a debian package.
#
###############################################################################

# build debian package
rm -f *.deb

mkdir -p ./debian/DEBIAN

find ./debian -type d | xargs chmod 755
find ./debian -type d | xargs chmod g-s

cp ./deb/control ./debian/DEBIAN
cp ./deb/preinst ./debian/DEBIAN
cp ./deb/postinst ./debian/DEBIAN

# make package!
dpkg-deb --build debian

mv debian.deb hobart-fbvnc_1.0-1_hti_armhf.deb


