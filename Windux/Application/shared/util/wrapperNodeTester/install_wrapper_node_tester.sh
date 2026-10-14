#!/usr/bin/env bash

if /bin/ping -c 3 -w 4 8.8.8.8 > /dev/null
then
    echo "Internet available pulling support packages down to install"
    /usr/bin/sudo apt-get update
    /usr/bin/sudo apt-get install qt5-default pyqt5-dev pyqt5-dev-tools
    /usr/bin/pip3 install pyserial
else
    echo "Internet not available installing application only"
fi

# Install desktop icon file
/bin/rm -f /home/pi/Desktop/wrapperNodeTester.desktop
/bin/sync
/bin/cp -f wrapperNodeTester.desktop /home/pi/Desktop
/bin/sync
# backup critical files and remove old release
if [ -d /home/pi/wrapper_node_tester_new_release ]
then
    # copy previous release ini config file to new release
    /bin/cp -f /home/pi/wrapper_node_tester/wrapper_node_tester.ini /home/pi/wrapper_node_tester_new_release
    # remove the old version of board test server
    /usr/bin/sudo /bin/rm -rf /home/pi/wrapper_node_tester
    # move new version to be current version
    /usr/bin/sudo /bin/mv /home/pi/wrapper_node_tester_new_release /home/pi/wrapper_node_tester
fi
/bin/sync
/usr/bin/sudo /sbin/reboot
