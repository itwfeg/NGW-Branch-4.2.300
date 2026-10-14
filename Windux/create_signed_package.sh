#!/bin/bash

#--------------------------------------------------------------------
# Stand-alone script to create a signed upgrade file.
# (Adapted from the create_package() function in installToTarget.sh)
#
# Parameter:
#
#     file_name_and_build_number
#
#     For example:  hti-linux-uboot-1.12.0-12082014_081512
#  
#--------------------------------------------------------------------


workingdirectory=`pwd`

cd ..

# this will checksum, sign, and bundle everything into a package

tgz=${1}.tgz
md5=${1}.md5

# generate MD5 checksum
cd ${workingdirectory}/..
python3 ${workingdirectory}/Application/shared/sessionHandler/python/md5checksum.py $tgz > $md5

# sign feature installer
cd ${workingdirectory}/Application/certs
./signFeatureInstaller.sh ${workingdirectory}/../$tgz

# create bundle
cd ${workingdirectory}/..
tar --create --verbose --gzip --owner=0 --group=0 --file ${1}.package.tgz ${md5} ${tgz} ${tgz}.crt ${tgz}.signed

# remove files
rm ${md5} ${tgz} ${tgz}.crt ${tgz}.signed

