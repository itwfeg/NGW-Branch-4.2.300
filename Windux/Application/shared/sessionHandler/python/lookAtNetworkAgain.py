#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import shellCommand
import configInfo
import string
import subprocess
import ntNetworkUtils
import bootSettings
import time
import osUtils

pythonInstallPath = os.path.join("usr","bin")
if os.name == 'nt':
    pythonInstallPath = configInfo.getPythonInstallPath()
#pythonScriptPath = os.path.join(configInfo.getRootFolder(),"python")

def lookAtNetwork():

    exitCode = 0
    cmd = []
    if os.name == 'nt':

        configSettings = bootSettings.getBootSettings()
        sys.stdout.flush()
        enabledAdapterName = ""
        if configSettings['connectionType'] == "1":
            #wireless
            enabledAdapterName = "Wireless Network Connection"
        elif configSettings['connectionType'] == 0:
            enabledAdapterName = "Local Area Connection"

        detectedIpAddress, detectedSubnetMask, detectedGateway = ntNetworkUtils.getAddressValues(enabledAdapterName)

        connected = ntNetworkUtils.ping(detectedGateway)
        #print "connected:" + str(connected)

        if not connected:
            for x in range(1,7):
                time.sleep(5)
                #print "retrying test"
                connected = ntNetworkUtils.ping(detectedGateway)
                if connected:
                    break

        if connected:

            output = detectedIpAddress
            output += "|"
            output += detectedGateway
            output += "|"
            output += detectedSubnetMask

            detectedDnsServers = ntNetworkUtils.getDnsServers(enabledAdapterName)
            for server in detectedDnsServers:
                output += "|" + server

            print(("OUTPUT|" + output.rstrip()))
            sys.stdout.flush()
        else:
            #print("NO")
            print("OUTPUT|None")
            sys.stdout.flush()
            time.sleep(1)
    return exitCode

if __name__ == "__main__":
    exitCode = 0
    exitCode = lookAtNetwork()
    sys.exit(exitCode)


