#!/bin/bash
#
# Prerequisites:
# sudo apt-get install devscripts debhelper
#


PACKAGE=libclio-master
VERSION=1.0.0

###############################################################################
#
# compile libclio
#
###############################################################################

# remove existing folder
if [ -d ${PACKAGE} ];
then
	rm -rf ${PACKAGE}
fi

# perform checkout
unzip ${PACKAGE}.zip

if [ ! -d ${PACKAGE} ];
then
	echo "failed to extract ${PACKAGE}!"
	exit -1
fi

# fixup rules to allow cross compile with custom toolchain
cp rules ${PACKAGE}/debian

# build
cd ${PACKAGE}

dpkg-buildpackage -a armhf -t armv7l-timesys-linux-gnueabihf -us -uc -b

cd ..


