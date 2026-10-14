#!/bin/bash

#
# This script will generate an upgrade patch from two existing hti-linux-core files.
#
# Usage:
# makeUpgrade hti-linux-core-OLD_VERSION.tgz hti-linux-core-NEW_VERSION.tgz
#
# Prerequisites:
# mountavfs -- sudo apt-get install avfs
# rsync -- sudo apt-get install rsync
#

PREV_CORE_FILE=${1}
NEW_CORE_FILE=${2}

CWD=`pwd`

if [ "${PREV_CORE_FILE}" = "" ];
then
    echo "You need to specify the old image filename."
    echo "USAGE: makeUpgrade hti-linux-core-OLD_VERSION.tgz hti-linux-core-NEW_VERSION.tgz"
    exit 1
fi

if [ "${NEW_CORE_FILE}" = "" ];
then
    echo "You need to specify the new image filename."
    echo "USAGE: makeUpgrade hti-linux-core-OLD_VERSION.tgz hti-linux-core-NEW_VERSION.tgz"
    exit 1
fi

UPGRADE_FILE="${NEW_CORE_FILE/hti-linux-core-/hti-linux-upgrade-}"
UPGRADE_LIST_FILE="${UPGRADE_FILE/.tgz/.list}"
UPGRADE_MD5_FILE="${UPGRADE_FILE/.tgz/.md5}"
UPGRADE_CRT_FILE=${UPGRADE_FILE}.crt
UPGRADE_SIGNED_FILE=${UPGRADE_FILE}.signed

echo -e "Upgrade delta from ${PREV_CORE_FILE} to ${NEW_CORE_FILE}\n\n" > ${UPGRADE_LIST_FILE}

#
# generate patch
#

# remove existing
rm -f upgrade.patch
rm -f upgrade.patch.sh
rm -f hobart.tar.gz

# generate new
mountavfs

# Let the virtual drive get mounted
ls -R ~/.avfs${PWD}/${NEW_CORE_FILE}\#/  > /dev/null
ls -R ~/.avfs${PWD}/${PREV_CORE_FILE}\#/  > /dev/null

rsync -av -W -c -z --protocol=30 --exclude=/usr/local/hobart --only-write-batch=upgrade.patch ~/.avfs${PWD}/${NEW_CORE_FILE}\#/ ~/.avfs${PWD}/${PREV_CORE_FILE}\#/ 2>&1 | tee -a ${UPGRADE_LIST_FILE}

tar -C ~/.avfs${PWD}/${NEW_CORE_FILE}\#/usr/local/hobart -pcvzf ${CWD}/hobart.tar.gz --exclude=videos .

#
# bundle patch into tarball for distribution
#

tar -cvzf ${UPGRADE_FILE} upgrade.patch hobart.tar.gz

# generate MD5 checksum
MD5=`md5sum ${UPGRADE_FILE} | awk '{ print $1 }'`
echo ${MD5} > ${UPGRADE_MD5_FILE}

## sign feature installer
#cd ${CWD}/Application/certs
#./signFeatureInstaller.sh ${CWD}/${UPGRADE_FILE}
#cd ${CWD}
#
## create bundle
#BASENAME=`basename ${UPGRADE_FILE} .tgz`
#
#tar --create --verbose --gzip --owner=0 --group=0 --file ${BASENAME}.package.tgz ${UPGRADE_FILE} ${UPGRADE_MD5_FILE} ${UPGRADE_CRT_FILE} ${UPGRADE_SIGNED_FILE}
#
## remove files
#rm ${UPGRADE_FILE} ${UPGRADE_MD5_FILE} ${UPGRADE_CRT_FILE} ${UPGRADE_SIGNED_FILE}

#
# cleanup
#

umountavfs

rm -f upgrade.patch
rm -f upgrade.patch.sh
rm -f hobart.tar.gz
