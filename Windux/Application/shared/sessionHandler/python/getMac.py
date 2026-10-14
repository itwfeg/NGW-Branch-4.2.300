#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import struct, sys, os, string, re,  shellCommand, osUtils
if os.name == 'posix':
    import fcntl,  socket
elif os.name == 'nt':
    import ntNetworkUtils

def getHwAddr(ifname):
    # try to use ethtool for MAC address first
    fp = os.popen("ethtool -P " + ifname)
    lines = fp.read()
    lines = lines.split('\n')

    for line in lines:
        if line[0:19] == "Permanent address: ":
            return line[19:]
    
    # fallback: open a socket and look at it
    # changed to only work in python3
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    info = fcntl.ioctl(s.fileno(), 0x8927,  struct.pack('256s', ifname[:15].encode('utf-8')))
    return ''.join(['%02x:' % int(c) for c in info[18:24]])[:-1]

def getNTMac(adapterName):
    import time
    
    disableAdapter = False
    answer = None
    netshAdapterName = 'Local Area Connection'
    netshNicType = 'lan'
    if adapterName == 'Realtek':
        netshAdapterName = 'Local Area Connection'
        netshNicType = 'lan'
        cmd = []
        cmd.append('sc')
        cmd.append('start')
        cmd.append('dot3Svc')
        osUtils.winRunElevated(cmd)
        #time.sleep(2)
    elif adapterName == 'Wireless':
        netshAdapterName = 'Wireless Network Connection'
        netshNicType = 'wlan'
        cmd = []
        cmd.append('sc')
        cmd.append('start')
        cmd.append('WlanSvc')
        osUtils.winRunElevated(cmd)
    elif adapterName == 'Network':
        netshAdapterName = 'Local Area Connection'
        cmd = []
        cmd.append('sc')
        cmd.append('start')
        cmd.append('dot3Svc')
        osUtils.winRunElevated(cmd)
        netshNicType = 'lan'

    # make sure wireless adapter is enabled
    wasEnabled = True
    state = ntNetworkUtils.getAdapterState(netshNicType,  netshAdapterName)
    if not state:
        wasEnabled = False
        ntNetworkUtils.setAdapterEnabled(netshAdapterName,  True)

    # get mac address
    answer = ntNetworkUtils.getMacAddress(netshNicType,  netshAdapterName)
    i = 0
    while not answer and i < 60:
        answer = ntNetworkUtils.getMacAddress(netshNicType,  netshAdapterName)
        i += 1
    
    # disable adapter if it was originally
    if not wasEnabled:
        ntNetworkUtils.setAdapterEnabled(netshAdapterName,  False)

    return answer

def getWiredMac(scale=True):
    try:
        if os.name == "posix":
            ethernetMac = getHwAddr('eth0')
        elif os.name == "nt":
            if scale:
                ethernetMac = getNTMac('Realtek')
            else:
                ethernetMac = getNTMac('Network')
            if ethernetMac == None:
                ethernetMac = 'No Device'
    except:
        ethernetMac = "No Device"
    return ethernetMac

def getWirelessMac(scale=True):
    try:
        if os.name == "posix":
            wirelessMac = getHwAddr('wlan0')
        elif os.name == "nt":
            if scale:
                wirelessMac = getNTMac('Wireless')
            else:
                # in case your development laptop changes
                wirelessMac = getNTMac('Wireless')
            if wirelessMac == None:
                wirelessMac = 'No Device'
    except:
        wirelessMac = "No Device"
    return wirelessMac

if __name__ == '__main__':
    ethernetMac = getWiredMac()
    wirelessMac = getWirelessMac()

    print((str(ethernetMac) + "|" + str(wirelessMac)))

