#!/bin/bash

# Note:
# When changing version also change ./deb/control
APP_NAME=wmsmartapp
APP_VERSION=1.0-0

#BIN_DAEMON=wmsmartd
BIN_UI=wmsmart

# -----------------------------------------------------------------------------

# build ui
rm -f Makefile
qmake -spec linux-timesys-g++ CONFIG+=release -o Makefile wmsmart.pro
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

mkdir -p ./debian/usr/local/hobart/apps/$APP_NAME
mkdir -p ./debian/usr/local/hobart/apps/$APP_NAME/res

cp ./res/* ./debian/usr/local/hobart/apps/$APP_NAME/res

cp $BIN_UI ./debian/usr/local/hobart/apps/$APP_NAME
cp $BIN_UI.config ./debian/usr/local/hobart/apps/$APP_NAME

cp logowhite.png ./debian/usr/local/hobart/apps/$APP_NAME

cp ./conf/$APP_NAME.conf ./debian/usr/local/hobart/apps
cp ./conf/$APP_NAME.png ./debian/usr/local/hobart/apps

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

mv debian.deb hobart-${APP_NAME}-app_${APP_VERSION}_hti_armhf.deb
