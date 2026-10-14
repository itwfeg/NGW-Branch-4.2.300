#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#######################################
#
#  Use of this python script will configure the HT scale
#  to the values currently stored in the scalesettings file
#  located in the directory define below.
#
#######################################

import sys
import os
import time
import shellCommand
import argparse
import sendDeviceEventMessage
import math
import subprocess
import setBrightness
import setVolume
import setAudioDevice
import beep
import sampleBeep
import shutil
import ntp

# Let's get a snapshot of all of the values in the scalesettings file
def getScaleSettings():
    scaleSettings = {}
    copyDBFile = False
    deleteDBFile = False
    if not os.path.exists("/usr/local/hobart/db/settingswizard/scalesettings"):
        copyDBFile = True
        deleteDBFile = False

    else:
        sizeOfDBFile = os.path.getsize("/usr/local/hobart/db/settingswizard/scalesettings")
        if sizeOfDBFile == 0:
            copyDBFile = True

    if copyDBFile:
        if deleteDBFile:
            os.remove("/usr/local/hobart/db/settingswizard/scalesettings")
        shutil.copy2('/usr/local/hobart/db/settingswizard/scalesettingsBACKUP', '/usr/local/hobart/db/settingswizard/scalesettings')

    dbFile = open("/usr/local/hobart/db/settingswizard/scalesettings", "r")
    for line in dbFile:
        line = line.strip()
        keyValuePair = line.split('=')
        ####################################################################
        #1
        if keyValuePair[0] == "cBrightness":
            # set customer brightness
            scaleSettings['cBrightness'] = str(convertStr(keyValuePair[1]))
        ####################################################################
        #2
        elif keyValuePair[0] == "oBrightness":
            #set operator brightness
            scaleSettings['oBrightness'] = str(convertStr(keyValuePair[1]))
        ####################################################################
        #3
        elif keyValuePair[0] == "dimDisplayTimeout":
            scaleSettings['dimDisplayTimeout'] = keyValuePair[1]
        ####################################################################
        #4
        elif keyValuePair[0] == "cVolume":
            scaleSettings['cVolume'] = keyValuePair[1]
        ####################################################################
        #5
        elif keyValuePair[0] == "oVolume":
            scaleSettings['oVolume'] = keyValuePair[1]
        ####################################################################
        #6
        elif keyValuePair[0] == "isAstronomical":
            scaleSettings['cVolume'] = keyValuePair[1]
        ####################################################################
        #7
        elif keyValuePair[0] == "connectionType":
            scaleSettings['connectionType'] = keyValuePair[1]
        ####################################################################
        #8
        elif keyValuePair[0] == "connectionName":
            scaleSettings['connectionName'] = keyValuePair[1]
        ####################################################################
        #9
        elif keyValuePair[0] == "gcd":
            scaleSettings['gcd'] = keyValuePair[1]
        ####################################################################
        #10
        elif keyValuePair[0] == "timezone":
            scaleSettings['timezone'] = keyValuePair[1]
        ####################################################################
        #11
        elif keyValuePair[0] == "isStatic":
            scaleSettings['isStatic'] = keyValuePair[1]
        ####################################################################
        #12
        elif keyValuePair[0] == "gateway":
            scaleSettings['gateway'] = keyValuePair[1]
        ####################################################################
        #13
        elif keyValuePair[0] == "netmask":
            scaleSettings['netmask'] = keyValuePair[1]
        ####################################################################
        #14
        elif keyValuePair[0] == "ipAddress":
            scaleSettings['ipAddress'] = keyValuePair[1]
        ####################################################################
        #15
        elif keyValuePair[0] == "autoDNS":
            scaleSettings['autoDNS'] = keyValuePair[1]
        ####################################################################
        #16
        elif keyValuePair[0] == "dns1":
            scaleSettings['dns1'] = keyValuePair[1]
        ####################################################################
        #17
        elif keyValuePair[0] == "dns2":
            scaleSettings['dns2'] = keyValuePair[1]
        ####################################################################
        #18
        elif keyValuePair[0] == "dns3":
            scaleSettings['dns3'] = keyValuePair[1]
        ####################################################################
        #19
        elif keyValuePair[0] == "security":
            scaleSettings['security'] = keyValuePair[1]
        ####################################################################
        #20
        elif keyValuePair[0] == "encryption":
            scaleSettings['encryption'] = keyValuePair[1]
        ####################################################################
        #21
        elif keyValuePair[0] == "firstTimeExport":
            scaleSettings['firstTimeExport'] = keyValuePair[1]
        ####################################################################
        #22
        elif keyValuePair[0] == "displayTab":
            scaleSettings['displayTab'] = keyValuePair[1]
        ####################################################################
        #23
        elif keyValuePair[0] == "customerSoundTab":
            scaleSettings['customerSoundTab'] = keyValuePair[1]
        ####################################################################
        #24
        elif keyValuePair[0] == "operatorSoundTab":
            scaleSettings['operatorSoundTab'] = keyValuePair[1]
        ####################################################################
        #25
        elif keyValuePair[0] == "timeTab":
            scaleSettings['timeTab'] = keyValuePair[1]
        ####################################################################
        #26
        elif keyValuePair[0] == "networkTab":
            scaleSettings['networkTab'] = keyValuePair[1]
        ####################################################################
        #27
        elif keyValuePair[0] == "hostTab":
            scaleSettings['hostTab'] = keyValuePair[1]
        ####################################################################
        #28
        elif keyValuePair[0] == "timeKeeper":
            scaleSettings['timeKeeper'] = keyValuePair[1]
        ####################################################################
        #29
        elif keyValuePair[0] == "ntpServer":
            scaleSettings['ntpServer'] = keyValuePair[1]
        ####################################################################
        #30
        elif keyValuePair[0] == "ntpFrequency":
            scaleSettings['ntpFrequency'] = keyValuePair[1]
        ####################################################################
        #31
        elif keyValuePair[0] == "ntpDayOfWeek":
            scaleSettings['ntpDayOfWeek'] = keyValuePair[1]
        ####################################################################
        #32
        elif keyValuePair[0] == "ntpTimeOfDay":
            scaleSettings['ntpTimeOfDay'] = keyValuePair[1]
        ####################################################################
        #33
        elif keyValuePair[0] == "ntpTimeZone":
            scaleSettings['ntpTimeZone'] = keyValuePair[1]
        ####################################################################
        #34
        elif keyValuePair[0] == "ntpAntiMeridian":
            scaleSettings['ntpAntiMeridian'] = keyValuePair[1]
        ####################################################################
        #35
        elif keyValuePair[0] == "wpaPassphrase":
            scaleSettings['wpaPassphrase'] = keyValuePair[1]
        ####################################################################
        #36
        elif keyValuePair[0] == "eapUsername":
            scaleSettings['eapUsername'] = keyValuePair[1]
        ####################################################################
        #37
        elif keyValuePair[0] == "eapPassword":
            scaleSettings['eapPassword'] = keyValuePair[1]
        ####################################################################
        #38
        elif keyValuePair[0] == "outputSpeaker":
            scaleSettings['outputSpeaker'] = keyValuePair[1]

    dbFile.close()
    return scaleSettings

