#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import socket, struct
import os

def getConnectionType():
    answer = "wire"
    if os.name == 'posix':
        dbFile = open("/usr/local/hobart/db/settingswizard/scalesettings", "r")
        for line in dbFile:
            keyValuePair = line.split('=')
            if keyValuePair[0] == "connectionType":
                # 0 = wire      1 = wifi
                answer = str(keyValuePair[1].rstrip())
        dbFile.close()
    elif os.name == 'nt':
        import scaleSettingsInterface
        answer =  scaleSettingsInterface.getValue('connectionType')
    if answer == "0":
        return "wire"
    elif answer == "1":
        return "wireless"
    return "wire"

def getIpAddress():
    '''
    Get IP Address of machine script is executed on
    '''
    ipAddress = ''
    if os.name == 'posix':
        try:
            import fcntl
            adapter = 'eth0'
            if getConnectionType() == 'wireless':
                adapter = 'wlan0'
            s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            ipAddress = socket.inet_ntoa(fcntl.ioctl(
                s.fileno(),
                0x8915,
                struct.pack('256s', adapter[:15].encode('utf-8'))
            )[20:24])
            exitCode = 0
        except:
            exitCode = 1
            ipAddress = ""
    else:
        result = ''
        try:
            import ntNetworkUtils
            ipAddress = ntNetworkUtils.getIpAddress()
            exitCode = 0
        except:
            exitCode = 1
            ipAddress = ""
        '''if lanType == 'wire':

            ethernetPort = "Ethernet adapter Local Area Connection:"

            cmd = []
            cmd.append("ipconfig")
            cmd.append("/all")
            exitCode, stdout, stderr = shellCommand.execute(cmd)

            if exitCode == 0:
                try:
                    ethernetInfo = re.split(ethernetPort,stdout)
                    macAddress = re.search("Physical Address[. ]*: (([a-zA-Z0-9]{2}-){5}[a-zA-Z0-9]{2})\r\n", ethernetInfo[1])
                    macAddress = str(macAddress.group(1))
                    macAddress = macAddress.upper().replace('-',':')
                except:
                    exitCode = 1

        elif lanType == 'wireless':
            
            cmd = []
            cmd.append("netsh")
            cmd.append("wlan")
            cmd.append("show")
            cmd.append("interfaces")
            cmd.append("|")
            cmd.append("findstr")
            cmd.append("Physical address")
            
            exitCode, stdout, stderr = shellCommand.execute(cmd)

            if exitCode == 0:
                try:
                    macAddress = string.strip(stdout)
                    macAddress = re.search("Physical address[ ]*: (([a-zA-Z0-9]{2}:){5}[a-zA-Z0-9]{2})", macAddress)
                    macAddress = string.upper(macAddress.group(1))
                    exitCode = 0
                except:
                    exitCode = 1
        '''
    return (exitCode, ipAddress)

if __name__=='__main__':
    exitCode, ipAddress = getIpAddress()
    print(ipAddress)
    sys.exit(exitCode)
