#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys, os, string
import argparse
import shellCommand
import time

linuxWirelessDevice = 'wlan0'
linuxWiredDevice = 'eth0'

def setWirelessLinkRateNow():
    exitCode = 0
    if os.name == "posix":
        try:
            time.sleep(30)
            cmd = []
            cmd.append("iwlist")
            cmd.append("wlan0")
            cmd.append("scan")

            '''
            with open("setWirelessLinkRate.txt", "w") as myfi:
                for item in cmd:
                    myfi.write("%s\n" % item)
                    myfi.write("XXXXXXXXX\n")
            myfi.close()
            '''
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            if exitCode == 0:
                filler = "nothing"
        except:
            exitCode = 0
            pass

    else:
        exitCode = 0

    return exitCode

if __name__ == "__main__":

    try:
        exitCode  = setWirelessLinkRateNow()
    except:
        pass
    sys.exit(0)

