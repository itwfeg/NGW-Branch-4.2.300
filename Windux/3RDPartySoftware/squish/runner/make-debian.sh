#!/bin/bash
#
# Note:
# For Ubuntu 20.04 the package qtbase5-private-dev is required to build squishidl
#
# This package uses a custom version of libalgorithms... you need to move out the toolchain one
# then restore it afterward:
# /opt/hb6mxd_combo/toolchain/lib$ sudo mv libalgorithms.so libalgorithms.so.old
# <do your build>
# /opt/hb6mxd_combo/toolchain/lib$ sudo mv libalgorithms.so.old libalgorithms.so
#

SQUISHVER=6.6.0

SQUISH=squish-${SQUISHVER}-qt-src
SQUISHIDL=${HOME}/squish/bin/squishidl

QMAKE=/opt/Qt5.4.1/5.4/gcc_64/bin/qmake

# copy squish license to home folder for compilation
if [ -f ${HOME}/.squish-3-license ];
then
	echo "squish is licensed"
else
	cp ./conf/license ${HOME}/.squish-3-license
fi

# verify qmake exists
if [ -f ${QMAKE} ];
then
	echo "found qmake"
else
	QMAKE=$(which qmake)
fi

#
# Squish requires it's own tool (squishidl) to build squish. Here we check if present on the system
# and build if needed.
#

# check squishidl on system
if [ -f ${SQUISHIDL} ];
then
	echo "squishidl present... yay!"
else
	echo "squishidl is missing... need to build..."

	# extract source code
	rm -rf ${SQUISH}
	tar -xf ${SQUISH}.tar.gz
	cd ${SQUISH}

	# configure and build
	./configure --with-qmake=${QMAKE} --disable-all --enable-idl --enable-pure-qt4
	./build -j16

	# install
	./build install DESTDIR=${HOME}/squish

	cd ..
fi

#
# Now we are ready to build server, runner, and wrappers for ARM target.
#

echo "building squish..."
rm -rf debian

# setup environment
export ARCH=arm
export CROSS_COMPILE=armv7l-timesys-linux-gnueabihf-
export PATH=/opt/hb6mxd_combo/toolchain/ccache:/opt/hb6mxd_combo/toolchain/bin:${PATH}

# extract and patch source code
rm -rf ${SQUISH}
tar -xf ${SQUISH}.tar.gz

cd ${SQUISH}

patch -p1 < ../hobart-squish-6.6.0.patch

# configure and build
#
# we want the following to build:
#   - squishserver
#   - squishrunner
#     - python plugin (this allows record and playback using python scripts)
#   - wrappers
#     - qt wrapper (this allows squish to attach to scaleui and gcd)
#
LD=armv7l-timesys-linux-gnueabihf-ld ./configure CXX=armv7l-timesys-linux-gnueabihf-g++ \
	--with-ar=armv7l-timesys-linux-gnueabihf-ar \
	--with-ranlib=armv7l-timesys-linux-gnueabihf-ranlib \
	--disable-all \
	--enable-server \
	--enable-runner \
	--enable-wrappers \
	--disable-js \
	--enable-pure-qt4 \
	--enable-python \
	--disable-python3 \
	--with-python-lib=/opt/hb6mxd_combo/toolchain/usr/lib/libpython2.7.so \
	--with-python-includedir=/opt/hb6mxd_combo/toolchain/usr/include/python2.7 \
	--enable-x11 \
	--with-x11-libdir=/opt/hb6mxd_combo/toolchain/usr/lib \
	--with-x11-includedir=/opt/hb6mxd_combo/toolchain/usr/include \
	--with-squishidl=${SQUISHIDL}

./build

if ! [ -f bin/squishrunner ];
then
	echo "squishrunner failed to build!"
	exit
fi

if ! [ -f bin/squishserver ];
then
	echo "squishserver failed to build!"
	exit
fi

if ! [ -f bin/startaut ];
then
	echo "startaut failed to build!"
	exit
fi

# install
./build install DESTDIR=${PWD}/../debian/usr

cd ..

if [ -f debian/usr/bin/squishrunner ] && [ -f debian/usr/bin/squishserver ] && [ -f debian/usr/bin/startaut ];
then
	echo "squish build success!"
else
	echo "squish failed to build!"
	exit
fi

#
# Okay lets create a debian package.
#

# build debian package
rm -f *.deb

mkdir -p ./debian/etc

cp ./conf/squishserverrc ./debian/etc

mkdir -p ./debian/root/.squish/ver1

cp ./conf/license ./debian/root/.squish-3-license
cp ./conf/server.ini ./debian/root/.squish/ver1

mkdir -p ./debian/DEBIAN

find ./debian -type d | xargs chmod 755
find ./debian -type d | xargs chmod g-s

cp ./deb/control ./debian/DEBIAN
cp ./deb/preinst ./debian/DEBIAN
cp ./deb/postinst ./debian/DEBIAN

# make package!
dpkg-deb --build -Zgzip debian

mv debian.deb hobart-squish_6.6-0_hti_armhf.deb


