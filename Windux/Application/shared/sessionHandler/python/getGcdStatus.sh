#!/bin/bash
PATH=/usr/bin:/bin:/usr/sbin:/sbin
## getGcdStatus.sh This script will determin if the gcd display 
## is physically connected. 
##################################################################

PATH=/usr/bin:/bin:/usr/sbin:/sbin

STATUS=`cat /usr/local/gpio/gcd_display_detect/value`
if [ "$STATUS" == 0 ]; then
echo "connected"
	exit 1	#display connected
else
echo "disconnected"
	exit 0  #display disconnected
fi



