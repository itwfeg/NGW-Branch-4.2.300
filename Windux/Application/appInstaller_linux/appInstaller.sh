#!/bin/sh

WEBSERVICE=apache2
SCALESERVICE=scale_service
#/usr/bin/appInstaller
echo Running appInstaller
./appInstaller

result=$?
if [ $result -eq 255 ]; then
    echo Error
else
    /usr/sbin/service $WEBSERVICE status
    if [ $? -ne 0 ]; then
        # web server is not running
        /usr/sbin/update-rc.d $WEBSERVICE defaults
        /usr/sbin/service $WEBSERVICE start
        # verify web server is running
        /usr/sbin/update-rc.d $SCALESERVICE defaults
        /usr/sbin/service $SCALESERVICE start
        /usr/sbin/update-rc.d firefox defaults
        /usr/sbin/update-rc.d -f appInstaller remove
        echo Launching appLauncher
        /usr/bin/appLauncher
    else
        /usr/sbin/update-rc.d $WEBSERVICE defaults
        /usr/sbin/update-rc.d $SCALESERVICE defaults
        /usr/sbin/update-rc.d firefox defaults
        /sbin/reboot
    fi
fi
