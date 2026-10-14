#!/bin/bash

#
# This script will generate an reduced upgrade file to be used.
# to update an HTseries device from the original major version 
# or later.
#
# Prerequisites:
# This script must be launched from the same dir that Windux and OS are in.

echo ""
echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
echo "STARTING REDUCED UPGRADE PACKAGE"
echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
echo ""

WINDUXREVFROM=${1}
WINDUXREVHEAD=${2}
OSREVFROM=${3}
OSREVHEAD=${4}
UPGRADEFILENAME=${5}
RELEASETAG=${6}

CWD=`pwd`

##################################

##    SET THESE VARIABLES BELOW

#Major windux revision num is the X.0.0 release's revision e.g., 3.0.0
majorWinduxRevisionNum=$WINDUXREVFROM

#Current windux revision num is the most current X.x.x e.g., 3.2.0
currentWinduxRevisionNum=$WINDUXREVHEAD

#Major os revision num is the X.0.0 release's revision e.g., 3.0.0 dated above
majorOsRevisionNum=OSREVFROM

#Current os revision num is the most current X.x.x e.g., 3.2.0 dated above
currentOsRevisionNum=OSREVHEAD

#which windux files are you interested in? 
sourceFilesOfInterest="Text*.txt|config_linux.dat|configdebug.dat|checkerboardLabel.bmp"
dongleKeyFilesOfInterest="keyInfo.py|verifyDongleKey.py|crypto.py|keyInfo.pyc|verifyDongleKey.pyc|crypto.pyc"
licensingFilesOfInterest="csid.py|volumeInfo.py|license.pyc|csid.py|volumeInfo.py"

newPythonFilename="newPythonFiles.txt"
newSoftkeysFilename="newSoftkeys.txt"
newDiagnosticsFilename="newDiagnostics.txt"
newFontsFilename="newFonts.txt"
newGraphicsFilename="newGraphics.txt"
newSourceFilename="newSource.txt"
newCategoriesFilename="newCategories.txt"
newProductsFilename="newProducts.txt"
newSpecialsFilename="newSpecialsFilename.txt"
newGCDLayoutsFilename="newGCDLayouts.txt"
newGCDLayouts1024Filename="newGCDLayouts1024.txt"
newGCDLayouts1366Filename="newGCDLayouts1366.txt"
newPlaylistsFilename="newPlaylists.txt"
newLicensingFilename="newLicensing.txt"
newDongleKeyFilename="newDongleKeys.txt"
# plus get /etc
# plus get /usr/bin/
# plus get /usr/sbin/
# plus get newOSFiles

##    END SETTING VARIABLES
####################################

workingdirectory=`pwd`

cd $workingdirectory

# Add files since the last major version that should be picked up by the tarball

echo "1 - Removing old application upgrade support files."
echo ""
rm ../$UPGRADEFILENAME &> /dev/null
echo "2 - Copying /usr/bin/ to the upgrade file."
echo ""
echo "3 - Copying /usr/sbin/ to the upgrade file."
echo ""
echo "4 - Copying /etc to the upgrade file."
echo ""
echo "5 - Finding new files in Repo."
echo ""

cd $workingdirectory/Windux

