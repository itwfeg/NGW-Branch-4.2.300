#!/bin/bash
#================================================
# Script Name:	start_hobart_setup.sh
# By:		Frank Albers
# Date:		June 22, 2011
# Purpose:	Used by the Windux project. This script allows user
#		accounts other than the loged in user to run the
#		Hobart Setup tool.
#================================================
clear
# check if required file exists
echo "Starting the Hobart Setup Tool"
echo "Takes about 10 seconds to start, have patience"

test -e /home/Hobart/AccessScale/config_os_mode.dat
if [ $? -ne 0 ]; then 
#	does not exist, create it
	> /home/Hobart/AccessScale/config_os_mode.dat > /dev/null
	if [ $? -ne 0 ]; then
		echo "Unable to create the file required by the Hobart Setup tool"
		echo "Verify directory permissions and try again"
		exit 4
	fi
fi
# file does exist, continue on
/usr/bin/HobartSetup.exe
rm /home/Hobart/AccessScale/config_os_mode.dat > /dev/null
if [ $? -ne 0 ]; then
# not able to remove file
	echo "Unable to remove the required file used by the Hobart Setup tool"
	echo "Verify directory permissions and/or manually remove file"
	exit 3
fi
#done
exit

