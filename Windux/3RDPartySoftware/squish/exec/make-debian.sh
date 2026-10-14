#!/bin/bash

# setup environment
export ARCH=arm
export CROSS_COMPILE=armv7l-timesys-linux-gnueabihf-
export PATH=/opt/timesys-hf/nitrogen6x/toolchain/ccache:/opt/timesys-hf/nitrogen6x/toolchain/bin:${PATH}

#
# Build exec for ARM target.
#

echo "building exec..."

rm -rf squishexecutive-build
mkdir squishexecutive-build

cd squishexecutive-build

# run qmake (DEBUG)
#qmake DEFINES+=SQUISHEXECUTIVE_DEBUG ${PWD}/../squishexecutive-src/squishexecutive.pro

# run qmake (RELEASE)
qmake ${PWD}/../squishexecutive-src/squishexecutive.pro

# build!
make

cd ..

if [ -f ./squishexecutive-build/squishexecutive ];
then
	echo "squishexecutive build success!"
else
	echo "squishexecutive failed to build!"
	exit
fi

#
# Okay lets create a debian package.
#

# assemble package
rm -rf debian

mkdir -p ./debian/usr/bin

cp ./squishexecutive-build/squishexecutive ./debian/usr/bin

mkdir -p ./debian/root/.squish

cp ./conf/squishexecutive.conf ./debian/root/.squish

mkdir -p ./debian/DEBIAN

find ./debian -type d | xargs chmod 755
find ./debian -type d | xargs chmod g-s

cp ./deb/control ./debian/DEBIAN
cp ./deb/preinst ./debian/DEBIAN
cp ./deb/postinst ./debian/DEBIAN

# build debian package
rm -f *.deb

dpkg-deb --build debian

mv debian.deb hobart-squishexec_1.0-0_hti_armhf.deb


