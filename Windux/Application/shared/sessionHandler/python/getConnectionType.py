#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import subprocess

# format or return string is: isConnected#,strength#,deviceType,
#                                 1=yes       0-5      0=wire
#                                 0=no                 1=wireless

#change these values below for testing
#print("1,5,1")
#sys.stdout.flush()

def isWireless(stub):
    if os.name == "posix":
        answer = "0"
        dbFile = open("/usr/local/hobart/db/settingswizard/scalesettings", "r")
        for line in dbFile:
            keyValuePair = line.split('=')
            if keyValuePair[0] == "connectionType":
                # 0 = wire      1 = wifi
                answer = str(keyValuePair[1].rstrip())
        dbFile.close()
        return answer
    elif os.name == "nt":
        import scaleSettingsInterface
        return scaleSettingsInterface.getValue('connectionType')


if __name__ == "__main__":
    #exitCode = 1
    wireless = '2'

    wireless = isWireless('')
    print((wireless.rstrip()))

sys.exit(0)
