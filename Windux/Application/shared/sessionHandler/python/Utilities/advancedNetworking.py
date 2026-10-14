#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys, os
import argparse
import shellCommand
import time

linuxWirelessDevice = 'wlan0'
linuxWiredDevice = 'eth0'

def setAdvancedSettings(device):
    exitCode = 0
    if os.name == "posix":
        try:
            if device == linuxWirelessDevice:
                cmd = []
                cmd.append("iwconfig")
                cmd.append("wlan0")
                cmd.append("power")
                cmd.append("off")
                exitCode, stdout, stderr = shellCommand.executeShellCommandBackground(cmd)

                localLinkRate = ""
                localAutomaticLinkRate = ""
                localDeviceType = "0"

                dbFile = open("/usr/local/hobart/db/settingswizard/scalesettings", "r")
                for line in dbFile:
                    line = line.strip()
                    keyValuePair = line.split('=')
                    if keyValuePair[0] == "linkRate":
                        localLinkRate = keyValuePair[1].rstrip()
                    elif keyValuePair[0] == "automaticLinkRate":
                        localAutomaticLinkRate = keyValuePair[1].rstrip()
                    elif keyValuePair[0] == "connectionType":
                        localDeviceType = keyValuePair[1].rstrip()
                dbFile.close()

                if localDeviceType == "1":
                    if localLinkRate != "auto" and localLinkRate != "" and localLinkRate != None:
                        '''
                        cmd = []
                        cmd.append("iw")
                        cmd.append("wlan0")
                        cmd.append("set")
                        cmd.append("bitrates")
                        cmd.append("legacy-2.4")
                        if localAutomaticLinkRate == "0":
                            cmd.append(localLinkRate.replace('M',''))
                        elif localAutomaticLinkRate == "1":
                            if localLinkRate == "1M":
                                cmd.append("1")
                            elif localLinkRate == "11M":
                                cmd.append("1 11")
                            elif localLinkRate == "24M":
                                cmd.append("1 11 24")
                            elif localLinkRate == "36M":
                                cmd.append("1 11 24 36")
                            elif localLinkRate == "48M":
                                cmd.append("1 11 24 36 48")
                            elif localLinkRate == "54M":
                                cmd.append("1 11 24 36 48 54")
                            elif localLinkRate == "149M":
                                cmd.append("1 11 24 36 48 54 149")
                        exitCode, stdout, stderr = shellCommand.executeShellCommandBackground(cmd)
                        '''

                        cmda = []
                        cmda.append("iwconfig")
                        cmda.append("wlan0")
                        cmda.append("rate")
                        cmda.append(localLinkRate)
                        if localAutomaticLinkRate == "1" or localAutomaticLinkRate == None:
                            cmda.append("auto")
                        elif localAutomaticLinkRate == "0":
                            cmda.append("fixed")
                        exitCode, stdout, stderr = shellCommand.execute(cmda)

                        cmda = []
                        cmda.append("iwlist")
                        cmda.append("wlan0")
                        cmda.append("scan")
                        exitCode, stdout, stderr = shellCommand.execute(cmda)

                        '''
                        with open("advancedNetworkingHistory.txt", "w") as myfi:
                            for item in cmd:
                                myfi.write("%s\n" % item)
                            myfi.write("XXXXXXXXX\n")
                            for itema in cmda:
                                myfi.write("%s\n" % itema)
                            #myfi.write(wpa_supplicantFileContents + "\n")
                        myfi.close()
                        '''
        except:
            exitCode = 0
            pass

    else:
        try:
            # TODO:
            # Need to implement for Windoze
            cmd = []
            exitCode, stdout, stderr = shellCommand.execute(cmd)
        except:
            exitCode = 0

    return exitCode

if __name__ == "__main__":
    exitCode = 0
    try:
        exitCode  = setAdvancedSettings(linuxWirelessDevice)
    except:
        pass
    sys.exit(exitCode)
