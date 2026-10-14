#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# probably working in python3

import sys
import os

if sys.executable.endswith("pythonw.exe"):
    sys.stdout = open(os.devnull, "w");
    sys.stderr = open(os.path.join("C:\Program Files\Hobart", "stderr-" + os.path.basename(sys.argv[0])), "w")
import shellCommand
import configInfo
import string
import subprocess, shlex
import bootSettings
# import osUtils
import time
import platform
from threading import Timer

# import logging

if platform.system() == 'Linux':
    pass
    # logging.basicConfig(filename='/tmp/connectNetworkStartupNew.log', filemode='w', level=logging.DEBUG, format='%(asctime)s %(message)s', datefmt='%m/%d/%Y %I:%M:%S %p')
else:
    pass
    # logging.basicConfig(filename='C:\\Program Files\\Hobart\\startseq.log', filemode='w', level=logging.DEBUG, format='%(asctime)s %(message)s', datefmt='%m/%d/%Y %I:%M:%S %p')

# logging.debug('Beginning of connectNetworkStartupNew logging')

# THIS FILE NEEDS TO BE IN ALL HT versions, it is HTi and HTx safe
# tries to connect to a netowrk up to 15 times, use during system startup

pythonInstallPath = os.path.join("usr", "bin")
if os.name == 'nt':
    pythonInstallPath = configInfo.getPythonInstallPath()
pythonScriptPath = os.path.join(configInfo.getRootFolder(), "python")


