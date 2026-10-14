#!/usr/bin/env python2
# -*- coding: utf-8 -*-

# needs to be updated for python3 (replace popen4 usage)

import sys
import os
import shellCommand
import argparse

def get_gateway(ifname):
    cmd = "ip route list dev "+ ifname + " | awk ' /^default/ {print $3}'"
    fin,fout = os.popen4(cmd)
    result = fout.read()
    #print result
    return result

def testConnection(interface):

    if interface == 'wired':
        interface = 'eth0'
    else:
        interface = 'wlan0'

    connected = True

    gateway = get_gateway(interface)

    cmd = []
    cmd.append("ping")
    cmd.append(str(gateway))
    cmd.append("-c")
    cmd.append("1")
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    for line in stdout.split("\n"):
        if "Unreachable" in line:
            connected = False
        elif "unknown" in line:
            connected = False

    if connected:
        exitCode = 0
    else:
        exitCode = 1
    return exitCode

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="test gateway connection")
    parser.add_argument("interface",
                        action="store",
                        choices=['wired','wireless'],
                        help="specify interface device (wireless, wired)")

    args = parser.parse_args()

    exitCode = testConnection(args.interface)

    sys.exit(exitCode)
