#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import struct, sys, os, re,  shellCommand, optparse

def powerNetworkDevices(device, state):
    if os.name == "posix":
        if device == "wifi":
            cmd = []
            cmd.append("ifconfig")
            cmd.append("wlan0")
            if state == "on":
                cmd.append("up")
            elif state == "off":
                cmd.append("down")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
        elif device == "wire":
            cmd = []
            cmd.append("ifconfig")
            cmd.append("eth0")
            if state == "on":
                cmd.append("up")
            elif state == "off":
                cmd.append("down")
            exitCode, stdout, stderr = shellCommand.execute(cmd)


p = optparse.OptionParser()

# Now add some CLI options
##########################
p.add_option("-d", "--device",
             dest="device",
             help="The device to switch - wifi or wire")
p.add_option("-p", "--power",
             dest="power",
             help="The power mode - on or off")

# Parse User's CLI args
#######################
options, args = p.parse_args()

if __name__ == '__main__':

    if os.name == "posix":
        powerNetworkDevices(options.device, options.power)
        scaleSettings = {}
        dbFile = open("/usr/local/hobart/db/settingswizard/scalesettings", "r")
        for line in dbFile:
            line = line.strip()
            keyValuePair = line.split('=')
            ####################################################################
            #Server Cert Filename
            #csrFilename=
            #csrKeyFilename=
            if keyValuePair[0] == "isStatic":
                # set customer brightness
                scaleSettings['isStatic'] = str(keyValuePair[1])
            ####################################################################

        dbFile.close()

        if options.power == "on":
            if scaleSettings['isStatic'] == "0":
                # run dhcpcd on device
                cmd = []
                cmd.append("dhcpcd")
                if options.device == "wifi":
                    cmd.append("wlan0")
                elif options.device == "wire":
                    cmd.append("eth0")
                exitCode, stdout, stderr = shellCommand.execute(cmd)

