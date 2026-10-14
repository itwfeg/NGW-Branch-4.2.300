#!/bin/bash
# Performs a build of the runtime files for the Windux Application
# and optionally zeroize the DB
if [ $# == 4 ]
then
    username="$1"
    zeroize="$2"
    device="$3"
    cleanbuild="$4"
else
    echo -n "Enter unix username: "
    read username
    echo

    echo -n "Do you want to zeroize the DB? (Y/N): "
    read -n 1 response
    echo

    echo -n "What device would like to build HLX=1,EWS=2,AWS=3?"
    read -n 1 device
    echo

    echo -n "Do you want a clean build? (Y/N): "
    read -n 1 cleanbuild
    echo

fi

if [ "$zeroize" == "Y" ]
then
    echo "Zeroing databases"
    cd /etc/hobart/dbfiles
    rm -f *
    cp -f ../dbempty/* .
fi


if [ "$device" == "1" ]
then
    echo "Constructing firmware files for HLX"
    rm -f /lib/firmware/hobart/*
    cp -f /media/sf_windux/Application/hlx_firmware_bins/* /lib/firmware/hobart
    cp -f /media/sf_windux/Config_files/etc/hobart/hlx_version.ini /etc/hobart
elif [ "$device" == "2" ]
then
    echo "Constructing firmware files for EWS"
    rm -f /lib/firmware/hobart/*
    cp -f /media/sf_windux/Application/ews_firmware_bins/* /lib/firmware/hobart
    cp -f /media/sf_windux/Config_files/etc/hobart/ews_version.ini /etc/hobart
elif [ "$device" == "3" ]
then
    echo "Constructing firmware files for AWS"
    rm -f /lib/firmware/hobart/*
    cp -f /media/sf_windux/Application/aws_firmware_bins/* /lib/firmware/hobart
    cp -f /media/sf_windux/Config_files/etc/hobart/aws_version.ini /etc/hobart
else
    echo "Error on input"
    exit 1
fi

echo "Building scale application libraries"
if [ "$cleanbuild" == "Y" ]
then
    echo "Performing a clean build"
    cd $HOBART_APP/..
    make clean
fi

make --warn-undefined-variables -k 2>makelog

cd $HOBART_APP/source/app
make --warn-undefined-variables -k daemon 2>makelog

if [ -f chobart_d ]
then
    chown $username.$username chobart_d
    cp -f chobart_d /usr/bin
    echo "Build completed successfully"
fi
