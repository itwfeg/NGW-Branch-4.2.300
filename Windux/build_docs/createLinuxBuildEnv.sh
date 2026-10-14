#!/bin/bash
# This document is the instructions for setting up the build environment required for the Windux Linux firmware builds.
# It assumes that a Windows directory structure, C:\windux, has been created and populated with code from the Windux 
# source code repository’s ~/CommonCodeBase/MAINBRANCH_WINDUX project.  With Visual SourceSafe or SourceAnywhere 
# select the MAINBRANCH_WINDUX tree and perform a Get Latest Version for latest source file versions and select 
# Recursive to get the entire tree.  Answer Yes All to create directories not found.   This may take some time to 
# complete.
# NOTE: This is a re-write of the original document named, developer_howto, with changes and additions. It also 
# incorporates information from the RunningTheAppFromLaptopLinux document.
# 1.	Make sure the laptop your installing on has a valid and active internet connection.
# 2.	Using Sun VirtuallBox, create a VM with 20GB virtual disk and 1GB RAM.
# 3.	Install Ubuntu 9.10 from the Live CD to the VM created. Accept all defaults and create a home directory. For
#       the remainder of this document your home directory will be referenced by the ‘~’ character, as in cd ~.
# 4.	All commands listed below may be copied from Windows side with Control-C and then select inside VM Virtual
#       box terminal shell and depress both mouse buttons down to paste into terminal shell window to execute.  
#       If prompted for sudo password enter you linux account password that you created when you installed linux.
# 5.	Launch Ubuntu’s Update Manager and apply the latest Ubuntu updates by performing the following:
#   a.	sudo apt-get update
#   b.	sudo apt-get upgrade
# 6.	Restart the Linux session.
# 7.	In order to access the Windows host folders C:\windux\... you must install VirtualBox guest additions in 
#       Linux. Reference appendix A for instructions. 
# 8.	Once VirtualBox guest additions are installed you can now setup the Windows host folders, C:\windux\..., 
#       as a shared folder with our Linux guest VM. Reference appendix B for instructions. Make note that you are
#       trying to share the folder C:\windux such that it can be found in your Linux VM under the path 
#       /media/sf_windux.
#
# run this script with sudo and an internet connection
# sudo -i
# createLinuxBuildEnv.sh
# 
# Should anything fail during build uncomment the following
# to remove all build products produced by script and rerun
# replace $username found in any lines below with your
# linux username
#
if [ `whoami` != 'root' ]
then
    echo "You must perform 'sudo -i' command and become root"
    echo "to run this script."
    exit 1
fi

rm -rf /home/$username/Hobart
rm -rf /home/Hobart
rm -rf /usr/local/hobart
rm -rf /var/www

ping -c 1 -w 1 www.itw.com
if [ $? -eq 1 ]
then
    echo "You do not appear to have a connection to the internet"
    echo "Please establish a connection to the network to perform"
    echo "Build Environment Setup"
fi

echo -n "Enter unix username: "
read username

if [ ! -d /media/sf_windux ]
then
    echo "You have not mapped the Linux shared folder windux to the Windows side"
    exit 1
fi

dpkg --get-selections | grep gcj
if [ $? -eq 0 ]
then
    echo "Removing gcj from system"
    apt-get -y purge gcj
fi

java -version |& grep -q "Java(TM) SE Runtime"
if [ $? -eq 1 ]
then
    echo "Installing Java SE JDK and Runtime"
    apt-get -y install sun-java6-jre
    apt-get -y install sun-java6-jdk
    apt-get -y install sun-java6-plugin
fi

PACKS=( automake autoconf libtool g++ language-pack-es language-pack-fr )
for i in "${PACKS[@]}"; do
    dpkg --get-selections | grep "$i"
    if [ $? -eq 1 ]
    then
	echo "Installing $i"
	apt-get -y install $i
    else
	echo "Package $i already installed"
    fi
done

