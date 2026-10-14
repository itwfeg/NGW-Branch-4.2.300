#!/bin/bash
#
# This script builds a Micro SM Backend installation RPM for Edgify.
#
# Usage:
#   $ ./makeEdgifyMicroBackend.sh
#
#   Must be run from Windux folder!
#
# Requirements:
#   rpm
#   rpmlint
#

#set -x
set -e

pwd=`pwd`

NAME=edgify-micro-smbackend
VERSION=1.0.0
RELEASE=12
ARCH=noarch

WINDUXDIR=${pwd}
SMDIR=${WINDUXDIR}/../ScaleManagement

SOURCEDIR=${NAME}-${VERSION}
SOURCEFILE=${SOURCEDIR}.tar.gz

RPMDIR=${HOME}/rpmbuild
RPMSPECFILE=${RPMDIR}/SPECS/${NAME}.spec

#######################
# Create RPM Structure
#######################

if [ -d ${RPMDIR} ]; then
    rm -rf ${RPMDIR}
fi

mkdir -p ${RPMDIR}/BUILD
mkdir -p ${RPMDIR}/BUILDROOT
mkdir -p ${RPMDIR}/RPMS
mkdir -p ${RPMDIR}/SOURCES
mkdir -p ${RPMDIR}/SPECS
mkdir -p ${RPMDIR}/SRPMS

if [ ! -d ${RPMDIR} ]; then
    echo "Failed to create folder ${RPMDIR} for RPM"
    exit 1
fi

#################
# Gather Sources
#################

if [ -d ${SOURCEDIR} ]; then
    rm -rf ${SOURCEDIR}
fi

mkdir ${SOURCEDIR}

# Note:
# Make sure to add any files here also down below in the spec file.

cp ${SMDIR}/smBackend/autoconfig.service ${SOURCEDIR}
cp ${SMDIR}/smBackend/microSmBackend ${SOURCEDIR}
cp ${SMDIR}/smBackend/microSmBackend.service ${SOURCEDIR}

cp ${SMDIR}/smBackend/autoconfig.py ${SOURCEDIR}
cp ${SMDIR}/smBackend/microSmBackend.py ${SOURCEDIR}
cp ${SMDIR}/smBackend/networkManagerNew.py ${SOURCEDIR}
cp ${SMDIR}/smBackend/networkManagerNMCLI.py ${SOURCEDIR}

cp ${WINDUXDIR}/Application/shared/sessionHandler/python/applicationTimer.py ${SOURCEDIR}
cp ${WINDUXDIR}/Application/shared/sessionHandler/python/configInfo.py ${SOURCEDIR}
cp ${WINDUXDIR}/Application/shared/sessionHandler/python/deviceType.py ${SOURCEDIR}
cp ${WINDUXDIR}/Application/shared/sessionHandler/python/edgifyComms.py ${SOURCEDIR}
cp ${WINDUXDIR}/Application/shared/sessionHandler/python/encryptDecrypt.py ${SOURCEDIR}
cp ${WINDUXDIR}/Application/shared/sessionHandler/python/file_io.py ${SOURCEDIR}
cp ${WINDUXDIR}/Application/shared/sessionHandler/python/getBoardType.py ${SOURCEDIR}
cp ${WINDUXDIR}/Application/shared/sessionHandler/python/getHostname.py ${SOURCEDIR}
cp ${WINDUXDIR}/Application/shared/sessionHandler/python/getMac.py ${SOURCEDIR}
cp ${WINDUXDIR}/Application/shared/sessionHandler/python/getMacAddress.py ${SOURCEDIR}
cp ${WINDUXDIR}/Application/shared/sessionHandler/python/getNetworkInfo.py ${SOURCEDIR}
cp ${WINDUXDIR}/Application/shared/sessionHandler/python/getSerialFlashInfo.py ${SOURCEDIR}
cp ${WINDUXDIR}/Application/shared/sessionHandler/python/installCert.py ${SOURCEDIR}
cp ${WINDUXDIR}/Application/shared/sessionHandler/python/messageBox.py ${SOURCEDIR}
cp ${WINDUXDIR}/Application/shared/sessionHandler/python/ntp.py ${SOURCEDIR}
cp ${WINDUXDIR}/Application/shared/sessionHandler/python/osUtils.py ${SOURCEDIR}
cp ${WINDUXDIR}/Application/shared/sessionHandler/python/ping.py ${SOURCEDIR}
cp ${WINDUXDIR}/Application/shared/sessionHandler/python/serialFlash.py ${SOURCEDIR}
cp ${WINDUXDIR}/Application/shared/sessionHandler/python/setNTP.py ${SOURCEDIR}
cp ${WINDUXDIR}/Application/shared/sessionHandler/python/setTime.py ${SOURCEDIR}
cp ${WINDUXDIR}/Application/shared/sessionHandler/python/setTimezone.py ${SOURCEDIR}
cp ${WINDUXDIR}/Application/shared/sessionHandler/python/sftpProtocol.py ${SOURCEDIR}
cp ${WINDUXDIR}/Application/shared/sessionHandler/python/shellCommand.py ${SOURCEDIR}
cp ${WINDUXDIR}/Application/shared/sessionHandler/python/tags.py ${SOURCEDIR}
cp ${WINDUXDIR}/Application/shared/sessionHandler/python/updateCrontab.py ${SOURCEDIR}
cp ${WINDUXDIR}/Application/shared/sessionHandler/python/updateTimeNTP.py ${SOURCEDIR}

