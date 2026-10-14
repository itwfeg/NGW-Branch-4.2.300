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
import configInfo
import createWirelessProfile

from setNetwork import get_gateway, get_ip_address, get_network_mask, get_DNS

SIOCGIFNETMASK = 0x891b


# Let's get a parser to use
###########################
p = OptionParser()

# Now add some CLI options
##########################
p.add_option("-D", "--device",
             dest="device",
             help="Network adapter to use. 1=Wireless, 0=Wired")

# Parse User's CLI args
#######################
options, args = p.parse_args()

if os.name == 'nt':
    time.sleep(10)
    import ntNetworkUtils
    enabledAdapterName = "Wireless Network Connection"
    if options.device == "1":
        enabledAdapterName = "Wireless Network Connection"
    elif options.device == "0":
        enabledAdapterName = "Local Area Connection"

    detectedIpAddress, detectedSubnetMask, detectedGateway = ntNetworkUtils.getAddressValues(enabledAdapterName)
    #print "IP:" + detectedIpAddress
    #print "Subnet" + detectedSubnetMask
    #print "detectedGateway:" + detectedGateway

    connected = ntNetworkUtils.ping(detectedGateway)
    #print "connected:" + str(connected)

    if not connected:
        for x in range(1,5):
            time.sleep(3)
            #print "retrying test"
            connected = ntNetworkUtils.ping(detectedGateway)
            if connected:
                break

    #print str(connected)
    if connected:
        #print("YES")
        #sys.stdout.flush()

        # gather output
        output = detectedIpAddress
        output += "|" + detectedGateway
        output += "|" + detectedSubnetMask

        detectedDnsServers = ntNetworkUtils.getDnsServers(enabledAdapterName)
        for server in detectedDnsServers:
            output += "|" + server

        print(("OUTPUT|" + output))
        sys.stdout.flush()
    else:
        print("OUTPUT|None")
        sys.stdout.flush()
else:
    dev = "eth0" if options.device == 1 else "wlan0"
    connected = True
    
    gateway = get_gateway(dev)
    ip = get_ip_address(dev)
    mask = get_network_mask(dev)
    dns = get_DNS().split("|")
    
    if len(gateway) < 7:
        connected = False
    else:
        cmd = []
        cmd.append("ping")
        cmd.append(str(gateway))
        cmd.append("-c")
        cmd.append("3")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        sys.stdout.flush()
        for line in stdout.split("\n"):
            if "Unreachable" in line or "unknown" in line:
                connected = False
    if connected:
        out = "OUTPUT|%s|%s|%s" % (ip, gateway, mask)
        
        out += "|".join(dns)
        
        print(out)
    else:
        print("OUTPUT|None")

sys.exit(0)
