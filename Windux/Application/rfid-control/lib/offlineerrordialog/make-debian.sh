#!/bin/bash

PACKAGE=walmart-offline-error-dialog
VERSION=1.0-0

# remove output folder
if [ -d debian ];
then
	rm -rf debian
fi

###############################################################################
#
# compile package
#
###############################################################################

BIN_EXE=offlineErrorDialog

# build binary
rm -f Makefile
qmake -spec linux-timesys-g++ CONFIG+=release -o Makefile offlineErrorDialog.pro
make clean
make

if [ -f $BIN_EXE ];
then
	echo "$BIN_EXE build success"
else
	echo "target $BIN_EXE does not exist!"
	exit
fi

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
mkdir -p ./debian/usr/bin

cp $BIN_EXE ./debian/usr/bin

# make package!
find ./debian -type d | xargs chmod 755
find ./debian -type d | xargs chmod g-s

dpkg-deb -Zgzip --build debian

mv debian.deb hobart-${PACKAGE}_${VERSION}_hti_armhf.deb


