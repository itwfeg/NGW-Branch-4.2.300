#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys, os, string
import argparse
import shellCommand

def stripWirelessDevice():
    exitCode = 0
    if os.name == "posix":
        # LINUX OS
        try:
            cmd = []
            cmd.append("rm")
            cmd.append("-r")
            cmd.append("-f")
            cmd.append("/etc/udev/rules.d/70-persistent-net.rules")
            exitCode, stdout, stderr = shellCommand.execute(cmd)

            cmd = []
            cmd.append("sync")
            exitCode, stdout, stderr = shellCommand.execute(cmd)

            # now update /etc/hostname for permanent change to take place
            #rulesFileHandle = open("/etc/udev/rules.d/70-persistent-net.rules", 'r')
            #hostnameFileHandle.write(hostname+ "\n")
            #lines = rulesFileHandle.readlines()
            #rulesFileHandle.close()

            #rulesFileHandle = open("/etc/udev/rules.d/70-persistent-net.rules", 'w')

            #writeNextLine = True
            #for line in lines:
            #    if line != "# PCI device 0x168c:0x0034 (ath9k)"+"\n":
            #        if writeNextLine:
            #            rulesFileHandle.write(line)
            #        else:
            #            writeNextLine = True
            #    else:
            #        writeNextLine = False
            #rulesFileHandle.close()

        except:
            exitCode = 1

    else:
        print("TBD")
    return exitCode

if __name__ == "__main__":

    exitCode  = stripWirelessDevice()
    sys.exit(exitCode)
