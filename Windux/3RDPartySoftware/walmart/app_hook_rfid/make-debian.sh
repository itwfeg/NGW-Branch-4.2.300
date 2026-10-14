#!/bin/bash

PACKAGE=walmart-apphook-rfid
VERSION=1.0-0

SO_VERSION=1.0

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

BIN_SHARED_LIB=libapp_hook_rfid.so.1.0.0

# build hook
rm -f Makefile
qmake -spec linux-timesys-g++ CONFIG+=release -o Makefile app_hook_rfid.pro
make clean
make

if [ -f $BIN_SHARED_LIB ];
then
	echo "$BIN_SHARED_LIB build success"
else
	echo "target $BIN_SHARED_LIB does not exist!"
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
mkdir -p ./debian/usr/lib
mkdir -p ./debian/usr/share/$PACKAGE
mkdir -p ./debian/var/log

cp $BIN_SHARED_LIB ./debian/usr/lib/libHobartCustomerHookWalmartRFID.so.$SO_VERSION

ln -sf ./libHobartCustomerHookWalmartRFID.so.$SO_VERSION ./debian/usr/lib/libHobartCustomerHookWalmartRFID.so.1

cp ./hobart_internal.config ./debian/usr/share/$PACKAGE
cp ./logging.config ./debian/usr/share/$PACKAGE

# make package!
find ./debian -type d | xargs chmod 755
find ./debian -type d | xargs chmod g-s

dpkg-deb -Zgzip --build debian

mv debian.deb hobart-${PACKAGE}_${VERSION}_hti_armhf.deb


