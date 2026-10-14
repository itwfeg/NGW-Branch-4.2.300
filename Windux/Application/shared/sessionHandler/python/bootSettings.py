#!/usr/bin/env python2
# -*- coding: utf-8 -*-

# not tested in python3

# sets boot values for volume and brightness

import sys
import os
import shellCommand
import argparse
import math
import subprocess
import setBrightness
import setVolume
import beep
import shutil
import configInfo

scaleSettingsPath = configInfo.getRootFolder() + os.path.join(os.sep, 'db',  'settingswizard',  'scalesettings')
scaleSettingsBackupPath = configInfo.getRootFolder() + os.path.join(os.sep, 'db',  'settingswizard',  'scalesettingsBACKUP')

def convertStr(s):
    return str(int(s) + 104)

def getBootSettings():
    print("Hello")
    scaleSettingsPath = configInfo.getRootFolder() + os.path.join(os.sep, 'db',  'settingswizard',  'scalesettings')
    scaleSettingsBackupPath = configInfo.getRootFolder() + os.path.join(os.sep, 'db',  'settingswizard',  'scalesettingsBACKUP')

    bootSettings = {}

    scaleSettingsExist = os.path.exists(scaleSettingsPath)
    if not scaleSettingsExist or os.path.getsize(scaleSettingsPath) == 0:
        if scaleSettingsExist:
            os.remove(scaleSettingsPath)
        if os.path.exists(scaleSettingsBackupPath):
            shutil.copy2(scaleSettingsBackupPath, scaleSettingsPath)

    dbFile = open(scaleSettingsPath, "r")
    
    for line in dbFile:
        line = line.strip()
        keyValuePair = line.split('=')
        if keyValuePair[0] == "oBrightness":
            # set operator brightness
            #print("operator is original-" + str(keyValuePair[1]) + " new-" + str(convertStr(keyValuePair[1])))
            bootSettings['oBrightness'] = str(convertStr(keyValuePair[1]))

        elif keyValuePair[0] == "cBrightness":
            #set customer brightness
            #print("customer is original-" + str(keyValuePair[1]) + " new-" + str(convertStr(keyValuePair[1])))
            bootSettings['cBrightness'] = str(convertStr(keyValuePair[1]))

        elif keyValuePair[0] == "cVolume":
            bootSettings['cVolume'] = keyValuePair[1]

        elif keyValuePair[0] == "dimDisplayTimeout":
            bootSettings['dimDisplayTimeout'] = keyValuePair[1]

        elif keyValuePair[0] == "powerSaveMode":
            bootSettings['powerSaveMode'] = keyValuePair[1]

        elif keyValuePair[0] == "powerSaveEnabled":
            bootSettings['powerSaveEnabled'] = keyValuePair[1]

        elif keyValuePair[0] == "connectionType":
            bootSettings['connectionType'] = keyValuePair[1]

        elif keyValuePair[0] == "connectionName":
            bootSettings['connectionName'] = keyValuePair[1]
        elif keyValuePair[0] == "oVolume":
            if keyValuePair[1].strip() == 'on':
                bootSettings['oVolume'] = "128"
            else:
                bootSettings['oVolume'] = "0"
        elif keyValuePair[0] == "gcd":
            bootSettings['gcd'] = keyValuePair[1]
        elif keyValuePair[0] == "isStatic":
            bootSettings['isStatic'] = keyValuePair[1]
        elif keyValuePair[0] == "timeKeeper":
            bootSettings['timeKeeper'] = keyValuePair[1]
        elif keyValuePair[0] == "timezone":
            bootSettings['timezone'] = keyValuePair[1]
        elif keyValuePair[0] == "ntpServer":
            bootSettings['ntpServer'] = keyValuePair[1]
        elif keyValuePair[0] == "ntpFrequency":
            bootSettings['ntpFrequency'] = keyValuePair[1]
        elif keyValuePair[0] == "ntpDayOfWeek":
            bootSettings['ntpDayOfWeek'] = keyValuePair[1]
        elif keyValuePair[0] == "ntpTimeOfDay":
            bootSettings['ntpTimeOfDay'] = keyValuePair[1]
        elif keyValuePair[0] == "ntpTimeZone":
            bootSettings['ntpTimeZone'] = keyValuePair[1]
        elif keyValuePair[0] == "gateway":
            bootSettings['gateway'] = keyValuePair[1]

        elif keyValuePair[0] == "gateway":
            bootSettings['gateway'] = keyValuePair[1]
        elif keyValuePair[0] == "netmask":
            bootSettings['netmask'] = keyValuePair[1]
        elif keyValuePair[0] == "ipAddress":
            bootSettings['ipAddress'] = keyValuePair[1]
        elif keyValuePair[0] == "autoDNS":
            bootSettings['autoDNS'] = keyValuePair[1]
        elif keyValuePair[0] == "dns1":
            bootSettings['dns1'] = keyValuePair[1]
        elif keyValuePair[0] == "dns2":
            bootSettings['dns2'] = keyValuePair[1]
        elif keyValuePair[0] == "dns3":
            bootSettings['dns3'] = keyValuePair[1]
        elif keyValuePair[0] == "security":
            bootSettings['security'] = keyValuePair[1]
        elif keyValuePair[0] == "eapUsername":
            bootSettings['eapUsername'] = keyValuePair[1]
        elif keyValuePair[0] == "eapPassword":
            bootSettings['eapPassword'] = keyValuePair[1]

    dbFile.close()
    
    if os.name == 'posix':
        copyInterfacesFile = False
        deleteInterfacesFile = True
        if not os.path.exists("/etc/network/interfaces"):
            copyInterfacesFile = True
            deleteInterfacesFile = False

        else:
            sizeOfInterfacesFile = os.path.getsize("/etc/network/interfaces")
            if sizeOfInterfacesFile == 0:
                copyInterfacesFile = True

        if copyInterfacesFile:
            if deleteInterfacesFile:
                os.remove("/etc/network/interfaces")
            if bootSettings['connectionType'] == '1' and bootSettings['isStatic'] == "0":
                shutil.copy2('/etc/network/interfacesWIFI-DHCP', '/etc/network/interfaces')
            elif bootSettings['connectionType'] == '1' and bootSettings['isStatic'] == "1":
                shutil.copy2('/etc/network/interfacesWIFI-STATIC', '/etc/network/interfaces')
            elif bootSettings['connectionType'] == '0' and bootSettings['isStatic'] == "1":
                shutil.copy2('/etc/network/interfacesWIRE-STATIC', '/etc/network/interfaces')
            elif bootSettings['connectionType'] == '0' and bootSettings['isStatic'] == "0":
                shutil.copy2('/etc/network/interfacesWIRE-DHCP', '/etc/network/interfaces')
    
    return bootSettings

if __name__ == "__main__":

    bootSettings = getBootSettings()    
    exitCode = setBrightness.linuxSetBacklight(value=bootSettings['oBrightness'], device=2)
    if bootSettings['gcd'] == "0":
        exitCode = setBrightness.linuxSetBacklight(value=0, device=1)
    else:
        exitCode = setBrightness.linuxSetBacklight(value=bootSettings['cBrightness'], device=1)
    exitCode = setVolume.volumeSetting(bootSettings['cVolume'])

    sys.exit(exitCode)
