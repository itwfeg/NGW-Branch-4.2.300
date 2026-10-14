#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import shellCommand
#import can

#
#   http://python-can.readthedocs.org/en/latest/index.html
#

def main():
    
    #initialize the can interface and set bus speed to 1Mhz
    cmd = []
    cmd.append("ip")
    cmd.append("link")
    cmd.append("set")
    cmd.append("can0")
    cmd.append("up")
    cmd.append("type")
    cmd.append("can")
    cmd.append("bitrate")    
    cmd.append("250000")
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    if exitCode != 0:
        print("Can bus interface failed to initialize.")
    else:
        print("Can bus ready for use.")

    cmd = []
    cmd.append("ifconfig")
    cmd.append("can0")
    cmd.append("up")
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    if exitCode != 0:
        print("Can bus interface driver failed to load.")
    else:
        print(stdout)


if __name__ == "__main__":
    main()
    
