#!/bin/bash

#   release - build release tarballs on the build server
targettype='release'

svnrepo="https://engdev1a.itwfeg.biz:8443/svn"
gitrepo="https://buildmgr:build1mgr@engdev1a.itwfeg.biz:3000/weigh-wrap"

workingdirectory=`pwd`

export PYTHONPATH="${workingdirectory}/Application/shared/sessionHandler/python"

#---------------------------------------------------
# Set SVN path accordingly for each repository.
# Used to create the SVN tag for the build.

#svnDirOs="fastrack-os-linux/trunk/"
#svnDirScaleManagement="fastrack-hte/trunk/"
# Make sure svnDirWindux does not have "Windux" at the
#    end so that its tag will be created properly
#svnDirWindux="fastrack/branches/branch-4.2.100-ElevatorUI/"
#svnDirMultimedia="fastrack-multimedia/trunk/"
#---------------------------------------------------
# REDO THESE
svnDirOs="fastrack-os-linux/branch-4.2.0-5.2.0/"
svnDirScaleManagement="fastrack-hte/branch-4.2.0-5.2.0/"
# Make sure svnDirWindux does not have "Windux" at the
#    end so that its tag will be created properly
svnDirWindux="fastrack/branches/branch-4.2.100-ElevatorUI/"
svnDirMultimedia="fastrack-multimedia/trunk/"
#---------------------------------------------------

year=`date +"%Y"`
month=`date +"%m"`
day=`date +"%d"`
hour=`date +"%H"`
minute=`date +"%M"`
seconds=`date +"%S"`

# NOTE:
# bump minor release as we go, do not bump this value unless you are building
# a release to be tested
# do not bump major, minor release number until ready for production or after
# production
releaseid="5.2.100"
releasetag="${releaseid}-${month}${day}${year}_${hour}${minute}${seconds}"

# NOTE:
# These are for the slicerui. The do not need bumped unless slicerui has changed
# do not bump major, minor release number until ready for production or after
# production
slicerreleaseid="0.0.009"
slicerreleasetag="${slicerreleaseid}-${month}${day}${year}_${hour}${minute}${seconds}"

# NOTE:
# These are for the corona psu. The do not need bumped unless corona psu has changed
# do not bump major, minor release number until ready for production or after
# production
coronapsureleaseid="0.0.001"
coronapsureleasetag="${coronapsureleaseid}-${month}${day}${year}_${hour}${minute}${seconds}"

# NOTE:
# These are for the tdcc. The do not need bumped unless tdcc has changed
# do not bump major, minor release number until ready for production or after
# production
tdccreleaseid="0.0.001"
tdccreleasetag="${tdccreleaseid}-${month}${day}${year}_${hour}${minute}${seconds}"

# SOF-2326: Application Upgrade
# ------------------ Application Upgrade Settings ------------------
# Set this flag to "y" if you need to include the preinstall directory
newPreinstall="n"
# Where do we try to pull the $basetarball from
applicationBaseServerCoreDir="K:\\weighwrap\\engineering_builds\\HTseries\\HTi\\linux\\live\\"
applicationBaseServer="engdev1a"
applicationBaseServerUser="buildmgr"
# This tarball must be sitting in `pwd`/.. or located at $engdev1HtiLinuxCoreDir
basetarball="hti-linux-core-4.1.300-12142021_140708.tgz"

# SOF-670: Generate KPC firmware 
#PrinterFW-WeigherFW X.X.X-X.X.X
#kpcAppFirmwareVer="10.1.14-10.1.1"
kpcAppFirmwareVer="10.1.16-10.1.1"
# X.X.X
#kpcBootFirmwareVer="0.0.8"
kpcBootFirmwareVer="0.0.10"

# Free Standing scale firmware
# fsssAppFirmwareVer="0.0.29-11.1.0"
# fsssAppFirmwareVer=0.0.44-11.1.0 - update to 1.0.0 for 5.2.100 release
fsssAppFirmwareVer="1.0.0-11.1.0"
# X.X.X - NOT USED FOR NOW: Use KPC Bootloader
fsssBootFirmwareVer="0.0.10"

# Hands Free Pager Device Firmware X.X
hfpdFirmwareVer="1.10"

# SOF-4447: Weigher firmware for Wrapper
wrapperWeigherFirmwareVer="1.5"

# SOF-1275: Automatically generate wrapper firmware
#wrapperFirmwareVer="10.17"
wrapperFirmwareVer="10.22"
cameraFirmwareVer="9.1"
buildWrapperFirmware="n"
includeClearTraySupport="n"

# SOF-4509: Need to also generate wrapper firmware for NGW 2nd-Label-Applier, including TOF firmware
wrapperFirmwareVerNGW2p="11.32"
cameraFirmwareVerNGW2p="9.2"
tofAppFirmwareNGW2pVer="0.0.89"
tofBootFirmwareNGW2pVer="0.0.85"
buildWrapperFirmwareNGW2p="n"

servicekeyrequired="y"
newdb="y"
newvideos="y"
newscalesettings="y"
debug="n"
build="y"
cleanImages="y"
svnupdate="y"

buildOs="n"
buildOsKernel="n"
buildOsUboot="n"
buildOsRecovery="n"
buildOs4x="y"

buildApp="y"
target=""
release="${workingdirectory}/../Release"
rootFs="${release}/rootfs"
liveRfs="${rootFs}/hobart"
recoveryRfs="${rootFs}/recovery"
application="${release}/application"
uboot="${release}/uboot"
lang_en="${release}/training_videos_english"
lang_es="${release}/training_videos_spanish"
lang_fr="${release}/training_videos_french"
features="${release}/features"
tempDir="${release}/tmp"
hfpdDir="${release}/hfpd"
baseVerFile="base"
cpcmd="cp"
cpRecursiveCmd="cp -R"
shcmd="sh -c"
factory="${workingdirectory}/../Os/embedded/timesys/factory"
factoryBuild="${factory}/build_armv7l-timesys-linux-gnueabihf" 
#   The SVN revision number of the major release, e.g., 3.0.0 = 16655
WinduxRevisionFrom="16839"
#   The SVN revision number of the head (current release you are making)
WinduxRevisionHead="17035"
#   The OS revision number of the major release, e.g., 3.0.0 = 1723
OSRevisionFrom="1723"
#   The OS revision number of the head (current release you are making)
OSRevisionHead="1822"

# Set this for customer build
customerSpecificFeature="${workingdirectory}/3RDPartySoftware/walmart/hti-linux-feature-wm_rmt_prnt-${releasetag}.package.tgz"

if [ "$svnupdate" = "y" ]; then
    # NOTE: You need to check out Os, ScaleManagement, and Windux under same directory
    echo "Updating OS working copy from SVN"
    cd ../Os

    if [ "$buildOs" = "y" ]; then
        # Only reliable way to make sure factory changes are built
        rm -rf embedded

        # Force kernel and uboot builds when RFS build
        buildOsKernel="y"
        buildOsUboot="y"

    fi

    svn update
    mkdir -p ../Os/embedded/timesys/itw-boundary

    if [ "$buildOsKernel" = "y" ]; then
        echo "Pulling kernel sources..."
        if [ -d ../Os/embedded/timesys/itw-boundary/Hobart-Kernel ]; then
            cd embedded/timesys/itw-boundary/Hobart-Kernel
            git pull
            cd ../../../..
        elif [ "$buildOs4x" = "y" ]; then
            git clone -b 4.9.x_1.0.0_itw ${gitrepo}/linux-imx6 ../Os/embedded/timesys/itw-boundary/Hobart-Kernel
        else
            git clone -b 3.14.52-itw ${gitrepo}/Hobart-Kernel ../Os/embedded/timesys/itw-boundary/Hobart-Kernel
        fi

        # overwrite timesys config
        cp ../Os/embedded/timesys/itw-boundary/Hobart-Kernel/arch/arm/configs/itw_defconfig ${factory}/src/local/k/kernel/kernel-3.14/config-3.14
        cp ../Os/embedded/timesys/itw-boundary/Hobart-Kernel/arch/arm/boot/dts/imx6q-hb6mx.dts ${factory}/src/local/k/kernel/kernel-3.14
        cp ../Os/embedded/timesys/itw-boundary/Hobart-Kernel/arch/arm/boot/dts/imx6dl-hb6mx.dts ${factory}/src/local/k/kernel/kernel-3.14
        cp ../Os/embedded/timesys/itw-boundary/Hobart-Kernel/arch/arm/boot/dts/imx6q-itw.dts ${factory}/src/local/k/kernel/kernel-3.14
        cp ../Os/embedded/timesys/itw-boundary/Hobart-Kernel/arch/arm/boot/dts/imx6dl-itw.dts ${factory}/src/local/k/kernel/kernel-3.14
        cp ../Os/embedded/timesys/itw-boundary/Hobart-Kernel/arch/arm/boot/dts/imx6q-ark.dts ${factory}/src/local/k/kernel/kernel-3.14
        cp ../Os/embedded/timesys/itw-boundary/Hobart-Kernel/arch/arm/boot/dts/imx6q-ark-hdmi.dts ${factory}/src/local/k/kernel/kernel-3.14
        cp ../Os/embedded/timesys/itw-boundary/Hobart-Kernel/arch/arm/boot/dts/imx6q-ark-lcd.dts ${factory}/src/local/k/kernel/kernel-3.14
        cp ../Os/embedded/timesys/itw-boundary/Hobart-Kernel/arch/arm/boot/dts/imx6dl-ark.dts ${factory}/src/local/k/kernel/kernel-3.14
        cp ../Os/embedded/timesys/itw-boundary/Hobart-Kernel/arch/arm/boot/dts/imx6dl-ark-hdmi.dts ${factory}/src/local/k/kernel/kernel-3.14
        cp ../Os/embedded/timesys/itw-boundary/Hobart-Kernel/arch/arm/boot/dts/imx6dl-ark-lcd.dts ${factory}/src/local/k/kernel/kernel-3.14

        echo "Pulling firmware..."
        if [ -d ../Os/embedded/timesys/itw-boundary/linux-firmware ]; then
            cd embedded/timesys/itw-boundary/linux-firmware
            git pull
            cd ../../../..
        else
            git clone ${gitrepo}/linux-firmware  ../Os/embedded/timesys/itw-boundary/linux-firmware
            # USE 20200519
            cd ../Os/embedded/timesys/itw-boundary/linux-firmware
            git checkout tags/20200519
            cd ../../../..
            
        fi
    fi

    if [ "$buildOsUboot" = "y" ]; then
        echo "Pulling uboot sources..."
        if [ -d ../Os/embedded/timesys/itw-boundary/Hobart-Uboot ]; then
            cd embedded/timesys/itw-boundary/Hobart-Uboot
            git pull
            cd ../../../..
        # Dont use new UBOOT for now
        #elif [ "$buildOs4x" = "y" ]; then
        #    git clone -b 2017.07_itw ${gitrepo}/u-boot-imx6 ../Os/embedded/timesys/itw-boundary/Hobart-Uboot
        else
            git clone -b 2016.03-itw ${gitrepo}/Hobart-Uboot ../Os/embedded/timesys/itw-boundary/Hobart-Uboot
        fi
    fi

    if [ "$buildApp" = "y" ]; then
        echo "Updating Scale Management working copy from SVN"
        cd ../ScaleManagement; svn update
        echo "Updating Windux working copy from SVN"
        cd ../Windux; svn update
        echo "Updating Multimedia working copy from SVN"
        cd ../Multimedia; svn update
    fi
fi

cd ${workingdirectory}

# get the u-boot version from the u-boot makefile
uboot_version=`python getUbootVersion.py ${workingdirectory}/../Os/embedded/timesys/itw-boundary/Hobart-Uboot/Makefile`
if [ "$uboot_version" = "None" ]; then
    echo "Can not compute uboot version number from Makefile"
    exit 1
fi

# create uboot version file
rm -f ${workingdirectory}/../Os/embedded/rfsOverlays/hobart/uboot/uboot-version.txt
echo "${uboot_version}" >  ${workingdirectory}/../Os/embedded/rfsOverlays/hobart/uboot/uboot-version.txt

# create bitmap splash with u-boot version string in it
rm -f ${workingdirectory}/../Os/embedded/rfsOverlays/hobart/uboot/u-version.bmp
python ${workingdirectory}/Application/shared/sessionHandler/python/genVersionBitmap.py ${uboot_version} ${workingdirectory}/../Os/embedded/rfsOverlays/hobart/uboot/u-version.bmp
if [ $? -ne 0 ]; then
    echo "Uboot version bitmap did not get created"
    exit 1
fi

# create rotated bitmap splash with u-boot version string in it for slicer
rm -f ${workingdirectory}/../Os/embedded/rfsOverlays/hobart/uboot/slicer/u-version.bmp
python ${workingdirectory}/Application/shared/sessionHandler/python/genVersionBitmap.py --rotate 270 ${uboot_version} ${workingdirectory}/../Os/embedded/rfsOverlays/hobart/uboot/slicer/u-version.bmp
if [ $? -ne 0 ]; then
    echo "Uboot version bitmap did not get created (rotated)"
    exit 1
fi

# create rotated bitmap splash with u-boot version string in it for corona psu
rm -f ${workingdirectory}/../Os/embedded/rfsOverlays/hobart/uboot/coronaPsu/u-version.bmp
python ${workingdirectory}/Application/shared/sessionHandler/python/genVersionBitmap.py ${uboot_version} ${workingdirectory}/../Os/embedded/rfsOverlays/hobart/uboot/coronaPsu/u-version.bmp
if [ $? -ne 0 ]; then
    echo "Uboot version bitmap did not get created"
    exit 1
fi

# create rotated bitmap splash with u-boot version string in it for tdcc
rm -f ${workingdirectory}/../Os/embedded/rfsOverlays/hobart/uboot/tdcc/u-version.bmp
python ${workingdirectory}/Application/shared/sessionHandler/python/genVersionBitmap.py ${uboot_version} ${workingdirectory}/../Os/embedded/rfsOverlays/hobart/uboot/tdcc/u-version.bmp
if [ $? -ne 0 ]; then
    echo "Uboot version bitmap did not get created"
    exit 1
fi

# Generate/Update ScaleManagement backend dependency list
if [ -d ../ScaleManagement ]; then

    echo "Generating Scale Management backend dependency list "
    ./runBuildDependencyList.sh
    svn commit sm_backend_dependencies.txt -m "Updated with new entries (generated using ScaleManagement/smBackend/build_dependency_list.py)"
fi

