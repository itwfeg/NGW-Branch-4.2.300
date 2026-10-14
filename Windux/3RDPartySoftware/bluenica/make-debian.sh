#!/bin/bash

BIN_DAEMON=bluez
BIN_UI=bluezviewer

# build daemon
rm -f Makefile
qmake -spec linux-timesys-g++ CONFIG+=release -o Makefile bluez.pro
make clean
make

if [ -f $BIN_DAEMON ];
then
	echo "$BIN_DAEMON build success"
else
	echo "target $BIN_DAEMON does not exist!"
	exit -1
fi

# build ui
rm -f Makefile
qmake -spec linux-timesys-g++ CONFIG+=release -o Makefile bluezviewer.pro
make clean
make

if [ -f $BIN_UI ];
then
	echo "$BIN_UI build success"
else
	echo "target $BIN_UI does not exist!"
	exit
fi

# assemble app components
rm -rf debian

mkdir -p ./debian/usr/local/hobart/apps/bluez
mkdir -p ./debian/usr/local/hobart/apps/bluez/res

cp ./res/* ./debian/usr/local/hobart/apps/bluez/res

cp $BIN_DAEMON ./debian/usr/local/hobart/apps/bluez
cp $BIN_UI ./debian/usr/local/hobart/apps/bluez

cp ./conf/hbtemp.conf ./debian/usr/local/hobart/apps
cp ./conf/hbtemp.png ./debian/usr/local/hobart/apps

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

mv debian.deb hobart-hbtemp-app_1.0-0_hti_armhf.deb
