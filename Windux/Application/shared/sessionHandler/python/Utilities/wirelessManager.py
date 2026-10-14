#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys, os, string
import argparse
import shellCommand

def initialize(device):
    exitCode = 1
    if os.name == "posix":
        try:
            cmd = []
            cmd.append("/usr/sbin/wpa_supplicant")
            cmd.append("-Dwext")
            cmd.append("-i%s" % (device))
            cmd.append("-c/etc/wpa_supplicant.conf")
            cmd.append("-B")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            
            cmd = []
            cmd.append("dhcpcd")
            cmd.append(device)
            exitCode, stdout, stderr = shellCommand.execute(cmd)            
            
        except:
            exitCode = 1

    else:
        # Nothing to do for Windows
        exitCode = 0
            
    return exitCode

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="initialize wireless device management")
    parser.add_argument("device",
                        action="store", 
                        help="wireless device name")

    args = parser.parse_args()

    exitCode  = initialize(args.device)
    sys.exit(exitCode)
