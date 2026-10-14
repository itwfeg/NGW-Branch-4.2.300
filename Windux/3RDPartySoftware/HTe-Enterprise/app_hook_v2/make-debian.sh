#!/bin/bash

PACKAGE=hte-apphook
VERSION=2.0-0

SO_VERSION=2.0

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

BIN_SHARED_LIB=libapp_hook.so.1.0.0

# build hook
rm -f Makefile
qmake -spec linux-timesys-g++ CONFIG+=release -o Makefile app_hook.pro
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
mkdir -p ./debian/usr/local/hobart/python
mkdir -p ./debian/usr/local/hobart/db/hte
mkdir -p ./debian/usr/local/share/ca-certificates
mkdir -p ./debian/usr/share/$PACKAGE
mkdir -p ./debian/var/log

cp $BIN_SHARED_LIB ./debian/usr/lib/libHobartCustomerHookHTe.so.$SO_VERSION

ln -sf ./libHobartCustomerHookHTe.so.$SO_VERSION ./debian/usr/lib/libHobartCustomerHookHTe.so.2

cp ./hteHealthCheck.py ./debian/usr/local/hobart/python

cp ./healthCheckAddCronJob.py ./debian/usr/share/$PACKAGE
cp ./hobart_internal.config ./debian/usr/share/$PACKAGE
cp ./hte.config ./debian/usr/share/$PACKAGE
cp ./logging.config ./debian/usr/share/$PACKAGE
cp ./web_headers.config ./debian/usr/share/$PACKAGE

cp ./hteapphook_01.db ./debian/usr/local/hobart/db/hte

# make package!
find ./debian -type d | xargs chmod 755
find ./debian -type d | xargs chmod g-s

dpkg-deb -Zgzip --build debian

mv debian.deb hobart-${PACKAGE}_${VERSION}_hti_armhf.deb