cp ${WINDUXDIR}/Application/shared/sessionHandler/python/Utilities/simpleParserConfig.py ${SOURCEDIR}

cp ${WINDUXDIR}/Application/shared/util/tdcc/rgdManager/encryptDecryptBB.py ${SOURCEDIR}

cp ${WINDUXDIR}/3RDPartySoftware/edgify/docker/docker-compose-agent.yml ${SOURCEDIR}
cp ${WINDUXDIR}/3RDPartySoftware/edgify/docker/edgify-agent.env ${SOURCEDIR}

# generate script for installation of Edgify
cat << EOF >> ${SOURCEDIR}/installEdgify.sh
#!/bin/bash

if [ ! -f /home/root/.edgifyclients ]; then
    echo "One or more files required for installation is missing."
    exit 1
elif [ ! -f /home/root/docker-compose-agent.yml ]; then
    echo "One or more files required for installation is missing."
    exit 1
elif [ ! -f /home/root/edgify-agent.env ]; then
    echo "One or more files required for installation is missing."
    exit 1
fi

# Authenticate to Edigy Docker Repo
USER=edgifyclients
PWD=\$(cat /home/root/.edgifyclients)

docker login -u \$USER -p \$PWD

if [ $? -ne 0 ]; then
    echo "Docker login failed authentication!"
    exit 1
fi

# Install Edgify Agent
docker-compose -f /home/root/docker-compose-agent.yml pull

if [ $? -ne 0 ]; then
    echo "Failed to pull Edgify Agent Docker Image!"
    exit 1
fi

# Startup Edgify
docker-compose -f /home/root/docker-compose-agent.yml up -d

if [ $? -ne 0 ]; then
    echo "Failed to start Edgify Agent Docker Image!"
    exit 1
fi

# Pull Worker
docker pull edgify/edgify-worker-cpu:1.17.6

if [ $? -ne 0 ]; then
    echo "Failed to pull Edgify Worker Image!"
    exit 1
fi

# Pull Collaborator
docker pull edgify/edgify-collaborator:1.17.6

if [ $? -ne 0 ]; then
    echo "Failed to pull Edgify Collaborator Image!"
    exit 1
fi

echo "Success!"
rm /home/root/.edgifyclients

EOF

chmod +x ${SOURCEDIR}/installEdgify.sh

# generate script for SD card modification
cat << EOF >> ${SOURCEDIR}/setupJetsonNano.sh
#!/bin/bash

SHOULD_REBOOT=0

# ensure hobart dir exists
if [ ! -d /usr/local/hobart ]; then
    mkdir -p /usr/local/hobart
fi

# create 2GB swap
if [ ! -f /usr/local/hobart/swap ]; then

    # create swap file
    fallocate -l 2G /swapfile
    chmod 600 /swapfile
    mkswap /swapfile
    
    # turn swap on
    swapon /swapfile
    
    # make permanent
    echo "/swapfile swap swap defaults 0 0" >> /etc/fstab
    
    # update swappiness
    sysctl vm.swappiness=10
    
    # make permanent
    echo "vm.swappiness=10" >> /etc/sysctl.conf

    # set flag
    touch /usr/local/hobart/swap
fi