def getScaleSettings():
    # print "Hello"
    scaleSettingsPath = configInfo.getRootFolder() + os.path.join(os.sep, 'db', 'settingswizard', 'scalesettings')
    scaleSettingsBackupPath = configInfo.getRootFolder() + os.path.join(os.sep, 'db', 'settingswizard',
                                                                        'scalesettingsBACKUP')

    scaleSettings = {}

    scaleSettingsExist = os.path.exists(scaleSettingsPath)
    dbFile = open(scaleSettingsPath, "r")

    for line in dbFile:
        line = line.strip()
        keyValuePair = line.split('=')
        # print(line)
        # print "XXX"
        # print("length = " + keyValuePair.count())
        # print "VV"
        if len(keyValuePair[0]) == 0:
            # print "about to fail"
            # keyValuePair[0] = "646464"
            # keyValuePair[1] = "646464"
            pass
        # print "BB"
        # if len(keyValuePair) < 2:
        #    keyValuePair[1] = ""
        # print("Value = " + keyValuePair[1])

        if keyValuePair[0] == "oBrightness":
            # set operator brightness
            # print("operator is original-" + str(keyValuePair[1]) + " new-" + str(convertStr(keyValuePair[1])))
            scaleSettings['oBrightness'] = keyValuePair[1]

        elif keyValuePair[0] == "cBrightness":
            # set customer brightness
            # print("customer is original-" + str(keyValuePair[1]) + " new-" + str(convertStr(keyValuePair[1])))
            scaleSettings['cBrightness'] = keyValuePair[1]

        elif keyValuePair[0] == "cVolume":
            scaleSettings['cVolume'] = keyValuePair[1]

        elif keyValuePair[0] == "dimDisplayTimeout":
            scaleSettings['dimDisplayTimeout'] = keyValuePair[1]

        elif keyValuePair[0] == "connectionType":
            scaleSettings['connectionType'] = keyValuePair[1]

        elif keyValuePair[0] == "connectionName":
            scaleSettings['connectionName'] = keyValuePair[1]

        elif keyValuePair[0] == "oVolume":
            if keyValuePair[1].strip() == 'on':
                scaleSettings['oVolume'] = "128"
            else:
                scaleSettings['oVolume'] = "0"
        elif keyValuePair[0] == "gcd":
            scaleSettings['gcd'] = keyValuePair[1]
        elif keyValuePair[0] == "isStatic":
            scaleSettings['isStatic'] = keyValuePair[1]
        elif keyValuePair[0] == "timeKeeper":
            scaleSettings['timeKeeper'] = keyValuePair[1]
        elif keyValuePair[0] == "timezone":
            scaleSettings['timezone'] = keyValuePair[1]
        elif keyValuePair[0] == "ntpServer":
            scaleSettings['ntpServer'] = keyValuePair[1]
        elif keyValuePair[0] == "ntpFrequency":
            scaleSettings['ntpFrequency'] = keyValuePair[1]
        elif keyValuePair[0] == "ntpDayOfWeek":
            scaleSettings['ntpDayOfWeek'] = keyValuePair[1]
        elif keyValuePair[0] == "ntpTimeOfDay":
            scaleSettings['ntpTimeOfDay'] = keyValuePair[1]
        elif keyValuePair[0] == "ntpTimeZone":
            scaleSettings['ntpTimeZone'] = keyValuePair[1]
        elif keyValuePair[0] == "gateway":
            scaleSettings['gateway'] = keyValuePair[1]
        elif keyValuePair[0] == "netmask":
            scaleSettings['netmask'] = keyValuePair[1]
        elif keyValuePair[0] == "ipAddress":
            scaleSettings['ipAddress'] = keyValuePair[1]
        elif keyValuePair[0] == "autoDNS":
            scaleSettings['autoDNS'] = keyValuePair[1]
        elif keyValuePair[0] == "dns1":
            scaleSettings['dns1'] = keyValuePair[1]
        elif keyValuePair[0] == "dns2":
            scaleSettings['dns2'] = keyValuePair[1]
        elif keyValuePair[0] == "dns3":
            scaleSettings['dns3'] = keyValuePair[1]
        elif keyValuePair[0] == "security":
            scaleSettings['security'] = keyValuePair[1]
        elif keyValuePair[0] == "eapUsername":
            scaleSettings['eapUsername'] = keyValuePair[1]
        elif keyValuePair[0] == "eapPassword":
            scaleSettings['eapPassword'] = keyValuePair[1]
        elif keyValuePair[0] == "wpaPassphrase":
            scaleSettings['wpaPassphrase'] = keyValuePair[1]
        elif keyValuePair[0] == "encryption":
            scaleSettings['encryption'] = keyValuePair[1]
        elif keyValuePair[0] == "linkRate":
            scaleSettings['linkRate'] = keyValuePair[1]
        elif keyValuePair[0] == "automaticLinkRate":
            scaleSettings['automaticLinkRate'] = keyValuePair[1]
        elif keyValuePair[0] == "channel":
            scaleSettings['channel'] = keyValuePair[1]
        elif keyValuePair[0] == "serverCertFilename":
            scaleSettings['serverCertFilename'] = keyValuePair[1]
        elif keyValuePair[0] == "clientCertFilename":
            scaleSettings['clientCertFilename'] = keyValuePair[1]
        elif keyValuePair[0] == "clientCertUsersFilename":
            scaleSettings['clientCertUsersFilename'] = keyValuePair[1]
        elif keyValuePair[0] == "clientCertPassword":
            scaleSettings['clientCertPassword'] = keyValuePair[1]
        elif keyValuePair[0] == "DNSSuffix1":
            scaleSettings['DNSSuffix1'] = keyValuePair[1]
        elif keyValuePair[0] == "DNSSuffix2":
            scaleSettings['DNSSuffix2'] = keyValuePair[1]
        elif keyValuePair[0] == "DNSSuffix3":
            scaleSettings['DNSSuffix3'] = keyValuePair[1]
        elif keyValuePair[0] == "DNSSuffix4":
            scaleSettings['DNSSuffix4'] = keyValuePair[1]
        elif keyValuePair[0] == "DNSSuffix5":
            scaleSettings['DNSSuffix5'] = keyValuePair[1]
        elif keyValuePair[0] == "DNSSuffix6":
            scaleSettings['DNSSuffix6'] = keyValuePair[1]
        elif keyValuePair[0] == "hostname":
            scaleSettings['hostname'] = keyValuePair[1]
        # print("key = " + keyValuePair[0])
    dbFile.close()

    return scaleSettings


def getLinuxWEPKeys():
    WEPKeysPath = os.path.join(os.sep, 'etc', 'wpa_supplicant.conf')

    WEPKeys = {}

    WEPKeysPathExist = os.path.exists(WEPKeysPath)
    if not WEPKeysPathExist or os.path.getsize(WEPKeysPath) == 0:
        return WEPKeys

    dbFile = open(WEPKeysPath, "r")

    for line in dbFile:
        line = line.strip()
        keyValuePair = line.split('=')
        if keyValuePair[0] == "wep_tx_keyidx":
            WEPKeys['defaultKey'] = keyValuePair[1]

        elif keyValuePair[0] == "wep_key0":
            WEPKeys['key1'] = keyValuePair[1]

        elif keyValuePair[0] == "wep_key1":
            WEPKeys['key2'] = keyValuePair[1]

        elif keyValuePair[0] == "wep_key2":
            WEPKeys['key3'] = keyValuePair[1]

        elif keyValuePair[0] == "wep_key3":
            WEPKeys['key4'] = keyValuePair[1]

    dbFile.close()

    return WEPKeys


def kill_proc(proc, timeout):
    timeout["value"] = True
    print("killing process now")
    proc.kill()


