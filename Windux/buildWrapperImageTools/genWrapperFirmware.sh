#!/bin/bash

#set -x

#-------------------------------------------------------------------------------------------
#  Automates building of wrapper firmware upgrade file
#
#  Using the wrapper node firmware and camera node firmware provided,
#
#  1. Exports the appropriate ".S" files from the wrapper firmware repo in SVN
#  2. Extracts and parses the SVN log message
#  3. Executes runBuildWrapperFirmware.sh to generate the wrapper firmware upgrade file
#  4. Copies the wrapper firmware upgrade file to CustomInstallers/featureWrapperFirmware,
#     adds to SVN and commits the files.  It uses the SVN log message from the wrapper
#     firmware SVN repo in this commit.
#  5. The optional parameter "NGW2P" must be passed in when building wrapper firmware
#     upgrade for a 2nd Label Applier, as it will extract the ".S" files from a different
#     path in the Subversion repository. Along with the wrapper firmware for the 2nd
#     Label Applier, it will also package the TOF App and Boot firmware.
#
#     If you pass the "NGW2P" parameter, then you must include the TOP app and boot
#     firmware versions.
#
#
#  Parameters:  Wrapper-node-version Camera-node-version NGW2P-optional TOF-app-version TOF-boot-version
#
#      For example:  genWrapperFirmware.sh  8.77  9.1
#
#          or        genWrapperFirmware.sh  8.77  9.1  NGW2P  00.90  00.85
#
#  Prerequisites:
#
#        This shell script assumes the current directory is Windux/buildWrapperImageTools
#        and the directorty CustomInstallers/featureWrapperFirmware exists
#
#        This shell script also assumes the SVN URL for the wrapper firmware is:
#                                 https://engdev1a.itwfeg.biz:8443/svn/wrapper/firmwareFiles/ngw/
#
#        Also need to have the xmllint linux utility installed to parse the SVN log message.
#-------------------------------------------------------------------------------------------

if [ $# -lt 2 ] ; then
    echo -e "\nYou must specify the  \"Wrapper-node-version\"  \"Camera-node-version\"."
    echo -e "\nOptionally you can specify \"NGW2P\", plus the \"tof-app-version\" and \"tof-boot-version\"."
    echo -e "\nFor example:   genWrapperFirmware.sh  8.77  9.1\n"
    echo -e "\n or            genWrapperFirmware.sh  8.77  9.1   NGW2P  0.0.90  0.0.85\n"

    exit 1
fi

# SOF-4509 - Generate wrapper firmware upgrade for NGW 2nd Label Applier
buildNGW2P="n"
tofAppVer="0.0"
tofBootVer="0.0"
if [ $# -eq 5 ] ; then
    if [ "$3" = "NGW2P" ] ; then
        buildNGW2P="y"
        tofAppVer=$4
        tofBootVer=$5
    fi
fi

nodeVer=$1
cameraVer=$2

# ... append ".0" to the version in the filename
fileVer=$1.0

firmwareUpgradeFilename=htseries-any-ngw-$fileVer-release.package.tgz
if [ "$buildNGW2P" = "y" ] ; then
    firmwareUpgradeFilename=htseries-any-ngw2p-$fileVer-release.package.tgz
fi


svnWrapperRepo=https://engdev1a.itwfeg.biz:8443/svn/wrapper/firmwareFiles/

# Remove any old files
rm -f ./Node1_APP.elf.p.S ./Node2_APP.elf.p.S ./Node3_APP.elf.p.S ./Node4_APP.elf.p.S ./Node5_APP.elf.p.S ./WrapperCameraSystem.elf.p.S

# SOF-4509 - Generate wrapper firmware upgrade for NGW 2nd Label Applier
svnNGWDir=ngw
svnNGWCameraDir=camera
if [ "$buildNGW2P" = "y" ] ; then
    svnNGWDir=ngw_2_Printers
    svnNGWCameraDir=camera_ngw_2_Printers
fi

# Export wrapper firmware files from wrapper firmware SVN repository
svn export $svnWrapperRepo/$svnNGWDir/$nodeVer/Node1_APP.elf.p.S Node1_APP.elf.p.S
svn export $svnWrapperRepo/$svnNGWDir/$nodeVer/Node2_APP.elf.p.S Node2_APP.elf.p.S
svn export $svnWrapperRepo/$svnNGWDir/$nodeVer/Node3_APP.elf.p.S Node3_APP.elf.p.S
svn export $svnWrapperRepo/$svnNGWDir/$nodeVer/Node4_APP.elf.p.S Node4_APP.elf.p.S
svn export $svnWrapperRepo/$svnNGWDir/$nodeVer/Node5_APP.elf.p.S Node5_APP.elf.p.S
svn export $svnWrapperRepo/$svnNGWCameraDir/$cameraVer/WrapperCameraSystem.elf.p.S WrapperCameraSystem.elf.p.S

if [ "$buildNGW2P" = "y" ] ; then
    svn export $svnWrapperRepo/tof/app/$tofAppVer/PPPSensor-K64.bin sourceTOFApp.bin
    svn export $svnWrapperRepo/tof/boot/$tofBootVer/k64-bootloader.bin sourceTOFBoot.bin
fi


# Grab the svn log message from the wrapper firmware SVN repository
logMsg=`svn log $svnWrapperRepo/$svnNGWDir/$nodeVer --xml | xmllint --xpath "//log/logentry/msg/text()" -`
fullLogMsg="NGW Wrapper Firmware version $nodeVer: $logMsg"

# Generate the upgrade file
./runBuildWrapperFirmware.sh $fileVer $nodeVer $cameraVer $buildNGW2P $tofAppVer $tofBootVer
exitVal=$?

if [ $exitVal != "0" ]; then

    exit 1

else

    # Make sure the upgrade file was generated
    if [ ! -f ./$firmwareUpgradeFilename ]; then

        echo "Wrapper upgrade file was not generated."

        exit 1
    fi

    # Need to check if the file is already in SVN
    cd ../CustomInstallers/featureWrapperFirmware
    svn info $firmwareUpgradeFilename 1> /dev/null 2>&1
    isVersioned=$?

    if [ $isVersioned = "0" ]; then
    # Need to remove the existing file from SVN,
    # to avoid getting multiple rev's of the exact file

        #svn delete -m "Deleting existing file." $firmwareUpgradeFilename
        svn delete $firmwareUpgradeFilename

    fi

    # Now copy the file to CustomInstallers, add & commit to SVN
    cp ../../buildWrapperImageTools/$firmwareUpgradeFilename .
    svn add $firmwareUpgradeFilename
    svn commit $firmwareUpgradeFilename -m "$fullLogMsg"

fi

