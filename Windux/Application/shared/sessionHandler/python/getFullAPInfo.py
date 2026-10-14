#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import struct, sys, os, string, re,  shellCommand
if os.name == 'posix':
    import fcntl,  socket

def getFullAPInfoText():
    answer = ""
    try:
        if os.name == "posix":
            cmd = []
            cmd.append("iw")
            cmd.append("dev")
            cmd.append("wlan0")
            cmd.append("scan")
            exitcode, stdout, stderr = shellCommand.execute(cmd, shell=True)
            if len(stdout) > 0:
                answer = stdout
            else:
                answer = "Not Connected"
    except:
        pass

    return answer

if __name__ == '__main__':
    #answer = getStationDump()
    print(str(getFullAPInfoText()))

