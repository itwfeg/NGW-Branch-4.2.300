#!/bin/bash

BIN_DAEMON=hfpdd
BIN_UI=hfpdui

# assemble app components
rm -rf debian

mkdir -p ./debian/usr/local/hobart/apps/hfpd
mkdir -p ./debian/usr/local/hobart/apps/hfpd/res

cp ./ui/res/* ./debian/usr/local/hobart/apps/hfpd/res

cp ./daemon/$BIN_DAEMON ./debian/usr/local/hobart/apps/hfpd
cp ./ui/$BIN_UI ./debian/usr/local/hobart/apps/hfpd

cp ./conf/hfpd.conf ./debian/usr/local/hobart/apps
cp ./conf/hfpd.png ./debian/usr/local/hobart/apps

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

mv debian.deb hobart-hfpd-app_1.0-0_hti_armhf.deb
