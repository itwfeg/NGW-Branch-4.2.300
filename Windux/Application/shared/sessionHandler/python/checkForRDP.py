#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import string
import shellCommand

# HT-326
#--------------------------------------------------------------
# Checks for an active RDP connection.
#     Returns 1 if there is a active connection, else returns 0
#--------------------------------------------------------------
def checkForRDP():
    if os.name == "posix":
        # LINUX OS
        try:
        
            rval = os.system('netstat -an | grep -q " *:3389 .* ESTABLISHED"')
            
            if (rval == 0):
                print("Connected")
                exitCode = 1
            else:
                print("Not Connected")
                exitCode = 0
        except:
            exitCode = 0
    else:
        # WINDOWS OS
        # TODO
        exitCode = 0

    return (exitCode)


if __name__=='__main__':
    exitCode = checkForRDP()
    sys.exit(exitCode)