# grow primary partition size
if [ ! -f /usr/local/hobart/expand ]; then

    # target disk
    disk='mmcblk0'

    # get last partition
    part=\$(grep  "\${disk}" /proc/partitions | tail -1 | awk '{print \$4}' | xargs)
    partN=\$(echo \$part | tail -c 2)

    # fdisk
    # delete and recreate the last partition with the largest size possible.
    (
        echo d       # Delete partition
        echo n       # Add a new partition
        echo \$partN  # Last partition
        echo         # First sector (Accept default: 2048)
        echo         # Last sector (Accept default: varies)
        echo w       # Write changes
    ) | fdisk /dev/\$disk 

    # update filesystem to match new partition size
    resize2fs /dev/\$part

    # we need to reboot for changes to take effect
    SHOULD_REBOOT=1

    # set flag
    touch /usr/local/hobart/expand
fi

# check if we should reboot
if [ "\$SHOULD_REBOOT" = "1" ]; then
    echo 'Setup complete... rebooting...'

    # reboot!
    reboot
fi
EOF

chmod +x ${SOURCEDIR}/setupJetsonNano.sh

cat << EOF >> ${SOURCEDIR}/setupRaspberryIOT.sh
#!/bin/bash

SHOULD_REBOOT=0

# ensure hobart dir exists
if [ ! -d /usr/local/hobart ]; then
    mkdir -p /usr/local/hobart
fi

# setup hostname
MAC=\$(cat /sys/class/net/eth0/address)
ADDR=\$(echo "\$MAC" | tr -d : )
HOSTNAME=edgify-\${ADDR}

rm /etc/hostname
echo \$HOSTNAME > /etc/hostname

# grow primary partition size
if [ ! -f /usr/local/hobart/expand ]; then

    # target disk
    disk='mmcblk0'

    # get last partition
    part=\$(grep  "\${disk}" /proc/partitions | tail -1 | awk '{print \$4}' | xargs)
    partN=\$(echo \$part | tail -c 2)
    
    # save starting sector number
    start=\$(fdisk -l | grep ^/dev/\${part} | awk -F" " '{print \$2}')

    # fdisk
    # delete and recreate the last partition with the largest size possible.
    (
        echo d       # Delete partition
        echo 2       # 2nd partition
        echo n       # Add a new partition
        echo p       # Primary type
        echo \$partN  # Last partition
        echo \$start  # First sector (Accept default: 2048)
        echo         # Last sector (Accept default: varies)
        echo w       # Write changes
    ) | fdisk /dev/\$disk 

    # reload partition table
    partprobe

    # update filesystem to match new partition size
    resize2fs /dev/\$part

    # we need to reboot for changes to take effect
    SHOULD_REBOOT=1

    # set flag
    touch /usr/local/hobart/expand
fi

# create 2GB swap
if [ ! -f /usr/local/hobart/swap ]; then

    # create swap file
    fallocate -l 2G /swapfile
    chmod 600 /swapfile
    mkswap /swapfile
    
    # turn swap on
    swapon /swapfile
    
    # make permanent
    echo "/swapfile swap swap defaults 0 0" >> /etc/fstab
    
    # update swappiness
    sysctl vm.swappiness=10
    
    # make permanent
    echo "vm.swappiness=10" >> /etc/sysctl.conf

    # set flag
    touch /usr/local/hobart/swap
fi

# create network bridge
if [ ! -f /usr/local/hobart/bridge ]; then

    # create bridge and attach both NICs as slaves
    nmcli con add ifname br0 type bridge con-name br0 stp no
    nmcli con add type bridge-slave ifname eth0 master br0
    nmcli con add type bridge-slave ifname enp1s0 master br0

    # bring down all interfaces
    nmcli con down 'Wired connection 2'
    nmcli con down 'Wired connection 1'
    nmcli con down br0
    
    # bring up bridge interface
    nmcli con up br0
    
    # we need to reboot for changes to take effect
    SHOULD_REBOOT=1

    # set flag
    touch /usr/local/hobart/bridge
fi


# check if we should reboot
if [ "\$SHOULD_REBOOT" = "1" ]; then
    echo 'Setup complete... rebooting...'

    # reboot!
    reboot
fi

EOF

chmod +x ${SOURCEDIR}/setupRaspberryIOT.sh


# generate tarball
tar --create --file ${SOURCEFILE} ${SOURCEDIR}

if [ ! -f ${SOURCEFILE} ]; then
    echo "Failed to create source tarball"
    exit 1
fi

