#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import subprocess
import string
import time
import struct
import socket
import shellCommand
import glob
from datetime import datetime
from optparse import OptionParser

def getCurrentCount():
    dbFileName = "/usr/local/hobart/db/settingswizard/offlineCount"
    count = 0
    try:
        dbFile = open(dbFileName, "r")
        for line in dbFile:
            keyValuePair = line.split('=')
            if keyValuePair[0] == "count":
                count = str(keyValuePair[1])
                break
        dbFile.close()
    except:
        count = "0"
    count = count
    return str(count)


def resetOfflineCount():
    dbFileName = "C:\Program Files\Hobart\db\settingswizard\offlineCount"
    if os.name == "posix":
        dbFileName = "/usr/local/hobart/db/settingswizard/offlineCount"
    count = "1"
    try:
        import fileinput
        for line in fileinput.FileInput(dbFileName, inplace=1):
            if line.startswith('count='):
                line = 'count=1'
            success = True
            print((line.rstrip()))
    except:
        count = "1"
        pass

if __name__ == "__main__":
    exitCode = 1
    
    try:
        if int(getCurrentCount()) > 1:
            resetOfflineCount()
    except:
        resetOfflineCount()    
sys.exit(0)


