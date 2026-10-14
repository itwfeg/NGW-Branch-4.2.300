#!/bin/bash

# remove output folder
if [ -d debian ];
then
	rm -rf debian
fi

###############################################################################
#
# compile xrdp
#
###############################################################################

# remove existing folder
if [ -d xrdp ];
then
	rm -rf xrdp
fi

# perform checkout
#git clone https://github.com/neutrinolabs/xrdp
tar -zxvf xrdp-0.9.0.tar.gz

if [ ! -d xrdp ];
then
	echo "failed to clone xrdp!"
	exit -1
fi

# patch source
patch -p0 < xrdp-libtool.patch
patch -p0 < xrdp-hobart-patch.patch

# configure and build
cd xrdp

./bootstrap
./configure --host=armv7l-timesys-linux-gnueabihf --enable-jpeg
make -j16

# install
make install DESTDIR=${PWD}/../debian

cd ..

# copy in config
cp xrdp-init debian/etc/init.d/xrdp
cp xrdp-sesman debian/etc/pam.d
cp sesman.ini debian/etc/xrdp
cp xrdp.ini debian/etc/xrdp

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

mv debian.deb hobart-xrdp_0.9-0_hti_armhf.deb