# cleanup
mv ${SOURCEFILE} ${RPMDIR}/SOURCES
rm -rf ${SOURCEDIR}

#####################
# Generate Spec File
#####################

SPECFILE=${RPMDIR}/SPECS/${NAME}.spec

# remove existing spec
if [ -f ${SPECFILE} ]; then
    rm ${SPECFILE}
fi

# generate new spec
cat << EOF >> ${RPMDIR}/SPECS/${NAME}.spec
Name:           ${NAME}
Version:        ${VERSION}
Release:        ${RELEASE}%{?dist}
Summary:        Micro SM Backend for Edgify
BuildArch:      ${ARCH}

License:        Commercial
Source0:        %{name}-%{version}.tar.gz

Requires:       python3
Requires:       systemd


%description
Micro SM Backend daemon services for Edgify devices.


%prep
%setup -q


%pre
# upgrade
if [ \$1 -eq 2 ]; then
  if [ -f %{_sysconfdir}/systemd/system/microSmBackend.service ]; then
    systemctl stop microSmBackend
  fi
  if [ -f %{_sysconfdir}/systemd/system/autoconfig.service ]; then
    systemctl stop autoconfig
  fi
fi


%post
systemctl daemon-reload
systemctl start microSmBackend
systemctl start autoconfig

# install
if [ \$1 -eq 1 ]; then
  # enable services
  systemctl enable microSmBackend
  systemctl enable autoconfig

  dtcompat=\$(cat /proc/device-tree/compatible)

  # setup rpi
  if [[ "\$dtcompat" == *"raspberry"* ]]; then
    %{_bindir}/setupRaspberryIOT.sh
  fi
  
  # setup nano
  if [[ "\$dtcompat" == *"jetson"* ]]; then
    %{_bindir}/setupJetsonNano.sh
  fi
fi


%preun
# uninstall
if [ \$1 -eq 0 ]; then
  if [ -f %{_sysconfdir}/systemd/system/microSmBackend.service ]; then
    systemctl stop microSmBackend
    systemctl disable microSmBackend
  fi
  if [ -f %{_sysconfdir}/systemd/system/autoconfig.service ]; then
    systemctl stop autoconfig
    systemctl disable autoconfig
  fi
fi


%install
rm -rf \$RPM_BUILD_ROOT
mkdir -p \$RPM_BUILD_ROOT%{_bindir}

cp autoconfig.py \$RPM_BUILD_ROOT%{_bindir}
cp microSmBackend.py \$RPM_BUILD_ROOT%{_bindir}
cp networkManagerNew.py \$RPM_BUILD_ROOT%{_bindir}
cp networkManagerNMCLI.py \$RPM_BUILD_ROOT%{_bindir}

cp applicationTimer.py \$RPM_BUILD_ROOT%{_bindir}
cp configInfo.py \$RPM_BUILD_ROOT%{_bindir}
cp deviceType.py \$RPM_BUILD_ROOT%{_bindir}
cp edgifyComms.py \$RPM_BUILD_ROOT%{_bindir}
cp encryptDecrypt.py \$RPM_BUILD_ROOT%{_bindir}
cp encryptDecryptBB.py \$RPM_BUILD_ROOT%{_bindir}
cp file_io.py \$RPM_BUILD_ROOT%{_bindir}
cp getBoardType.py \$RPM_BUILD_ROOT%{_bindir}
cp getHostname.py \$RPM_BUILD_ROOT%{_bindir}
cp getMac.py \$RPM_BUILD_ROOT%{_bindir}
cp getMacAddress.py \$RPM_BUILD_ROOT%{_bindir}
cp getNetworkInfo.py \$RPM_BUILD_ROOT%{_bindir}
cp getSerialFlashInfo.py \$RPM_BUILD_ROOT%{_bindir}
cp installCert.py \$RPM_BUILD_ROOT%{_bindir}
cp messageBox.py \$RPM_BUILD_ROOT%{_bindir}
cp ntp.py \$RPM_BUILD_ROOT%{_bindir}
cp osUtils.py \$RPM_BUILD_ROOT%{_bindir}
cp ping.py \$RPM_BUILD_ROOT%{_bindir}
cp serialFlash.py \$RPM_BUILD_ROOT%{_bindir}
cp setNTP.py \$RPM_BUILD_ROOT%{_bindir}
cp setTime.py \$RPM_BUILD_ROOT%{_bindir}
cp setTimezone.py \$RPM_BUILD_ROOT%{_bindir}
cp sftpProtocol.py \$RPM_BUILD_ROOT%{_bindir}
cp shellCommand.py \$RPM_BUILD_ROOT%{_bindir}
cp simpleParserConfig.py \$RPM_BUILD_ROOT%{_bindir}
cp tags.py \$RPM_BUILD_ROOT%{_bindir}
cp updateCrontab.py \$RPM_BUILD_ROOT%{_bindir}
cp updateTimeNTP.py \$RPM_BUILD_ROOT%{_bindir}

