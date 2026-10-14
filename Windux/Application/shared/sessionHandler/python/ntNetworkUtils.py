#!/usr/bin/env python2
# -*- coding: utf-8 -*-

# not tested in python3
# windows only?

import threading
import shellCommand
import configInfo
import osUtils
import re
import string
import os
import subprocess

def getValue(adapterType,  valueType,  adapterName,  valueFormat = "([^\r\n]*)"):
    value = None

    # run netsh command
    exitCode, stdout, stderr = shellCommand.execute(['netsh', adapterType, 'show', 'interface', 'name=', adapterName])

    if valueType == "Receive":
        for line in stdout.splitlines():
            if line.find("Receive rate") > -1:
                s = line.split(":")
                s = s[1].strip()
                value = s
                #print value
    else:
        # parse out desired info
        if adapterName == "*":
            valueMatch = re.search("\s+" + valueType + "\s*:\s*(" + valueFormat + ")",  stdout,  re.I)
            if valueMatch:
                value = valueMatch.group(1).strip()
        else:
            adapterNameMatch = re.search("\s+Name\s*:\s*" + adapterName + "\s*[\r\n]",  stdout)
            if adapterNameMatch:
                section = stdout[adapterNameMatch.end(0):]
                nextAdapterMatch = re.search("\s+Name\s*:\s*", section)
                if nextAdapterMatch:
                    section = section[:nextAdapterMatch.start(0)]
                valueMatch = re.search("\s+" + valueType + "\s*:\s*" + valueFormat,  section,  re.I)
                if valueMatch:
                    value = valueMatch.group(1).strip()
    return value

def getAdapterState(adapterType,  adapterName):           
    return getValue(adapterType,  "State",  adapterName)
    
def getMacAddress(adapterType,  adapterName = "*"):    
    macAddress = getValue(adapterType,  "Physical Address",  adapterName,  "([0-9A-F]{2}([-:][0-9A-F]{2}){1,5})")
    if macAddress:
        macAddress = macAddress.replace('-',':').upper()
    return macAddress

def getWlanAdapterSsid(adapterName = "*"):
    return getValue("wlan",  "SSID",  adapterName)

def getWireWinSpeed(adapterName = "*"):
    return getValue("lan", "SSID", adapterName)
def getWlanAdapterChannel(adapterName = "*"):
    return getValue("wlan",  "Channel",  adapterName, "([0-9]+)")

def getWlanAdapterWinSpeed(adapterName = "*"):
    return getValue("wlan",  "Receive", adapterName)

def getWlanAdapterSignalStrength(adapterName = "*"):
    return getValue("wlan",  "Signal",  adapterName, "([0-9]+)")
    
def getAddressValues(adapterName = "*"):
    ipAddress = None
    subnetMask = None
    defaultGateway = None
    
    # run netsh command
    exitCode, stdout, stderr = shellCommand.execute(['netsh', 'interface', 'ipv4', 'show', 'address', 'name=', adapterName])
    
    # parse out desired info

    ipAddressMatch = re.search("\s+IP Address\s*:\s*((?:[0-9]{1,3}\.){3}[0-9]{1,3})",  stdout,  re.I)
    if ipAddressMatch:
        ipAddress = ipAddressMatch.group(1)
    subnetMaskMatch = re.search("\s+Subnet Prefix\s*:\s*(?:[0-9]{1,3}\.){3}[0-9]{1,3}.*mask\s*((?:[0-9]{1,3}\.){3}[0-9]{1,3})",  stdout,  re.I)
    if subnetMaskMatch:
        subnetMask = subnetMaskMatch.group(1)
    defaultGatewayMatch = re.search("\s+Default Gateway\s*:\s*((?:[0-9]{1,3}\.){3}[0-9]{1,3})",  stdout,  re.I)
    if defaultGatewayMatch:
        defaultGateway = defaultGatewayMatch.group(1)
        
    return (ipAddress,  subnetMask,  defaultGateway)
    
def getAddressValue(valueType,  adapterName = None,  valueFormat = "[^\r\n]*"):
    value = None
    #print "HERE"
    # run netsh command
    stdout = None
    if str(adapterName) == "Wireless Network Connection":
        cmd = 'netsh interface ipv4 show address name="Wireless Network Connection"'
        netshcmd = subprocess.Popen(cmd,universal_newlines=True,shell=True,stderr=subprocess.STDOUT,stdout=subprocess.PIPE)
        text = netshcmd.stdout.read()
        retCode = netshcmd.wait()
        stdout = text
    elif str(adapterName) == "Local Area Connection":
        cmd = 'netsh interface ipv4 show address name="Local Area Connection"'
        netshcmd = subprocess.Popen(cmd,universal_newlines=True,shell=True,stderr=subprocess.STDOUT,stdout=subprocess.PIPE)
        text = netshcmd.stdout.read()
        retCode = netshcmd.wait()
        stdout = text
    elif adapterName:
        exitCode, stdout, stderr = shellCommand.execute(['netsh', 'interface', 'ipv4', 'show', 'address', 'name=', '"' + adapterName + '"'])
    else:
        exitCode, stdout, stderr = shellCommand.execute(['netsh', 'interface', 'ipv4', 'show', 'address'])
    # parse out value
    if valueType == "Default Gateway":
        try:
            for line in stdout.splitlines():
                if line.find("Default Gateway") > -1:
                    s = line.split(":")
                    s = s[1].strip()
                    value = s
        except:
            value = None
            pass
    else:
        valueMatch = re.search("\s+" + valueType + "\s*:\s*" + valueFormat,  stdout,  re.I)
        if valueMatch:
            value = valueMatch.group(1).strip()

    return value
    
