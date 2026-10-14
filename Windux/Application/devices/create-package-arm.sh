#!/bin/bash

# Looks like an IPK is just a DEB in disguise (except using gzip instead of tar).
# http://inportb.com/2010/10/19/making-an-opkg-package/
# http://tldp.org/HOWTO/html_single/Debian-Binary-Package-Building-HOWTO/

# $1 = device name
# $2 = package name
# $3 = package type (ipk, deb, or tgz)

if ! [ -d ${1} ]; then
    echo "Device does not exist!"
    echo
    echo "USAGE: create-package-arm.sh DEVICE PACKAGENAME PACKAGETYPE"
    exit
fi

if ! [ -d ${1}/${2} ]; then
    echo "Package name does not exist!"
    echo
    echo "USAGE: create-package-arm.sh DEVICE PACKAGENAME PACKAGETYPE"
    exit
fi

case ${3} in
    ipk )
        echo "Creating IPK package..."
        ;;
    deb )
        echo "Creating DEB package..."
        ;;
    tgz )
        echo "Creating TGZ package..."
        ;;
    * )
        echo "Unsupported package type!"
        echo
        echo "USAGE: create-package-arm.sh DEVICE PACKAGENAME PACKAGETYPE"
        exit

esac

#
# create packages
#

rm -rf debian

if [ -d common/${2} ]; then
    cd common/${2}
    python createPackage.py
    cd ../..

    cp -rp common/${2}/debian .
fi

cd ${1}/${2}
python createPackage.py
cd ../..

cp -rp ${1}/${2}/debian .

#
# build package
#

VERSION=`grep 'Version:' debian/DEBIAN/control | cut -c 10-`

find ./debian -type d | xargs chmod 755

case ${3} in
    ipk )
        fakeroot dpkg-deb -Zgzip --build debian
        ;;
    deb )
        fakeroot dpkg-deb --build debian
        ;;
    tgz )
        cd debian
        tar --exclude=DEBIAN -cvzf ../debian.deb .
        cd ..
        ;;
    * )
        ;;
esac

mv debian.deb hobart-${2}_${VERSION}_${1}_arm.${3}