def connectToNetwork():
    if os.name == 'posix':
        # this routine is just a stub now
        exitCode = 0

        doIt = False
        connected = "NO"
        commandFileName = ""
        try:
            if os.path.exists("/usr/local/hobart/db/settingswizard/networkCommandsAtBoot.txt"):
                # print "netCommandsatboot exists"
                commandFileName = "/usr/local/hobart/db/settingswizard/networkCommandsAtBoot.txt"
                doIt = True
            else:
                # print "networkCommandsAtBoot.txt does not exist"

                # this would be from an upgrade from a version before setNEtwork is being called 20 times at boot
                # we have to create the file from scratch based on the content of scalesettings

                WEPKeysExist = False
                settings = getScaleSettings()

                contents = "\"/usr/local/hobart/python/setNetwork.py\""
                contents += " \"-c\" \"0\""
                contents += " \"-I\" \"" + settings['ipAddress'] + "\""
                contents += " \"-s\" \"" + settings['isStatic'] + "\""
                contents += " \"-D\" \"" + settings['autoDNS'] + "\""
                contents += " \"-B\" \"" + settings['netmask'] + "\""
                contents += " \"-G\" \"" + settings['gateway'] + "\""
                contents += " \"-1\" \"" + settings['dns1'] + "\""
                contents += " \"-2\" \"" + settings['dns2'] + "\""
                contents += " \"-3\" \"" + settings['dns3'] + "\""
                contents += " \"-W\" \"" + settings['connectionType'] + "\""
                contents += " \"-A\" \"" + settings['wpaPassphrase'] + "\""
                contents += " \"-S\" \"" + settings['connectionName'] + "\""
                contents += " \"-M\" "

                # a mapping for security method
                # the value is numbered in scalesettings but setNetwork wants the text
                if settings['security'] == "0":
                    contents += "\"OPEN\""
                elif settings['security'] == "1":
                    contents += "\"WPA-Personal\""
                elif settings['security'] == "2":
                    contents += "\"WPA EAP-TLS\""
                elif settings['security'] == "3":
                    contents += "\"WPA-Enterprise (PEAP)\""
                elif settings['security'] == "4":
                    contents += "\"WPA-Enterprise (LEAP)\""
                elif settings['security'] == "5":
                    contents += "\"WPA2-Personal\""
                elif settings['security'] == "6":
                    contents += "\"WPA2 EAP-TLS\""
                elif settings['security'] == "7":
                    contents += "\"WPA2-Enterprise (PEAP)\""
                elif settings['security'] == "8":
                    contents += "\"WPA2-Enterprise (LEAP)\""
                elif settings['security'] == "9":
                    contents += "\"802.1X (PEAP)\""
                elif settings['security'] == "10":
                    contents += "\"802.1X (LEAP)\""

                # a mapping for encryption algorithm
                # the value is numbered in scalesettings but setNetwork wants the text
                contents += " \"-E\" "
                if settings['security'] == "0":
                    if settings['encryption'] == "0":
                        contents += "\"WEP\""
                        # add wep keys too
                        # open up wpa_supplicant.conf and extract keys
                        if settings['connectionType'] == 1:
                            keys = getLinuxWEPKeys()

                            if len(keys) > 0:
                                # there are keys and file is not empty
                                WEPKeysExist = True
                                contents += " \"-w\" \"" + keys['key1'] + "\""
                                contents += " \"-x\" \"" + keys['key2'] + "\""
                                contents += " \"-y\" \"" + keys['key3'] + "\""
                                contents += " \"-z\" \"" + keys['key4'] + "\""
                                contents += " \"-K\" \"" + keys['defaultKey'] + "\""

                    elif settings['encryption'] == "1":
                        contents += "\"None\""
                elif settings['security'] == "9" or settings['security'] == "10":
                    contents += "\"WEP\""
                else:
                    if settings['encryption'] == "0":
                        contents += "\"AES\""
                    elif settings['encryption'] == "1":
                        contents += "\"TKIP\""

                contents += " \"-V\" \"0\""  # VPN User not used yet
                contents += " \"-P\" \"0\""  # VPN Password not used yet
                contents += " \"-v\" \"0\""

                # WEP KEYs below
                # must get these from wpa_supplicant.conf file
                if not WEPKeysExist:
                    contents += " \"-w\" \"0\""
                    contents += " \"-x\" \"0\""
                    contents += " \"-y\" \"0\""
                    contents += " \"-z\" \"0\""
                    contents += " \"-K\" \"0\""

                contents += " \"-N\" \"" + settings['eapUsername'] + "\""
                contents += " \"-n\" \"" + settings['eapPassword'] + "\""
                contents += " \"-X\" \"0\""
                contents += " \"-a\" \"" + settings['linkRate'] + "\""
                contents += " \"-b\" \"" + settings['automaticLinkRate'] + "\""
                contents += " \"-d\" \"" + settings['channel'] + "\""

                if 'DNSSuffix1' in settings:
                    contents += " \"-5\" \"" + settings['DNSSuffix1'] + "\""
                else:
                    contents += " \"-5\" \"0\""
                if 'DNSSuffix2' in settings:
                    contents += " \"-6\" \"" + settings['DNSSuffix2'] + "\""
                else:
                    contents += " \"-6\" \"0\""
                if 'DNSSuffix3' in settings:
                    contents += " \"-7\" \"" + settings['DNSSuffix3'] + "\""
                else:
                    contents += " \"-7\" \"0\""
                if 'DNSSuffix4' in settings:
                    contents += " \"-8\" \"" + settings['DNSSuffix4'] + "\""
                else:
                    contents += " \"-8\" \"0\""
                if 'DNSSuffix5' in settings:
                    contents += " \"-9\" \"" + settings['DNSSuffix5'] + "\""
                else:
                    contents += " \"-9\" \"0\""
                if 'DNSSuffix6' in settings:
                    contents += " \"-4\" \"" + settings['DNSSuffix6'] + "\""
                else:
                    contents += " \"-4\" \"0\""

                # Now add the EAP TLS CERTS

                if 'serverCertFilename' in settings:
                    contents += " \"-L\" \"" + settings['serverCertFilename'] + "\""
                if 'clientCertUsersFilename' in settings:
                    contents += " \"-Z\" \"" + settings['clientCertUsersFilename'] + "\""

                # now write the file
                commandFileName = "/usr/local/hobart/db/settingswizard/networkCommandsAtBoot.txt"
                doIt = True

                with open(commandFileName, "w") as text_file:
                    text_file.write("%s" % contents)
        except:
            pass
            # print "Failed"
        doIt = True
        if doIt:
            iterator = 0
            timeout_sec = 60

            timeout = 60
            try:
                f = open(commandFileName, "r")

                line = f.readline()
                cmd = 'python ' + line
                proc = subprocess.Popen(shlex.split(cmd), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                timeout = {"value": False}
                timer = Timer(timeout_sec, kill_proc, [proc, timeout])
                timer.start()
                stdout, stderr = proc.communicate()
                stdout = str(stdout, encoding="utf-8")
                timer.cancel()
                stdout = stdout.split('\n')
                for lin in stdout:
                    if lin.find("YES") != -1:
                        connected = "YES"
                    # print "connected"

                f.close()
                connected = "YES"
            except:
                connected = "NO"
            iterator = iterator + 1

        # print "finished"

        if connected == "NO":
            exitCode = 0
        elif connected == "YES":
            exitCode = 0

    elif os.name == 'nt':
        exitCode = 0
        time.sleep(11)
        enabledAdapterName = ""
        configSettings = bootSettings.getBootSettings()
        if os.path.exists("C:\\Program Files\\Hobart\\Db\\settingswizard\\lastNetworkRun.txt"):
            connected = False
            numberOfTries = 0
            connectedSearchString = "YES"
            while not connected and numberOfTries < 20:
                try:
                    logging.debug("connectNetworkStartupNew: Starting try# %d in connectToNetwork()", numberOfTries)
                    f = open('C:\\Program Files\\Hobart\\Db\\settingswizard\\lastNetworkRun.txt', "r")
                    line = f.readline()

                    cmd = 'pythonw ' + line

                    info = subprocess.STARTUPINFO()
                    info.dwFlags |= subprocess.STARTF_USESHOWWINDOW
                    info.wShowWindow = subprocess.SW_HIDE

                    numberOfTries = numberOfTries + 1
                    if numberOfTries > 1:
                        time.sleep(2)
                    p = subprocess.Popen(cmd, startupinfo=info, stdout=subprocess.PIPE, shell=False)
                    out, err = p.communicate()

                    if out.find(connectedSearchString) > -1:
                        logging.debug("connectNetworkStartupNew: CONNECTED")
                        connected = True
                    else:
                        logging.debug("connectNetworkStartupNew: NOT CONNECTED")

                    f.close()
                except:
                    pass
        else:
            logging.debug("connectNetworkStartupNew: lastNetworkRun.txt DOES NOT EXIST")
            pass

        # sys.exit(exitCode)


if __name__ == "__main__":
    exitCode = 0
    connectToNetwork()
    sys.exit(exitCode)
