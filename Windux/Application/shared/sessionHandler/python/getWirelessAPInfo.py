#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys, os, string
from optparse import OptionParser
import shlex
import time, subprocess
import shellCommand
import socket, struct
if os.name == "posix":
    import fcntl
import shutil
import re
import createWirelessProfile
import math

if os.name == "posix":
    f = open("apScans.txt" , "w")

    try:
        stdout = ""
        cmd = []
        cmd.append("iwlist")
        #cmd.append("dev")
        cmd.append("wlan0")
        cmd.append("scan")
        exitCode, stdout, stderr = shellCommand.execute(cmd, shell=True)
        #f.write("iwlist wlan0 scan\n")
        #print stdout
        SSIDList = [];
        linestrings = ""
        includeIt = True;

        #print(linestrings)
        #print("\n\n")
        strengthNumber = "0"
        encrypt = "AES"
        eraseLast = False
        shouldFindAnSSID = False

        failedToRun = False
        #for line in linestrings.splitlines():
        answer = stdout
        
        if failedToRun:
            time.sleep(3)
            print("happeninig")
            cmd = []
            cmd.append("iwlist")
            #cmd.append("dev")
            cmd.append("wlan0")
            cmd.append("scan")
            exitCode, stdout, stderr = shellCommand.execute(cmd, shell=True)
            #f.write("iwlist wlan0 scan 1\n")
            #f.write("BBBBBBBBBBBBBBBB\n" + stdout + "\n")
            answer = stdout

        #f.write("XXXXXXXXXXXXXXX\n\n\n\n")
        #f.write("WWWWWWWWWWWWWWWWW\nWWWWWWWWWWW\n\noutside answer\n" + answer + "\n")
        #print stdout
        count = 0
        f.write(stdout)
        for line in stdout.splitlines():
            count = count + 1;
            #print("here")
            if line.find("ESSID:\"") > -1:
                shouldFindAnSSID = False
                s = line.split(":")
                if s[1].find("\"\"") > -1:
                    s = "N/A"
                else:
                    s = s[1].replace("\"", "")
                    s = s.rstrip()
                #if len(s[0]) > 0 and s[0] != '' and s[0] not in SSIDList:

                #print(str(len(s[0])) + '\n')
                #f.write("SSID: " + s[0] + "\n")
                SSIDList.append(s)
                #print s[0]
                includeIt = True

            elif line.find("Quality=") > -1:

                shouldFindAnSSID = True
                s = line.split("=")
                noise = s[2].split(" ")
                s = s[1].split(" ")
                s = s[0].split("/")

                percentage = 100 * int(s[0])/70

                if percentage < 21:
                    SSIDList.append('1')
                elif percentage < 41:
                    SSIDList.append('2')
                elif percentage < 61:
                    SSIDList.append('3')
                elif percentage < 81:
                    SSIDList.append('4')
                elif percentage < 110:
                    SSIDList.append('5')

                #SSIDList.append(str(percentage))
                #SSIDList.append(str(noise[0]))

                #SSIDList.append(s[0])
                #print s[0]
            elif line.find("Channel:") > -1:
                if shouldFindAnSSID:
                    SSIDList.pop()
                    SSIDList.pop()
                    SSIDList.pop()
                shouldFindAnSSID = True
                s = line.split(":")
                SSIDList.append(s[1])
                # Debug
                #print channelFound
            elif line.find("Frequency:") > -1:
                shouldFindAnSSID = True
                s = line.split(":")
                s = s[1].split("(")
                strg = s[0].rstrip()
                SSIDList.append(strg)

        #print(str(count))
        #testFile.close()
        if shouldFindAnSSID:
            SSIDList.pop()
            SSIDList.pop()
            SSIDList.pop()
            pass
        #print SSIDList
        returnMessage = ""
        for SSID in SSIDList:
            returnMessage += str(SSID) + "|"

        #if eraseLast:
        #    returnMessage = returnMessage[:-3]
        #powerNetworkDevices("30")
        #f.close()

        print((str(returnMessage.lstrip(' '))))
        sys.stdout.flush()
        #if failedToRun:
        #    cmd = []
        #    cmd.append("ifup")
        #    cmd.append("wlan0")
        #    exitCode, stdout, stderr = shellCommand.execute(cmd)
    except:
        #print("failed")
        #f.write("iwlist wlan0 scan failed\n")
        raise
    f.close()
else:
    TBD = "windows"
