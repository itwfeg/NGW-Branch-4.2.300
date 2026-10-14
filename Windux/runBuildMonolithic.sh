#---------------------------------------------------------------------------------------------------
#  Executes buildMonolithicCorePreInstallFeature.py and buildMonolithicCoreFeature.py under Linux.
#
#  This requires Python 2.7, which is not installed on ENGDEV1, but can be run on wwbuildserver.
# 
#  Parameters: Release tag in the format:  NN.NN.NN-MMDDYYYY_HHMMSS
#
#      For example:  runBuildMonolithic.sh  3.0.0-06132017_130607
#---------------------------------------------------------------------------------------------------

if [ $# -lt 1 ] ; then
        echo -e "\nYou need to specify the release tag.\n"
        exit 0
fi

reltag=$1
workingdirectory=`pwd`

export PYTHONPATH="$workingdirectory/../ScaleManagement:$workingdirectory/../ScaleManagement/smBackend:$workingdirectory/Application/shared/sessionHandler/python:$workingdirectory/Application/shared/util/dongleKey:$workingdirectory/Application/shared/util/licensing"

python buildMonolithicCorePreInstallFeature.py $reltag

python buildMonolithicCoreFeature.py $reltag \
	../hti-linux-core-$reltag.tgz \
	../hti-linux-recovery_core-$reltag.package.tgz \
	../hti-linux-uboot-$reltag.package.tgz

cp hti-linux-feature-monolithic_core_preinstall-$reltag.tgz ../hti-linux-feature-monolithic_core_preinstall-$reltag.tgz
cp hti-linux-feature-monolithic_core-$reltag.tgz ../hti-linux-feature-monolithic_core-$reltag.tgz

./create_signed_package.sh hti-linux-feature-monolithic_core_preinstall-$reltag
./create_signed_package.sh hti-linux-feature-monolithic_core-$reltag


# Clean up temp files used to build the monolithic
rm -f startupSequence.py rc.local recovery.sh
rm -f hte-feature_monolithic_core_preinstall-*.*
rm -f hti-linux-feature-monolithic_core_preinstall-$reltag.*
rm -f hti-linux-feature-monolithic_core-$reltag.*
