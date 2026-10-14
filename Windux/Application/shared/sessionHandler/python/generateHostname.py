#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import getMac
import os
import setHostname
import sys

def generateHostname():

    exitCode = 0
    macAddress = getMac.getWiredMac()
    
    macAddress = macAddress.rstrip()

    if macAddress == 'No Device':
        exitCode = 1
    
    if exitCode == 0:
        macAddress = macAddress.replace(':','')

        prefix = 'ht'
        if os.path.exists(os.path.join("/","etc","hobart","blackbox")):
            prefix = 'bb'

        hostname = prefix + macAddress
        return hostname
    else:
        return 'htseries'

if __name__ == "__main__":
    hostname = generateHostname()
    print("hostname: %s" % hostname)
    setHostname.setHostname(hostname)
    sys.exit(0)
