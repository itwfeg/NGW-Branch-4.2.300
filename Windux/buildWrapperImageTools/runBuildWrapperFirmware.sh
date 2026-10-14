#!/bin/bash

#set -x

#--------------------------------------------------------------------------------------------
#  Executes buildWrapperImage.py under Linux, to generate firmware upgrade for wrapper nodes.
#
#  Parameters: Upgrade-file-version Wrapper-node-version Camera-node-version NGW2P-optional \
#                TOF-app-version-optional TOF-boot-version-optional
#
#      For example:  runBuildWrapperFirmware.sh  8.77.0  8.77  9.1  
#
#       or           runBuildWrapperFirmware.sh  8.77.0  8.77  9.1  ngw2p-value  00.90  00.85
#
#
#  Note: This shell script assumes the current directory is Windux/buildWrapperImageTools
#--------------------------------------------------------------------------------------------

if [ $# -lt 3 ] ; then
    echo -e "\nYou must specify the  \"Upgrade-file-version\"  \"Wrapper-node-version\"  \"Camera-node-version\"."
    echo -e "\nOptionally you can specify \"NGW2P (y or n)\", plus the \"tof-app-version\" and \"tof-boot-version\"."
    echo -e "\nFor example:   runBuildWrapperFirmware.sh  8.77.0  8.77  9.1\n"
    echo -e "\n or:           runBuildWrapperFirmware.sh  8.77.0  8.77  9.1  y  0.0.90  0.0.85\n"

    exit 1
fi

# SOF-4509 - Generate wrapper firmware upgrade for NGW 2nd Label Applier
buildNGW2P="n"
tofAppVer=0.0
tofBootVer=0.0
if [ $# -eq 6 ] ; then
    if [ "$4" = "y" ] ; then
        buildNGW2P="y"
        tofAppVer="$5"
        tofBootVer="$6"
    fi
fi

fileVer=$1
nodeVer=$2
cameraVer=$3

if [ ! -f ./Node1_APP.elf.p.S ] || \
    [ ! -f ./Node2_APP.elf.p.S ] || \
    [ ! -f ./Node3_APP.elf.p.S ] || \
    [ ! -f ./Node4_APP.elf.p.S ] || \
    [ ! -f ./Node5_APP.elf.p.S ] || \
    [ ! -f ./WrapperCameraSystem.elf.p.S ]

    then
    echo "Missing wrapper firmware \".S\" files, build terminated"

    exit 1
fi


# SOF-4509 - Generate wrapper firmware upgrade for NGW 2nd Label Applier
if [ "$buildNGW2P" = "y" ] ; then
    if [ ! -f ./sourceTOFApp.bin ] || \
        [ ! -f ./sourceTOFBoot.bin ]

        then
        echo "Missing wrapper firmware \"TOF\" files, build terminated"

        exit 1
    fi
fi

releaseType=ngw
if [ "$buildNGW2P" = "y" ] ; then
    releaseType="ngw2p"
fi

python buildWrapperImage.py $releaseType $fileVer $nodeVer $cameraVer $tofAppVer $tofBootVer
