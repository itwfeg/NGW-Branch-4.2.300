#!/bin/bash

# There is a nice tutorial for DEBs at:
# http://tldp.org/HOWTO/html_single/Debian-Binary-Package-Building-HOWTO/

#
# Follow the README to build SIP and PyQt5 before running this script.
#

#
# build data.tar.gz
#

while ! [ -d ./root ] ; do

    read -p "Binary is missing from image, please copy and press ENTER to continue... "

done

cp -r ./root ./debian

#
# build control.tar.gz
#

mkdir -p ./debian/DEBIAN

find ./debian -type d | xargs chmod 755

cp ./control ./debian/DEBIAN

#
# make package!
#

# tell it to compress with gzip (best for compatibility)
dpkg-deb -Zgzip --build debian

mv debian.deb hobart-hti-pyqt_5.5-1_armhf.deb
