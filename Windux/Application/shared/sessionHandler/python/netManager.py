#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys, os
import argparse
import shellCommand
import time
import bootSettings
import configparser
import configInfo
import connectNetworkStartupNew

if os.name == 'posix':
    wirelessDevice = 'wlan0'
    wiredDevice = 'eth0'
else:
    wirelessDevice = 'Wireless Network Connection'
    wiredDevice = 'Local Area Connection'

def initialize(device, isStatic):
    exitCode = 0
    #print "starting netManager"
    if True:
        #print " getting into it"
        connectNetworkStartupNew.connectToNetwork()
        #print "getting out of it"
    else:
        exitCode = 1
        if os.name == "posix":
            try:
                if device == wirelessDevice and int(isStatic) == 1:
                    pass
                    
                    cmd = []
                    cmd.append("ifconfig")
                    cmd.append("wlan0")
                    cmd.append("up")
                    exitCode, stdout, stderr = shellCommand.execute(cmd)
                    time.sleep(5)
                    cmd = []
                    print("C")
                    time.sleep(2)
                    cmd.append("/etc/init.d/wpa_supplicant")
                    cmd.append("-Dwext")
                    cmd.append("-iwlan0")
                    cmd.append("-c/etc/wpa_supplicant.conf")
                    cmd.append("-B")
                    exitCode, stdout, stderr = shellCommand.execute(cmd)


                else:
                    if device == wirelessDevice and int(isStatic) == 0:
                        print("A")
                        
                        cmd = []
                        cmd.append("ifconfig")
                        cmd.append("wlan0")
                        cmd.append("up")
                        exitCode, stdout, stderr = shellCommand.execute(cmd)
                        time.sleep(1)

                        cmd = []
                        cmd.append("killall")
                        cmd.append("wpa_supplicant")
                        exitCode, stdout, stderr = shellCommand.execute(cmd)

                        cmd = []
                        cmd.append("killall")
                        cmd.append("dhcpcd")
                        exitCode, stdout, stderr = shellCommand.execute(cmd)
                        print("start")
                        cmd = []
                        cmd.append("killall")
                        cmd.append("wpa_cli")
                        exitCode, stdout, stderr = shellCommand.execute(cmd)

                        cmd = []
                        cmd.append("ifconfig")
                        cmd.append("wlan0")
                        cmd.append("up")
                        exitCode, stdout, stderr = shellCommand.execute(cmd)

                        print("startB")
                        cmd = []
                        cmd.append("iwconfig")
                        cmd.append("wlan0")
                        cmd.append("power")
                        cmd.append("off")
                        #exitCode, stdout, stderr = shellCommand.executeShellCommandBackground(cmd)

                    if device == wirelessDevice and int(isStatic) == 0:
                        print("B")
                        cmd = []
                        cmd.append("ifconfig")
                        cmd.append("wlan0")
                        cmd.append("up")
                        exitCode, stdout, stderr = shellCommand.execute(cmd)

                        cmd = []
                        print("C")
                        time.sleep(2)
                        cmd.append("/etc/init.d/wpa_supplicant")
                        cmd.append("-Dwext")
                        cmd.append("-iwlan0")
                        cmd.append("-c/etc/wpa_supplicant.conf")
                        cmd.append("-B")
                        exitCode, stdout, stderr = shellCommand.execute(cmd)


                        cmd = []
                        cmd.append("/etc/init.d/wpa_supplicantForSetupUtility")
                        cmd.append("start")
                        #exitCode, stdout, stderr = shellCommand.execute(cmd)
                        cmd = []
                        cmd.append("/usr/sbin/wpa_cli")
                        cmd.append("-a")
                        cmd.append("/usr/bin/wpa_actionForSetupUtility")
                        #exitCode, stdout, stderr = shellCommand.execute(cmd)

                        
                        cmd = []
                        time.sleep(2)
                        cmd.append("dhcpcd")
                        cmd.append("-A")
                        cmd.append("wlan0")
                        exitCode, stdout, stderr = shellCommand.executeShellCommandBackground(cmd)
                    else:
                        if device == wirelessDevice:
                            cmd = []
                            cmd.append("ifconfig")
                            cmd.append("wlan0")
                            cmd.append("up")
                            exitCode, stdout, stderr = shellCommand.execute(cmd)

                        if device == wirelessDevice:
                            print("here")
                            time.sleep(2)
                            cmd = []
                            cmd.append("/usr/sbin/wpa_supplicant")
                            cmd.append("-Dwext")
                            cmd.append("-i%s" % (device))
                            cmd.append("-c/etc/wpa_supplicant.conf")
                            cmd.append("-B")
                            exitCode, stdout, stderr = shellCommand.execute(cmd)
                            #time.sleep(8)

                        if int(isStatic) == 0:
                            cmd = []
                            cmd.append("dhcpcd")
                            cmd.append(str(device))
                            exitCode, stdout, stderr = shellCommand.execute(cmd)
                        else:
                            print("ConfigA")
                            configSettings = {}
                            print("ConfigB")
                
                            if device != wirelessDevice:
                                cmd = []
                                cmd.append("ifup")
                                cmd.append(str(device))
                                exitCode, stdout, stderr = shellCommand.execute(cmd)

                            hobartSettingsFile = "/usr/local/hobart/db/settingswizard/scalesettings"

                            settingsParser = configparser.SafeConfigParser()
                            #if os.path.exists(settingsFile):
                            #    settingsParser.read([settingsFile])

                            if os.path.exists(hobartSettingsFile):
                                bootSetting = {}

                                dbFile = open(hobartSettingsFile, "r")

                                for line in dbFile:
                                    line = line.strip()
                                    keyValuePair = line.split('=')

                                    if keyValuePair[0] == "gateway":
                                        print("found a gateway")
                                        bootSetting['gateway'] = str(keyValuePair[1])

                                dbFile.close()

                            ######################################################
                            print("gatewaydd")
                            cmd = []
                            cmd.append("ip")
                            cmd.append("route")
                            cmd.append("add")
                            cmd.append("default")
                            cmd.append("via")
                            cmd.append(str(bootSetting['gateway']))
                            #cmd.append(str(configSettings['gateway']))
                            print((str(cmd)))
                            time.sleep(4)                    
                            exitCode, stdout, stderr = shellCommand.execute(cmd)
             
            except:
                print("Error")
                exitCode = 1

        else:
            try:
                
                import ntNetworkUtils
                
                # WHY ON EARTH IS THERE ANOTHER VERSION OF THE scalesettings FILE?
                settingsFile = os.path.join(configInfo.getRootFolder(), 'db', 'settingswizard', 'networkSettings.cfg')

                hobartSettingsFile = "C:\Program Files\Hobart\Db\SettingsWizard\scalesettings"

                settingsParser = configparser.SafeConfigParser()
                #if os.path.exists(settingsFile):
                #    settingsParser.read([settingsFile])

                if os.path.exists(hobartSettingsFile):
                    bootSettings = {}

                    dbFile = open(hobartSettingsFile, "r")

                    for line in dbFile:
                        line = line.strip()
                        keyValuePair = line.split('=')
                        if keyValuePair[0] == "oBrightness":
                            bootSettings['oBrightness'] = str(keyValuePair[1])

                        elif keyValuePair[0] == "cBrightness":
                            bootSettings['cBrightness'] = str(keyValuePair[1])

                        elif keyValuePair[0] == "connectionType":
                            bootSettings['connectionType'] = str(keyValuePair[1])

                        elif keyValuePair[0] == "connectionName":
                            bootSettings['connectionName'] = str(keyValuePair[1])

                        elif keyValuePair[0] == "isStatic":
                            bootSettings['isStatic'] = str(keyValuePair[1])

                        elif keyValuePair[0] == "gateway":
                            bootSettings['gateway'] = str(keyValuePair[1])

                        elif keyValuePair[0] == "ipAddres":
                            bootSettings['ipAddress'] = str(keyValuePair[1])

                        elif keyValuePair[0] == "netmask":
                            bootSettings['netmask'] = str(keyValuePair[1])

                        elif keyValuePair[0] == "autoDNS":
                            bootSettings['autoDNS'] = str(keyValuePair[1])

                        elif keyValuePair[0] == "dns1":
                            bootSettings['dns1'] = str(keyValuePair[1])
                        elif keyValuePair[0] == "dns2":
                            bootSettings['dns2'] = str(keyValuePair[1])
                        elif keyValuePair[0] == "dns3":
                            bootSettings['dns3'] = str(keyValuePair[1])

                    dbFile.close()

                ######################################################
                disabledDevice = ""

                if str(bootSettings['connectionType']) == "1":
                    disabledDevice = wiredDevice
                else:
                    disabledDevice = wirelessDevice
                keepGoing = True

                #if str(bootSettings['isStatic']) == "0":
                #    ntNetworkUtils.enableDHCPClientService(True)

                if str(bootSettings['connectionType']) == "0" and str(bootSettings['isStatic']) == "1":
                    if str(bootSettings['ipAddress']) == "0.0.0.0" or str(bootSettings['ipAddress']) == "646464" or str(bootSettings['ipAddress']) == "" or str(bootSettings['ipAddress']) == "...":
                        #ntNetworkUtils.enableDHCPClientService(False)
                        ntNetworkUtils.setAdapterEnabled(disabledDevice,  False)
                        ntNetworkUtils.setAdapterEnabled(device, False)
                        keepGoing = False

                if keepGoing:
                    # disable appropriate device
                    #ntNetworkUtils.setAdapterEnabled(disabledDevice,  False)

                    # enable appropriate device
                    #ntNetworkUtils.setAdapterEnabled(device,  True)

                    # handle address
                    if str(bootSettings['isStatic']) == '1':
                        # static addresssing
                        #ntNetworkUtils.enableDHCPClientService(False)
                        addressSection = 'address'

                        ipAddress = str(bootSettings['ipAddress'])
                        subnetMask = str(bootSettings['netmask'])
                        gateway = str(bootSettings['gateway'])
                        #if settingsParser.has_section(addressSection):
                        #    ipAddress = settingsParser.get(addressSection,  'ipAddress')
                        #    subnetMask = settingsParser.get(addressSection,  'subnetMask')
                        #    gateway = settingsParser.get(addressSection,  'gateway')

                        ntNetworkUtils.setAddress(device,  ipAddress,  subnetMask, gateway)

                    else:
                        # dhcp addressing
                        #ntNetworkUtils.enableDHCPClientService(True)
                        ntNetworkUtils.setDhcp(device)

                    # handle dns server settings
                    dnsServersSection = 'dnsservers'

                    # DNS section
                    ntNetworkUtils.deleteAllDnsServers(device)

                    if str(bootSettings['autoDNS']) == "0":
                        if str(bootSettings['dns1']) != "0.0.0.0" and str(bootSettings['dns1']) != "":
                            ntNetworkUtils.addDnsServer(str(bootSettings['dns1']))
                        if str(bootSettings['dns2']) != "0.0.0.0" and str(bootSettings['dns2']) != "":
                            ntNetworkUtils.addDnsServer(str(bootSettings['dns2']))
                        if str(bootSettings['dns3']) != "0.0.0.0" and str(bootSettings['dns3']) != "":
                            ntNetworkUtils.addDnsServer(str(bootSettings['dns3']))



                    #if settingsParser.has_section(dnsServersSection):
                    #    # static dns servers
                    #    #ntNetworkUtils.deleteAllDnsServers(device)

                    #    ntNetworkUtils.addDnsServer(settingsParser.get(dnsServersSection,  'primary'))
                    #    if settingsParser.has_option(dnsServersSection,  'secondary'):
                    #        ntNetworkUtils.addDnsServer(settingsParser.get(dnsServersSection,  'secondary'))
                    #    if settingsParser.has_option(dnsServersSection,  'tertiary'):
                    #        ntNetworkUtils.addDnsServer(settingsParser.get(dnsServersSection,  'tertiary'))

                    else:
                        # auto dns servers
                        ntNetworkUtils.setAutoDns(device)
                    #
                    if device == wirelessDevice:
                        # make sure deny all filter is off
                        ntNetworkUtils.setWlanDenyallFilter(False)

                        wirelessSection = 'wireless'
                        wirelessProfileName = settingsParser.get(wirelessSection,  'profileName')
                        wirelessProfileXMLFile = settingsParser.get(wirelessSection,  'profileXMLFile')

                        # setup profile
                        ntNetworkUtils.deleteAllProfilesForAdapter('wlan', device)
                        ntNetworkUtils.addProfileForAdapter('wlan', device,  wirelessProfileXMLFile)

                        # If PEAP, also need to import EAPUserProfile.xml
                        import re
                        securityMethod = settingsParser.get(wirelessSection,  'securityMethod')
                        if re.search("\b[Pp][Ee][Aa][Pp]\b",  securityMethod):
                            wirelessEAPUserDataXMLFile = settingsParser.get(wirelessSection,  'eapUserDataXMLFile')
                            ntNetworkUtils.importEapProfile(wirelessEAPUserDataXMLFile,  wirelessProfileName)

                        # connect
                        ssid = settingsParser.get(wirelessSection,  'ssid')
                        ntNetworkUtils.connectWlan(device,  wirelessProfileName,  ssid)

            except:
                exitCode = 1

    return exitCode

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="initialize wireless device management")
    parser.add_argument("device",
                        action="store",
                        help="wireless device name")
    parser.add_argument("isStatic",
                        action="store",
                        help="1 = staticIP, 0 = DHCP")

    args = parser.parse_args()

    exitCode  = initialize(args.device, args.isStatic)
    sys.exit(exitCode)
