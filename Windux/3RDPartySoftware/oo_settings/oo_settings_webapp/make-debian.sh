#!/bin/bash

# Note:
# When changing version also change ./deb/control
APP_NAME=settings
APP_VERSION=1.3-1

#BIN_DAEMON=webappd
BIN_UI=webapp

# -----------------------------------------------------------------------------

## build daemon
#rm -f Makefile
#qmake -spec linux-timesys-g++ CONFIG+=release -o Makefile webappd.pro
#make clean
#make
#
#if [ -f $BIN_DAEMON ];
#then
#	echo "$BIN_DAEMON build success"
#else
#	echo "target $BIN_DAEMON does not exist!"
#	exit -1
#fi

# build ui
rm -f Makefile
qmake -spec linux-timesys-g++ CONFIG+=release -o Makefile webapp.pro 
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

#cp $BIN_DAEMON ./debian/usr/local/hobart/apps/$APP_NAME
#cp $BIN_DAEMON.config ./debian/usr/local/hobart/apps/$APP_NAME

cp $BIN_UI ./debian/usr/local/hobart/apps/$APP_NAME
cp $BIN_UI.config ./debian/usr/local/hobart/apps/$APP_NAME

cp settings.png ./debian/usr/local/hobart/apps/$APP_NAME

#cp ../proxySettings ./debian/usr/local/hobart/apps/$APP_NAME

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
cp ./deb/prerm ./debian/DEBIAN

# make package!
dpkg-deb -Zgzip --build debian

mv debian.deb hobart-${APP_NAME}-app_${APP_VERSION}_hti_armhf.deb