# Converts the percentage of
def convertStr(s):
    return str(int(s) + 104)

if __name__ == "__main__":

    scaleSettings = getScaleSettings()

    #################################################################
    # Set the operator display brightness
    exitCode = setBrightness.linuxSetBacklight(value=scaleSettings['oBrightness'], device=2)

    #################################################################
    #Set the customer display brightness
    if scaleSettings['gcd'] == "0":
        exitCode = setBrightness.linuxSetBacklight(value=0, device=1)
    else:
        exitCode = setBrightness.linuxSetBacklight(value=scaleSettings['cBrightness'], device=1)

    #################################################################
    # set the speaker volume
    exitCode = setVolume.volumeSetting(scaleSettings['cVolume'])

    #################################################################
    # set the touch beeper on or off
    oVolume = ""
    if scaleSettings['oVolume'] == "on":
        oVolume = "128"
    elif scaleSettings['oVolume'] == "off":
        oVolume = "0"
    exitCode = sampleBeep.linuxBeep(str(oVolume))
    exitCode = sendDeviceEventMessage.sendMessage(server='localhost',port=5077,message='beeperVolume=%s' % (str(oVolume)))
    time.sleep(0.5)
    exitCode = sampleBeep.linuxBeep("0")

    #################################################################
    # set the customer speaker output device USB or internal
    if scaleSettings['outputSpeaker'] == "0":
        # internal
        exitCode = setAudioDevice.setAudioDevice("internal")
    elif scaleSettings['outputSpeaker'] == "1":
        # external
        exitCode = setAudioDevice.setAudioDevice("external")

    if scaleSettings['timeKeeper'].upper() != 'MANUAL':

        # call set ntp to set the parameters for the NTP server
        cmd = []
        cmd.append("/usr/bin/python")
        cmd.append("setNTP.py")

        cmd.append("-F")
        cmd.append(scaleSettings['ntpFrequency'])
        cmd.append("-D")
        cmd.append(scaleSettings['ntpDayOfWeek'])
        cmd.append("-H")
        cmd.append(scaleSettings['ntpTimeOfDay'][:2])
        cmd.append("-M")
        cmd.append(scaleSettings['ntpTimeOfDay'][3:5])
        cmd.append("-T")
        cmd.append(scaleSettings['ntpTimeZone'])

        exitCode, stdout = shellCommand.execute(cmd)

        if exitCode == 0:

            # set the time zone on the scale to specified timezone
            cmd = []
            cmd.append("/usr/bin/python")
            cmd.append("setTimezone.py")
            cmd.append("-t")
            cmd.append(scaleSettings['ntpTimeZone'])
            exitCode, stdout = shellCommand.execute(cmd)

            if exitCode == 0:

                # see if the NTP server can be pinged
                exitCode, stdout = ping.pingSystem(scaleSettings['ntpServer'], 1)

                if exitCode == 0:

                    # NTP server could be pinged so get current date/time from it
                    exitCode, month, day, year, hours, minutes, seconds = ntp.getCurrentTime(scaleSettings['ntpServer'])

                    if exitCode == 0:

                        # update the scales time based on NTP server
                        cmd = []
                        cmd.append("/usr/bin/python")
                        cmd.append("setTime.py")
                        cmd.append("-Y")
                        cmd.append(year)
                        cmd.append("-M")
                        cmd.append(month)
                        cmd.append("-D")
                        cmd.append(day)
                        cmd.append("-H")
                        cmd.append(hours)
                        cmd.append("-m")
                        cmd.append(minutes)
                        cmd.append('-S')
                        cmd.append(seconds)
                        cmd.append('-t')
                        cmd.append(scaleSettings['ntpTimeZone'])
                        cmd.append('-A')
                        cmd.append("0")

                        exitCode, stdout = shellCommand.execute(cmd)
    else:

        # must call this script with following data to set time manually
        # month|day|year|hours|minutes|seconds|timezone

        if len(sys.argv) == 2:
            tokens = sys.argv[1].split("|")
            month = tokens[0]
            day = tokens[1]
            year = tokens[2]
            hours = tokens[3]
            minutes = tokens[4]
            seconds = tokens[5]
            timezone = tokens[6]

            # Need to set the time to time passed in as arguments
            cmd = []
            cmd.append("/usr/bin/python")
            cmd.append("setTime.py")
            cmd.append("-Y")
            cmd.append(year)
            cmd.append("-M")
            cmd.append(month)
            cmd.append("-D")
            cmd.append(day)
            cmd.append("-H")
            cmd.append(hours)
            cmd.append("-m")
            cmd.append(minutes)
            cmd.append('-S')
            cmd.append(seconds)
            cmd.append('-t')
            cmd.append(timezone)
            cmd.append('-A')
            cmd.append("0")

            exitCode, stdout = shellCommand.execute(cmd)
