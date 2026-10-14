#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import re
import shellCommand
import getSerialFlashInfo
import platform
import locale

from getMac import getHwAddr as linuxGetMacAddress

def getMacAddressRemoteHostByIp(ipAddress):
    macAddress = None

    cmd = []
    cmd.append("arp")
    cmd.append("-a")
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    if exitCode == 0:
        if stdout is not None:
            if platform.system() == 'Linux':
                item = stdout.split("\n")
                for line in item:
                    lineItems = line.split()
                    if ipAddress in lineItems:
                        macAddress = lineItems[1].replace(':', '')

            elif platform.system() == 'Windows':
                item =  stdout.split("\n")
                for line in item:
                    lineItems = line.split()
                    if ipAddress in lineItems:
                        macAddress = lineItems[1].replace('-', '')
    return macAddress


def getMacAddress(lanType='wire'):
    macAddress = None
    exitCode = 1

    if os.name == "posix":
        if lanType == 'wire':
            try:
                macAddress = linuxGetMacAddress('eth0')
                exitCode = 0
            except:
                pass

        elif lanType == 'wireless':
            try:
                macAddress = linuxGetMacAddress('wlan0')
                exitCode = 0
            except:
                pass

    else:
        try:
            modelName = getSerialFlashInfo.getModelName()
        except:
            modelName = ''
#
        if modelName:
            import ntNetworkUtils
            if lanType == 'wire':
                try:
                    macAddress = ntNetworkUtils.getMacAddress('lan')
                    exitCode = 0
                except:
                    pass

            elif lanType == 'wireless':
                try:
                    macAddress = ntNetworkUtils.getMacAddress('wlan')
                    exitCode = 0
                except:
                    pass
#
        else:
            import wmi
            import pythoncom
            # use WMI interface to gain access to adapters
            pythoncom.CoInitialize()
            c = wmi.WMI()

            # enumerate all adapters
            for a in c.Win32_NetworkAdapter():
                # only want the physical ones
                if ((1 == a.PhysicalAdapter) and (a.NetConnectionId is not None)):
                    print((str(a.NetConnectionId) + ': ' + str(a.Description)))

                    iface = None

                    # which interface wired or wireless
                    if lanType == 'wire':
                        iface = 'Ethernet adapter ' + str(a.NetConnectionId) + ':'
                    elif lanType == 'wireless':
                        iface = 'Wireless LAN adapter ' + str(a.NetConnectionId) + ':'

                    # process this adapter
                    if ((iface is not None) and ('VMware' not in str(a.NetConnectionId))):
                        cmd = []
                        cmd.append("ipconfig")
                        cmd.append("/all")
                        exitCode, stdout, stderr = shellCommand.execute(cmd)

                        if ((exitCode == 0) and (iface in stdout)):
                            locale_name, encoding = locale.getdefaultlocale()

                            if "sp" == locale_name.split("_")[0].lower():
                                # TODO: Set this to the spanish pattern to match in the ipconfig output.
                                pattern = "Physical Address[. ]*: (([a-zA-Z0-9]{2}-){5}[a-zA-Z0-9]{2})\r\n"
                            else:
                                pattern = "Physical Address[. ]*: (([a-zA-Z0-9]{2}-){5}[a-zA-Z0-9]{2})\r\n"

                            try:
                                ethernetInfo = re.split(iface, stdout)
                                macAddress = re.search(pattern, ethernetInfo[1])
                                macAddress = str(macAddress.group(1))
                                macAddress = macAddress.upper().replace('-',':')
                                return (exitCode,  macAddress)
                            except:
                                exitCode = 1
#
    return (exitCode,  macAddress)

if __name__ == "__main__":
    wiredMacAddress = None
    wirelessMacAddress = None

    exitCode,  wiredMacAddress = getMacAddress(lanType='wire')
    exitCode,  wirelessMacAddress = getMacAddress(lanType='wireless')

    if wiredMacAddress == None:
        wiredMacAddress = "No Device"

    if wirelessMacAddress == None:
        wirelessMacAddress = "No Device"

    print((wiredMacAddress + "|" + wirelessMacAddress))
    #SOF-1712
    #print getMacAddressRemoteHostByIp('10.3.177.89')
    sys.exit(exitCode)
