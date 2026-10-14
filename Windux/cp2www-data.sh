#-------------------------------------------------
# Copies files from the HTe build directory to the
# engdev1 TWiki builds repository for FasTrack
#
# Parameters:
#
# Build #, in the format: 0.0.0-00000000_000000
#
#       For example:  0.0.31-08122013_182459
#-------------------------------------------------

if [ $# -lt 1 ] ; then
        echo -e "\nYou need to specify the build number.\n"
        exit 0
fi

#set -x

target_hti_linux_dir="/srv/www/engdev1.itwfeg.biz/builds/HTseries/HTi/linux"
target_hte_any_dir="/srv/www/engdev1.itwfeg.biz/builds/HTseries"
target_owner="www-data"
target_group="www-data"

target_server="engdev1"
target_username="buildmgr"

#----------------------------------------------------
# Assumes you are starting from the Windux directory,
# so cd up one level to the "build" directory.
#----------------------------------------------------
cd ..

# Check if the first file exists to determine if the build # is valid
if [ !  -f hti-linux-usd-$1.tgz ] ; then
        echo -e "\nFile does not exist: Please check the build number.\n"
        exit 0
fi

#------------------------------------------------------------------------------------
# If running from ENGDEV1, then we are just copying to another location on the server
#------------------------------------------------------------------------------------
if [ "$HOSTNAME" = "engdev1" ] || [ "$HOSTNAME" = "ENGDEV1" ] ; then
    #------------
    # Application
    #------------
    # cp --preserve=timestamps hti-linux-application-$1.* $target_hti_linux_dir/app/.

    # chown ${target_owner}:${target_group} $target_hti_linux_dir/app/hti-linux-application-$1.*

    #-----
    # Live
    #-----
    cp --preserve=timestamps hti-linux-core-$1.* $target_hti_linux_dir/live/.
    
    chown ${target_owner}:${target_group} $target_hti_linux_dir/live/hti-linux-core-$1.*
    
    # Temporarily keep the 'core.tgz' and 'core.md5' files.
    cp --preserve=timestamps hti-linux-core-$1.tgz $target_hti_linux_dir/live/.
    cp --preserve=timestamps hti-linux-core-$1.md5 $target_hti_linux_dir/live/.

    chown ${target_owner}:${target_group} $target_hti_linux_dir/live/hti-linux-core-$1.tgz
    chown ${target_owner}:${target_group} $target_hti_linux_dir/live/hti-linux-core-$1.md5

    #--------
    # Upgrade
    #--------
    # cp --preserve=timestamps hti-linux-upgrade-$1.* $target_hti_linux_dir/upgrades/.

    # chown ${target_owner}:${target_group} $target_hti_linux_dir/upgrades/hti-linux-upgrade-$1.*

    #-------
    # U-Boot
    #-------
    cp --preserve=timestamps hti-linux-uboot-$1.* $target_hti_linux_dir/uBoot/.

    chown ${target_owner}:${target_group} $target_hti_linux_dir/uBoot/hti-linux-uboot-$1.*

    #----
    # uSD
    #----
    cp --preserve=timestamps hti-linux-usd-$1.* $target_hti_linux_dir/uSd/.

    chown ${target_owner}:${target_group} $target_hti_linux_dir/uSd/hti-linux-usd-$1.*

    #---------
    # Recovery
    #---------
    cp --preserve=timestamps hti-linux-recovery-$1.* $target_hti_linux_dir/recovery/.

    chown ${target_owner}:${target_group} $target_hti_linux_dir/recovery/hti-linux-recovery-$1.*

    #------------
    # FPC upgrade
    #------------
    cp --preserve=timestamps htseries-any-fpc-$1.* $target_hte_any_dir/fpc/.

    chown ${target_owner}:${target_group} $target_hte_any_dir/fpc/htseries-any-fpc-$1.*

    #-----
    # Lang
    #-----
    cp --preserve=timestamps htseries-any-training_videos_*-$1.* $target_hte_any_dir/lang/.

    chown ${target_owner}:${target_group} $target_hte_any_dir/lang/htseries-any-training_videos_*-$1.*

    #---------
    # Features
    #---------
    cp --preserve=timestamps hti-linux-feature-*-$1.* $target_hti_linux_dir/features/.

    chown ${target_owner}:${target_group} $target_hti_linux_dir/features/hti-linux-feature-*-$1.*

fi


#------------------------------------------------------------------------
# If running from wwbuildserver, then we need to SCP the files to ENGDEV1
#------------------------------------------------------------------------
if [ "$HOSTNAME" = "wwbuildserver" ] || [ "$HOSTNAME" = "WWBUILDSERVER" ] ; then

    echo "Copying files to: " $target_server
    
    #------------
    # Application
    #------------
    scp -p hti-linux-application-$1.* $target_username@$target_server:/$target_hti_linux_dir/app/.

    #-----
    # Live
    #-----
    
    scp -p hti-linux-core-$1.* $target_username@$target_server:/$target_hti_linux_dir/live/.
    
    # Temporarily keep the 'core.tgz' and 'core.md5' files.
    scp -p hti-linux-core-$1.tgz $target_username@$target_server:/$target_hti_linux_dir/live/.
    scp -p hti-linux-core-$1.md5 $target_username@$target_server:/$target_hti_linux_dir/live/.
    
    #--------
    # Upgrade
    #--------
    # scp -p $target_username@$target_server:/$target_hti_linux_dir/upgrades/.

    #-------
    # U-Boot
    #-------
    scp -p hti-linux-uboot-$1.* $target_username@$target_server:/$target_hti_linux_dir/uBoot/.

    #----
    # uSD
    #----
    scp -p hti-linux-usd-$1.* $target_username@$target_server:/$target_hti_linux_dir/uSd/.

    #---------
    # Recovery
    #---------
    scp -p hti-linux-recovery-$1.* $target_username@$target_server:/$target_hti_linux_dir/recovery/.

    #------------
    # FPC upgrade
    #------------
    scp -p htseries-any-fpc-$1.* $target_username@$target_server:/$target_hte_any_dir/fpc/.
    scp -p htseries-any-kpc-bootloader-$1.* $target_username@$target_server:/$target_hte_any_dir/fpc/.

    #-------------
    # HFPD upgrade
    #-------------
    scp -p htseries-any-hfpd-application-$1.* $target_username@$target_server:/$target_hte_any_dir/fpc/.

    #-----
    # Lang
    #-----
    scp -p htseries-any-training_videos_*-$1.* $target_username@$target_server:/$target_hte_any_dir/lang/.

    #---------
    # Features
    #---------
    scp -p hti-linux-feature-*-$1.* $target_username@$target_server:/$target_hti_linux_dir/features/.

fi
