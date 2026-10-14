#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import subprocess
import scaleSettingsInterface

device = scaleSettingsInterface.getValue("connectionType")

if device == "1":
    defaultGateway = subprocess.check_output("route -n | grep 'UG[ \t]' | grep 'wlan0' | awk '{print $2}'", shell=True)
elif device == "0":
    defaultGateway = subprocess.check_output("route -n | grep 'UG[ \t]' | grep 'eth0' | awk '{print $2}'", shell=True)
    
defaultGateway = defaultGateway.decode('utf-8')

os.system("ping -c4 " + defaultGateway.strip() + ">/dev/null 2>&1" )
