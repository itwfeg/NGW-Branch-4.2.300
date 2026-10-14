#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
if sys.executable.endswith("pythonw.exe"):
  sys.stdout = open(os.devnull, "w");
  sys.stderr = open(os.path.join("C:\Program Files\Hobart", "stderr-"+os.path.basename(sys.argv[0])), "w")
import shellCommand
import configInfo
import subprocess
import ntNetworkUtils
import bootSettings
import time
import osUtils

pythonInstallPath = os.path.join("usr","bin")
if os.name == 'nt':
    pythonInstallPath = configInfo.getPythonInstallPath()
pythonScriptPath = os.path.join(configInfo.getRootFolder(),"python")

def connectToNetwork():

    exitCode = 0
    cmd = []
    if os.name == 'nt':
        enabledAdapterName = ""
        configSettings = bootSettings.getBootSettings()
        connectionType = int(configSettings['connectionType'])
        #print configSettings
        #time.sleep(2)
        if connectionType == 1:
            #print "WIFI"
            enabledAdapterName = "Wireless Network Connection"

            #Add certs if EAP-TLS
            '''if configSettings['security'] == "2" or configSettings['security'] == "6":
                print "made it"
                import ntCertificateUtils

                currentCertsInfo = ntCertificateUtils.getCurrentCertsInfo()
                #print currentCertsInfo

                for aCert in currentCertsInfo:
                    #print aCert
                    line = aCert.rstrip()
                    keyValuePair = line.split('=')
                    if keyValuePair[0] == "root":
                        aCertFilePath = keyValuePair[1]
                        if len(keyValuePair) == 4:
                            print "doing root"
                            if keyValuePair[3] == "NONE":
                                keyValuePair[3] = ""
                            try:
                                valid, answer = ntCertificateUtils.storeTrustedRootCertificate(aCertFilePath, keyValuePair[3])
                                ntCertificateUtils.addCAtoNTAuth(aCertFilePath)

                            except:
                                pass
                    elif keyValuePair[0] == "client":
                        aCertFilePath = keyValuePair[1]
                        if len(keyValuePair) == 4:
                            print "doing client"
                            if keyValuePair[3] == "NONE":
                                keyValuePair[3] = ""
                            try:
                                #time.sleep(10)
                                valid, answer = ntCertificateUtils.storeClientCertificate(aCertFilePath, keyValuePair[3])
                            except:
                                pass
            '''


            cmd = []
            cmd.append('sc')
            cmd.append('stop')
            cmd.append('dot3Svc')
            osUtils.winRunElevated(cmd)
            cmd = []
            cmd.append('sc')
            cmd.append('start')
            cmd.append('WlanSvc')
            osUtils.winRunElevated(cmd)
            #time.sleep(2)
            ntNetworkUtils.setAdapterEnabled("Local Area Connection",  False)
            #time.sleep(1)
            ntNetworkUtils.setAdapterEnabled("Wireless Network Connection",  True)
            #time.sleep(1)
            #print "A"

            if configSettings['security'] == "2" or configSettings['security'] == "6":
                # MUST BE EAP-TLS so install the certs again
                import ntCertificateUtils
                currentCertsInfo = ntCertificateUtils.getCurrentCertsInfo()
                for aCert in currentCertsInfo:
                    #print aCert
                    line = aCert.rstrip()
                    keyValuePair = line.split('=')
                    if keyValuePair[0] == "root":
                        aCertFilePath = keyValuePair[1]
                        if len(keyValuePair) == 4:
                            #print "doing root"
                            if keyValuePair[3] == "NONE":
                                keyValuePair[3] = ""
                            try:
                                valid, answer = ntCertificateUtils.storeTrustedRootCertificate(aCertFilePath, keyValuePair[3])
                                ntCertificateUtils.addCAtoNTAuth(aCertFilePath)

                            except:
                                pass
                    elif keyValuePair[0] == "client":
                        aCertFilePath = keyValuePair[1]
                        if len(keyValuePair) == 4:
                            #print "doing client"
                            if keyValuePair[3] == "NONE":
                                keyValuePair[3] = ""
                            try:
                                #time.sleep()
                                valid, answer = ntCertificateUtils.storeClientCertificate(aCertFilePath, keyValuePair[3])
                            except:
                                pass

            if configSettings['security'] == "3" or configSettings['security'] == "4" or int(configSettings['security']) > 6:
                cmd = []
                cmd.append(os.path.join('C:/','Program Files','Hobart','bin','Hobart-EAPUtility.exe'))
                cmd.append('sp')
                wirelessProfileXMLFile = configInfo.getRootFolder() + '\\db\\settingswizard\\WirelessProfile.xml'
                #wirelessEAPUserDataXMLFile
                cmd.append(wirelessProfileXMLFile)
                cmd.append(os.path.join('C:/','Program Files','Hobart','Db','SettingsWizard','EAPUserData.xml'))
                cmd.append(configSettings['connectionName'])
                cmd.append('WirelessProfile')
                cmd.append(configSettings['eapUsername'])
                cmd.append(configSettings['eapPassword'])
                exitCode, stdout, stderr = shellCommand.execute(cmd)

            else:
                cmd = []
                cmd.append('netsh')
                cmd.append('wlan')
                cmd.append('add')
                cmd.append('profile')
                cmd.append('filename=C:\Program Files\Hobart\Db\settingswizard\WirelessProfile.xml')
                cmd.append('interface=Wireless Network Connection')
                cmd.append('user=all')
                exitCode, stdout, stderr = shellCommand.execute(cmd)
                #time.sleep(1)
                #print "B"
                #print stdout
                #print stderr
                #time.sleep(3)
                cmd = []
                cmd.append('netsh')
                cmd.append('wlan')
                cmd.append('connect')
                cmd.append('name=WirelessProfile')
                cmd.append('interface=Wireless Network Connection')
                exitCode, stdout, stderr = shellCommand.execute(cmd)
                #print "C"
                #print stdout
                #print stderr


        elif connectionType == 0:
            #print "WIRE"
            enabledAdapterName = "Local Area Connection"
            cmd = []
            cmd.append('sc')
            cmd.append('stop')
            cmd.append('WlanSvc')
            osUtils.winRunElevated(cmd)
            #print "D"
            cmd = []
            cmd.append('sc')
            cmd.append('start')
            cmd.append('dot3Svc')
            osUtils.winRunElevated(cmd)
            #print "E"
            ntNetworkUtils.setAdapterEnabled("Local Area Connection",  True)
            #print "F"
            ntNetworkUtils.setAdapterEnabled("Wireless Network Connection",  False)
            #print "G"
        time.sleep(2)
        if configSettings['isStatic'] == "1":
            #ntNetworkUtils.enableDHCPClientService(False)
            #print "STATIC"
            ntNetworkUtils.setAddress(enabledAdapterName,  configSettings['ipAddress'],  configSettings['netmask'],  configSettings['gateway'])
            #print "H"
            addressSection = 'address'
            #settingsParser.add_section(addressSection)
            #settingsParser.set(addressSection,  'ipAddress',  options.ipAddress)
            #settingsParser.set(addressSection,  'subnetMask',  options.subnetMask)
            #settingsParser.set(addressSection,  'gateway',  options.gateway)
        else:
            #print("dhcp")
            sys.stdout.flush()
            #ntNetworkUtils.enableDHCPClientService(True)
            ntNetworkUtils.setDhcp(enabledAdapterName)
            #print "J"
        if configSettings['autoDNS'] == "1":
            #print("autoDNS")
            sys.stdout.flush()
            ntNetworkUtils.setAutoDns(enabledAdapterName)
            #print "K"
        else:
            #print("manual dns")
            sys.stdout.flush()
            ntNetworkUtils.deleteAllDnsServers(enabledAdapterName)
            #print "L"
            #dnsServersSection = 'dnsservers'
            #settingsParser.add_section(dnsServersSection)

            ntNetworkUtils.addDnsServer(enabledAdapterName,  configSettings['dns1'])
            #print "M"
            if len(configSettings['dns2']) > 6:
                ntNetworkUtils.addDnsServer(enabledAdapterName,  configSettings['dns2'])
                #print "N"
            if len(configSettings['dns3']) > 6:
                ntNetworkUtils.addDnsServer(enabledAdapterName,  configSettings['dns3'])
                #print "O"


        #zzzzzzzzzzzzzzzzzzzzzzzzzzz
        ''' time.sleep(3)
        print "3"
        sys.stdout.flush()
        cmd = []
        cmd.append(os.path.join(pythonInstallPath, "pythonw"))
        cmd.append("C:\Program Files\Hobart\python\setNetwork.py")
        networkFile = open("C:\Program Files\Hobart\Db\settingswizard\lastNetworkRun.txt", "r")
        lines = networkFile.readlines()
        networkFile.close()
        lineNum = 1
        for line in lines:
            line = string.strip(line)
            if lineNum > 1:
                cmd.append(line)
            lineNum = lineNum + 1
        print str(cmd)

        #logFile = open("C:\Program Files\Hobart\Db\settingswizard\logAtBootWooz.txt", "w")
        #logFile.write("tried to do it")
        #logFile.close()
        #p = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
        #out, err = p.communicate()

        #result = out.split('\n')
        #for lin in result:
        #    if not lin.startswith('#'):
        #        print(lin)
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        #print "okay"
        #print stdout
        #return exitCode
        print "4"
        sys.stdout.flush()
        enabledAdapterName = ""
        if connectionType == 1:
            #wireless
            enabledAdapterName = "Wireless Network Connection"
        elif connectionType == 0:
            enabledAdapterName = "Local Area Connection"

        detectedIpAddress, detectedSubnetMask, detectedGateway = ntNetworkUtils.getAddressValues(enabledAdapterName)
        if detectedIpAddress != None:
            print("IP is: " + detectedIpAddress)
        if detectedSubnetMask != None:
            print("Mask is : " + detectedSubnetMask)
        if detectedGateway != None:
            print("gateway is : " + detectedGateway)
        if detectedGateway == None:
            #Gateway was blown away so set it
            cmd = []
            cmd.append(os.path.join(pythonInstallPath, "pythonw"))
            cmd.append("C:\Program Files\Hobart\python\setNetwork.py")
            networkFile = open("C:\Program Files\Hobart\Db\settingswizard\lastNetworkRun.txt", "r")
            lines = networkFile.readlines()
            networkFile.close()
            lineNum = 1
            for line in lines:
                line = string.strip(line)
                if lineNum > 1:
                    cmd.append(line)
                lineNum = lineNum + 1
            #print str(cmd)

            #p = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
            #out, err = p.communicate()

            #result = out.split('\n')
            #for lin in result:
            #    if not lin.startswith('#'):
            #        print(lin)
            exitCode, stdout, stderr = shellCommand.execute(cmd)

            gateway = configSettings['gateway']
            ipAddress = configSettings['ipAddress']
            netmask = configSettings['netmask']
            if configSettings['isStatic'] == "1":
                #ntNetworkUtils.enableDHCPClientService(False)
                time.sleep(2)
                ntNetworkUtils.setAddress(enabledAdapterName, ipAddress, netmask, gateway)


        # test connection
        print("11")
        sys.stdout.flush()

        #print "IP:" + str(detectedIpAddress)
        #print "Subnet" + str(detectedSubnetMask)
        #print "detectedGateway:" + str(detectedGateway)
        #sys.stdout.flush()
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
            #print("YES")
            sys.stdout.flush()
            detectedIpAddress, detectedSubnetMask, detectedGateway = ntNetworkUtils.getAddressValues(enabledAdapterName)
   
            # gather output

            output = detectedIpAddress
            output += "|"
            output += detectedGateway
            output += "|"
            output += detectedSubnetMask

            detectedDnsServers = ntNetworkUtils.getDnsServers(enabledAdapterName)
            for server in detectedDnsServers:
                output += "|" + server

            print("OUTPUT|" + output.rstrip())
            sys.stdout.flush()
        else:
            #print("NO")
            print("OUTPUT|None")
            sys.stdout.flush()
            time.sleep(1)

            cmd = []
            cmd.append(os.path.join(pythonInstallPath, "python"))
            cmd.append("C:\Program Files\Hobart\python\setNetwork.py")
            networkFile = open("C:\Program Files\Hobart\Db\settingswizard\lastNetworkRun.txt", "r")
            lines = networkFile.readlines()
            networkFile.close()
            lineNum = 1
            for line in lines:
                line = string.strip(line)
                if lineNum > 1:
                    cmd.append(line)
                lineNum = lineNum + 1
            print str(cmd)

            #p = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
            #out, err = p.communicate()

            #result = out.split('\n')
            #for lin in result:
            #    if not lin.startswith('#'):
            #        print(lin)
            exitCode, stdout, stderr = shellCommand.execute(cmd)
        '''

    return exitCode

if __name__ == "__main__":
    exitCode = 0
    exitCode = connectToNetwork()
    sys.exit(exitCode)

