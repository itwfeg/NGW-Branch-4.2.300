#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import shellCommand
import time
import re
import configInfo

def processScanList(data):
    """
    Determine if bluetooth scanned any devices in returned output and if so
    return true else false.
    """
    # split output buffer into list of lines based on carriage control found
    if os.name == 'posix':
        scanResultLines = re.split("[\n]+", data)
    else:
        scanResultLines = re.split("[\r\n]+", data)

    bluetoothDeviceFound = False
    foundScanHeader = False

    # for each line in output
    for i in range(0,len(scanResultLines)):

        # if the current outpout line has data in it besides whitespace
        if scanResultLines[i].strip() != "":

            print((scanResultLines[i]))
            # split the line based on whitespace
            tokens = scanResultLines[i].strip().split()

            # make sure at least two tokens on line
            if len(tokens) >= 2:

                # see if we are looking at a Scanning ... line
                if tokens[0].upper() == "SCANNING":

                    foundScanHeader = True

                # else look for bluetooth address line
                else:

                    # see if line is a bluetooth address line of a device found
                    if re.match("[0-9a-f]{2}([-:])[0-9a-f]{2}(\\1[0-9a-f]{2}){4}$", tokens[0].lower().strip()):

                        # if found scanning header line prior to bluetooth address line
                        if foundScanHeader:
                            # found a match on an bluetooth device address
                            # so bluetooth working
                            bluetoothDeviceFound = True
                            break

                        # else found value bluetooth address but no scanning... header line
                        else:
                            bluetoothDeviceFound = False
                            break

    return bluetoothDeviceFound

def bluetoothScan():
    exitCode = 1
    if os.name == "posix":

#       Nitrogen 6x stuff
#        # Code currently only works with Boundary Nitrogen 6x board
#        cmd = []
#        cmd.append("echo")
#        cmd.append("176")
#        cmd.append(">")
#        cmd.append("/sys/class/gpio/export")
#
#        exitCode, stdout, stderr = shellCommand.execute(cmd)
#
#        if exitCode != 0:
#            sys.exit(exitCode)
#
#        cmd = []
#        cmd.append("echo")
#        cmd.append("low")
#        cmd.append(">")
#        cmd.append("/sys/class/gpio/gpio176/direction")
#
#        exitCode, stdout, stderr = shellCommand.execute(cmd)
#        if exitCode != 0:
#            sys.exit(exitCode)
#
#        time.sleep(5)
#
#        cmd = []
#        cmd.append("echo")
#        cmd.append("high")
#        cmd.append(">")
#        cmd.append("/sys/class/gpio/gpio176/direction")
#
#        exitCode, stdout, stderr = shellCommand.execute(cmd)
#        if exitCode != 0:
#            sys.exit(exitCode)

#        cmd = []
#        cmd.append("hciattach")
#        cmd.append("-t")
#        cmd.append("30")
#        cmd.append("-s")
#        cmd.append("115200")
#        cmd.append("/dev/ttymxc2")
#        cmd.append("texas")
#        cmd.append("3000000")
#        cmd.append("flow")

#        exitCode, stdout, stderr = shellCommand.execute(cmd)
#        if exitCode != 0:
#            sys.exit(exitCode)

        cmd = []
        cmd.append("hciconfig")
        cmd.append("hci0")
        cmd.append("up")

        exitCode, stdout, stderr = shellCommand.execute(cmd, shell=True)

        if (exitCode == 0):
            cmd = []
            cmd.append("hcitool")
            cmd.append("-i")
            cmd.append("hci0")
            cmd.append("scan")

            exitCode, stdout, stderr = shellCommand.execute(cmd)

            if (exitCode == 0):
                if processScanList(stdout):
                    cmd = []
                    cmd.append("hciconfig")
                    cmd.append("hci0")
                    cmd.append("down")
                    
                    exitCode, stdout, stderr = shellCommand.execute(cmd)

        return exitCode

    # Windows
    elif os.name == "nt":

        cmd = []
        cmd.append(configInfo.getRootFolder() + os.path.join(os.sep, "bin", "bluetoothutil"))
        cmd.append("-s")

        exitCode, stdout, stderr = shellCommand.execute(cmd)
        
        return exitCode

if __name__ == "__main__":
    #data = """Scanning ...
    #        68:A3:C4:49:A5:04            LGGWW5Q1
    #        00:02:76:3E:05:21             HP Bluetooth Mouse X4000b"""
    #if processScanList(data):
    #    print 'Found bluetooth device'
    exitCode = bluetoothScan()
    sys.exit(exitCode)
