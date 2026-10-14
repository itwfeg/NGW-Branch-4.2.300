#!/usr/bin/env bash

if /bin/ping -c 3 -w 4 8.8.8.8 > /dev/null
then
    echo "Internet available pulling support packages down to install"
    /usr/bin/sudo apt-get update
    /usr/bin/sudo apt-get install qt5-default pyqt5-dev pyqt5-dev-tools
    /usr/bin/sudo /usr/bin/pip3 install pyserial
    /usr/bin/sudo /usr/bin/pip3 install fabric2
    /usr/bin/sudo /usr/bin/pip3 install patchwork
    /usr/bin/sudo /usr/bin/pip3 install netaddr
    /usr/bin/sudo /usr/bin/pip3 install scapy
else
    echo "Internet not available installing application only"
fi

# Install desktop icon file
/bin/rm -f /home/pi/Desktop/cpu_memory_tester.desktop
/bin/sync
/bin/cp -f cpu_memory_tester.desktop /home/pi/Desktop
/bin/sync
# backup critical files and remove old release
if [ -d /home/pi/cpu_memory_tester_new_release ]
then
    /usr/bin/sudo /bin/rm -rf /home/pi/cpu_memory_tester
    # move new version to be current version
    /usr/bin/sudo /bin/mv /home/pi/cpu_memory_tester_new_release /home/pi/cpu_memory_tester
fi
/bin/sync
/usr/bin/sudo /sbin/reboot