#clean & get all of etc /usr/bin and /usr/sbin
rm -rf ./upgradeRFS
mkdir ./upgradeRFS
mkdir ./upgradeRFS/usr
mkdir ./upgradeRFS/usr/bin
cp -r $workingdirectory/Release/rootfs/hobart/usr/bin/* ./upgradeRFS/usr/bin
mkdir ./upgradeRFS/usr/sbin
cp -r $workingdirectory/Release/rootfs/hobart/usr/sbin/* ./upgradeRFS/usr/bin
mkdir ./upgradeRFS/etc
cp -r $workingdirectory/Release/rootfs/hobart/usr/etc/* ./upgradeRFS/etc
mkdir ./upgradeRFS/usr/local/
mkdir ./upgradeRFS/usr/local/hobart

#### PYTHON
cd $workingdirectory/Windux/Application/shared/sessionHandler/python
svn diff -r $WINDUXREVFROM:$WINDUXREVHEAD --summarize > $workingdirectory/Windux/$newPythonFilename
cd $workingdirectory/Windux
sed -e "s/M       //g" -i $newPythonFilename
sed -e "s/ M      //g" -i $newPythonFilename
sed -e "s/A       //g" -i $newPythonFilename
sed -e "s/ A      //g" -i $newPythonFilename
sed -e "s/MM      //g" -i $newPythonFilename
sed --in-place '/D       /d' $newPythonFilename
sed --in-place '/ D      /d' $newPythonFilename

mkdir ./upgradeRFS/usr/local/hobart/python
cd $workingdirectory/Windux/Application/shared/sessionHandler/python
xargs -a $workingdirectory/Windux/$newPythonFilename cp -t $workingdirectory/Windux/upgradeRFS/usr/local/hobart/python

#### SOFTKEYS
cd $workingdirectory/Windux/Application/System/softkeys
svn diff -r $WINDUXREVFROM:$WINDUXREVHEAD --summarize > $workingdirectory/Windux/$newSoftkeysFilename
cd $workingdirectory/Windux
sed -e "s/M       //g" -i $newSoftkeysFilename
sed -e "s/ M      //g" -i $newSoftkeysFilename
sed -e "s/A       //g" -i $newSoftkeysFilename
sed -e "s/ A      //g" -i $newSoftkeysFilename
sed -e "s/MM      //g" -i $newSoftkeysFilename
sed --in-place '/D       /d' $newSoftkeysFilename
sed --in-place '/ D      /d' $newSoftkeysFilename

mkdir ./upgradeRFS/usr/local/hobart/images
mkdir ./upgradeRFS/usr/local/hobart/images/softkeys
cd $workingdirectory/Windux/Application/System/softkeys
xargs -a $workingdirectory/Windux/$newSoftkeysFilename cp -t $workingdirectory/Windux/upgradeRFS/usr/local/hobart/images/softkeys

#### DIAGNOSTICS
cd $workingdirectory/Windux/Application/shared/sessionHandler/images/diagnostics
svn diff -r $WINDUXREVFROM:$WINDUXREVHEAD --summarize > $workingdirectory/Windux/$newDiagnosticsFilename
cd $workingdirectory/Windux
sed -e "s/M       //g" -i $newDiagnosticsFilename
sed -e "s/ M      //g" -i $newDiagnosticsFilename
sed -e "s/A       //g" -i $newDiagnosticsFilename
sed -e "s/ A      //g" -i $newDiagnosticsFilename
sed -e "s/MM      //g" -i $newDiagnosticsFilename
sed --in-place '/D       /d' $newDiagnosticsFilename
sed --in-place '/ D      /d' $newDiagnosticsFilename

mkdir ./upgradeRFS/usr/local/hobart/images/system
cd $workingdirectory/Windux/Application/shared/sessionHandler/images/diagnostics
xargs -a $workingdirectory/Windux/$newDiagnosticsFilename cp -t $workingdirectory/Windux/upgradeRFS/usr/local/hobart/images/system

#### FONTS
cd $workingdirectory/Windux/Application/System/fonts
svn diff -r $WINDUXREVFROM:$WINDUXREVHEAD --summarize > $workingdirectory/Windux/$newFontsFilename
cd $workingdirectory/Windux
sed -e "s/M       //g" -i $newFontsFilename
sed -e "s/ M      //g" -i $newFontsFilename
sed -e "s/A       //g" -i $newFontsFilename
sed -e "s/ A      //g" -i $newFontsFilename
sed -e "s/MM      //g" -i $newFontsFilename
sed --in-place '/D       /d' $newFontsFilename
sed --in-place '/ D      /d' $newFontsFilename

mkdir ./upgradeRFS/usr/local/hobart/system
mkdir ./upgradeRFS/usr/local/hobart/system/fonts
cd $workingdirectory/Windux/Application/System/fonts
xargs -a $workingdirectory/Windux/$newFontsFilename cp -t $workingdirectory/Windux/upgradeRFS/usr/local/hobart/system/fonts

#### GRAPHICS
cd $workingdirectory/Windux/Application/System/graphics
svn diff -r $WINDUXREVFROM:$WINDUXREVHEAD --summarize > $workingdirectory/Windux/$newGraphicsFilename
cd $workingdirectory/Windux
sed -e "s/M       //g" -i $newGraphicsFilename
sed -e "s/ M      //g" -i $newGraphicsFilename
sed -e "s/A       //g" -i $newGraphicsFilename
sed -e "s/MM      //g" -i $newGraphicsFilename
sed --in-place '/D       /d' $newGraphicsFilename
sed --in-place '/ D      /d' $newGraphicsFilename

mkdir ./upgradeRFS/usr/local/hobart/system/graphics
cd $workingdirectory/Windux/Application/System/graphics
xargs -a $workingdirectory/Windux/$newGraphicsFilename cp -t $workingdirectory/Windux/upgradeRFS/usr/local/hobart/system/graphics

#### TEXT
cd $workingdirectory/Windux/Application/source
svn diff -r $WINDUXREVFROM:$WINDUXREVHEAD --summarize | egrep $sourceFilesOfInterest> $workingdirectory/Windux/$newSourceFilename
cd $workingdirectory/Windux
sed -e "s/M       //g" -i $newSourceFilename
sed -e "s/ M      //g" -i $newSourceFilename
sed -e "s/A       //g" -i $newSourceFilename
sed -e "s/MM      //g" -i $newSourceFilename
sed --in-place '/D       /d' $newSourceFilename
sed --in-place '/ D      /d' $newSourceFilename

cd $workingdirectory/Windux/Application/source
xargs -a $workingdirectory/Windux/$newSourceFilename cp -t $workingdirectory/Windux/upgradeRFS/etc/hobart

#### CATEGORIES
cd $workingdirectory/Windux/Application/System/categories
svn diff -r $WINDUXREVFROM:$WINDUXREVHEAD --summarize > $workingdirectory/Windux/$newCategoriesFilename
cd $workingdirectory/Windux
sed -e "s/M       //g" -i $newCategoriesFilename
sed -e "s/ M      //g" -i $newCategoriesFilename
sed -e "s/A       //g" -i $newCategoriesFilename
sed -e "s/MM      //g" -i $newCategoriesFilename
sed --in-place '/D       /d' $newCategoriesFilename
sed --in-place '/ D      /d' $newCategoriesFilename

mkdir ./upgradeRFS/usr/local/hobart/system/categories
cd $workingdirectory/Windux/Application/source
xargs -a $workingdirectory/Windux/$newCategoriesFilename cp -t $workingdirectory/Windux/upgradeRFS/usr/local/hobart/system/categories

#### PRODUCTS
cd $workingdirectory/Windux/Application/System/products
svn diff -r $WINDUXREVFROM:$WINDUXREVHEAD --summarize > $workingdirectory/Windux/$newProductsFilename
cd $workingdirectory/Windux
sed -e "s/M       //g" -i $newProductsFilename
sed -e "s/ M      //g" -i $newProductsFilename
sed -e "s/A       //g" -i $newProductsFilename
sed -e "s/MM      //g" -i $newProductsFilename
sed --in-place '/D       /d' $newProductsFilename
sed --in-place '/ D      /d' $newProductsFilename

mkdir ./upgradeRFS/usr/local/hobart/system/products
cd $workingdirectory/Windux/Application/System/products
xargs -a $workingdirectory/Windux/$newProductsFilename cp -t $workingdirectory/Windux/upgradeRFS/usr/local/hobart/system/products

#### DONGLE KEY
cd $workingdirectory/Windux/Application/shared/util/dongleKey
svn diff -r $WINDUXREVFROM:$WINDUXREVHEAD --summarize | egrep $dongleKeyFilesOfInterest> $workingdirectory/Windux/$newDongleKeyFilename
cd $workingdirectory/Windux
sed -e "s/M       //g" -i $newDongleKeyFilename
sed -e "s/ M      //g" -i $newDongleKeyFilename
sed -e "s/A       //g" -i $newDongleKeyFilename
sed -e "s/MM      //g" -i $newDongleKeyFilename
sed --in-place '/D       /d' $newDongleKeyFilename
sed --in-place '/ D      /d' $newDongleKeyFilename

cd $workingdirectory/Windux/Application/shared/sessionHandler/python
xargs -a $workingdirectory/Windux/$newDongleKeyFilename cp -t $workingdirectory/Windux/upgradeRFS/usr/local/hobart/python

#### LICENSING
cd $workingdirectory/Windux/Application/shared/util/licensing
svn diff -r $WINDUXREVFROM:$WINDUXREVHEAD --summarize | egrep $licensingFilesOfInterest> $workingdirectory/Windux/$newLicensingFilename
cd $workingdirectory/Windux
sed -e "s/M       //g" -i $newLicensingFilename
sed -e "s/ M      //g" -i $newLicensingFilename
sed -e "s/A       //g" -i $newLicensingFilename
sed -e "s/MM      //g" -i $newLicensingFilename
sed --in-place '/D       /d' $newLicensingFilename
sed --in-place '/ D      /d' $newLicensingFilename

mkdir ./upgradeRFS/usr/local/hobart/backend
cd $workingdirectory/Windux/Application/shared/sessionHandler/python
xargs -a $workingdirectory/Windux/$newLicensingFilename cp -t $workingdirectory/Windux/upgradeRFS/usr/local/hobart/python
xargs -a $workingdirectory/Windux/$newLicensingFilename cp -t $workingdirectory/Windux/upgradeRFS/usr/local/hobart/backend

#### SPECIALS
cd $workingdirectory/Windux/Application/shared/sessionHandler/images/specials
svn diff -r $WINDUXREVFROM:$WINDUXREVHEAD --summarize > $workingdirectory/Windux/$newSpecialsFilename
cd $workingdirectory/Windux
sed -e "s/M       //g" -i $newSpecialsFilename
sed -e "s/ M      //g" -i $newSpecialsFilename
sed -e "s/A       //g" -i $newSpecialsFilename
sed -e "s/ A      //g" -i $newSpecialsFilename
sed -e "s/MM      //g" -i $newSpecialsFilename
sed --in-place '/D       /d' $newSpecialsFilename
sed --in-place '/ D      /d' $newSpecialsFilename

mkdir ./upgradeRFS/usr/local/hobart/images/specials
cd $workingdirectory/Windux/Application/shared/sessionHandler/images/specials
xargs -a $workingdirectory/Windux/$newSpecialsFilename cp -t $workingdirectory/Windux/upgradeRFS/usr/local/hobart/images/specials

#### Playlists
  
cd $workingdirectory/Windux/Application/customerInterface/gcd/playlists
svn diff -r $WINDUXREVFROM:$WINDUXREVHEAD --summarize > $workingdirectory/Windux/$newPlaylistsFilename
cd $workingdirectory/Windux
sed -e "s/M       //g" -i $newPlaylistsFilename
sed -e "s/ M      //g" -i $newPlaylistsFilename
sed -e "s/A       //g" -i $newPlaylistsFilename
sed -e "s/MM      //g" -i $newPlaylistsFilename
sed --in-place '/D       /d' $newPlaylistsFilename
sed --in-place '/ D      /d' $newPlaylistsFilename

mkdir ./upgradeRFS/usr/local/hobart/playlists
cd $workingdirectory/Windux/Application/customerInterface/gcd/playlists
xargs -a $workingdirectory/Windux/$newPlaylistsFilename cp -t $workingdirectory/Windux/upgradeRFS/usr/local/hobart/playlists
    
#### GCD LAYOUTS

cd $workingdirectory/Windux/Application/customerInterface/gcd/layouts
svn diff -r $WINDUXREVFROM:$WINDUXREVHEAD --summarize > $workingdirectory/Windux/$newGCDLayoutsFilename
cd $workingdirectory/Windux
sed -e "s/M       //g" -i $newGCDLayoutsFilename
sed -e "s/ M      //g" -i $newGCDLayoutsFilename
sed -e "s/A       //g" -i $newGCDLayoutsFilename
sed -e "s/ A      //g" -i $newGCDLayoutsFilename
sed -e "s/MM      //g" -i $newGCDLayoutsFilename
sed --in-place '/D       /d' $newGCDLayoutsFilename
sed --in-place '/ D      /d' $newGCDLayoutsFilename

mkdir ./upgradeRFS/usr/local/hobart/layouts
cd $workingdirectory/Windux/Application/customerInterface/gcd/layouts
xargs -a $workingdirectory/Windux/$newGCDLayoutsFilename cp -t $workingdirectory/Windux/upgradeRFS/usr/local/hobart/layouts

#### 1024
newGCDLayouts1366Filename="newGCDLayouts1366.txt"
cd $workingdirectory/Windux/Application/customerInterface/gcd/layouts/1024_600
svn diff -r $WINDUXREVFROM:$WINDUXREVHEAD --summarize > $workingdirectory/Windux/$newGCDLayouts1024Filename
cd $workingdirectory/Windux
sed -e "s/M       //g" -i $newGCDLayouts1024Filename
sed -e "s/ M      //g" -i $newGCDLayouts1024Filename
sed -e "s/A       //g" -i $newGCDLayouts1024Filename
sed -e "s/ A      //g" -i $newGCDLayouts1024Filename
sed -e "s/MM      //g" -i $newGCDLayouts1024Filename
sed --in-place '/D       /d' $newGCDLayouts1024Filename
sed --in-place '/ D      /d' $newGCDLayouts1024Filename

mkdir ./upgradeRFS/usr/local/hobart/layouts/1024_600
cd $workingdirectory/Windux/Application/customerInterface/gcd/layouts/1024_600
xargs -a $workingdirectory/Windux/$newGCDLayouts1024Filename cp -t $workingdirectory/Windux/upgradeRFS/usr/local/hobart/layouts/1024_600

#### 1366
cd $workingdirectory/Windux/Application/customerInterface/gcd/layouts/1366_768
svn diff -r $WINDUXREVFROM:$WINDUXREVHEAD --summarize > $workingdirectory/Windux/$newGCDLayouts1366Filename
cd $workingdirectory/Windux
sed -e "s/M       //g" -i $newGCDLayouts1366Filename
sed -e "s/ M      //g" -i $newGCDLayouts1366Filename
sed -e "s/A       //g" -i $newGCDLayouts1366Filename
sed -e "s/ A      //g" -i $newGCDLayouts1366Filename
sed -e "s/MM      //g" -i $newGCDLayouts1366Filename
sed --in-place '/D       /d' $newGCDLayouts1366Filename
sed --in-place '/ D      /d' $newGCDLayouts1366Filename

mkdir $workingdirectory/Windux/upgradeRFS/usr/local/hobart/layouts/1366_768
cd $workingdirectory/Windux/Application/customerInterface/gcd/layouts/1366_768
xargs -a $workingdirectory/Windux/$newGCDLayouts1366Filename cp -t $workingdirectory/Windux/upgradeRFS/usr/local/hobart/layouts/1366_768

#### INDIVIDUAL FILES
cd $workingdirectory/Windux
mkdir $workingdirectory/Windux/upgradeRFS/usr/local/share
mkdir $workingdirectory/Windux/upgradeRFS/usr/local/share/ca-certificates
cp $workingdirectory/Windux/Application/certs/HobartRootCA.crt $workingdirectory/Windux/upgradeRFS/usr/local/share/ca-certificates/
cp  $workingdirectory/Windux/Application/certs/HobartRootCA.crt $workingdirectory/Windux/upgradeRFS/etc/hobart

mkdir ./upgradeRFS/root
mkdir ./upgradeRFS/root/.ssh
cp $workingdirectory/Windux/Application/certs/ssh/hobart_ssh.pub $workingdirectory/Windux/upgradeRFS/root/.ssh/authorized_keys

cp $workingdirectory/Windux/Application/source/db/defaults_linux.ini $workingdirectory/Windux/upgradeRFS/etc/hobart

mkdir $workingdirectory/Windux/upgradeRFS/usr/local/hobart/nowServing
mkdir $workingdirectory/Windux/upgradeRFS/usr/local/hobart/nowServing/res
cp $workingdirectory/Windux/Application/customerInterface/nowServing/res/* $workingdirectory/Windux/upgradeRFS/usr/local/hobart/nowServing/res

cp $workingdirectory/Windux/scaleui.config $workingdirectory/Windux/upgradeRFS/usr/local/hobart

cp $workingdirectory/Windux/gcd.config $workingdirectory/Windux/upgradeRFS/usr/local/hobart

cp $workingdirectory/Windux/videoPlayer.config $workingdirectory/Windux/upgradeRFS/usr/local/hobart

cp $workingdirectory/Windux/gstpd.config $workingdirectory/Windux/upgradeRFS/usr/local/hobart

mkdir $workingdirectory/Windux/upgradeRFS/usr/local/hobart/db
mkdir $workingdirectory/Windux/upgradeRFS/usr/local/hobart/db/scale
mkdir $workingdirectory/Windux/upgradeRFS/usr/local/hobart/db/scale/dbempty
cp $workingdirectory/Windux/Application/scaleDB.empty/dbftm.db $workingdirectory/Windux/upgradeRFS/usr/local/hobart/db/scale/dbempty

cp $workingdirectory/Windux/Application/certs/verifyFeatureInstaller.sh $workingdirectory/Windux/upgradeRFS/usr/local/hobart/python

mkdir ./upgradeRFS/usr/local/hobart/db/settingswizard
cp $workingdirectory/Windux/Application/shared/sessionHandler/db/scalesettings $workingdirectory/Windux/upgradeRFS/usr/local/hobart/db/settingswizard

echo "6 - Compressing new software into the upgrade file."
echo ""
#read -p "Go..."

cd $workingdirectory/Windux/upgradeRFS
tar -czvf $workingdirectory/$UPGRADEFILENAME ./* > $workingdirectory/hti-linux-application-upgrade-$RELEASETAG.list

rm -rf $workingdirectory/Windux/upgradeRFS
rm $workingdirectory/Windux/$newPythonFilename
rm $workingdirectory/Windux/$newSoftkeysFilename
rm $workingdirectory/Windux/$newDiagnosticsFilename
rm $workingdirectory/Windux/$newFontsFilename
rm $workingdirectory/Windux/$newGraphicsFilename
rm $workingdirectory/Windux/$newSourceFilename
rm $workingdirectory/Windux/$newCategoriesFilename
rm $workingdirectory/Windux/$newProductsFilename
rm $workingdirectory/Windux/$newSpecialsFilename
rm $workingdirectory/Windux/$newGCDLayoutsFilename
rm $workingdirectory/Windux/$newGCDLayouts1024Filename
rm $workingdirectory/Windux/$newGCDLayouts1366Filename
rm $workingdirectory/Windux/$newPlaylistsFilename
rm $workingdirectory/Windux/$newDongleKeyFilename
rm $workingdirectory/Windux/$newLicensingFilename



echo "7 - Cleaning up uneccessary files."
echo ""
echo ""
echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
echo "FINISHED REDUCED UPGRADE PACKAGE"
echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
echo ""
#read -p "Pause now..."