def getIpAddress(adapterName = None):
    return getAddressValue("IP Address", adapterName, "((?:[0-9]{1,3}\.){3}[0-9]{1,3})")
    
def getSubnetMask(adapterName = None):
    return getAddressValue("Subnet Prefix", adapterName, "(?:[0-9]{1,3}\.){3}[0-9]{1,3}.*mask\s*((?:[0-9]{1,3}\.){3}[0-9]{1,3})")
    
def getGateway(adapterName = None):
    return getAddressValue("Default Gateway", adapterName, "((?:[0-9]{1,3}\.){3}[0-9]{1,3})")
    
def getDhcpServer():

    dhcpServer=''
    # run netsh command
    exitCode, stdout, stderr = shellCommand.execute(['ipconfig', '/all'])
    
    dhcpServerMatch = re.search("\s+DHCP Server . . . . . . . . . . . :\s*((?:[0-9]{1,3}\.){3}[0-9]{1,3})",  stdout,  re.I)

    if dhcpServerMatch:
        dhcpServer = dhcpServerMatch.group(1)
        
    return dhcpServer
    
def getDnsServers(adapterName):
    dnsServerMatchRe = re.compile("((?:[0-9]{1,3}\.){3}[0-9]{1,3})")
    
    dnsServers = []
    
    # run netsh command
    exitCode, stdout, stderr = shellCommand.execute(['netsh', 'interface', 'ipv4', 'show', 'dnsservers', 'name=', adapterName])
    
    # parse out desired info
    dnsServerMatch = dnsServerMatchRe.search(stdout)
    while dnsServerMatch:
        dnsServers.append(dnsServerMatch.group(1))
        dnsServerMatch = dnsServerMatchRe.search(stdout,  dnsServerMatch.end(1))
        
    return dnsServers
    
def getWlanSsidStrengths():
    strengthMatchRe = re.compile("\s+Signal\s*:\s*([0-9]+)")
    toLineEndMatch = re.compile("([^\r\n]*)")

    hiddenNetworkCount = 0
    ssidStrengthsBySsid = {}
    
    # run netsh command
    exitCode, stdout, stderr = shellCommand.execute(['netsh', 'wlan', 'show', 'networks', 'mode=BSSID'])
    
    # parse out desired info
    for section in re.split("\nSSID\s+[0-9]+\s*:", stdout):
        strengthMatch = strengthMatchRe.search(section)
        if strengthMatch:
            maxStrength = int(strengthMatch.group(1))
            ssidMatch = toLineEndMatch.search(section)
            ssid = None
            if ssidMatch:
                ssid = ssidMatch.group(1).strip()
            if not ssid:
                hiddenNetworkCount += 1
                ssid = 'Hidden network ' + str(hiddenNetworkCount)
            strengthMatch = strengthMatchRe.search(section,  strengthMatch.end(1))
            while strengthMatch:
                maxStrength = max(maxStrength,  int(strengthMatch.group(1)))
                strengthMatch = strengthMatchRe.search(section,  strengthMatch.end(1))
            ssidStrengthsBySsid[ssid] = maxStrength
    return ssidStrengthsBySsid
    
def waitForConnection(wirelessAdapterType,  wirelessAdapterName,  timeout):
    connectedMatchRe = re.compile("\s*connected",  re.I)
    # make sure wireless adapter is enabled
    timeoutEvent = threading.Event()
    timeoutTimer = threading.Timer(timeout, (lambda:timeoutEvent.set()))
    state = getAdapterState(wirelessAdapterType,  wirelessAdapterName)
    timeoutTimer.start()
    while not state and not connectedMatchRe.match(state):
        if timeoutEvent.wait(.50):
            return False
        state = getAdapterState(wirelessAdapterType,  wirelessAdapterName)
    return True
   
def ping(address):
    # run ping command
    cmd = []
    cmd.append("ping")
    cmd.append("-n")
    cmd.append("2")
    cmd.append("-w")
    cmd.append("600")
    cmd.append(address)
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    #exitCode, stdout, stderr = shellCommand.execute(['ping',  address])

    # parse out desired info
    pingSuccessMatch = re.search("\s+Reply\s+from\s*(?:[0-9]{1,3}\.){3}[0-9]{1,3}\s*:\s*bytes=[0-9]+",  stdout,  re.I)
    
    return pingSuccessMatch != None

def setAdapterEnabled(adapterName,  isEnabled):
    enabledSetting = None
    if isEnabled:
        enabledSetting = 'admin=ENABLED'
    else:
        enabledSetting = 'admin=DISABLED'
    return osUtils.winRunElevated(['netsh', 'interface', 'set', 'interface', adapterName, enabledSetting])