mkdir -p \$RPM_BUILD_ROOT/home/root

cp docker-compose-agent.yml \$RPM_BUILD_ROOT/home/root
cp edgify-agent.env \$RPM_BUILD_ROOT/home/root

echo '732d9bea-3c74-4c85-b105-cb367eb2bc23' > \$RPM_BUILD_ROOT/home/root/.edgifyclients

cp installEdgify.sh \$RPM_BUILD_ROOT%{_bindir}
cp setupJetsonNano.sh \$RPM_BUILD_ROOT%{_bindir}
cp setupRaspberryIOT.sh \$RPM_BUILD_ROOT%{_bindir}

mkdir -p \$RPM_BUILD_ROOT%{_sysconfdir}/hobart

touch \$RPM_BUILD_ROOT%{_sysconfdir}/hobart/edgify

mkdir -p \$RPM_BUILD_ROOT%{_sysconfdir}/systemd/system

cp autoconfig.service \$RPM_BUILD_ROOT%{_sysconfdir}/systemd/system
cp microSmBackend.service \$RPM_BUILD_ROOT%{_sysconfdir}/systemd/system


%files
%{_bindir}/autoconfig.py
%{_bindir}/microSmBackend.py
%{_bindir}/networkManagerNew.py
%{_bindir}/networkManagerNMCLI.py

%{_bindir}/applicationTimer.py
%{_bindir}/configInfo.py
%{_bindir}/deviceType.py
%{_bindir}/edgifyComms.py
%{_bindir}/encryptDecrypt.py
%{_bindir}/encryptDecryptBB.py
%{_bindir}/file_io.py
%{_bindir}/getBoardType.py
%{_bindir}/getHostname.py
%{_bindir}/getMac.py
%{_bindir}/getMacAddress.py
%{_bindir}/getNetworkInfo.py
%{_bindir}/getSerialFlashInfo.py
%{_bindir}/installCert.py
%{_bindir}/messageBox.py
%{_bindir}/ntp.py
%{_bindir}/osUtils.py
%{_bindir}/ping.py
%{_bindir}/serialFlash.py
%{_bindir}/setNTP.py
%{_bindir}/setTime.py
%{_bindir}/setTimezone.py
%{_bindir}/sftpProtocol.py
%{_bindir}/shellCommand.py
%{_bindir}/simpleParserConfig.py
%{_bindir}/tags.py
%{_bindir}/updateCrontab.py
%{_bindir}/updateTimeNTP.py

/home/root/docker-compose-agent.yml
/home/root/edgify-agent.env
/home/root/.edgifyclients

%{_bindir}/installEdgify.sh
%{_bindir}/setupJetsonNano.sh
%{_bindir}/setupRaspberryIOT.sh

%{_sysconfdir}/hobart/edgify

%{_sysconfdir}/systemd/system/autoconfig.service
%{_sysconfdir}/systemd/system/microSmBackend.service

%changelog
* Mon Feb  21 2022 Randy Blankley <randy.blankley@itwfeg.com> - ${VERSION}
- First version being packaged
EOF

#############
# Validation
#############

rpmlint -v ${RPMSPECFILE}

result=$?
if [ $result -ne  0 ]; then
    echo "RPM Validation failed"
    exit 1
fi

########
# Build
########

rpmbuild --define "_exec_prefix /usr/local/hobart" -ba ${RPMSPECFILE}

result=$?
if [ $result -ne  0 ]; then
    echo "RPM Build failed"
    exit 1
fi

RPM=${RPMDIR}/RPMS/${ARCH}/${NAME}-${VERSION}-${RELEASE}.${ARCH}.rpm

if [ ! -f ${RPM} ]; then
    echo "Deployment RPM does not exist!"
    exit 1
fi

mv ${RPM} .

# cleanup
rm -rf ${RPMDIR}

#######
# Done
#######

echo "Done!"