HOBART_APP=/home/$username/Hobart/shared_code/Application
export HOBART_APP=/home/$username/Hobart/shared_code/Application

if [ ! -d /home/$username/Hobart ]
then
    echo "User Hobart directory not found creating"
    mkdir -p /home/$username/Hobart/shared_code
    chmod u=rwx,g=rx,o=rx /home/$username/Hobart
    if [ -d /media/sf_windux ]
    then
	/bin/cp -R -f /media/sf_windux/* /home/$username/Hobart/shared_code
	chown -R $username.$username /home/$username/Hobart
	echo "export HOBART_APP=/home/$username/Hobart/shared_code/Application" >> /home/$username/.bashrc
	echo "export HOBART_APP=/home/$username/Hobart/shared_code/Application" >> /root/.bashrc
    else
	echo "Can not find /media/sf_windux shared folder to copy to user Hobart directory"
	echo "Please configure shared folder in Linux to access Windows VSS source tree"
	exit 1
    fi
else
    echo "User Hobart directory found skipping shared folder copy"
    echo "If directory empty remove it to force copy on next run"
fi

PACKS=( dselect )
for i in "${PACKS[@]}"; do
    dpkg --get-selections | grep "$i"
    if [ $? -eq 1 ]
    then
	echo "Installing $i"
	apt-get -y install $i
    else
	echo "Package $i already installed"
    fi
done

echo "Installing required build environment packages"
dpkg --set-selections < $HOBART_APP/../build_docs/installed-software.linux.hobart
if [ $? -eq 1 ]
then
    echo "Error installing build environment packages"
    exit 1
else
    echo "Build environment packages installed successfully"
    echo "Updating build environment packages"
    dselect install
fi

PACKS=( curl wmctrl i2c-tools libusb-1.0-0-dev libusb-dev libudev-dev libcurl4-openssl-dev )
for i in "${PACKS[@]}"; do
    dpkg --get-selections | grep "$i"
    if [ $? -eq 1 ]
    then
	echo "Installing $i"
	apt-get -y install $i
    else
	echo "Package $i already installed"
    fi
done

if [ ! -d /etc/hobart ]
then
    mkdir /etc/hobart
    chmod u=rwx,g=rwx,o=r /etc/hobart
    /bin/cp -R -f /media/sf_windux/Config_files/etc/hobart/* /etc/hobart
    chmod u=rwx,g=rwx,o=rx /etc/hobart/*
fi
if [ ! -f /home/$username/.Xdefaults ]
then
    cp -f /media/sf_windux/Config_files/etc/hobart/Xdefaults.en /home/$username/.Xdefaults
    xrdb -load /home/$username/.Xdefaults
fi

DIRS=( /var/www /lib/firmware/hobart /usr/share/themes/Clearlooks/gtk-2.0 /usr/local/hobart /usr/local/hobart/system /usr/local/hobart/user/fonts /usr/local/hobart/user/graphics /home/$username/Hobart/AccessScale /home/$username/Hobart/AccessScale/System /home/$username/Hobart/AccessScale/User /home/$username/Hobart/AccessScale/User/Fonts /home/$username/Hobart/AccessScale/User/Graphics )
for i in "${DIRS[@]}"; do
    if [ ! -d $i ]
    then
	mkdir -p $i
    fi
done

echo "Populating linux file system from source tree"
mkdir -p /var/www/images
mkdir -p /var/www/languages

mkdir -p /usr/local/hobart/system/flashkeygfx

mkdir -p /usr/local/hobart/system/fonts

mkdir -p /usr/local/hobart/system/graphics

mkdir -p /home/Hobart/AccessScale/System

cp -R -f /media/sf_windux/Application/webPages/* /var/www

cp -f /media/sf_windux/3RDPartySoftware/goAheadWebServer/LINUX/webs /usr/bin

cp -f /media/sf_windux/Application/scripts/win_cmd.sh /usr/bin
cp -f /media/sf_windux/Application/scripts/www_cmd.sh /usr/bin
cp -f /media/sf_windux/Application/scripts/xvkbd.sh /usr/bin
cp -f /media/sf_windux/Application/scripts/enablecore /usr/bin
cp -f /media/sf_windux/Application/scripts/e2fsck.conf /etc
cp -f /media/sf_windux/Application/scripts/xorg.conf /etc/X11
rm -f /etc/X11/xorg.conf
cp -f /media/sf_windux/3RDPartySoftware/SunJavaPolicy/java.policy /etc/java-6-sun/security/java.policy
cp -f /media/sf_windux/Application/scripts/rc.local /etc/rc.local
cp -f /media/sf_windux/Config_files/usr/share_themes/Clearlooks/gtkrc /usr/share/themes/Clearlooks/gtk-2.0

cp –r -f /media/sf_windux/Config_files/usr/local/hobart/system /usr/local/hobart

cp -r -f /media/sf_windux/Config_files/usr/local/hobart/system/flashkeygfx/* /usr/local/hobart/system/flashkeygfx

cp -r -f /media/sf_windux/Config_files/usr/local/hobart/system/fonts/* /usr/local/hobart/system/fonts

cp -r -f /media/sf_windux/Config_files/usr/local/hobart/system/graphics/* /usr/local/hobart/system/graphics


# The lines that follows seems odd to me, why copy same files to two different locations plus /home/Hobart does not exist

if [ ! -d /home/Hobart ]
then
    mkdir /home/Hobart
fi

cp -r -f /usr/local/hobart/system/* /home/Hobart/AccessScale/System/.

# End of questionable lines

#echo "Setting java security policy"
#ln –s /usr/lib/jvm/java-6-sun/jre/lib/security/java.policy /etc/java-6-sun/security/java.policy

rm -rf /home/$username/i2c-tools-3.0.2

echo "Install and build i2c-tools-3.0.2"
cd /home/$username
apt-get source i2c-tools
tar xvzf i2c-tools_3.0.2.orig.tar.gz
if [ $? -eq 1 ]
then
    echo "Can not extract downloaded i2c-tools tarball"
    exit 1
fi

cd /home/$username/i2c-tools-3.0.2
make
if [ $? -eq 1 ]
then
    echo "Can not build i2c-tools"
    exit 1
fi
make install
if [ $? -eq 1 ]
then
    echo "Can not install i2c-tools"
    exit 1
fi
rm -rf /home/$username/i2c-tools-3.0.2

rm -rf /home/$username/freetype-2.4.2

echo "Installing freetype package"
cd /home/$username
curl --silent --location http://download.savannah.gnu.org/releases/freetype/freetype-2.4.2.tar.gz > freetype-2.4.2.tar.gz
if [ $? -eq 1 ]
then
    echo "Can not download freetype fonts"
    exit 1
fi

tar xvzf freetype-2.4.2.tar.gz
if [ $? -eq 1 ]
then
    echo "Can not extract downloaded freetype fonts"
    exit 1
fi

cd /home/$username/freetype-2.4.2
sh autogen.sh
if [ $? -eq 1 ]
then
    echo "freetype autogen failed"
    exit 1
fi
./configure
if [ $? -eq 1 ]
then
    echo "freetype configure failed"
fi
make
if [ $? -eq 1 ]
then
    echo "freetype build failed"
fi
make install
if [ $? -eq 1 ]
then
    echo "freetype install failed"
fi
rm -rf /home/$username/freetype-2.4.2

echo "Installing fonts"
mkdir -p /usr/share/fonts/truetype/hobart
chmod ugo=rwx /usr/share/fonts/truetype/hobart

cp -f /home/Hobart/AccessScale/System/fonts/* /usr/share/fonts/truetype/hobart
#chown -R $username.$username /home/$username/Hobart
apt-get -y autoremove

# perform build of source files and verify chobart_d executable is built
/home/$username/linuxBuild.sh $username Y 1 Y