def startNetworkService(adapterName):
    if adapterName == "Wireless Network Connection":
        serviceName = "WlanSvc"
    return shellCommand.execute(['sc', 'start',serviceName])
def disconnectWlan():
    return shellCommand.execute(['netsh', 'wlan', 'disconnect'])

def connectWlan(adapterName,  profileName,  ssid):
    import sys
    sys.stdout.flush()
    return shellCommand.execute(['netsh', 'wlan', 'connect', 'interface=', adapterName, 'name=', profileName, 'ssid=', ssid])
    
def setWlanDenyallFilter(isAdded):
    addedSettings = None
    if isAdded:
        addedSettings =  'add'
    else:
        addedSettings =  'delete'
    return osUtils.winRunElevated(['netsh', 'wlan', addedSettings, 'filter', 'permission=denyall', 'networktype=infrastructure'])
    
def addWlanAddAllowFilter(ssid):
    return osUtils.winRunElevated(['netsh', 'wlan', 'add', 'filter', 'permission=allow', 'ssid=', ssid, 'networktype=infrastructure'])

def deleteAllProfilesForAdapter(adapterType,  adapterName):
    return osUtils.winRunElevated(['netsh', adapterType, 'delete', 'profile', 'interface=', adapterName, 'name=*'])

def addProfileForAdapter(adapterType,  adapterName, fileName):
    return osUtils.winRunElevated(['netsh', adapterType, 'add', 'profile', 'interface=', adapterName, 'filename=', fileName])

def importEapProfile(fileName,  profileName):
    return osUtils.winRunElevated([configInfo.getRootFolder() + '\\bin\\wlanutil', fileName,  profileName])

def setAddress(adapterName,  ip, subnetMask,  gateway):
    if gateway == "0":
        return osUtils.winRunElevated(['netsh', 'interface', 'ip', 'set', 'address', adapterName, 'static', ip, subnetMask])
    else:
        return osUtils.winRunElevated(['netsh', 'interface', 'ip', 'set', 'address', adapterName, 'static', ip, subnetMask, gateway])

def enableDHCPClientService(startIt):
    if startIt:
        return osUtils.winRunElevated(['net', 'start', 'DHCP Client'])
    else:
        return osUtils.winRunElevated(['net', 'stop', 'DHCP Client'])
def setDhcp(adapterName):
    return osUtils.winRunElevated(['netsh', 'interface', 'ipv4', 'set', 'address', 'name=', adapterName, 'source=dhcp', 'store=persistent'])

def setAutoDns(adapterName):
    return osUtils.winRunElevated(['netsh', 'interface', 'ipv4', 'set', 'dnsservers', 'name=', adapterName, 'source=dhcp'])

def deleteAllDnsServers(adapterName):
    return osUtils.winRunElevated(['netsh', 'interface', 'ipv4', 'set', 'dnsservers', 'name=', adapterName, 'static','0.0.0.0', 'validate=', 'no'])

def addPrimaryDNSServer(adapterName, server):
    return osUtils.winRunElevated(['netsh', 'interface', 'ipv4', 'add', 'dnsserver', 'name=', adapterName, 'address=', server, 'index=1', 'validate=no'])
def addSecondaryDNSServer(adapterName, server):
    return osUtils.winRunElevated(['netsh', 'interface', 'ipv4', 'add', 'dnsserver', 'name=', adapterName, 'address=', server, 'index=2', 'validate=no'])
def addTertiaryDNSServer(adapterName, server):
    return osUtils.winRunElevated(['netsh', 'interface', 'ipv4', 'add', 'dnsserver', 'name=', adapterName, 'address=', server, 'index=3', 'validate=no'])
def addDnsServer(adapterName,  server):
    return osUtils.winRunElevated(['netsh', 'interface', 'ipv4', 'add', 'dnsservers', 'name=', adapterName, 'addr=', server,  'validate=no'])
def deletePrimaryDNS(enabledAdapterName):
    return osUtils.winRunElevated(['netsh', 'interface', 'ipv4', 'set', 'dns', enabledAdapterName, '0.0.0.0', 'index=1', 'validate=no'])
def deleteSecondaryDNS(enabledAdapterName):
    return osUtils.winRunElevated(['netsh', 'interface', 'ipv4', 'set', 'dns', enabledAdapterName, '0.0.0.0', 'index=2', 'validate=no'])
def deleteTertiaryDNS(enabledAdapterName):
    return osUtils.winRunElevated(['netsh', 'interface', 'ipv4', 'set', 'dns', enabledAdapterName, '0.0.0.0', 'index=3', 'validate=no'])
def deleteZeroDNSEntry(enabledAdapterName):
    return osUtils.winRunElevated(['netsh', 'interface', 'ipv4', 'delete', 'dnsserver', enabledAdapterName, "0.0.0.0"])
def setWirelessChannel(enabledAdapterName):
    pass
def setWirelessLinkRate(enabledAdapterName):
    pass
