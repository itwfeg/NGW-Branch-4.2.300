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
        exit 1
fi

#set -x

target_hti_linux_dir="K:\weighwrap\engineering_builds\HTseries\HTi\linux"
target_hte_any_dir="K:\weighwrap\engineering_builds\HTseries"

#target_server="engdev1a.itwfeg.biz"
target_server="10.3.128.72"
target_username="buildmgr"
target_pass="build1MGR@"

#----------------------------------------------------
# Assumes you are starting from the Windux directory,
# so cd up one level to the "build" directory.
#----------------------------------------------------
cd ..

# Check if the first file exists to determine if the build # is valid
if [ !  -f hti-linux-usd-$1.tgz ] ; then
        echo -e "\nFile does not exist: Please check the build number.\n"
        exit 1
fi

#------------------------------------------------------------------------
# If running from wwbuildserver, then we need to SCP the files to ENGDEV1
#------------------------------------------------------------------------
if [ "$HOSTNAME" = "wwbuildserver" ] || [ "$HOSTNAME" = "WWBUILDSERVER" ] ; then

    echo -e "\nCopying files to: " "$target_server"
    echo -E "Target directory: " "$target_hti_linux_dir"
    echo -e "\n\c"
    
    #------------
    # Application
    #------------
    sshpass -p ""$target_pass"" scp -p hti-linux-application-$1.* $target_username@$target_server:/$target_hti_linux_dir/app/.

    #-----
    # Live
    #-----
    
    sshpass -p ""$target_pass"" scp -p hti-linux-core-$1.* $target_username@$target_server:/$target_hti_linux_dir/live/.
    
    # Temporarily keep the 'core.tgz' and 'core.md5' files.
    sshpass -p ""$target_pass"" scp -p hti-linux-core-$1.tgz $target_username@$target_server:/$target_hti_linux_dir/live/.
    sshpass -p ""$target_pass"" scp -p hti-linux-core-$1.md5 $target_username@$target_server:/$target_hti_linux_dir/live/.
    
    #--------
    # Upgrade
    #--------
    # scp -p $target_username@$target_server:/$target_hti_linux_dir/upgrades/.

    #-------
    # U-Boot
    #-------
    sshpass -p ""$target_pass"" scp -p hti-linux-uboot-$1.* $target_username@$target_server:/$target_hti_linux_dir/uBoot/.

    #----
    # uSD
    #----
    sshpass -p ""$target_pass"" scp -p hti-linux-usd-$1.* $target_username@$target_server:/$target_hti_linux_dir/uSd/.

    #---------
    # Recovery
    #---------
    sshpass -p ""$target_pass"" scp -p hti-linux-recovery-$1.* $target_username@$target_server:/$target_hti_linux_dir/recovery/.

    #------------
    # FPC upgrade
    #------------

    echo -e "\n\c"
    echo -E "Target directory: " "$target_hte_any_dir"
    echo -e "\n\c"

    sshpass -p ""$target_pass"" scp -p htseries-any-fpc-$1.* $target_username@$target_server:/$target_hte_any_dir/fpc/.
    sshpass -p ""$target_pass"" scp -p htseries-any-kpc-bootloader-$1.* $target_username@$target_server:/$target_hte_any_dir/fpc/.
    sshpass -p ""$target_pass"" scp -p htseries-any-fsss-bootloader-$1.* $target_username@$target_server:/$target_hte_any_dir/fpc/.

    #-------------
    # HFPD upgrade
    #-------------
    sshpass -p ""$target_pass"" scp -p htseries-any-hfpd-application-$1.* $target_username@$target_server:/$target_hte_any_dir/fpc/.

    #-----
    # Lang
    #-----
    sshpass -p ""$target_pass"" scp -p htseries-any-training_videos_*-$1.* $target_username@$target_server:/$target_hte_any_dir/lang/.

    #---------
    # Features
    #---------

    echo -e "\n\c"
    echo -E "Target directory: " "$target_hti_linux_dir"
    echo -e "\n\c"

    sshpass -p ""$target_pass"" scp -p hti-linux-feature-*-$1.* $target_username@$target_server:/$target_hti_linux_dir/features/.

fi
