#!/bin/bash

VERSION=16.1.21-003

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

# make package!
find ./debian -type d | xargs chmod 755
find ./debian -type d | xargs chmod g-s

# extract devicewise
mkdir -p ./debian/opt

tar xvf ./devicewise/dw.tar.gz -C ./debian/opt

# configure telit startup
mkdir -p ./debian/etc/hobart

touch ./debian/etc/hobart/telit

# for Ubuntu 18.04 you need to add "-Zgzip" to the below command line
fakeroot dpkg-deb -Zgzip --build debian

mv debian.deb hobart-devicewise_${VERSION}_hti_armhf.deb