if [ "$build" = "y" ]; then

    ################################################################################################
    # RECOVERY
    ################################################################################################
    if [ "$buildOsRecovery" = "y" ]; then

        # make sure ARCH and CROSS_COMPILE are NOT set for factory build
        unset ARCH
        unset CROSS_COMPILE

        echo "Building Recovery RFS via factory"
        cd ${factory}

        # Build Factory and U-Boot dual
        cp ./src/local/workorder-recovery .config
        yes ''| make oldconfig

        # Load our kernel default configuration
        if [ -d "${factoryBuild}" ]; then
            make clean
        fi

        make

        if [ $? -ne 0 ]; then
            echo "Recovery partition did not build, build terminated"
            exit 1
        fi

        # Copy RFS to SVN Repo
        echo "Copy rootfs to recovery..."
        cp ${factoryBuild}/images/rfs/rootfs.tar.gz ${workingdirectory}/../Os/embedded/rfsOverlays/recoveryfs.tar.gz

        echo "Done!"
        exit 0
    fi

    if [ "$targettype" = "release" ]; then

        # SOF-1275 Generate Wrapper Firmware upgrade file
        if [ "$buildWrapperFirmware" = "y" ]; then

            cd ${workingdirectory}/../Windux/buildWrapperImageTools
            ./genWrapperFirmware.sh $wrapperFirmwareVer $cameraFirmwareVer

            if [ $? -ne 0 ]; then
                echo "Wrapper Firmware upgrade build failed, build terminated"
                exit 1
            fi
        fi

        # SOF-4509: Generate Wrapper Firmware upgrade file for NGW 2nd-Label-Applier
        if [ "$buildWrapperFirmwareNGW2p" = "y" ]; then

            cd ${workingdirectory}/../Windux/buildWrapperImageTools
            ./genWrapperFirmware.sh $wrapperFirmwareVerNGW2p $cameraFirmwareVerNGW2p NGW2P $tofAppFirmwareNGW2pVer $tofBootFirmwareNGW2pVer

            if [ $? -ne 0 ]; then
                echo "Wrapper Firmware (2nd-Label-Applier) upgrade build failed, build terminated"
                exit 1
            fi
        fi

        cd ${workingdirectory}

        echo "Performing baseline tag in SVN"
        svn copy ${svnrepo}/${svnDirOs} ${svnrepo}/fastrack-os-linux/tags/hti-${releasetag} -m "Release hti-${releasetag} - release tagged"
        svn copy ${svnrepo}/${svnDirScaleManagement} ${svnrepo}/fastrack-hte/tags/hti-${releasetag} -m "Release hti-${releasetag} - release tagged"
        svn copy ${svnrepo}/${svnDirWindux} ${svnrepo}/fastrack/tags/hti-${releasetag} -m "Release hti-${releasetag} - release tagged"
        svn copy ${svnrepo}/${svnDirMultimedia} ${svnrepo}/fastrack-multimedia/tags/hti-${releasetag} -m "Release hti-${releasetag} - release tagged"

        echo "Performing baseline tag in SVN"
        # FT-1704
        # Tag FPC used in build for both ACR board and BCM board
        #svn copy ${svnrepo}/peripherals/branches/branch-FPC-AD7191 ${svnrepo}/peripherals/tags/branch-FPC-AD7191-hte-${releasetag} -m "Release branch-FPC-AD7191-hte-${releasetag} - release tagged"
        #svn copy ${svnrepo}/peripherals/branches/branch-FPC-cfgDefaults ${svnrepo}/peripherals/tags/branch-FPC-cfgDefaults-hte-${releasetag} -m "Release branch-FPC-cfgDefaults-hte-${releasetag} - release tagged"
        #svn copy ${svnrepo}/peripherals/branches/branch-cat3 ${svnrepo}/peripherals/tags/branch-cat3-hte-${releasetag} -m "Release branch-cat3-hte-${releasetag} - release tagged"
        # Use ladder barcode
        #svn copy ${svnrepo}/peripherals/branches/branch-FPC-6.1-SOF-2141 ${svnrepo}/peripherals/tags/branch-FPC-6.1-SOF-2141-hte-${releasetag} -m "Release branch-FPC-6.1-SOF-2141-hte-${releasetag} - release tagged"
        #svn copy ${svnrepo}/peripherals/branches/branch-BCM-newTimingTables ${svnrepo}/peripherals/tags/branch-BCM-newTimingTables-hte-${releasetag} -m "Release branch-BCM-newTimingTables-hte-${releasetag} - release tagged"
        # PJR-301 Weigher Code separation
        svn copy ${svnrepo}/peripherals/branches/branch-FPC-ARA-Cutter ${svnrepo}/peripherals/tags/branch-FPC-ARA-Cutter-hte-${releasetag} -m "Release branch-FPC-ARA-Cutter-hte-${releasetag} - release tagged"
        svn copy ${svnrepo}/peripherals/branches/branch-BCM-PJR301-WeigherSeparation ${svnrepo}/peripherals/tags/branch-BCM-PJR301-WeigherSeparation-hte-${releasetag} -m "Release branch-BCM-PJR301-WeigherSeparation-hte-${releasetag} - release tagged"
        # Tag KPC firmware
        svn copy ${svnrepo}/peripherals/KPC-Firmware-Files/App/${kpcAppFirmwareVer} ${svnrepo}/peripherals/tags/KPC-Firmware-Files_App_${kpcAppFirmwareVer}-hte-${releasetag} -m "Release KPC-Firmware-Files_App_${kpcAppFirmwareVer}-hte-${releasetag} - release tagged"
        svn copy ${svnrepo}/peripherals/KPC-Firmware-Files/Boot/${kpcBootFirmwareVer} ${svnrepo}/peripherals/tags/KPC-Firmware-Files_Boot_${kpcBootFirmwareVer}-hte-${releasetag} -m "Release KPC-Firmware-Files_Boot_${kpcBootFirmwareVer}-hte-${releasetag} - release tagged"
        # Tag FSSS firmware
        svn copy ${svnrepo}/peripherals/FSSS-KPC-Firmware-Files/App/${fsssAppFirmwareVer} ${svnrepo}/peripherals/tags/FSSS-KPC-Firmware-Files_App_${fsssAppFirmwareVer}-hte-${releasetag} -m "Release FSSS-KPC-Firmware-Files_App_${fsssAppFirmwareVer}-hte-${releasetag} - release tagged"
        svn copy ${svnrepo}/peripherals/FSSS-KPC-Firmware-Files/Boot/${fsssBootFirmwareVer} ${svnrepo}/peripherals/tags/FSSS-KPC-Firmware-Files_Boot_${fsssBootFirmwareVer}-hte-${releasetag} -m "Release FSSS-KPC-Firmware-Files_Boot_${fsssBootFirmwareVer}-hte-${releasetag} - release tagged"        
        # tag Hands Free Firmware
        svn copy ${svnrepo}/peripherals/HFPD-Firmware-Files/Application/${hfpdFirmwareVer} ${svnrepo}/peripherals/tags/HFPD-Firmware-Files_Application_${hfpdFirmwareVer}-hte-${releasetag} -m "Release HFPD-Firmware-Files_App_${hfpdFirmwareVer}-hte-${releasetag} - release tagged"
        # SOF-4447: Tag Weigher firmware for the Wrapper
        svn copy "${svnrepo}/peripherals/trunk/Access USB Weigher/Executables/WeigherBinary/v${wrapperWeigherFirmwareVer}" ${svnrepo}/peripherals/tags/Wrapper-Weigher-Firmware-Files_${wrapperWeigherFirmwareVer}-hte-${releasetag} -m "Release Wrapper-Weigher-Firmware-Files_${wrapperWeigherFirmwareVer}-hte-${releasetag} - release tagged"


        if [ -f version1.ini ]; then
            rm -f version1.ini
        fi

        # Update build number in the application
        cp ./Application/source/app/BuildNumber.cpp BuildNumber1.cpp
        cat BuildNumber1.cpp | sed -e "s/^const std::string BuildNumber::buildNumber\s.*=.*$/const std::string BuildNumber::buildNumber = \"hte-${releasetag}\";/" > ./Application/source/app/BuildNumber.cpp

        # Update version.ini ...
        #  First need to restore what is in SVN - this does not have the pre-pack firmware versions
        rm -f ./Application/source/version.ini
        svn update ./Application/source/version.ini

        # Update build number in version.ini
        cp ./Application/source/version.ini version1.ini
        cat version1.ini | sed -e "s/^component.2.version\s.*=.*$/component.2.version = 2.${releaseid}/" > version.ini

        # Commit version.ini and BuildNumber.cpp with current build number (but not with the pre-pack versions)
        cp version.ini ./Application/source/version.ini

        # HT-3 Update version.ini with Pre-Pack components
        python ${workingdirectory}/Application/shared/sessionHandler/python/updateFirmwareVersionINI.py ${workingdirectory}/version.ini ${workingdirectory}/Application/prepack_firmware_bins

        # Update build number in the SLICER application
        cp ./Application/customerInterface/slicer/slicerversion.cpp slicerversion1.cpp
        cat slicerversion1.cpp | sed -e "s/^const std::string SlicerVersion::buildNumber\s.*=.*$/const std::string SlicerVersion::buildNumber = \"pss-${slicerreleasetag}\";/" > ./Application/customerInterface/slicer/slicerversion.cpp
        logMsg="Build: ${releasetag} \nSlicer Build: ${slicerreleasetag}"
        svn commit ./Application/source/version.ini ./Application/source/app/BuildNumber.cpp ./Application/customerInterface/slicer/slicerversion.cpp -m "`echo -e ${logMsg}`"

    fi

    ################################################################################################
    # RFS
    ################################################################################################
    if [ "$buildOs" = "y" ]; then

        # make sure ARCH and CROSS_COMPILE are NOT set for factory build
        unset ARCH
        unset CROSS_COMPILE

        echo "Building RFS via factory"
        cd ${factory}

        # Build Factory and U-Boot dual
        cp ./src/local/workorder-ark-dual .config
        #cp ./src/local/workorder-combo-dual .config
        yes ''| make oldconfig

        # Load our kernel default configuration
        if [ -d "${factoryBuild}" ]; then
            make clean
        fi

        make

        if [ $? -ne 0 ]; then
            echo "Factory did not build, build terminated"
            exit 1
        fi

        # copy vpu firmware local
        cp ${factoryBuild}/firmware-imx-5.3/firmware-imx-5.3/firmware/vpu/vpu_fw_imx6* .

    fi

    ################################################################################################
    # KERNEL
    ################################################################################################
    if [ "$buildOsKernel" = "y" ]; then

        # make sure ARCH and CROSS_COMPILE are NOT set for factory build
        unset ARCH
        unset CROSS_COMPILE

        echo "Building Kernel via factory"
        cd ${factory}

        ## Build Kernel image
        #cp ./src/local/workorder-combo-dual .config
        #
        #make kernel-distclean
        #make kernel

        #-----------------------------------------------------------------------
        # ARA
        #-----------------------------------------------------------------------

        # Build Factory and U-Boot dual
        cp ./src/local/workorder-combo-dual .config

        make kernel_devicetree-distclean
        make kernel_devicetree

        cp ${factoryBuild}/rfs/boot/imx6q-itw.dtb imx6q-itw.dtb

        # Build Factory and U-Boot solo
        cp ./src/local/workorder-combo-solo .config

        make kernel_devicetree-distclean
        make kernel_devicetree

        cp ${factoryBuild}/rfs/boot/imx6dl-itw.dtb imx6dl-itw.dtb

        #-----------------------------------------------------------------------
        # ARK
        #-----------------------------------------------------------------------

        # dual/quad
        cp ./src/local/workorder-ark-dual .config

        make kernel_devicetree-distclean
        make kernel_devicetree

        cp ${factoryBuild}/rfs/boot/imx6q-ark.dtb imx6q-ark.dtb

        # dual/quad HDMI
        cp ./src/local/workorder-ark-dual-hdmi .config

        make kernel_devicetree-distclean
        make kernel_devicetree

        cp ${factoryBuild}/rfs/boot/imx6q-ark-hdmi.dtb imx6q-ark-hdmi.dtb

        # dual/quad LCD
        cp ./src/local/workorder-ark-dual-lcd .config

        make kernel_devicetree-distclean
        make kernel_devicetree

        cp ${factoryBuild}/rfs/boot/imx6q-ark-lcd.dtb imx6q-ark-lcd.dtb

        # solo
        cp ./src/local/workorder-ark-solo .config

        make kernel_devicetree-distclean
        make kernel_devicetree

        cp ${factoryBuild}/rfs/boot/imx6dl-ark.dtb imx6dl-ark.dtb

        # solo HDMI
        cp ./src/local/workorder-ark-solo-hdmi .config

        make kernel_devicetree-distclean
        make kernel_devicetree

        cp ${factoryBuild}/rfs/boot/imx6dl-ark-hdmi.dtb imx6dl-ark-hdmi.dtb

        # solo LCD
        cp ./src/local/workorder-ark-solo-lcd .config

        make kernel_devicetree-distclean
        make kernel_devicetree

        cp ${factoryBuild}/rfs/boot/imx6dl-ark-lcd.dtb imx6dl-ark-lcd.dtb

        #-----------------------------------------------------------------------
        # BCM
        #-----------------------------------------------------------------------

        # Build Factory and U-Boot dual BCM
        cp ./src/local/workorder-bcm-dual .config

        make kernel_devicetree-distclean
        make kernel_devicetree

        cp ${factoryBuild}/rfs/boot/imx6q-hb6mx.dtb imx6q-hb6mx.dtb

        # Build Factory and U-Boot solo
        cp ./src/local/workorder-bcm-solo .config

        make kernel_devicetree-distclean
        make kernel_devicetree

        cp ${factoryBuild}/rfs/boot/imx6dl-hb6mx.dtb imx6dl-hb6mx.dtb

    fi

    ################################################################################################
    # UBOOT
    ################################################################################################
    if [ "$buildOsUboot" = "y" ]; then

        # make sure ARCH and CROSS_COMPILE are NOT set for factory build
        unset ARCH
        unset CROSS_COMPILE

        echo "Building Uboot via factory"
        cd ${factory}

        #-----------------------------------------------------------------------
        # ARA
        #-----------------------------------------------------------------------

        # Build Factory and U-Boot dual
        cp ./src/local/workorder-combo-dual .config

        # common u-boot image for s/d/q ARA boards
        make u-boot-distclean
        make u-boot

        if [ ! -f ../itw-boundary/Hobart-Uboot/SPL ]; then
            echo " ... U-boot for ARK did not build"
            exit 1
        elif [ ! -f ../itw-boundary/Hobart-Uboot/u-boot.img ]; then
            echo " ... U-boot for ARK did not build"
            exit 1
        fi

        # combine both images into one image usable for field upgrades
        dd if=/dev/zero count=500 bs=1K | tr '\000' '\377' > u-boot-itw-spl.imx
        dd if=../itw-boundary/Hobart-Uboot/SPL of=u-boot-itw-spl.imx bs=1K seek=0 conv=notrunc
        dd if=../itw-boundary/Hobart-Uboot/u-boot.img of=u-boot-itw-spl.imx bs=1K seek=63 conv=notrunc

        # make three copies of the new u-boot (compatible with field upgrade)
        cp u-boot-itw-spl.imx u-boot-itw.imx
        cp u-boot-itw-spl.imx u-boot-s-itw.imx
        cp u-boot-itw-spl.imx u-boot-q-itw.imx
        rm -f u-boot-itw-spl.imx

        #-----------------------------------------------------------------------
        # ARK
        #-----------------------------------------------------------------------

        # Build UBOOT and DTBs
        cp ./src/local/workorder-ark-dual .config

        make u-boot-distclean
        make u-boot

        if [ ! -f ../itw-boundary/Hobart-Uboot/SPL ]; then
            echo " ... U-boot for ARK did not build"
            exit 1
        elif [ ! -f ../itw-boundary/Hobart-Uboot/u-boot.img ]; then
            echo " ... U-boot for ARK did not build"
            exit 1
        fi

        # combine both images into one image usable for field upgrades
        dd if=/dev/zero count=500 bs=1K | tr '\000' '\377' > u-boot-ark.imx
        dd if=../itw-boundary/Hobart-Uboot/SPL of=u-boot-ark.imx bs=1K seek=0 conv=notrunc
        dd if=../itw-boundary/Hobart-Uboot/u-boot.img of=u-boot-ark.imx bs=1K seek=68 conv=notrunc

        #-----------------------------------------------------------------------
        # BCM
        #-----------------------------------------------------------------------

        # Build Factory and U-Boot dual BCM
        cp ./src/local/workorder-bcm-dual .config

        make u-boot-distclean
        make u-boot

        if [ ! -f ../itw-boundary/Hobart-Uboot/u-boot.imx ]; then
            echo " ... U-boot for BCM Dual did not build"
            exit 1
        fi

        mv ../itw-boundary/Hobart-Uboot/u-boot.imx u-boot.imx

        # Build Factory and U-Boot quad BCM
        cp ./src/local/workorder-bcm-quad .config

        make u-boot-distclean
        make u-boot

        if [ ! -f ../itw-boundary/Hobart-Uboot/u-boot.imx ]; then
            echo " ... U-boot for BCM Quad did not build"
            exit 1
        fi

        mv ../itw-boundary/Hobart-Uboot/u-boot.imx u-boot-q.imx

        # Build Factory and U-Boot solo
        cp ./src/local/workorder-bcm-solo .config

        make u-boot-distclean
        make u-boot

        if [ ! -f ../itw-boundary/Hobart-Uboot/u-boot.imx ]; then
            echo " ... U-boot for BCM Solo did not build"
            exit 1
        fi

        mv ../itw-boundary/Hobart-Uboot/u-boot.imx u-boot-s.imx

    fi

    ################################################################################################
    # APP
    ################################################################################################
    if [ "$buildApp" = "y" ]; then

        # SOF-3705 - Disabling this for now, a static built copy of 6.6.0 has been checked in. The new 6.6.0 version
        # requires toolchain modifications...
        # build squish before rest of application because of PATH issues
        #cd ${workingdirectory}/../Windux/3RDPartySoftware/squish/runner
        #./make-debian.sh

        # build application
        export ARCH=arm
        export CROSS_COMPILE=armv7l-timesys-linux-gnueabihf-
        export PATH="/opt/hb6mxd_combo/toolchain/ccache:/opt/hb6mxd_combo/toolchain/bin:$PATH"

        # build with debug turned on
        #export BUILD_TYPE=debug

        cd $workingdirectory

        echo "Building application binaries"
        ./massiveClean.sh
        # build binaries
        make clean

        # remove the binaries just for posterity
        rm -f ./Application/operatorInterface/scaleui/scaleui
        rm -f ./Application/operatorInterface/videoPlayer/videoPlayer
        rm -f ./Application/shared/gstpd/gstpd
        rm -f ./Application/operatorInterface/mb/mb
        rm -f ./Application/operatorInterface/autocalibrationrig/autocalibrationrig
        rm -f ./Application/operatorInterface/dwconfig/dwconfig
        rm -f ./Application/operatorInterface/scaleModelChoice/scaleModelChoice
        rm -f ./Application/customerInterface/gcd/gcd
        rm -f ./Application/customerInterface/nowServing/nowServing
        rm -f ./Application/customerInterface/slicer/slicerui
        rm -f ./Application/operatorInterface/weighercalib/weighercalib
        rm -f ./Application/source/app/chobart_d
        rm -f ./Application/HobartHandsFree/daemon/hfpdd
        rm -f ./Application/HobartHandsFree/ui/hfpdui
        rm -f ./Application/output/libHobartCustomerHook.so

        rm -f ./Application/hydra/events/libevents.so.1.0.0
        rm -f ./Application/hydra/eventadapters/libeventadapters.so.1.0.0
        rm -f ./Application/hydra/hydramc/hydramc
        rm -f ./Application/hydra/hobartsessionman/hobartsessionman
        rm -f ./Application/hydra/hobartcomms/hobartcomms
        rm -f ./Application/hydra/booda/booda
        rm -f ./Application/hydra/test_hydramc/test_hydramc
        rm -f ./Application/hydra/test_hobartsessionman/test_hobartsessionman
        rm -f ./Application/hydra/test_hobartcomms/test_hobartcomms
	
        echo "Grabbing prebuilt chilkat archive libraries from SVN"
        svn update

        echo "Building the application binaries"
        make -j32

        if [ $? -ne 0 ]; then
            echo "Application did not build correctly, build terminated"
            exit 1
        fi

        # Check to make sure the executables were re-built
        if [ ! -x ./Application/operatorInterface/scaleui/scaleui ] || [ ! -x ./Application/customerInterface/gcd/gcd ] || \
           [ ! -x ./Application/customerInterface/nowServing/nowServing ] || [ ! -x ./Application/customerInterface/slicer/slicerui ] || \
           [ ! -x ./Application/source/app/chobart_d ] || [ ! -x ./Application/operatorInterface/videoPlayer/videoPlayer ] || \
           [ ! -x ./Application/shared/gstpd/gstpd ] || [ ! -x ./Application/operatorInterface/mb/mb ] || \
           [ ! -x ./Application/operatorInterface/dwconfig/dwconfig ] || [ ! -x ./Application/operatorInterface/autocalibrationrig/autocalibrationrig ] || \
           [ ! -x ./Application/operatorInterface/dwconfig/dwconfig ] || [ ! -x ./Application/operatorInterface/weighercalib/weighercalib ] || \
           [ ! -x ./Application/operatorInterface/scaleModelChoice/scaleModelChoice ] || [ ! -x ./Application/HobartHandsFree/daemon/hfpdd ] || \
           [ ! -x ./Application/HobartHandsFree/ui/hfpdui ] || [ ! -x ./Application/output/libHobartCustomerHook.so ]
        then

            echo "Application did not build correctly, build terminated"

            if [ ! -x ./Application/operatorInterface/scaleui/scaleui ]; then
                echo " ... scaleui did not build"
            fi

            if [ ! -x ./Application/customerInterface/gcd/gcd ]; then
                echo " ... gcd did not build"
            fi

            if [ ! -x ./Application/customerInterface/nowServing/nowServing ]; then
                echo " ... nowServing did not build"
            fi

            if [ ! -x ./Application/customerInterface/slicer/slicerui ]; then
                echo " ... slicerui did not build"
            fi

            if [ ! -x ./Application/source/app/chobart_d ]; then
                echo " ... chobart_d did not build"
            fi

            if [ ! -x ./Application/operatorInterface/videoPlayer/videoPlayer ]; then
                echo " ... videoPlayer did not build"
            fi

            if [ ! -x ./Application/shared/gstpd/gstpd ]; then
                echo " ... gstpd did not build"
            fi

            if [ ! -x ./Application/operatorInterface/mb/mb ]; then
                echo " ... mb did not build"
            fi

            if [ ! -x ./Application/operatorInterface/dwconfig/dwconfig ]; then
                echo " ... dwconfig did not build"
            fi

            if [ ! -x ./Application/operatorInterface/autocalibrationrig/autocalibrationrig ]; then
                echo " ... autocalibrationrig did not build"
            fi
			
            if [ ! -x ./Application/operatorInterface/weighercalib/weighercalib ]; then
                echo " ... weighercalib did not build"
            fi

            if [ ! -x ./Application/operatorInterface/scaleModelChoice/scaleModelChoice ]; then
                echo " ... scaleModelChoice did not build"
            fi

            if [ ! -x ./Application/HobartHandsFree/daemon/hfpdd ]; then
                echo " ... hfpdd did not build"
            fi

            if [ ! -x ./Application/HobartHandsFree/ui/hfpdui ]; then
                echo " ... hfpdui did not build"
            fi

            if [ ! -x ./Application/output/libHobartCustomerHook.so ]; then
                echo " ... hook did not build"
            fi

            exit 1
        fi

        #----------------------------------------------------------------------

        echo "Building Application Python modules"

        cd ${workingdirectory}

        # remove already byte compiled python files
        rm -f ${workingdirectory}/Application/shared/sessionHandler/python/*.pyc
        rm -rf ${workingdirectory}/Application/shared/sessionHandler/python/__pycache__

        # compile all files
        python3 -m compileall ${workingdirectory}/Application/shared/sessionHandler/python

        # if any error compiling
        if [ $? -ne 0 ]; then
            echo "Python module(s) did not build correctly, build terminated"
            exit 1
        fi

        # must remove byte compiled files since python interpreter version is
        # different between engdev1 (2.6.5) and (2.7.3) on i.MX6 target
        # note that python3 puts the .pyc files in the __pycache__ folder
        rm -f ${workingdirectory}/Application/shared/sessionHandler/python/*.pyc
        rm -rf ${workingdirectory}/Application/shared/sessionHandler/python/__pycache__

        echo "Building SmBackend Python modules"

        # Scale Management backend build
        cd ${workingdirectory}

        rm -f ${workingdirectory}/../ScaleManagement/smBackend/*.pyc
        rm -rf ${workingdirectory}/../ScaleManagement/smBackend/__pycache__

        python3 -m compileall ${workingdirectory}/../ScaleManagement/smBackend

        if [ $? -ne 0 ]; then
            echo "SmBackend Python module(s) did not build correctly, build terminated"
            exit 1
        fi

        # must remove byte compiled files since python interpreter version is
        # different between engdev1 (2.6.5) and (2.7.3) on i.MX6 target
        rm -f ${workingdirectory}/../ScaleManagement/smBackend/*.pyc
        rm -rf ${workingdirectory}/../ScaleManagement/smBackend/__pycache__

        #----------------------------------------------------------------------

        # build hobart packages
        echo "Building Hobart packages"

        cd ${workingdirectory}/../Windux/Application/HobartHandsFree
        ./make-debian.sh &

        # build 3rd party packages
        echo "Building 3rd Party packages (Stage 1)"

        cd ${workingdirectory}/../Windux/3RDPartySoftware/bluenica
        ./make-debian.sh &

        cd ${workingdirectory}/../Windux/3RDPartySoftware/bluez
        ./make-debian.sh &

        cd ${workingdirectory}/../Windux/3RDPartySoftware/walmart/adservice/feature
        python build_feature.py

        cd ${workingdirectory}/../Windux/3RDPartySoftware/squish/exec
        ./make-debian.sh &

        cd ${workingdirectory}/../Windux/3RDPartySoftware/walmart/app_hook
        ./make-debian.sh &

        cd ${workingdirectory}/../Windux/3RDPartySoftware/walmart/app_hook_v2
        ./make-debian.sh &

        cd ${workingdirectory}/../Windux/3RDPartySoftware/HTe-Enterprise/app_hook_v2
        ./make-debian.sh &

        cd ${workingdirectory}/../Windux/3RDPartySoftware/walmart/fpplitewebapp
        ./make-debian.sh 

        cd ${workingdirectory}/../Windux/3RDPartySoftware/walmart/offlineerrordialog
        ./make-debian.sh &

        cd ${workingdirectory}/../Windux/3RDPartySoftware/oo_settings/oo_settings_webapp
        ./make-debian.sh &

        cd ${workingdirectory}/../Windux/3RDPartySoftware/walmart/wmsmart
        ./make-debian.sh &

        cd ${workingdirectory}/../Windux/3RDPartySoftware/tiliter/tiliter_hook
        ./make-debian.sh &

        cd ${workingdirectory}/../Windux/3RDPartySoftware/fbvnc
        ./make-debian.sh &

        cd ${workingdirectory}/../Windux/3RDPartySoftware/xrdp
        ./make-debian.sh &

        cd ${workingdirectory}/../Windux/3RDPartySoftware/mosquitto
        ./make-debian.sh &

        cd ${workingdirectory}/../Windux/3RDPartySoftware/wpa_supplicant
        ./make-debian.sh &

        cd ${workingdirectory}/../Windux/3RDPartySoftware/clio
        ./make-debian.sh &

        cd ${workingdirectory}/../Windux/3RDPartySoftware/telit
        ./make-debian.sh &

        cd ${workingdirectory}/../Windux/3RDPartySoftware/iperf
        ./make-debian.sh &

        cd ${workingdirectory}/../Windux/3RDPartySoftware/memtester
        ./make-debian.sh &

        cd ${workingdirectory}/../Windux/3RDPartySoftware/glmark2
        ./make-debian.sh &

        cd ${workingdirectory}/../Windux/3RDPartySoftware/devregs
        ./make-debian.sh &

        cd ${workingdirectory}/../Windux/3RDPartySoftware/edgify/edgify_hook
        ./make-debian.sh &

        # wait for 3rd party packages to build...
        FAIL=0

        for job in `jobs -p`; do
            wait $job || let "FAIL+=1"
        done

        if [ "$FAIL" == "0" ]; then
            echo "All 3rd Party packages built!"
        else
            echo "One or more 3rd party packages failed! Build terminated..."
            exit 1
        fi

        # build 3rd party packages that have dependancies on those above
        echo "Building 3rd Party packages (Stage 2)"

        cd ${workingdirectory}/../Windux/3RDPartySoftware/edgify/edgifytrainer
        ./make-debian.sh &

        # wait for 3rd party packages to build...
        FAIL=0

        for job in `jobs -p`; do
            wait $job || let "FAIL+=1"
        done

        if [ "$FAIL" == "0" ]; then
            echo "All 3rd Party packages built!"
        else
            echo "One or more 3rd party packages failed! Build terminated..."
            exit 1
        fi

        #build SD Migration Feature
        cd ${workingdirectory}
        sh $workingdirectory/Application/shared/util/SD_Migration_Feature/build_sd_migration.sh

    fi
fi

# delete release tree
if [ -d ${release} ]; then
    rm -rf ${release}
fi

mkdir -p ${liveRfs}
mkdir -p ${recoveryRfs}
mkdir -p ${application}
mkdir -p ${uboot}
mkdir -p ${lang_en}
mkdir -p ${lang_es}
mkdir -p ${lang_fr}
mkdir -p ${features}
mkdir -p ${tempDir}
mkdir -p ${liveRfs}/uboot
mkdir -p ${hfpdDir}

# generate live RFS
echo "Extracting rootfs..."
fakeroot tar xvzf ${factoryBuild}/images/rfs/rootfs.tar.gz -C ${liveRfs}

# Fixup libg2d with version 6.2.2
cp ${factoryBuild}/gpu-viv-g2d-6.2.2.p0/imx-gpu-g2d-6.2.2.p0/g2d/usr/lib/* ${liveRfs}/usr/lib/.

# Remove all the stuff we do not need
rm -rf ${liveRfs}/opt/fsl-samples
rm -rf ${liveRfs}/opt/viv_samples
rm  -f ${liveRfs}/usr/lib/lib_mp3*
rm -rf ${liveRfs}/usr/share/imx-mm

# Remove the timesys xorg conf that conflicts with ours
rm ${liveRfs}/usr/share/X11/xorg.conf.d/99-timesys.conf

# We don't do this in version 2.5 and prior (it's broken)... may not need it.
#mkdir -p ${liveRfs}/usr/local/hobart/recovery/
#$cpcmd ${workingdirectory}/../Os/embedded/rfsOverlays/recoveryfs.tar.gz ${liveRfs}/usr/local/hobart/recovery/recovery.tgz

# generate recovery RFS
echo "Extracting recoveryfs..."
fakeroot tar xvzf ${workingdirectory}/../Os/embedded/rfsOverlays/recoveryfs.tar.gz -C ${recoveryRfs}

# Fixup libg2d with version 6.2.2
cp ${factoryBuild}/gpu-viv-g2d-6.2.2.p0/imx-gpu-g2d-6.2.2.p0/g2d/usr/lib/* ${recoveryRfs}/usr/lib/.

# TBD: Aaron there may need to be different pruning with new RFS
# Remove all the stuff we do not need during an upgrade/recovery
rm -rf ${recoveryRfs}/opt/fsl-samples
rm -rf ${recoveryRfs}/opt/viv_samples
rm  -f ${recoveryRfs}/usr/lib/lib_mp3*
rm -rf ${recoveryRfs}/usr/share/imx-mm

rm -rf ${recoveryRfs}/usr/bin/chobart_d
rm -rf ${recoveryRfs}/usr/bin/strace
rm -rf ${recoveryRfs}/usr/bin/perl
rm -rf ${recoveryRfs}/usr/bin/perl5.14.2
rm -rf ${recoveryRfs}/usr/bin/gdbserver
rm -rf ${recoveryRfs}/usr/bin/alsaloop
rm -rf ${recoveryRfs}/usr/bin/alsamixer
rm -rf ${recoveryRfs}/usr/bin/alsaucm
rm -rf ${recoveryRfs}/usr/bin/amidi
rm -rf ${recoveryRfs}/usr/bin/amixer
rm -rf ${recoveryRfs}/usr/bin/animate
rm -rf ${recoveryRfs}/usr/bin/aplay
rm -rf ${recoveryRfs}/usr/bin/aplaymidi
rm -rf ${recoveryRfs}/usr/local/hobart/videos/*
rm -rf ${recoveryRfs}/usr/local/hobart/html/*

# Remove the timesys xorg conf that conflicts with ours
rm ${recoveryRfs}/usr/share/X11/xorg.conf.d/99-timesys.conf

# Not needed ... this is handled in startupSequence
rm -f ${liveRfs}/etc/init.d/calibrate-touchscreen

rm -f ${liveRfs}/u-boot.nopadding

rm -f ${liveRfs}/6x_upgrade
rm -f ${liveRfs}/6x_bootscript

# save release info to version file
echo "Release ID: ${releaseid}" > ${liveRfs}/SOFTWARE_VERSION_INFO
echo "Creation Date: ${month}/${day}/${year}" >> ${liveRfs}/SOFTWARE_VERSION_INFO
echo "Creation Time: ${hour}:${minute}:${seconds}" >> ${liveRfs}/SOFTWARE_VERSION_INFO

cp -f ${liveRfs}/SOFTWARE_VERSION_INFO ${recoveryRfs}

# Need to have the directory paths present for the following version files
mkdir -p ${liveRfs}/usr/local/hobart
mkdir -p ${recoveryRfs}/usr/local/hobart

# save release info to slicer version file
echo "Release ID: ${slicerreleaseid}" > ${liveRfs}/usr/local/hobart/SLICER_VERSION_INFO
echo "Creation Date: ${month}/${day}/${year}" >> ${liveRfs}/usr/local/hobart/SLICER_VERSION_INFO
echo "Creation Time: ${hour}:${minute}:${seconds}" >> ${liveRfs}/usr/local/hobart/SLICER_VERSION_INFO

# save release info to corona psu version file
echo "Release ID: ${coronapsureleaseid}" > ${liveRfs}/usr/local/hobart/CORONA_PSU_VERSION_INFO
echo "Creation Date: ${month}/${day}/${year}" >> ${liveRfs}/usr/local/hobart/CORONA_PSU_VERSION_INFO
echo "Creation Time: ${hour}:${minute}:${seconds}" >> ${liveRfs}/usr/local/hobart/CORONA_PSU_VERSION_INFO

# save release info to tdcc version file
echo "Release ID: ${tdccreleaseid}" > ${liveRfs}/usr/local/hobart/TDCC_VERSION_INFO
echo "Creation Date: ${month}/${day}/${year}" >> ${liveRfs}/usr/local/hobart/TDCC_VERSION_INFO
echo "Creation Time: ${hour}:${minute}:${seconds}" >> ${liveRfs}/usr/local/hobart/TDCC_VERSION_INFO

cp -f ${liveRfs}/usr/local/hobart/SLICER_VERSION_INFO ${recoveryRfs}/usr/local/hobart/
cp -f ${liveRfs}/usr/local/hobart/CORONA_PSU_VERSION_INFO ${recoveryRfs}/usr/local/hobart/
cp -f ${liveRfs}/usr/local/hobart/TDCC_VERSION_INFO ${recoveryRfs}/usr/local/hobart/


$cpcmd ${factory}/u-boot* ${target}${liveRfs}/
$cpcmd ${factory}/*.dtb ${target}${liveRfs}/boot

$cpcmd -p -r ${workingdirectory}/../Os/embedded/rfsOverlays/hobart/uboot/* ${target}${uboot}/
$cpcmd ${factory}/u-boot* ${target}${uboot}/
$cpcmd ${factory}/*.dtb ${target}${uboot}/

# Copy entire boot directory from live partition to the recovery partition
rm -f ${target}${recoveryRfs}/boot/*
rm -f ${target}${recoveryRfs}/boot/uImage
$cpcmd ${liveRfs}/boot/* ${target}${recoveryRfs}/boot/.

# Copy live drivers to the recovery partition
$cpRecursiveCmd ${liveRfs}/lib/modules/* ${target}${recoveryRfs}/lib/modules/.

# Copy Solo and Dual/Quad vpu firmware to release
$cpcmd ${factory}/vpu_fw_imx6* ${liveRfs}/lib/firmware/vpu

if [ ! -d ${liveRfs}/lib/firmware/fpc ]; then
    mkdir -p ${liveRfs}/lib/firmware/fpc
    mkdir -p ${recoveryRfs}/lib/firmware/fpc
fi

#Bug Fix: ACR-53
if [ ! -d ${liveRfs}/lib/firmware/apc ]; then
    mkdir -p ${liveRfs}/lib/firmware/apc
    mkdir -p ${recoveryRfs}/lib/firmware/apc
fi

# SOF-1063 Firmware for AWS3 (USB), NGW (Hybrid USB/CAN), NGS (CAN)
if [ ! -d ${liveRfs}/lib/firmware/hobart ]; then    
    mkdir -p ${liveRfs}/lib/firmware/hobart
fi

if [ ! -d ${liveRfs}/lib/firmware/kpc ]; then   
    mkdir -p ${liveRfs}/lib/firmware/kpc
    mkdir -p ${liveRfs}/lib/firmware/kpc/app
    mkdir -p ${liveRfs}/lib/firmware/kpc/boot
fi

# Free Standing Self Service Scale firmware
if [ ! -d ${liveRfs}/lib/firmware/fsss ]; then   
    mkdir -p ${liveRfs}/lib/firmware/fsss
    mkdir -p ${liveRfs}/lib/firmware/fsss/app
    mkdir -p ${liveRfs}/lib/firmware/fsss/boot
fi

if [ ! -d ${liveRfs}/lib/firmware/hfpd ]; then
    mkdir -p ${liveRfs}/lib/firmware/hfpd
fi

if [ ! -d ${liveRfs}/lib/firmware/hobart/ngw ]; then
    mkdir -p ${liveRfs}/lib/firmware/ngw
    mkdir -p ${liveRfs}/lib/firmware/ngw/weigher
    mkdir -p ${liveRfs}/lib/firmware/ngw/printer
    mkdir -p ${liveRfs}/lib/firmware/ngw/wrapper

    mkdir -p ${liveRfs}/lib/firmware/hobart/ngw
    mkdir -p ${liveRfs}/lib/firmware/hobart/ngw/wrapper/node1
    mkdir -p ${liveRfs}/lib/firmware/hobart/ngw/wrapper/node2
    mkdir -p ${liveRfs}/lib/firmware/hobart/ngw/wrapper/node3
    mkdir -p ${liveRfs}/lib/firmware/hobart/ngw/wrapper/node4
    mkdir -p ${liveRfs}/lib/firmware/hobart/ngw/wrapper/node5
    mkdir -p ${liveRfs}/lib/firmware/hobart/ngw/wrapper/node6
    mkdir -p ${liveRfs}/lib/firmware/hobart/ngw/wrapper/tof/app
    mkdir -p ${liveRfs}/lib/firmware/hobart/ngw/wrapper/tof/boot
fi

if [ ! -d ${liveRfs}/lib/firmware/hobart/ngs ]; then
    mkdir -p ${liveRfs}/lib/firmware/ngs
    mkdir -p ${liveRfs}/lib/firmware/ngs/weigher
    mkdir -p ${liveRfs}/lib/firmware/ngs/printer
    mkdir -p ${liveRfs}/lib/firmware/ngs/wrapper

    mkdir -p ${liveRfs}/lib/firmware/hobart/ngs
    mkdir -p ${liveRfs}/lib/firmware/hobart/ngs/wrapper/node1
    mkdir -p ${liveRfs}/lib/firmware/hobart/ngs/wrapper/node2
    mkdir -p ${liveRfs}/lib/firmware/hobart/ngs/wrapper/node3
    mkdir -p ${liveRfs}/lib/firmware/hobart/ngs/wrapper/node4
    mkdir -p ${liveRfs}/lib/firmware/hobart/ngs/wrapper/node5
    mkdir -p ${liveRfs}/lib/firmware/hobart/ngs/wrapper/node6
fi

# SOF-4449 Allow weigher firmware on wrapper to be upgraded on first boot
if [ ! -d ${liveRfs}/lib/firmware/weigher ]; then
    mkdir -p ${liveRfs}/lib/firmware/weigher
fi

# Copy overlays to live and recovery partitions
echo "Deploying overlays to target..."

$shcmd "chmod ug=rwx,o=rx ${workingdirectory}/../Os/embedded/rfsOverlays/hobart/etc/init.d/*"
$shcmd "chmod ug=rwx,o=rx ${workingdirectory}/../Os/embedded/rfsOverlays/hobart/usr/bin/*"
$cpRecursiveCmd ${workingdirectory}/../Os/embedded/rfsOverlays/hobart/* ${target}${liveRfs}/.
$cpRecursiveCmd ${workingdirectory}/../Os/embedded/rfsOverlays/hobart/* ${target}${application}/.

ln -s ../init.d/hobartinit ${target}${liveRfs}/etc/rc2.d/S06-hobartinit
ln -s ../init.d/hobartinit ${target}${liveRfs}/etc/rc3.d/S06-hobartinit
ln -s ../init.d/hobartinit ${target}${liveRfs}/etc/rc4.d/S06-hobartinit
ln -s ../init.d/hobartinit ${target}${liveRfs}/etc/rc5.d/S06-hobartinit

ln -s ../init.d/hobartinit ${target}${liveRfs}/etc/rc0.d/K95-hobartinit
ln -s ../init.d/hobartinit ${target}${liveRfs}/etc/rc1.d/K95-hobartinit
ln -s ../init.d/hobartinit ${target}${liveRfs}/etc/rc6.d/K95-hobartinit

$shcmd "chmod ug=rwx,o=rx ${workingdirectory}/../Os/embedded/rfsOverlays/recovery/etc/init.d/*"
$shcmd "chmod ug=rwx,o=rx ${workingdirectory}/../Os/embedded/rfsOverlays/recovery/usr/bin/*"
$cpRecursiveCmd ${workingdirectory}/../Os/embedded/rfsOverlays/recovery/* ${target}${recoveryRfs}/.

# Copy firmware to live and recovery partitions
echo "Deploying firmware to target..."

cp -a ${workingdirectory}/../Os/embedded/timesys/itw-boundary/linux-firmware/ar3k ${target}${liveRfs}/lib/firmware
cp -a ${workingdirectory}/../Os/embedded/timesys/itw-boundary/linux-firmware/ath9k_htc ${target}${liveRfs}/lib/firmware
cp -a ${workingdirectory}/../Os/embedded/timesys/itw-boundary/linux-firmware/ath10k ${target}${liveRfs}/lib/firmware
cp -a ${workingdirectory}/../Os/embedded/timesys/itw-boundary/linux-firmware/qca ${target}${liveRfs}/lib/firmware
cp -a ${workingdirectory}/../Os/embedded/timesys/itw-boundary/linux-firmware/ath3k-1.fw ${target}${liveRfs}/lib/firmware

cp -a ${workingdirectory}/../Os/embedded/timesys/itw-boundary/linux-firmware/imx ${target}${liveRfs}/lib/firmware
cp -a ${workingdirectory}/../Os/embedded/timesys/itw-boundary/linux-firmware/imx ${target}${recoveryRfs}/lib/firmware

#Bug Fix: ACR-53
#support BCM FPC (black peripheral controller board)
#svn export ${svnrepo}/peripherals/branches/branch-cat3/Project/FLASH/Exe/Fpc_108.bin ${target}${liveRfs}/lib/firmware/apc/sourceFPC.bin
# Use ladder barcode
#svn export ${svnrepo}/peripherals/branches/branch-BCM-newTimingTables/Project/FLASH/Exe/Fpc_108.bin ${target}${liveRfs}/lib/firmware/apc/sourceFPC.bin
# PJR-301 Weigher Code separation
svn export ${svnrepo}/peripherals/branches/branch-BCM-PJR301-WeigherSeparation/Project/FLASH/Exe/Fpc_108.bin ${target}${liveRfs}/lib/firmware/apc/sourceFPC.bin
#support ACR FPC (boundary system controller board)
#svn export ${svnrepo}/peripherals/branches/branch-FPC-AD7191/Project/FLASH/Exe/Fpc2_108.bin ${target}${liveRfs}/lib/firmware/fpc/sourceFPC2.bin
# SOF-1294 Reset to Weigher to Factory Defaults
#svn export ${svnrepo}/peripherals/branches/branch-FPC-cfgDefaults/Project/FLASH/Exe/Fpc2_108.bin ${target}${liveRfs}/lib/firmware/fpc/sourceFPC2.bin
# Use ladder barcode
#svn export ${svnrepo}/peripherals/branches/branch-FPC-6.1-SOF-2141/Project/FLASH/Exe/Fpc2_108.bin ${target}${liveRfs}/lib/firmware/fpc/sourceFPC2.bin
# PJR-301 Weigher Code separation
#svn export ${svnrepo}/peripherals/branches/branch-FPC-PJR301-WeigherSeparation/Project/FLASH/Exe/Fpc2_108.bin ${target}${liveRfs}/lib/firmware/fpc/sourceFPC2.bin
# Support for ARA w/Cutter (FPC version 7.0.0)
svn export ${svnrepo}/peripherals/branches/branch-FPC-ARA-Cutter/Project/FLASH/Exe/Fpc2_108.bin ${target}${liveRfs}/lib/firmware/fpc/sourceFPC2.bin
# SOF-4449 Allow weigher firmware on wrapper to be upgraded on first boot
svn export "${svnrepo}/peripherals/trunk/Access USB Weigher/Executables/WeigherBinary/v${wrapperWeigherFirmwareVer}/source.bin" ${target}${liveRfs}/lib/firmware/weigher/XPEWeigher.bin

# ACR-53
$cpcmd ${target}${liveRfs}/lib/firmware/apc/sourceFPC.bin ${tempDir}/.
$cpcmd ${target}${liveRfs}/lib/firmware/fpc/sourceFPC2.bin ${tempDir}/.

# export Kpc Application and Bootloader
# SOF-670: Generate KPC firmware 
svn export ${svnrepo}/peripherals/KPC-Firmware-Files/App/${kpcAppFirmwareVer}/freertos_generic.bin ${target}${liveRfs}/lib/firmware/kpc/app/sourceKPCApp.bin
svn export ${svnrepo}/peripherals/KPC-Firmware-Files/Boot/${kpcBootFirmwareVer}/freertos_bootloader.bin ${target}${liveRfs}/lib/firmware/kpc/boot/sourceKPCBoot.bin

# export Free Standing Scale firmware Application and Bootlaoder
svn export ${svnrepo}/peripherals/FSSS-KPC-Firmware-Files/App/${fsssAppFirmwareVer}/freertos_generic.bin ${target}${liveRfs}/lib/firmware/fsss/app/sourceFsssApp.bin
svn export ${svnrepo}/peripherals/FSSS-KPC-Firmware-Files/Boot/${fsssBootFirmwareVer}/freertos_bootloader.bin ${target}${liveRfs}/lib/firmware/fsss/boot/sourceFsssBoot.bin

# export hands free pager device firmware 
svn export ${svnrepo}/peripherals/HFPD-Firmware-Files/Application/${hfpdFirmwareVer}/IARHfpd.hex ${hfpdDir}/.

# Copy to tempDir so that the app gets bundled in the fpc upgrade
$cpcmd ${target}${liveRfs}/lib/firmware/kpc/app/sourceKPCApp.bin ${tempDir}/.
$cpcmd ${target}${liveRfs}/lib/firmware/fsss/app/sourceFsssApp.bin ${tempDir}/.

# SOF-4447 Need to include Wrapper weigher firmware in the FPC upgrade
$cpcmd ${target}${liveRfs}/lib/firmware/weigher/XPEWeigher.bin ${tempDir}/.

# HT-39
# Copy the prepack firmware from the checked-out working copy to the hobart directory

if [ ! -d ${liveRfs}/lib/firmware/hobart ]; then
    mkdir -p ${liveRfs}/lib/firmware/hobart
    mkdir -p ${recoveryRfs}/lib/firmware/hobart
fi

$cpcmd ${workingdirectory}/Application/prepack_firmware_bins/* ${target}${liveRfs}/lib/firmware/hobart/.
$cpcmd ${workingdirectory}/Application/prepack_firmware_bins/* ${target}${recoveryRfs}/lib/firmware/hobart/.

# HT-46
if [ ! -d ${liveRfs}/root/firmware ]; then
    mkdir -p ${liveRfs}/root/firmware
fi


if [ "$includeClearTraySupport" = "y" ]; then
	if [ ! -d ${liveRfs}/root/firmware/clear_tray_support ]; then
		mkdir -p ${liveRfs}/root/firmware/clear_tray_support
	fi
fi

# HT-46 Pre-Pack upgrade files (signed)
# HT-48 Replace 10.20 AWS upgrade with 10.50
$cpcmd ${workingdirectory}/CustomInstallers/featureWrapperFirmware/htseries-any-ngw-$wrapperFirmwareVer.0-release.package.tgz ${target}${liveRfs}/root/firmware/.
# SOF-4509 NGW 2nd Label Applier Firmware upgrade
$cpcmd ${workingdirectory}/CustomInstallers/featureWrapperFirmware/htseries-any-ngw2p-$wrapperFirmwareVerNGW2p.0-release.package.tgz ${target}${liveRfs}/root/firmware/.
# Remove wrapper firmware for old models
#$cpcmd ${workingdirectory}/CustomInstallers/featureWrapperFirmware/htseries-any-aws3-8.36.0-release.package.tgz ${target}${liveRfs}/root/firmware/.
# SOF-3055: Restore wrapper firmware for older models
$cpcmd ${workingdirectory}/CustomInstallers/featureWrapperFirmware/hte-aws-10.50.0-release.package.tgz ${target}${liveRfs}/root/firmware/.
$cpcmd ${workingdirectory}/CustomInstallers/featureWrapperFirmware/hte-ews-5.50.0-release.package.tgz ${target}${liveRfs}/root/firmware/.
$cpcmd ${workingdirectory}/CustomInstallers/featureWrapperFirmware/hte-cla-2.0.0-release.package.tgz ${target}${liveRfs}/root/firmware/.
$cpcmd ${workingdirectory}/CustomInstallers/featureWrapperFirmware/clear_trays/htseries-any-ngw-10.19.0-release.package.tgz ${target}${liveRfs}/root/firmware/clear_tray_support/.


# SOF-200
if [ ! -d ${liveRfs}/root/timings ]; then
    mkdir -p ${liveRfs}/root/timings
fi

# SOF-1675: Remove timings files from the release for now ...
#$cpcmd ${workingdirectory}/CustomInstallers/featureWrapperFirmware/timings/*.ht ${target}${liveRfs}/root/timings/.


cd ${workingdirectory}

if [ "$build" = "n" ]; then
    cp ${workingdirectory}/Application/source/version.ini version.ini
fi

if [ "$targettype" = "release" ]; then
    mkdir -p ${application}/usr/bin
    mkdir -p ${application}/usr/lib
fi

cd $workingdirectory

echo "Copying runtime binaries to target"
$cpcmd -p ./Application/operatorInterface/scaleui/scaleui ${target}${liveRfs}/usr/bin/.
$cpcmd -p ./Application/operatorInterface/scaleui/scaleui ${target}${application}/usr/bin/.

$cpcmd -p ./Application/operatorInterface/videoPlayer/videoPlayer ${target}${liveRfs}/usr/bin/.
$cpcmd -p ./Application/operatorInterface/videoPlayer/videoPlayer ${target}${application}/usr/bin/.

$cpcmd -p ./Application/shared/gstpd/gstpd ${target}${liveRfs}/usr/bin/.
$cpcmd -p ./Application/shared/gstpd/gstpd ${target}${application}/usr/bin/.

$cpcmd -p ./Application/operatorInterface/mb/mb ${target}${liveRfs}/usr/bin/.
$cpcmd -p ./Application/operatorInterface/mb/mb ${target}${application}/usr/bin/.
$cpcmd -p ./Application/operatorInterface/mb/mb ${target}${recoveryRfs}/usr/bin/.

$cpcmd -p ./Application/operatorInterface/dwconfig/dwconfig ${target}${liveRfs}/usr/bin/.
$cpcmd -p ./Application/operatorInterface/dwconfig/dwconfig ${target}${application}/usr/bin/.

$cpcmd -p ./Application/operatorInterface/scaleModelChoice/scaleModelChoice ${target}${liveRfs}/usr/bin/.
$cpcmd -p ./Application/operatorInterface/scaleModelChoice/scaleModelChoice ${target}${application}/usr/bin/.
$cpcmd -p ./Application/operatorInterface/scaleModelChoice/scaleModelChoice ${target}${recoveryRfs}/usr/bin/.

$cpcmd -p ./Application/operatorInterface/autocalibrationrig/autocalibrationrig ${target}${liveRfs}/usr/bin/.
$cpcmd -p ./Application/operatorInterface/autocalibrationrig/autocalibrationrig ${target}${application}/usr/bin/.

$cpcmd -p ./Application/operatorInterface/weighercalib/weighercalib ${target}${liveRfs}/usr/bin/.
$cpcmd -p ./Application/operatorInterface/weighercalib/weighercalib ${target}${application}/usr/bin/.

$cpcmd -p ./Application/customerInterface/gcd/gcd ${target}${liveRfs}/usr/bin/.
$cpcmd -p ./Application/customerInterface/gcd/gcd ${target}${application}/usr/bin/.

$cpcmd -p ./Application/customerInterface/nowServing/nowServing ${target}${liveRfs}/usr/bin/.
$cpcmd -p ./Application/customerInterface/nowServing/nowServing ${target}${application}/usr/bin/.

$cpcmd -p ./Application/customerInterface/slicer/slicerui ${target}${liveRfs}/usr/bin/.
$cpcmd -p ./Application/customerInterface/slicer/slicerui ${target}${application}/usr/bin/.

$cpcmd -p ./Application/source/app/chobart_d ${target}${liveRfs}/usr/bin/.
$cpcmd -p ./Application/source/app/chobart_d ${target}${application}/usr/bin/.

echo "Installing chilkat library"
$cpcmd -p ./3RDPartySoftware/chilkat/chilkat-9.5.0-armv7l-linux/lib/libchilkat-9.5.0.so ${target}${liveRfs}/usr/lib/.
$cpcmd -p ./3RDPartySoftware/chilkat/chilkat-9.5.0-armv7l-linux/lib/libchilkat-9.5.0.so ${target}${application}/usr/lib/.

echo "Installing customer hooks"
$cpcmd -p ./Application/output/libHobartCustomerHook.so ${target}${liveRfs}/usr/lib/libHobartCustomerHookEmpty.so
$cpcmd -p ./Application/output/libHobartCustomerHook.so ${target}${application}/usr/lib/libHobartCustomerHookEmpty.so

ln -s ./libHobartCustomerHookEmpty.so ${target}${liveRfs}/usr/lib/libHobartCustomerHook.so
ln -s ./libHobartCustomerHookEmpty.so ${target}${application}/usr/lib/libHobartCustomerHook.so

echo "Installing HYDRA"
$cpcmd -p ./Application/hydra/events/libevents.so.1.0.0 ${target}${liveRfs}/usr/lib/libevents.so.1.0.0
$cpcmd -p ./Application/hydra/events/libevents.so.1.0.0 ${target}${application}/usr/lib/libevents.so.1.0.0

ln -s ./libevents.so.1.0.0 ${target}${liveRfs}/usr/lib/libevents.so.1.0
ln -s ./libevents.so.1.0 ${target}${liveRfs}/usr/lib/libevents.so.1
ln -s ./libevents.so.1 ${target}${liveRfs}/usr/lib/libevents.so

ln -s ./libevents.so.1.0.0 ${target}${application}/usr/lib/libevents.so.1.0
ln -s ./libevents.so.1.0 ${target}${application}/usr/lib/libevents.so.1
ln -s ./libevents.so.1 ${target}${application}/usr/lib/libevents.so

$cpcmd -p ./Application/hydra/eventadapters/libeventadapters.so.1.0.0 ${target}${liveRfs}/usr/lib/libeventadapters.so.1.0.0
$cpcmd -p ./Application/hydra/eventadapters/libeventadapters.so.1.0.0 ${target}${application}/usr/lib/libeventadapters.so.1.0.0

ln -s ./libeventadapters.so.1.0.0 ${target}${liveRfs}/usr/lib/libeventadapters.so.1.0
ln -s ./libeventadapters.so.1.0 ${target}${liveRfs}/usr/lib/libeventadapters.so.1
ln -s ./libeventadapters.so.1 ${target}${liveRfs}/usr/lib/libeventadapters.so

ln -s ./libeventadapters.so.1.0.0 ${target}${application}/usr/lib/libeventadapters.so.1.0
ln -s ./libeventadapters.so.1.0 ${target}${application}/usr/lib/libeventadapters.so.1
ln -s ./libeventadapters.so.1 ${target}${application}/usr/lib/libeventadapters.so

$cpcmd -p ./Application/hydra/hydramc/hydramc ${target}${liveRfs}/usr/bin/.
$cpcmd -p ./Application/hydra/hydramc/hydramc ${target}${application}/usr/bin/.

$cpcmd -p ./Application/hydra/hobartsessionman/hobartsessionman ${target}${liveRfs}/usr/bin/.
$cpcmd -p ./Application/hydra/hobartsessionman/hobartsessionman ${target}${application}/usr/bin/.

$cpcmd -p ./Application/hydra/hobartcomms/hobartcomms ${target}${liveRfs}/usr/bin/.
$cpcmd -p ./Application/hydra/hobartcomms/hobartcomms ${target}${application}/usr/bin/.

$cpcmd -p ./Application/hydra/booda/booda ${target}${liveRfs}/usr/bin/.
$cpcmd -p ./Application/hydra/booda/booda ${target}${application}/usr/bin/.

# Copy Qt based language files
# FT-696: Pop Up File Chooser is still in English when you're in French translation

echo "Copying Qt based language files"

$shcmd "mkdir -p ${liveRfs}/usr/qt/translations"
$shcmd "mkdir -p ${application}/usr/qt/translations"

$shcmd "chmod ugo+rwx ${liveRfs}/usr/qt/translations"
$shcmd "chmod ugo+rwx ${application}/usr/qt/translations"

$cpcmd -p ./Application/shared/sessionHandler/languages/qt_??.qm ${target}${liveRfs}/usr/qt/translations/.
$cpcmd -p ./Application/shared/sessionHandler/languages/qt_??.qm ${target}${application}/usr/qt/translations/.

##################

# copy runtime fonts, graphics, etc.
echo "Copying runtime fonts, graphics, flash keys"
$shcmd "rm -rf ${liveRfs}/usr/local/hobart/system"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/system/flashkeygfx"
$shcmd "mkdir -p ${application}/usr/local/hobart/system/flashkeygfx"

$shcmd "chmod ugo+rwx ${liveRfs}/usr/local/hobart/system/flashkeygfx"
$shcmd "chmod ugo+rwx ${application}/usr/local/hobart/system/flashkeygfx"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/system/fonts"
$shcmd "mkdir -p ${application}/usr/local/hobart/system/fonts"

$shcmd "chmod ugo+rwx ${liveRfs}/usr/local/hobart/system/fonts"
$shcmd "chmod ugo+rwx ${application}/usr/local/hobart/system/fonts"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/system/graphics"
$shcmd "mkdir -p ${application}/usr/local/hobart/system/graphics"

$shcmd "chmod ugo+rwx ${liveRfs}/usr/local/hobart/system/graphics"
$shcmd "chmod ugo+rwx ${application}/usr/local/hobart/system/graphics"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/user/graphics"
$shcmd "mkdir -p ${application}/usr/local/hobart/user/graphics"

$shcmd "chmod ugo+rwx ${liveRfs}/usr/local/hobart/user/graphics"
$shcmd "chmod ugo+rwx ${application}/usr/local/hobart/user/graphics"

$shcmd "mkdir -p ${liveRfs}/usr/share/fonts/truetype/Hobart"
$shcmd "mkdir -p ${application}/usr/share/fonts/truetype/Hobart"

$shcmd "chmod ugo+rwx ${liveRfs}/usr/share/fonts/truetype/Hobart"
$shcmd "chmod ugo+rwx ${application}/usr/share/fonts/truetype/Hobart"

$cpcmd -p -r ./Application/System/softkeys/* ${target}${liveRfs}/usr/local/hobart/system/flashkeygfx/.
$cpcmd -p -r ./Application/System/softkeys/* ${target}${application}/usr/local/hobart/system/flashkeygfx/.

$cpcmd -p -r ./Application/System/fonts/* ${target}${liveRfs}/usr/local/hobart/system/fonts/.
$cpcmd -p -r ./Application/System/fonts/* ${target}${application}/usr/local/hobart/system/fonts/.

$cpcmd -p -r ./Application/System/fonts/* ${target}${liveRfs}/usr/share/fonts/truetype/Hobart/.
$cpcmd -p -r ./Application/System/fonts/* ${target}${application}/usr/share/fonts/truetype/Hobart/.

$cpcmd -p -r ./Application/System/graphics/* ${target}${liveRfs}/usr/local/hobart/system/graphics/.
$cpcmd -p -r ./Application/System/graphics/* ${target}${application}/usr/local/hobart/system/graphics/.

$shcmd "rm -rf ${liveRfs}/etc/hobart"
$shcmd "rm -rf ${application}/etc/hobart"

$shcmd "mkdir -p ${liveRfs}/etc/hobart"
$shcmd "mkdir -p ${application}/etc/hobart"

$cpcmd version.ini ${target}${liveRfs}/etc/hobart/.
$cpcmd version.ini ${target}${application}/etc/hobart/.

# create service key on storage media to avoid putting USB key in all the time
if [ "$servicekeyrequired" = "n" ]; then
    echo "Creating key files so no USB thumb drive needs inserted"
    $shcmd "mkdir -p ${liveRfs}/etc/hobart/service"
    $shcmd "mkdir -p ${application}/etc/hobart/service"
    $shcmd "touch ${liveRfs}/etc/hobart/service/key.dat"
    $shcmd "touch ${application}/etc/hobart/service/key.dat"
    if [ -f configc1.dat ]; then
        rm -f configc1.dat
    fi
    cp ./Application/source/configc.dat configc1.dat
    string='serviceKeyRemoveTimeout=0'
    sed -e "\|$string|h; \${x;s|$string||;{g;t};a\\" -e "$string" -e "}" configc1.dat > configc.dat
    $cpcmd -p configc.dat ${target}${liveRfs}/etc/hobart/.
    $cpcmd -p configc.dat ${target}${application}/etc/hobart/.
fi

if [ "$servicekeyrequired" = "y" ]; then
    # else no service key setup
    $shcmd "rm -rf ${liveRfs}/etc/hobart/service"
    $shcmd "rm -rf ${application}/etc/hobart/service"
    $cpcmd -p ./Application/source/configc.dat ${target}${liveRfs}/etc/hobart/.
    $cpcmd -p ./Application/source/configc.dat ${target}${application}/etc/hobart/.
fi

echo "Copying Hobart Root CA and SSH key to target"
$shcmd "mkdir -p ${liveRfs}/root/.ssh"
$shcmd "mkdir -p ${application}/root/.ssh"

$cpcmd -p ./Application/certs/HobartRootCA.crt ${target}${liveRfs}/etc/hobart/.
$cpcmd -p ./Application/certs/HobartRootCA.crt ${target}${application}/etc/hobart/.

$cpcmd -p ./Application/certs/ssh/hobart_ssh.pub ${target}${liveRfs}/root/.ssh/authorized_keys
$shcmd "chmod 700 ${target}${liveRfs}/root/.ssh"
$shcmd "chmod 600 ${target}${liveRfs}/root/.ssh/authorized_keys"

$cpcmd -p ./Application/certs/ssh/hobart_ssh.pub ${target}${application}/root/.ssh/authorized_keys
$shcmd "chmod 700 ${target}${application}/root/.ssh"
$shcmd "chmod 600 ${target}${application}/root/.ssh/authorized_keys"

echo "Copying Text translation files to target"
$cpcmd -p ./Application/source/Text*.txt ${target}${liveRfs}/etc/hobart/.
$cpcmd -p ./Application/source/Text*.txt ${target}${application}/etc/hobart/.

echo "Copying configuation files for c_hobartd to target"
$cpcmd -p ./Application/source/configdebug.dat ${target}${liveRfs}/etc/hobart/.
$cpcmd -p ./Application/source/configdebug.dat ${target}${application}/etc/hobart/.

$cpcmd -p ./Application/source/config_linux.dat ${target}${liveRfs}/etc/hobart/.
$cpcmd -p ./Application/source/config_linux.dat ${target}${application}/etc/hobart/.

$cpcmd -p ./Application/source/gravity_hillsboro.dat ${target}${liveRfs}/etc/hobart/.
$cpcmd -p ./Application/source/gravity_hillsboro.dat ${target}${application}/etc/hobart/.

$cpcmd -p ./Application/source/checkerboardLabel.bmp ${target}${liveRfs}/etc/hobart/.
$cpcmd -p ./Application/source/checkerboardLabel.bmp ${target}${application}/etc/hobart/.

$cpcmd -p ./Application/source/labelSensorTestLabel.bmp ${target}${liveRfs}/etc/hobart/.
$cpcmd -p ./Application/source/labelSensorTestLabel.bmp ${target}${application}/etc/hobart/.

$cpcmd -p ./Application/source/db/defaults_linux.ini ${target}${liveRfs}/etc/hobart/.
$cpcmd -p ./Application/source/db/defaults_linux.ini ${target}${application}/etc/hobart/.

$cpcmd -p ./Application/source/software_revision.ini ${target}${liveRfs}/etc/hobart/.
$cpcmd -p ./Application/source/software_revision.ini ${target}${application}/etc/hobart/.

echo "Copying configuation files for nowServing to target"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/nowServing"
$shcmd "mkdir -p ${application}/usr/local/hobart/nowServing"

$cpcmd ./Application/customerInterface/nowServing/nowServing.config ${target}${liveRfs}/usr/local/hobart/nowServing/.
$cpcmd ./Application/customerInterface/nowServing/nowServing.config ${target}${application}/usr/local/hobart/nowServing/.

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/nowServing/res"
$shcmd "mkdir -p ${application}/usr/local/hobart/nowServing/res"

$cpcmd ./Application/customerInterface/nowServing/res/* ${target}${liveRfs}/usr/local/hobart/nowServing/res/.
$cpcmd ./Application/customerInterface/nowServing/res/* ${target}${application}/usr/local/hobart/nowServing/res/.

echo "Copying configuation files for scaleui and gcd to target"
$cpcmd scaleui.config ${target}${liveRfs}/usr/local/hobart/.
$cpcmd scaleui.config ${target}${application}/usr/local/hobart/.

$cpcmd gcd.config ${target}${liveRfs}/usr/local/hobart/.
$cpcmd gcd.config ${target}${application}/usr/local/hobart/.

$cpcmd videoPlayer.config ${target}${liveRfs}/usr/local/hobart/.
$cpcmd videoPlayer.config ${target}${application}/usr/local/hobart/.

$cpcmd gstpd.config ${target}${liveRfs}/usr/local/hobart/.
$cpcmd gstpd.config ${target}${application}/usr/local/hobart/.

$cpcmd nowServing.config ${target}${liveRfs}/usr/local/hobart/.
$cpcmd nowServing.config ${target}${application}/usr/local/hobart/.

# create GPIO link folder
$shcmd "rm -rf ${liveRfs}/usr/local/gpio"
$shcmd "rm -rf ${recoveryRfs}/usr/local/gpio"

$shcmd "mkdir -p ${liveRfs}/usr/local/gpio"
$shcmd "mkdir -p ${recoveryRfs}/usr/local/gpio"

$shcmd "rm -rf ${recoveryRfs}/usr/local/hobart/resources"
$shcmd "mkdir -p ${recoveryRfs}/usr/local/hobart/resources"

# copy all runtime data over
echo "Creating /usr/local/hobart tree"
$shcmd "rm -rf ${liveRfs}/usr/local/hobart/docs"
$shcmd "rm -rf ${application}/usr/local/hobart/docs"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/docs"
$shcmd "mkdir -p ${application}/usr/local/hobart/docs"

# license directory
$shcmd "rm -rf ${liveRfs}/usr/local/hobart/license"
$shcmd "rm -rf ${application}/usr/local/hobart/license"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/license"
$shcmd "mkdir -p ${application}/usr/local/hobart/license"

$shcmd "rm -rf ${liveRfs}/usr/local/hobart/features"
$shcmd "rm -rf ${application}/usr/local/hobart/features"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/features"
$shcmd "mkdir -p ${application}/usr/local/hobart/features"

$shcmd "rm -rf ${liveRfs}/usr/local/hobart/apps"
$shcmd "rm -rf ${application}/usr/local/hobart/apps"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/apps"
$shcmd "mkdir -p ${application}/usr/local/hobart/apps"

#SOF-432 this needs a different implementation, maybe on first boot?
## Remote print for Walmart
#$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/apps/remote_print"
#$shcmd "mkdir -p ${application}/usr/local/hobart/apps/remote_print"

$shcmd "rm -rf ${liveRfs}/usr/local/hobart/audio"
$shcmd "rm -rf ${application}/usr/local/hobart/audio"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/audio"
$shcmd "mkdir -p ${application}/usr/local/hobart/audio"

$shcmd "rm -rf ${liveRfs}/usr/local/hobart/system/audio"
$shcmd "rm -rf ${application}/usr/local/hobart/system/audio"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/system/audio"
$shcmd "mkdir -p ${application}/usr/local/hobart/system/audio"

$shcmd "rm -rf ${liveRfs}/usr/local/hobart/html"
$shcmd "rm -rf ${application}/usr/local/hobart/html"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/html"
$shcmd "mkdir -p ${application}/usr/local/hobart/html"

# create download directory that chobart_d monitors for things like HTDF
$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/download"
$shcmd "mkdir -p ${application}/usr/local/hobart/download"
# HT-400 Commented out the line below for now ... FPC for combo board clobbered the FPC code on the BCM board,
#        causing it to lose all peripherals.
# $cpcmd -p -r  ${target}${liveRfs}/lib/firmware/fpc/sourceFPC.bin ${liveRfs}/usr/local/hobart/download/.

# SOF-2717: Create pending directory that chobart_d monitors
$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/pending"
$shcmd "mkdir -p ${application}/usr/local/hobart/pending"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/downloadHolding"
$shcmd "mkdir -p ${application}/usr/local/hobart/downloadHolding"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/staging"
$shcmd "mkdir -p ${application}/usr/local/hobart/staging"

# SOF-4769: Scheduled Merge
$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/merge"
$shcmd "mkdir -p ${application}/usr/local/hobart/merge"


if [ "$cleanImages" = "y" ]; then
    $shcmd "rm -rf ${liveRfs}/usr/local/hobart/images"
    $shcmd "rm -rf ${application}/usr/local/hobart/images"
fi

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/images"
$shcmd "mkdir -p ${application}/usr/local/hobart/images"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/images/specials"
$shcmd "mkdir -p ${application}/usr/local/hobart/images/specials"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/images/categories"
$shcmd "mkdir -p ${application}/usr/local/hobart/images/categories"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/images/planograms"
$shcmd "mkdir -p ${application}/usr/local/hobart/images/planograms"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/images/products"
$shcmd "mkdir -p ${application}/usr/local/hobart/images/products"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/images/selfServe"
$shcmd "mkdir -p ${application}/usr/local/hobart/images/selfServe"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/images/softkeys"
$shcmd "mkdir -p ${application}/usr/local/hobart/images/softkeys"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/images/specials"
$shcmd "mkdir -p ${application}/usr/local/hobart/images/specials"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/images/system"
$shcmd "mkdir -p ${application}/usr/local/hobart/images/system"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/images/system/categories"
$shcmd "mkdir -p ${application}/usr/local/hobart/images/system/categories"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/images/system/products"
$shcmd "mkdir -p ${application}/usr/local/hobart/images/system/products"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/images/system/selfServe"
$shcmd "mkdir -p ${application}/usr/local/hobart/images/system/selfServe"

$shcmd "rm -rf ${liveRfs}/usr/local/hobart/layouts"
$shcmd "rm -rf ${application}/usr/local/hobart/layouts"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/layouts"
$shcmd "mkdir -p ${application}/usr/local/hobart/layouts"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/layouts/1024_600"
$shcmd "mkdir -p ${application}/usr/local/hobart/layouts/1024_600"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/layouts/1366_768"
$shcmd "mkdir -p ${application}/usr/local/hobart/layouts/1366_768"

$shcmd "rm -rf ${liveRfs}/usr/local/hobart/playlists"
$shcmd "rm -rf ${application}/usr/local/hobart/playlists"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/playlists"
$shcmd "mkdir -p ${application}/usr/local/hobart/playlists"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/upgrade"
$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/labels"

$shcmd "rm -rf ${liveRfs}/usr/local/hobart/python"
$shcmd "rm -rf ${recoveryRfs}/usr/local/hobart/python"
$shcmd "rm -rf ${application}/usr/local/hobart/python"

$shcmd "rm -rf ${recoveryRfs}/usr/local/hobart/scaleApp"
$shcmd "mkdir -p ${recoveryRfs}/usr/local/hobart/scaleApp"

$shcmd "rm -rf ${recoveryRfs}/usr/local/hobart/u-boot"
$shcmd "mkdir -p ${recoveryRfs}/usr/local/hobart/u-boot"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/python"
$shcmd "mkdir -p ${application}/usr/local/hobart/python"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/python/packages"
$shcmd "mkdir -p ${application}/usr/local/hobart/python/packages"
## python not required in recovery partition
## $shcmd "mkdir -p ${recoveryRfs}/usr/local/hobart/python"

$shcmd "rm -rf ${liveRfs}/usr/local/hobart/eGtouch-2.5.25"
$shcmd "rm -rf ${recoveryRfs}/usr/local/hobart/eGtouch-2.5.25"
$shcmd "rm -rf ${application}/usr/local/hobart/eGtouch-2.5.25"

$shcmd "mkdir -p ${liveRfs}/usr/local/hobart/eGtouch-2.5.25"
$shcmd "mkdir -p ${recoveryRfs}/usr/local/hobart/eGtouch-2.5.25"
$shcmd "mkdir -p ${application}/usr/local/hobart/eGtouch-2.5.25"

$cpcmd -p -r ./Application/System/categories/* ${target}${liveRfs}/usr/local/hobart/images/system/categories/.
$cpcmd -p -r ./Application/System/categories/* ${target}${application}/usr/local/hobart/images/system/categories/.

$cpcmd -p -r ./Application/System/products/* ${target}${liveRfs}/usr/local/hobart/images/system/products/.
$cpcmd -p -r ./Application/System/products/* ${target}${application}/usr/local/hobart/images/system/products/.

$cpcmd -p -r ./Application/shared/sessionHandlerHooks/images/selfServe/* ${target}${liveRfs}/usr/local/hobart/images/system/selfServe/.
$cpcmd -p -r ./Application/shared/sessionHandlerHooks/images/selfServe/* ${target}${application}/usr/local/hobart/images/system/selfServe/.

$cpcmd -p -r ./Application/System/softkeys/* ${target}${liveRfs}/usr/local/hobart/images/softkeys/.
$cpcmd -p -r ./Application/System/softkeys/* ${target}${application}/usr/local/hobart/images/softkeys/.

$cpcmd -p -r ./Application/shared/sessionHandler/images/diagnostics/* ${target}${liveRfs}/usr/local/hobart/images/system/.
$cpcmd -p -r ./Application/shared/sessionHandler/images/diagnostics/* ${target}${application}/usr/local/hobart/images/system/.

# do not remove videos if already existing since this may take some time
if [ "$newvideos" = "y" ]; then
    echo "Removing videos directory on target"
    $shcmd "rm -rf ${liveRfs}/usr/local/hobart/videos"
    $shcmd "rm -rf ${application}/usr/local/hobart/videos"
    $shcmd "rm -rf ${lang_en}/usr/local/hobart/videos"
    $shcmd "rm -rf ${lang_es}/usr/local/hobart/videos"
    $shcmd "rm -rf ${lang_fr}/usr/local/hobart/videos"

    $shcmd "mkdir -p ${liveRfs}/usr/local/hobart/videos"
    $shcmd "mkdir -p ${lang_en}/usr/local/hobart/videos"
    $shcmd "mkdir -p ${lang_es}/usr/local/hobart/videos"
    $shcmd "mkdir -p ${lang_fr}/usr/local/hobart/videos"
fi

# copy default language file to root filesystem
$cpcmd -p ./Application/shared/sessionHandler/languages/lang ${target}${liveRfs}/usr/local/hobart/lang
$cpcmd -p ./Application/shared/sessionHandler/languages/lang ${target}${recoveryRfs}/usr/local/hobart/lang
$cpcmd -p ./Application/shared/sessionHandler/languages/lang ${target}${application}/usr/local/hobart/lang

# copy database over
if [ "$newdb" = "y" ]; then
    echo "Copying db files to target"
    $shcmd "rm -rf ${liveRfs}/usr/local/hobart/db"
    $shcmd "rm -rf ${application}/usr/local/hobart/db"
    $shcmd "rm -rf ${recoveryRfs}/usr/local/hobart/db"

    $shcmd "mkdir -p ${recoveryRfs}/usr/local/hobart/db"

    $shcmd "mkdir -p ${liveRfs}/usr/local/hobart/db/diagnostics"
    $shcmd "mkdir -p ${application}/usr/local/hobart/db/diagnostics"

    $shcmd "mkdir -p ${liveRfs}/usr/local/hobart/db/scale"
    $shcmd "mkdir -p ${application}/usr/local/hobart/db/scale"

    $shcmd "mkdir -p ${liveRfs}/usr/local/hobart/db/scale/dbfiles"
    $shcmd "mkdir -p ${application}/usr/local/hobart/db/scale/dbfiles"

    $shcmd "mkdir -p ${liveRfs}/usr/local/hobart/db/scale/dbempty"
    $shcmd "mkdir -p ${application}/usr/local/hobart/db/scale/dbempty"

    $cpcmd -p ./Application/scaleDB.empty/dbftt.db ${target}${liveRfs}/usr/local/hobart/db/scale/dbempty/.
    $cpcmd -p ./Application/scaleDB.empty/dbftt.db ${target}${application}/usr/local/hobart/db/scale/dbempty/.

    $cpcmd -p ./Application/scaleDB.empty/dbftt.db ${target}${liveRfs}/usr/local/hobart/db/scale/dbfiles/.
    $cpcmd -p ./Application/scaleDB.empty/dbftt.db ${target}${application}/usr/local/hobart/db/scale/dbfiles/.
fi

#echo "Compiling licensing scripts"
#python3 -m compileall ./Application/shared/util/dongleKey

# debug stuff
if [ "$debug" = "y" ]; then
    echo "Copying debug scripts to target"
    $cpcmd restart.sh ${target}${liveRfs}/usr/local/hobart/.
    $cpcmd restart.sh ${target}${application}/usr/local/hobart/.
    $shcmd "chmod ugo+rx ${liveRfs}/usr/local/hobart/restart.sh"
    $shcmd "chmod ugo+rx ${application}/usr/local/hobart/restart.sh"
fi

if [ "$debug" = "n" ]; then
    $shcmd "rm -f ${liveRfs}/usr/local/hobart/restart.sh"
    $shcmd "rm -f ${application}/usr/local/hobart/restart.sh"
fi

echo "Copying /usr/local/hobart runtime files to target"
$cpcmd -p ./Application/shared/sessionHandler/python/* ${target}${liveRfs}/usr/local/hobart/python/.
$cpcmd -p ./Application/shared/sessionHandler/python/* ${target}${application}/usr/local/hobart/python/.

echo "Converting all CRLF's to Unix LF's"
dos2unix ${target}${liveRfs}/usr/local/hobart/python/*.py
dos2unix ${target}${liveRfs}/usr/local/hobart/python/*.sh
dos2unix ${target}${application}/usr/local/hobart/python/*.py
dos2unix ${target}${application}/usr/local/hobart/python/*.sh

## python not required in recovery partition
## $cpcmd -p ./Application/shared/sessionHandler/python/* ${target}${recoveryRfs}/usr/local/hobart/python

$cpcmd -p ./Application/certs/verifyFeatureInstaller.sh ${target}${liveRfs}/usr/local/hobart/python/.
$cpcmd -p ./Application/certs/verifyFeatureInstaller.sh ${target}${application}/usr/local/hobart/python/.

# Remote Print Whitelist for Walmart
$cpcmd -p ./3RDPartySoftware/walmart/remote_print/plu_whitelist.csv ${target}${liveRfs}/usr/local/hobart/apps/remote_print
$cpcmd -p ./3RDPartySoftware/walmart/remote_print/plu_whitelist.csv ${target}${application}/usr/local/hobart/apps/remote_print

$cpcmd -p ./Application/shared/sessionHandler/audio/* ${target}${liveRfs}/usr/local/hobart/audio/.
$cpcmd -p ./Application/shared/sessionHandler/audio/* ${target}${application}/usr/local/hobart/audio/.

$cpcmd -p ./Application/shared/sessionHandler/audio/system/* ${target}${liveRfs}/usr/local/hobart/system/audio/.
$cpcmd -p ./Application/shared/sessionHandler/audio/system/* ${target}${application}/usr/local/hobart/system/audio/.

#SOF-432 this needs a different implementation, maybe on first boot?
## Menu Ordering for Walmart
#$cpRecursiveCmd -p ./Application/shared/sessionHandler/apps/* ${target}${liveRfs}/usr/local/hobart/apps
#find ${target}${liveRfs}/usr/local/hobart/apps -name '.svn' -exec rm -rf \{\} \;
#$cpRecursiveCmd -p ./Application/shared/sessionHandler/apps/* ${target}${application}/usr/local/hobart/apps
#find ${target}${application}/usr/local/hobart/apps -name '.svn' -exec rm -rf \{\} \;
#$shcmd "chmod -R u=rwx,go=rx ${target}${liveRfs}/usr/local/hobart/apps/*"
#$shcmd "chmod -R u=rwx,go=rx ${target}${application}/usr/local/hobart/apps/*"

# HT-15
$cpRecursiveCmd -p ./Application/shared/sessionHandler/html/* ${target}${liveRfs}/usr/local/hobart/html/.
find ${target}${liveRfs}/usr/local/hobart/html -name '.svn' -exec rm -rf \{\} \;
$cpRecursiveCmd -p ./Application/shared/sessionHandler/html/* ${target}${application}/usr/local/hobart/html/.
find ${target}${application}/usr/local/hobart/html -name '.svn' -exec rm -rf \{\} \;

$cpcmd -p ./Application/shared/sessionHandler/images/specials/* ${target}${liveRfs}/usr/local/hobart/images/specials/.
$cpcmd -p ./Application/shared/sessionHandler/images/specials/* ${target}${application}/usr/local/hobart/images/specials/.

$cpcmd -p ./Application/shared/sessionHandlerHooks/images/selfServe/* ${target}${liveRfs}/usr/local/hobart/images/system/selfServe/.
$cpcmd -p ./Application/shared/sessionHandlerHooks/images/selfServe/* ${target}${application}/usr/local/hobart/images/system/selfServe/.

$cpcmd -p ./Application/customerInterface/gcd/images/* ${target}${liveRfs}/usr/local/hobart/layouts/.
$cpcmd -p ./Application/customerInterface/gcd/images/* ${target}${application}/usr/local/hobart/layouts/.
$cpcmd -p ./Application/shared/sessionHandler/images/background_pick5.png ${target}${liveRfs}/usr/local/hobart/layouts/.
$cpcmd -p ./Application/shared/sessionHandler/images/background_pick5.png ${target}${application}/usr/local/hobart/layouts/.

$cpcmd -p ./Application/customerInterface/gcd/images/1024_600/* ${target}${liveRfs}/usr/local/hobart/layouts/1024_600/.
$cpcmd -p ./Application/customerInterface/gcd/images/1024_600/* ${target}${application}/usr/local/hobart/layouts/1024_600/.

$cpcmd -p ./Application/customerInterface/gcd/images/1366_768/* ${target}${liveRfs}/usr/local/hobart/layouts/1366_768/.
$cpcmd -p ./Application/customerInterface/gcd/images/1366_768/* ${target}${application}/usr/local/hobart/layouts/1366_768/.

$cpcmd -p ./Application/customerInterface/gcd/layouts/* ${target}${liveRfs}/usr/local/hobart/layouts/.
$cpcmd -p ./Application/customerInterface/gcd/layouts/* ${target}${application}/usr/local/hobart/layouts/.

$cpcmd -p ./Application/customerInterface/gcd/layouts/1024_600/* ${target}${liveRfs}/usr/local/hobart/layouts/1024_600/.
$cpcmd -p ./Application/customerInterface/gcd/layouts/1024_600/* ${target}${application}/usr/local/hobart/layouts/1024_600/.

$cpcmd -p ./Application/customerInterface/gcd/layouts/1366_768/* ${target}${liveRfs}/usr/local/hobart/layouts/1366_768/.
$cpcmd -p ./Application/customerInterface/gcd/layouts/1366_768/* ${target}${application}/usr/local/hobart/layouts/1366_768/.

$cpcmd -p ./Application/customerInterface/gcd/playlists/* ${target}${liveRfs}/usr/local/hobart/playlists/.
$cpcmd -p ./Application/customerInterface/gcd/playlists/* ${target}${application}/usr/local/hobart/playlists/.

if [ "$newvideos" = "y" ]; then
    echo "Copying videos to target"
    $cpcmd -p ../Multimedia/videos/en/* ${target}${liveRfs}/usr/local/hobart/videos/.

    $cpcmd -p ../Multimedia/videos/en/* ${lang_en}/usr/local/hobart/videos/.
    $cpcmd -p ../Multimedia/videos/es/* ${lang_es}/usr/local/hobart/videos/.
    $cpcmd -p ../Multimedia/videos/fr/* ${lang_fr}/usr/local/hobart/videos/.

fi

# Office Viewer install into release tree
rm -rf ${features}/office_viewer/usr/local/hobart/features/office_viewer
mkdir -p ${features}/office_viewer/usr/local/hobart/features/office_viewer

$cpcmd -p ./3RDPartySoftware/libreoffice/debs/LibreOfficeDev_4.4.7.2_Linux_deb.tar.gz ${features}/office_viewer/usr/local/hobart/features/office_viewer/.
$cpcmd -p ./3RDPartySoftware/libreoffice/debs/LibreOfficeDev_4.4.7.2_Linux_deb_helppack_en-US.tar.gz ${features}/office_viewer/usr/local/hobart/features/office_viewer/.
$cpcmd -p ./3RDPartySoftware/libreoffice/debs/LibreOfficeDev_4.4.7.2_Linux_deb_helppack_es.tar.gz ${features}/office_viewer/usr/local/hobart/features/office_viewer/.
$cpcmd -p ./3RDPartySoftware/libreoffice/debs/LibreOfficeDev_4.4.7.2_Linux_deb_helppack_fr.tar.gz ${features}/office_viewer/usr/local/hobart/features/office_viewer/.
$cpcmd -p ./3RDPartySoftware/libreoffice/debs/LibreOfficeDev_4.4.7.2_Linux_deb_langpack_en-US.tar.gz ${features}/office_viewer/usr/local/hobart/features/office_viewer/.
$cpcmd -p ./3RDPartySoftware/libreoffice/debs/LibreOfficeDev_4.4.7.2_Linux_deb_langpack_es.tar.gz ${features}/office_viewer/usr/local/hobart/features/office_viewer/.
$cpcmd -p ./3RDPartySoftware/libreoffice/debs/LibreOfficeDev_4.4.7.2_Linux_deb_langpack_fr.tar.gz ${features}/office_viewer/usr/local/hobart/features/office_viewer/.
$cpcmd -p ./3RDPartySoftware/libreoffice/install.py ${features}/office_viewer/usr/local/hobart/features/office_viewer/.

# Hands Free app
rm -rf ${features}/hfpd/usr/local/hobart/features/hfpd
mkdir -p ${features}/hfpd/usr/local/hobart/features/hfpd

$cpcmd -p ./Application/HobartHandsFree/*.deb ${features}/hfpd/usr/local/hobart/features/hfpd/.
$cpcmd -p ./Application/HobartHandsFree/feature/* ${features}/hfpd/usr/local/hobart/features/hfpd/.

# OpenJDK Java
rm -rf ${features}/java_jre/usr/local/hobart/features/java_jre
mkdir -p ${features}/java_jre/usr/local/hobart/features/java_jre

$cpcmd -p ./3RDPartySoftware/openjdk-java-7-jre-hf/* ${features}/java_jre/usr/local/hobart/features/java_jre/.

# Text to speech install into release tree
rm -rf ${features}/text_to_speech/usr/local/hobart/features/text_to_speech
mkdir -p ${features}/text_to_speech/usr/local/hobart/features/text_to_speech

$cpcmd -p ./3RDPartySoftware/espeak/espeakhf.tar.gz ${features}/text_to_speech/usr/local/hobart/features/text_to_speech/.
$cpcmd -p ./3RDPartySoftware/espeak/install.py ${features}/text_to_speech/usr/local/hobart/features/text_to_speech/.

# PDF viewer install into release tree
rm -rf ${features}/pdf_viewer/usr/local/hobart/features/pdf_viewer
mkdir -p ${features}/pdf_viewer/usr/local/hobart/features/pdf_viewer
tar xvzf ./3RDPartySoftware/ghostscript-fonts/ghostscript-fonts-std-8.11.tar.gz -C ${features}/pdf_viewer/usr/local/hobart/features/pdf_viewer

# Hobart Root CA feature
rm -rf ${features}/hobart_root_ca/usr/local/hobart/features/hobart_root_ca
mkdir -p ${features}/hobart_root_ca/usr/local/hobart/features/hobart_root_ca

$cpcmd -p ./Application/certs/feature/install.py ${features}/hobart_root_ca/usr/local/hobart/features/hobart_root_ca/.
$cpcmd -p ./Application/certs/ssh/hobart_ssh.pub ${features}/hobart_root_ca/usr/local/hobart/features/hobart_root_ca/.
$cpcmd -p ./Application/certs/HobartRootCA.crt ${features}/hobart_root_ca/usr/local/hobart/features/hobart_root_ca/.

# Squish record/playback
rm -rf ${features}/record_playback/usr/local/hobart/features/record_playback
mkdir -p ${features}/record_playback/usr/local/hobart/features/record_playback

$cpcmd -p ./3RDPartySoftware/squish/runner/*.deb ${features}/record_playback/usr/local/hobart/features/record_playback/.
$cpcmd -p ./3RDPartySoftware/squish/exec/*.deb ${features}/record_playback/usr/local/hobart/features/record_playback/.
$cpcmd -p ./3RDPartySoftware/squish/install.py ${features}/record_playback/usr/local/hobart/features/record_playback/.

#Online Ordering Settings App
rm -rf ${features}/oo_settings/usr/local/hobart/features/oo_settings
mkdir -p ${features}/oo_settings/usr/local/hobart/features/oo_settings

$cpcmd -p ./3RDPartySoftware/oo_settings/oo_settings_webapp/*.deb ${features}/oo_settings/usr/local/hobart/features/oo_settings/.
$cpcmd -p ./3RDPartySoftware/oo_settings/oo_settings_webapp/feature/* ${features}/oo_settings/usr/local/hobart/features/oo_settings/.
$cpcmd -p ./3RDPartySoftware/oo_settings/oo_settings_webapp/scripts/*.py ${features}/oo_settings/usr/local/hobart/features/oo_settings/.

# Walmart OS overlay feature
rm -rf ${features}/walmart/usr/local/hobart/features/walmart
mkdir -p ${features}/walmart/usr/local/hobart/features/walmart

$cpcmd -p ./3RDPartySoftware/walmart/* ${features}/walmart/usr/local/hobart/features/walmart/.

# Walmart FPP Lite app
rm -rf ${features}/walmart_fpplite/usr/local/hobart/features/walmart_fpplite
mkdir -p ${features}/walmart_fpplite/usr/local/hobart/features/walmart_fpplite

$cpcmd -p ./3RDPartySoftware/walmart/fpplitewebapp/*.deb ${features}/walmart_fpplite/usr/local/hobart/features/walmart_fpplite/.
$cpcmd -p ./3RDPartySoftware/walmart/fpplitewebapp/feature/* ${features}/walmart_fpplite/usr/local/hobart/features/walmart_fpplite/.

# Walmart Application Hook
rm -rf ${features}/walmart_apphook/usr/local/hobart/features/walmart_apphook
mkdir -p ${features}/walmart_apphook/usr/local/hobart/features/walmart_apphook

$cpcmd -p ./3RDPartySoftware/clio/*.deb ${features}/walmart_apphook/usr/local/hobart/features/walmart_apphook/.
$cpcmd -p ./3RDPartySoftware/walmart/app_hook/*.deb ${features}/walmart_apphook/usr/local/hobart/features/walmart_apphook/.
$cpcmd -p ./3RDPartySoftware/walmart/app_hook/feature/* ${features}/walmart_apphook/usr/local/hobart/features/walmart_apphook/.
$cpcmd -p ./3RDPartySoftware/walmart/offlineerrordialog/*.deb ${features}/walmart_apphook/usr/local/hobart/features/walmart_apphook/.
$cpcmd -p ./3RDPartySoftware/walmart/wmsmart/*.deb ${features}/walmart_apphook/usr/local/hobart/features/walmart_apphook/.

# Walmart Application Hook V2 (Phase 2+3 Deliverables)
rm -rf ${features}/walmart_apphook_v2/usr/local/hobart/features/walmart_apphook_v2
mkdir -p ${features}/walmart_apphook_v2/usr/local/hobart/features/walmart_apphook_v2

$cpcmd -p ./3RDPartySoftware/clio/*.deb ${features}/walmart_apphook_v2/usr/local/hobart/features/walmart_apphook_v2/.
$cpcmd -p ./3RDPartySoftware/walmart/app_hook_v2/*.deb ${features}/walmart_apphook_v2/usr/local/hobart/features/walmart_apphook_v2/.
$cpcmd -p ./3RDPartySoftware/walmart/app_hook_v2/feature/* ${features}/walmart_apphook_v2/usr/local/hobart/features/walmart_apphook_v2/.
$cpcmd -p ./3RDPartySoftware/walmart/offlineerrordialog/*.deb ${features}/walmart_apphook_v2/usr/local/hobart/features/walmart_apphook_v2/.
$cpcmd -p ./3RDPartySoftware/walmart/wmsmart/*.deb ${features}/walmart_apphook_v2/usr/local/hobart/features/walmart_apphook_v2/.

# HTe-Enterprise Application Hook V2
rm -rf ${features}/hte_apphook_v2/usr/local/hobart/features/hte_apphook_v2
mkdir -p ${features}/hte_apphook_v2/usr/local/hobart/features/hte_apphook_v2

$cpcmd -p ./3RDPartySoftware/clio/*.deb ${features}/hte_apphook_v2/usr/local/hobart/features/hte_apphook_v2/.
$cpcmd -p ./3RDPartySoftware/HTe-Enterprise/app_hook_v2/*.deb ${features}/hte_apphook_v2/usr/local/hobart/features/hte_apphook_v2/.
$cpcmd -p ./3RDPartySoftware/HTe-Enterprise/app_hook_v2/feature/* ${features}/hte_apphook_v2/usr/local/hobart/features/hte_apphook_v2/.

# Tiliter Application Hook
rm -rf ${features}/tiliter_apphook/usr/local/hobart/features/tiliter_apphook
mkdir -p ${features}/tiliter_apphook/usr/local/hobart/features/tiliter_apphook

$cpcmd -p ./3RDPartySoftware/clio/*.deb ${features}/tiliter_apphook/usr/local/hobart/features/tiliter_apphook/.
$cpcmd -p ./3RDPartySoftware/tiliter/tiliter_hook/*.deb ${features}/tiliter_apphook/usr/local/hobart/features/tiliter_apphook/.
$cpcmd -p ./3RDPartySoftware/tiliter/tiliter_hook/feature/* ${features}/tiliter_apphook/usr/local/hobart/features/tiliter_apphook/.

# Edgify Application Hook and Training App
rm -rf ${features}/edgify_apphook/usr/local/hobart/features/edgify_apphook
mkdir -p ${features}/edgify_apphook/usr/local/hobart/features/edgify_apphook

$cpcmd -p ./3RDPartySoftware/clio/*.deb ${features}/edgify_apphook/usr/local/hobart/features/edgify_apphook/.
$cpcmd -p ./3RDPartySoftware/edgify/edgify_hook/*.deb ${features}/edgify_apphook/usr/local/hobart/features/edgify_apphook/.
$cpcmd -p ./3RDPartySoftware/edgify/edgify_hook/feature/* ${features}/edgify_apphook/usr/local/hobart/features/edgify_apphook/.
$cpcmd -p ./3RDPartySoftware/edgify/edgifytrainer/*.deb ${features}/edgify_apphook/usr/local/hobart/features/edgify_apphook/.

# RDP feature
rm -rf ${features}/rdp/usr/local/hobart/features/rdp
mkdir -p ${features}/rdp/usr/local/hobart/features/rdp

$cpcmd -p ./3RDPartySoftware/xrdp/*.deb ${features}/rdp/usr/local/hobart/features/rdp/.
$cpcmd -p ./3RDPartySoftware/xrdp/install.py ${features}/rdp/usr/local/hobart/features/rdp/.

# BlueZ
rm -rf ${features}/bluez/usr/local/hobart/features/bluez
mkdir -p ${features}/bluez/usr/local/hobart/features/bluez

$cpcmd -p ./3RDPartySoftware/bluez/*.deb ${features}/bluez/usr/local/hobart/features/bluez/.
$cpcmd -p ./3RDPartySoftware/bluez/install.py ${features}/bluez/usr/local/hobart/features/bluez/.

# Mosquitto MQTT Broker
rm -rf ${features}/mqtt/usr/local/hobart/features/mqtt
mkdir -p ${features}/mqtt/usr/local/hobart/features/mqtt

$cpcmd -p ./3RDPartySoftware/mosquitto/*.deb ${features}/mqtt/usr/local/hobart/features/mqtt/.
$cpcmd -p ./3RDPartySoftware/mosquitto/install.py ${features}/mqtt/usr/local/hobart/features/mqtt/.

# WPA Supplicant
rm -rf ${features}/wpasupplicant/usr/local/hobart/features/wpasupplicant
mkdir -p ${features}/wpasupplicant/usr/local/hobart/features/wpasupplicant

$cpcmd -p ./3RDPartySoftware/wpa_supplicant/*.deb ${features}/wpasupplicant/usr/local/hobart/features/wpasupplicant/.
$cpcmd -p ./3RDPartySoftware/wpa_supplicant/install.py ${features}/wpasupplicant/usr/local/hobart/features/wpasupplicant/.

# Telit Device Wise
rm -rf ${features}/telit/usr/local/hobart/features/telit
mkdir -p ${features}/telit/usr/local/hobart/features/telit

$cpcmd -p ./3RDPartySoftware/telit/*.deb ${features}/telit/usr/local/hobart/features/telit/.
$cpcmd -p ./3RDPartySoftware/telit/install.py ${features}/telit/usr/local/hobart/features/telit/.

# iperf
rm -rf ${features}/iperf/usr/local/hobart/features/iperf
mkdir -p ${features}/iperf/usr/local/hobart/features/iperf

$cpcmd -p ./3RDPartySoftware/iperf/*.deb ${features}/iperf/usr/local/hobart/features/iperf/.
$cpcmd -p ./3RDPartySoftware/iperf/install.py ${features}/iperf/usr/local/hobart/features/iperf/.

# memtester
rm -rf ${features}/memtester/usr/local/hobart/features/memtester
mkdir -p ${features}/memtester/usr/local/hobart/features/memtester

$cpcmd -p ./3RDPartySoftware/memtester/*.deb ${features}/memtester/usr/local/hobart/features/memtester/.
$cpcmd -p ./3RDPartySoftware/memtester/install.py ${features}/memtester/usr/local/hobart/features/memtester/.

# Need memtester executable for ARK Boardsite - phase 0
$cpcmd -p ./3RDPartySoftware/memtester/memtester ${target}${liveRfs}/usr/bin/.
$cpcmd -p ./3RDPartySoftware/memtester/memtester ${target}${application}/usr/bin/.

# glmark2
rm -rf ${features}/glmark/usr/local/hobart/features/glmark
mkdir -p ${features}/glmark/usr/local/hobart/features/glmark

$cpcmd -p ./3RDPartySoftware/glmark2/*.deb ${features}/glmark/usr/local/hobart/features/glmark/.
$cpcmd -p ./3RDPartySoftware/glmark2/install.py ${features}/glmark/usr/local/hobart/features/glmark/.

# devregs
rm -rf ${features}/devregs/usr/local/hobart/features/devregs
mkdir -p ${features}/devregs/usr/local/hobart/features/devregs

$cpcmd -p ./3RDPartySoftware/devregs/*.deb ${features}/devregs/usr/local/hobart/features/devregs/.
$cpcmd -p ./3RDPartySoftware/devregs/install.py ${features}/devregs/usr/local/hobart/features/devregs/.

# Evolis printer drivers
rm -rf ${features}/evolis/usr/local/hobart/features/evolis
mkdir -p ${features}/evolis/usr/local/hobart/features/evolis

$cpcmd -p ./3RDPartySoftware/evolis/*.deb ${features}/evolis/usr/local/hobart/features/evolis/.
$cpcmd -p ./3RDPartySoftware/evolis/install.py ${features}/evolis/usr/local/hobart/features/evolis/.

# Bixolon printer drivers
rm -rf ${features}/bixolon/usr/local/hobart/features/bixolon
mkdir -p ${features}/bixolon/usr/local/hobart/features/bixolon

$cpcmd -p ./3RDPartySoftware/bixolon/*.tgz ${features}/bixolon/usr/local/hobart/features/bixolon/.
$cpcmd -p ./3RDPartySoftware/bixolon/install.py ${features}/bixolon/usr/local/hobart/features/bixolon/.

# HBTEMP app
rm -rf ${features}/hbtemp/usr/local/hobart/features/hbtemp
mkdir -p ${features}/hbtemp/usr/local/hobart/features/hbtemp

$cpcmd -p ./3RDPartySoftware/bluenica/*.deb ${features}/hbtemp/usr/local/hobart/features/hbtemp/.
$cpcmd -p ./3RDPartySoftware/bluenica/feature/* ${features}/hbtemp/usr/local/hobart/features/hbtemp/.

# Advertisement service app
rm -rf ${features}/adservice/usr/local/hobart/features/adservice
mkdir -p ${features}/adservice/usr/local/hobart/features/adservice

$cpcmd -p ./3RDPartySoftware/walmart/adservice/feature/*.tgz ${features}/adservice/usr/local/hobart/features/adservice/.
$cpcmd -p ./3RDPartySoftware/walmart/adservice/feature/install.py ${features}/adservice/usr/local/hobart/features/adservice/.

# FBVNC app
rm -rf ${features}/fbvnc/usr/local/hobart/features/fbvnc
mkdir -p ${features}/fbvnc/usr/local/hobart/features/fbvnc

$cpcmd -p ./3RDPartySoftware/fbvnc/*.deb ${features}/fbvnc/usr/local/hobart/features/fbvnc/.
$cpcmd -p ./3RDPartySoftware/fbvnc/install.py ${features}/fbvnc/usr/local/hobart/features/fbvnc/.

# PDF Viewer
mkdir -p ${features}/pdf_viewer/usr/local/hobart/features/pdf_viewer

# Service Key feature
rm -rf ${features}/servicekey/usr/local/hobart/features/servicekey
mkdir -p ${features}/servicekey/usr/local/hobart/features/servicekey

# SD Migration feature
rm -rf ${features}/sd_migration/usr/local/hobart/features/sd_migration
mkdir -p ${features}/sd_migration/usr/local/hobart/features/sd_migration

# Online Ordering feature
rm -rf ${features}/online_ordering/usr/local/hobart/features/online_ordering
mkdir -p ${features}/online_ordering/usr/local/hobart/features/online_ordering

$cpcmd -p ./3RDPartySoftware/online_ordering/web2py-scgihandler ${features}/online_ordering/usr/local/hobart/features/online_ordering/.
$cpcmd -p ./3RDPartySoftware/online_ordering/hydra-booda ${features}/online_ordering/usr/local/hobart/features/online_ordering/.
$cpcmd -p ./3RDPartySoftware/online_ordering/install.py ${features}/online_ordering/usr/local/hobart/features/online_ordering/.
$cpcmd -p ./3RDPartySoftware/online_ordering/config_services.sh ${features}/online_ordering/usr/local/hobart/features/online_ordering/.
$cpcmd -p ./3RDPartySoftware/online_ordering/db_schema_init.sql ${features}/online_ordering/usr/local/hobart/features/online_ordering/.
$cpcmd -p ./3RDPartySoftware/online_ordering/alert_overdue_orders.py ${features}/online_ordering/usr/local/hobart/features/online_ordering/.
$cpcmd -p ./3RDPartySoftware/online_ordering/migrate_db.py ${features}/online_ordering/usr/local/hobart/features/online_ordering/.
$cpcmd -p ./3RDPartySoftware/online_ordering/purge_old_orders.py ${features}/online_ordering/usr/local/hobart/features/online_ordering/.
$cpcmd -par ./3RDPartySoftware/online_ordering/web2py ${features}/online_ordering/usr/local/hobart/features/online_ordering/.

# copy online_ordering background image
$cpcmd -p ./3RDPartySoftware/online_ordering/images/online_ordering_desktop.jpg ${target}${liveRfs}/usr/share/lxde/wallpapers/online_ordering_desktop.jpg
$cpcmd -p ./3RDPartySoftware/online_ordering/images/online_ordering_desktop.jpg ${target}${application}/usr/share/lxde/wallpapers/online_ordering_desktop.jpg

echo "Installing python packages..."
tar xzvf ./3RDPartySoftware/python/twisted/zope.interface-*.linux-armv7l.tar.gz -C ${target}${liveRfs}
tar xzvf ./3RDPartySoftware/python/twisted/zope.interface-*.linux-armv7l.tar.gz -C ${target}${application}

tar xzvf ./3RDPartySoftware/python/twisted/Twisted-*.linux-armv7l.tar.gz -C ${target}${liveRfs}
tar xzvf ./3RDPartySoftware/python/twisted/Twisted-*.linux-armv7l.tar.gz -C ${target}${application}

fakeroot pip3 install --ignore-installed --force-reinstall --no-compile --target ${target}${liveRfs}/usr/lib/python3.6/site-packages/ --no-deps --no-index -r ./3RDPartySoftware/python/python3/requirements-python3.txt --find-links=./3RDPartySoftware/python/python3/
fakeroot pip3 install --ignore-installed --force-reinstall --no-compile --target ${target}${application}/usr/lib/python3.6/site-packages/ --no-deps --no-index -r ./3RDPartySoftware/python/python3/requirements-python3.txt --find-links=./3RDPartySoftware/python/python3/

fakeroot pip3 install --ignore-installed --force-reinstall --no-compile --root ${target}${liveRfs}/ --no-deps --no-index pip setuptools --find-links=./3RDPartySoftware/python/python3/
fakeroot pip3 install --ignore-installed --force-reinstall --no-compile --root ${target}${application}/ --no-deps --no-index pip setuptools --find-links=./3RDPartySoftware/python/python3/

# PyCrypto doesn't get put together 100% correctly by timesys
# we need to rename the .so binaries to match the target arch properly
rename "s/\.cpython-36m-x86_64-linux-gnu\.so$/\.so/" ${target}${liveRfs}/usr/lib/python3.6/site-packages/Crypto/Hash/*.so
rename "s/\.cpython-36m-x86_64-linux-gnu\.so$/\.so/" ${target}${application}/usr/lib/python3.6/site-packages/Crypto/Hash/*.so
rename "s/\.cpython-36m-x86_64-linux-gnu\.so$/\.so/" ${target}${liveRfs}/usr/lib/python3.6/site-packages/Crypto/Cipher/*.so
rename "s/\.cpython-36m-x86_64-linux-gnu\.so$/\.so/" ${target}${application}/usr/lib/python3.6/site-packages/Crypto/Cipher/*.so
rename "s/\.cpython-36m-x86_64-linux-gnu\.so$/\.so/" ${target}${liveRfs}/usr/lib/python3.6/site-packages/Crypto/Util/*.so
rename "s/\.cpython-36m-x86_64-linux-gnu\.so$/\.so/" ${target}${application}/usr/lib/python3.6/site-packages/Crypto/Util/*.so
rename "s/\.cpython-36m-x86_64-linux-gnu\.so$/\.so/" ${target}${liveRfs}/usr/lib/python3.6/site-packages/Crypto/PublicKey/*.so
rename "s/\.cpython-36m-x86_64-linux-gnu\.so$/\.so/" ${target}${application}/usr/lib/python3.6/site-packages/Crypto/PublicKey/*.so

# bluetooth lib also doesn't build correctly
rename "s/\.cpython-36m-x86_64-linux-gnu\.so$/\.so/" ${target}${application}/usr/lib/python3.6/site-packages/bluetooth/*.so

$cpcmd -p ./3RDPartySoftware/servicekey/install.py ${features}/servicekey/usr/local/hobart/features/servicekey/.

$cpcmd -p ./Application/shared/util/SD_Migration_Feature/Modules/* ${features}/sd_migration/usr/local/hobart/features/sd_migration/.


if [ "$newscalesettings" = "y" ]; then
    echo "Copying settings wizard config to target"
    $shcmd "rm -rf ${liveRfs}/usr/local/hobart/db/settingswizard"
    $shcmd "rm -rf ${application}/usr/local/hobart/db/settingswizard"
    $shcmd "mkdir -p ${liveRfs}/usr/local/hobart/db/settingswizard"
    $shcmd "mkdir -p ${application}/usr/local/hobart/db/settingswizard"
    $shcmd "mkdir -p ${liveRfs}/usr/local/hobart/db/settingswizard/CLIENTCERTSTORE"
    $shcmd "mkdir -p ${application}/usr/local/hobart/db/settingswizard/TRUSTEDROOTSTORE"
    $cpcmd -p ./Application/shared/sessionHandler/db/scalesettings ${target}${liveRfs}/usr/local/hobart/db/settingswizard/.
    $cpcmd -p ./Application/shared/sessionHandler/db/scalesettings ${target}${application}/usr/local/hobart/db/settingswizard/.
    $cpcmd -p ./Application/shared/sessionHandler/db/scalesettingsBB ${target}${liveRfs}/usr/local/hobart/db/settingswizard/.
    $cpcmd -p ./Application/shared/sessionHandler/db/scalesettingsBB ${target}${application}/usr/local/hobart/db/settingswizard/.
    $cpcmd -p ./Application/shared/sessionHandler/db/offlineCount ${target}${liveRfs}/usr/local/hobart/db/settingswizard/.
    $cpcmd -p ./Application/shared/sessionHandler/db/offlineCount ${target}${application}/usr/local/hobart/db/settingswizard/.
    $cpcmd -p ./Application/shared/sessionHandler/db/offlineDivision ${target}${liveRfs}/usr/local/hobart/db/settingswizard/.
    $cpcmd -p ./Application/shared/sessionHandler/db/offlineDivision ${target}${application}/usr/local/hobart/db/settingswizard/.
    
fi

$shcmd "chmod ugo+rx ${liveRfs}/usr/local/hobart/python/getGcdStatus.sh"

$shcmd "chmod ugo+rx ${liveRfs}/usr/local/hobart/python/dropCaches.sh"

# New (8/13/2014) for installing python packages before startupSequence.py is executed

$shcmd "chmod ugo+rx ${liveRfs}/usr/local/hobart/python/installPackages.sh"

# $shcmd "chmod ugo+rx ${liveRfs}/usr/local/hobart/python/installSetupTools.sh"

$shcmd "chmod ugo+rx ${liveRfs}/usr/local/hobart/python/verifyFeatureInstaller.sh"

# End of New (8/13/2014)

echo "Fixing permission on DBUS device query support"
$shcmd "chmod ugo+rx ${liveRfs}/usr/lib/dbus-daemon-launch-helper"
$shcmd "chmod u+s ${liveRfs}/usr/lib/dbus-daemon-launch-helper"

$shcmd "chmod ugo+rx ${recoveryRfs}/usr/lib/dbus-daemon-launch-helper"
$shcmd "chmod u+s ${recoveryRfs}/usr/lib/dbus-daemon-launch-helper"

# Fix Qt network manager going belly up when network disconnected then reconnected
# https://lists.qt-project.org/pipermail/interest/2015-December/020028.html
# https://bugreports.qt.io/browse/QTBUG-49760
echo "Fixing Qt Network Manager plugin"
$shcmd "chmod 0 ${liveRfs}/usr/lib/qt/plugins/bearer/libqnmbearer.so"

# python supplemental packages
# echo "Copying easy_install setuptools"
# $cpcmd -p ../ScaleManagement/ez_setup/setuptools-0.6c11-py2.7.egg ${target}${liveRfs}/usr/local/hobart/python/.
# $cpcmd -p ../ScaleManagement/ez_setup/setuptools-0.6c11-py2.7.egg ${target}${application}/usr/local/hobart/python/.
## python not required for recovery partition
## $cpcmd -p ../ScaleManagement/ez_setup/setuptools-0.6c11-py2.7.egg ${target}${recoveryRfs}/usr/local/hobart/python/.

# Need Scale Management backend on all scales
if [ -d ../ScaleManagement ]; then

    echo "Copying Scale Management backend to target: backend directory"
    python installSmBackend.py ${workingDirectory}/../ ${target}${liveRfs} ${target}${application}

    echo "Converting all CRLF's to Unix LF's"
    dos2unix ${target}${liveRfs}/usr/local/hobart/backend/*.py
    dos2unix ${target}${liveRfs}/usr/local/hobart/backend/*.sh
    dos2unix ${target}${application}/usr/local/hobart/backend/*.py
    dos2unix ${target}${application}/usr/local/hobart/backend/*.sh
fi

#echo "Compiling all python scripts on target"
# Can not compile on engdev1 since python version is 2.6.5, scale is 2.7.3
# which will cause mismatch.  must compile on scale.
#$shcmd "cd ${liveRfs}/usr/local/hobart/python; rm -f *.pyc; python3 -m compileall /usr/local/hobart/python"

echo "Copying licensing scripts to target"
$cpcmd ./Application/shared/util/dongleKey/keyInfo.py ${target}${liveRfs}/usr/local/hobart/python/.
$cpcmd ./Application/shared/util/dongleKey/keyInfo.py ${target}${application}/usr/local/hobart/python/.
## python not required for recovery partition
## $cpcmd ./Application/shared/util/dongleKey/keyInfo.py ${target}${recoveryRfs}/usr/local/hobart/python/.

$cpcmd ./Application/shared/util/dongleKey/verifyDongleKey.py ${target}${liveRfs}/usr/local/hobart/python/.
$cpcmd ./Application/shared/util/dongleKey/verifyDongleKey.py ${target}${application}/usr/local/hobart/python/.
## python not required for recovery partition
## $cpcmd ./Application/shared/util/dongleKey/verifyDongleKey.py ${target}${recoveryRfs}/usr/local/hobart/python/.

$cpcmd ./Application/shared/util/dongleKey/crypto.py ${target}${liveRfs}/usr/local/hobart/python/.
$cpcmd ./Application/shared/util/dongleKey/crypto.py ${target}${application}/usr/local/hobart/python/.
## python not required for recovery partition
## $cpcmd ./Application/shared/util/dongleKey/crypto.py ${target}${recoveryRfs}/usr/local/hobart/python/.

$cpcmd ./Application/shared/util/dongleKey/usbQuery.py ${target}${liveRfs}/usr/local/hobart/python/.
$cpcmd ./Application/shared/util/dongleKey/usbQuery.py ${target}${application}/usr/local/hobart/python/.
## python not required for recovery partition
## $cpcmd ./Application/shared/util/dongleKey/usbQuery.py ${target}${recoveryRfs}/usr/local/hobart/python/.

$cpcmd ./Application/shared/util/licensing/license.py ${target}${liveRfs}/usr/local/hobart/python/.
$cpcmd ./Application/shared/util/licensing/license.py ${target}${application}/usr/local/hobart/python/.
## python not required for recovery partition
## $cpcmd ./Application/shared/util/licensing/license.py ${target}${recoveryRfs}/usr/local/hobart/python/.

$cpcmd ./Application/shared/util/licensing/csid.py ${target}${liveRfs}/usr/local/hobart/python/.
$cpcmd ./Application/shared/util/licensing/csid.py ${target}${application}/usr/local/hobart/python/.
## python not required for recovery partition
## $cpcmd ./Application/shared/util/licensing/csid.py ${target}${recoveryRfs}/usr/local/hobart/python/.

$cpcmd ./Application/shared/util/licensing/volumeInfo.py ${target}${liveRfs}/usr/local/hobart/python/.
$cpcmd ./Application/shared/util/licensing/volumeInfo.py ${target}${application}/usr/local/hobart/python/.
## python not required for recovery partition
## $cpcmd ./Application/shared/util/licensing/volumeInfo.py ${target}${recoveryRfs}/usr/local/hobart/python/.


if [ "$targettype" = "release" ]; then

    # this function will checksum, sign, and bundle everything into a package
    function create_package {

        tgz=${1}.tgz
        md5=${1}.md5
        keepCore=${2}
    addBase=${3}

        # generate MD5 checksum
        cd ${workingdirectory}/..
        python ${workingdirectory}/Application/shared/sessionHandler/python/md5checksum.py $tgz > $md5

        # sign feature installer
        cd ${workingdirectory}/Application/certs
        ./signFeatureInstaller.sh ${workingdirectory}/../$tgz

        # create bundle
        cd ${workingdirectory}/..
        if [ "$addBase" = "y" ]; then
            echo ${basetarball} > ${baseVerFile}
            tar --create --verbose --gzip --owner=0 --group=0 --file ${1}.package.tgz ${md5} ${tgz} ${baseVerFile} ${tgz}.crt ${tgz}.signed
            rm ${baseVerFile}
        else
            tar --create --verbose --gzip --owner=0 --group=0 --file ${1}.package.tgz ${md5} ${tgz} ${tgz}.crt ${tgz}.signed
    fi

        # remove files
        if [ "$keepCore" = "y" ]; then
            rm ${tgz}.crt ${tgz}.signed
        else
            rm ${md5} ${tgz} ${tgz}.crt ${tgz}.signed
        fi
    }

    # this function will wait for jobs to complete
    function wait_for_jobs {
        FAIL=0

        for job in `jobs -p`; do
            wait $job || let "FAIL+=1"
        done

        if [ "$FAIL" != "0" ]; then
            echo "WARNING: One or more jobs failed!"
        fi
    }

    cd ${workingdirectory}/..

    # -----------------
    # hti-linux-feature
    # -----------------
    
    package_names=()

    htfeatures=("hobart_root_ca" "office_viewer" "pdf_viewer" "java_jre" "text_to_speech" "record_playback" "walmart" "walmart_fpplite" "oo_settings" "walmart_apphook" "walmart_apphook_v2" "hte_apphook_v2" "tiliter_apphook" "edgify_apphook" "rdp" "hbtemp" "fbvnc" "servicekey" "mqtt" "evolis" "bixolon" "wpasupplicant" "telit" "iperf" "memtester" "glmark" "devregs" "sd_migration" "hfpd" "bluez" "adservice" "online_ordering")

    for i in "${htfeatures[@]}"; do
        echo "Building feature ${i}..."

        package=hti-linux-feature-${i}-${releasetag}
        tar --create --verbose --gzip --owner=0 --group=0 --file ${package}.tgz -C ${features}/${i} . &
        package_names+=(${package})
    done

    wait_for_jobs

    for i in "${package_names[@]}"; do
        create_package ${i} n n
    done

    # ---------------------------
    # first boot pre-installation
    # ---------------------------

    # create folder for pre-installation features
    $shcmd "mkdir ${liveRfs}/usr/local/hobart/preinstall"

    # extract features to folder
    #
    # NOTE NOTE NOTE NOTE NOTE
    # if you add anything to this list be sure to update startupSequence.py and add the feature... search for "preinstall_features = []"...
    tar xzvf hti-linux-feature-office_viewer-${releasetag}.package.tgz -C ${target}${liveRfs}/usr/local/hobart/preinstall &
    tar xzvf hti-linux-feature-pdf_viewer-${releasetag}.package.tgz -C ${target}${liveRfs}/usr/local/hobart/preinstall &
    tar xzvf hti-linux-feature-fbvnc-${releasetag}.package.tgz -C ${target}${liveRfs}/usr/local/hobart/preinstall &
    tar xzvf hti-linux-feature-wpasupplicant-${releasetag}.package.tgz -C ${target}${liveRfs}/usr/local/hobart/preinstall &
    tar xzvf hti-linux-feature-bluez-${releasetag}.package.tgz -C ${target}${liveRfs}/usr/local/hobart/preinstall &

    wait_for_jobs

    # Customer specific feature
    #if [ ! -z "${customerSpecificFeature}" ]; then
    #    tar xzvf ${customerSpecificFeature} -C ${target}${liveRfs}/usr/local/hobart/preinstall
    #fi

    rm ${target}${liveRfs}/usr/local/hobart/preinstall/*.tgz.crt
    rm ${target}${liveRfs}/usr/local/hobart/preinstall/*.tgz.signed

    mkdir ${target}${application}/usr/local/hobart/preinstall/
    #add preinstall to the application upgrade
    if [ "$newPreinstall" = "y" ]; then
        $cpcmd -r ${target}${liveRfs}/usr/local/hobart/preinstall/ ${target}${application}/usr/local/hobart/    
    fi
    
    # copy slicer background image
    $cpcmd -p ${workingdirectory}/Application/customerInterface/slicer/slicer_wallpaper.jpg ${target}${liveRfs}/usr/local/hobart/preinstall/slicer_wallpaper.jpg
    $cpcmd -p ${workingdirectory}/Application/customerInterface/slicer/slicer_wallpaper.jpg ${target}${application}/usr/local/hobart/preinstall/slicer_wallpaper.jpg

    # copy tdcc background image
    $cpcmd -p ${workingdirectory}/Application/shared/util/tdcc/tdcc_desktop.jpg ${target}${liveRfs}/usr/share/lxde/wallpapers/tdcc_desktop.jpg
    $cpcmd -p ${workingdirectory}/Application/shared/util/tdcc/tdcc_desktop.jpg ${target}${application}/usr/share/lxde/wallpapers/tdcc_desktop.jpg

    # copy corona psu background image
    $cpcmd -p ${workingdirectory}/Application/shared/util/coronaPsu/images/corona_psu_desktop.jpg ${target}${liveRfs}/usr/share/lxde/wallpapers/corona_psu_desktop.jpg
    $cpcmd -p ${workingdirectory}/Application/shared/util/coronaPsu/images/corona_psu_desktop.jpg ${target}${application}/usr/share/lxde/wallpapers/corona_psu_desktop.jpg

	# copy freshserve wallpaper background image
    $cpcmd -p ${workingdirectory}/Application/shared/sessionHandlerHooks/images/selfServe/freshserve_wallpaper.jpg ${target}${liveRfs}/usr/share/lxde/wallpapers/freshserve_wallpaper.jpg
    $cpcmd -p ${workingdirectory}/Application/shared/sessionHandlerHooks/images/selfServe/freshserve_wallpaper.jpg ${target}${application}/usr/share/lxde/wallpapers/freshserve_wallpaper.jpg
    
    # copy Telit DeviceWISE
    $cpcmd -p ${workingdirectory}/3RDPartySoftware/telit/devicewise/dw.tar.gz ${target}${liveRfs}/usr/local/hobart/preinstall/.
    $cpcmd -p ${workingdirectory}/3RDPartySoftware/telit/devicewise/dw.tar.gz ${target}${application}/usr/local/hobart/preinstall/.
    
    # NOTE NOTE NOTE NOTE - This is for the Walmart TEMPLATE LABEL ONLY. When this is merged back to the trunk these lines need to be commented out.
    # copy Template Label
    # $cpcmd -p ${workingdirectory}/CustomInstallers/featureWalmartTemplateLabel/hti-linux-feature-wm_cake_labellic-3.3.101-04032019_103300.md5 ${target}${liveRfs}/usr/local/hobart/preinstall/.
    # $cpcmd -p ${workingdirectory}/CustomInstallers/featureWalmartTemplateLabel/hti-linux-feature-wm_cake_labellic-3.3.101-04032019_103300.tgz ${target}${liveRfs}/usr/local/hobart/preinstall/.

    # copy HFLB Device Programmer to preinstall
    # build runtime tarball
    python ${workingdirectory}/Application/shared/util/hflbProgrammer/buildRuntimeTarball.py ${release}/hflb_programmer_runtime.tar.gz
    # copy tarball to preinstall
    $cpcmd -p ${release}/hflb_programmer_runtime.tar.gz ${target}${liveRfs}/usr/local/hobart/preinstall/.
    $cpcmd -p ${release}/hflb_programmer_runtime.tar.gz ${target}${application}/usr/local/hobart/preinstall/.

    # copy corona PillarSync web server and modbus application to preinstall
    # build runtime tarball
    python ${workingdirectory}/Application/shared/util/coronaPsu/feature/buildTarball.py --runtime ${release}/pillar_sync_runtime.tar.gz
    # copy tarball to preinstall
    $cpcmd -p ${release}/pillar_sync_runtime.tar.gz ${target}${liveRfs}/usr/local/hobart/preinstall/.
    $cpcmd -p ${release}/pillar_sync_runtime.tar.gz ${target}${application}/usr/local/hobart/preinstall/.

    # copy python packages
    $cpcmd -p ${workingdirectory}/3RDPartySoftware/python/python3/* ${target}${liveRfs}/usr/local/hobart/preinstall/
    $cpcmd -p ${workingdirectory}/3RDPartySoftware/python/python3/* ${target}${application}/usr/local/hobart/preinstall/
    
    $cpcmd -p ${workingdirectory}/3RDPartySoftware/python/twisted/*linux-arm*gz ${target}${liveRfs}/usr/local/hobart/preinstall/
    $cpcmd -p ${workingdirectory}/3RDPartySoftware/python/twisted/*linux-arm*gz ${target}${application}/usr/local/hobart/preinstall/
    
    $cpcmd -p ${workingdirectory}/3RDPartySoftware/PyQt5/repos/hobart-hti-pyqt_5.5-1_armhf.deb ${target}${liveRfs}/usr/local/hobart/preinstall/hobart-hti-pyqt_5.5-1_armhf.deb
    $cpcmd -p ${workingdirectory}/3RDPartySoftware/PyQt5/repos/hobart-hti-pyqt_5.5-1_armhf.deb ${target}${application}/usr/local/hobart/preinstall/hobart-hti-pyqt_5.5-1_armhf.deb

    # -------------
    # core tarballs
    # -------------

    package_names=()

    echo "Building tarball for recovery and uSD creation outside of this script"
    package=hti-linux-usd-${releasetag}
    tar --create --verbose --gzip --owner=0 --group=0 --file ${package}.tgz -C ${rootFs} .

    # generate MD5 checksum
    python ${workingdirectory}/Application/shared/sessionHandler/python/md5checksum.py hti-linux-usd-${releasetag}.tgz > hti-linux-usd-${releasetag}.md5

    # NOTE:
    # The recovery tarfile below only contains changes from last majore OS release
    # This will not work if you move to another version of kernel and RFS since recovery partition will not be upgraded only the handfull
    # of files in this tarball
    echo "Building minimal tarball for minimal recovery creation outside of this script"
    package=hti-linux-recovery-${releasetag}
    tar --create --verbose --gzip --owner=0 --group=0 --file ${package}.tgz -C ${recoveryRfs} ./etc/init.d/rc.local ./etc/init.d/recovery.sh ./usr/bin/rsync ./boot ./lib/modules &
    package_names+=(${package})

    # NOTE:
    # This recovery tarball has the entire recovery partition OS and RFS files and is the one used in the uSD tarball to create entire
    # recovery partition from scratch
    echo "Building tarball for recovery creation outside of this script"
    package=hti-linux-recovery_core-${releasetag}
    tar --create --verbose --gzip --owner=0 --group=0 --file ${package}.tgz -C ${recoveryRfs} . &
    package_names+=(${package})

    echo "Building tarball for live application/OS upgrade release"

    # Remove videos from core.. they are installed already from the usd.  Futhermore, if they would
    # happen to need the vidoes (in any language) for some reason, they can take the feature.
    rm -rf ${rootFs}/hobart/usr/local/hobart/videos

    package=hti-linux-core-${releasetag}
    tar --create --verbose --gzip --owner=0 --group=0 --file ${package}.tgz -C ${rootFs}/hobart . &
    package_names+=(${package})

    echo "Building tarball for uboot release"
    package=hti-linux-uboot-${releasetag}
    tar --create --verbose --gzip --owner=0 --group=0 --file ${package}.tgz -C ${uboot} . &
    package_names+=(${package})

    wait_for_jobs

    for i in "${package_names[@]}"; do
        if [[ ${i} == "hti-linux-core-"* ]]; then
            create_package ${i} y n
        else
            create_package ${i} n n
        fi
    done

    # -------------------
    # application upgrade
    # -------------------
    cd ${workingdirectory}
    download="n"
    # check if basefile exsists and has a good md5 
    if [ -f "../$basetarball" ] && [ -f "../${basetarball%.*}.md5" ]; then
        echo "$basetarball file found. Verifying integrity..."
        md5=($(md5sum ../$basetarball))
        if [ "$md5" = "$(cat ../${basetarball%.*}.md5)" ]; then
            echo "$basetarball MD5 check passed"
        else
            echo "$basetarball MD5 check failed attempting to redownload the file"
            download="y"
        fi
        
    else
        download="y"
		echo "$basetarball or MD5 not found."
    fi
    
    if [ "$download" = "y" ]; then
        echo  "Attempting to pull from $applicationBaseServer"
        scp -p $applicationBaseServerUser@$applicationBaseServer:/$applicationBaseServerCoreDir/$basetarball ../$basetarball &
        scp -p $applicationBaseServerUser@$applicationBaseServer:/$applicationBaseServerCoreDir/${basetarball%.*}.md5 ../ &
        wait_for_jobs
        
        echo "$basetarball file downloaded. Verifying integrity..."
        md5=($(md5sum ../$basetarball))
        if [ "$md5" = "$(cat ../${basetarball%.*}.md5)" ]; then
            echo "$basetarball MD5 check passed"
        else
            echo "ERROR: $basetarball MD5 check failed "
            download="y"
        fi
    fi
    
    python ${workingdirectory}/CleanupApplicationDir.py -t ../${basetarball}
    cd ${workingdirectory}/..

    # Copying after cleanup because this file is used to verify upgrade compatibility 
    cp -f ${liveRfs}/SOFTWARE_VERSION_INFO ${target}${application}
    
    echo "Building tarball for application release"
    package=hti-linux-application-${releasetag}
    tar --create --verbose --gzip --owner=0 --group=0 --file ${package}.tgz -C ${application} .
    create_package ${package} n y

    # ------------
    # htseries-any
    # ------------

    package_names=()

    echo "Building tarball for FPC release"
    package=htseries-any-fpc-${releasetag}
    tar --create --verbose --gzip --owner=0 --group=0 --file ${package}.tgz -C ${tempDir} . &
    package_names+=(${package})

    echo "Building tarball for KPC bootloader"
    package=htseries-any-kpc-bootloader-${releasetag}
    tar --create --verbose --gzip --owner=0 --group=0 --file ${package}.tgz -C ${liveRfs}/lib/firmware/kpc/boot/ sourceKPCBoot.bin &
    package_names+=(${package})

    echo "Building tarball for FSSS bootloader"
    package=htseries-any-fsss-bootloader-${releasetag}
    tar --create --verbose --gzip --owner=0 --group=0 --file ${package}.tgz -C ${liveRfs}/lib/firmware/fsss/boot/ sourceFsssBoot.bin &
    package_names+=(${package})

    # build hfpd zip
    python ${workingdirectory}/buildHFPDFirmwareUpgrade.py --version "${hfpdFirmwareVer}" --source "${hfpdDir}/IARHfpd.hex" --destination "${hfpdDir}/IARHfpd-${hfpdFirmwareVer}.zip"

    echo "Building tarball for HFPD Firmware"
    package=htseries-any-hfpd-application-${releasetag}
    tar --create --verbose --gzip --owner=0 --group=0 --file ${package}.tgz -C ${hfpdDir} IARHfpd-${hfpdFirmwareVer}.zip &
    package_names+=(${package})

    training_videos=("training_videos_english" "training_videos_spanish" "training_videos_french")

    for i in "${training_videos[@]}"; do
        echo "Building tarball for ${i}..."
        package=htseries-any-${i}-${releasetag}
        tar --create --verbose --gzip --owner=0 --group=0 --file ${package}.tgz -C ${release}/${i} . &
        package_names+=(${package})
    done

    wait_for_jobs

    for i in "${package_names[@]}"; do
        create_package ${i} n n
    done

    # -------------------------------------
    # Send notification of build completion
    # -------------------------------------
    cd ${workingdirectory}/..
    dirName=`basename $(pwd)`
    python ${workingdirectory}/buildNotification.py ${releasetag} ${dirName}

fi
