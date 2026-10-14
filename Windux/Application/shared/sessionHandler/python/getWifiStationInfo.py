#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import struct, sys, os, string, re,  shellCommand
if os.name == 'posix':
    import fcntl,  socket

def getStationDump():
    answer = ""
    try:
        if os.name == "posix":
            cmd = []
            cmd.append("iw")
            cmd.append("dev")
            cmd.append("wlan0")
            cmd.append("link")
            exitcode, stdout, stderr = shellCommand.execute(cmd)
            if stdout.find("Not connected") == -1:
                cmd = []
                cmd.append("iwconfig")
                cmd.append("wlan0")
                exitcode, stdout, stderr = shellCommand.execute(cmd)
                if len(stdout) > 0:
                    for line in stdout.splitlines():
                        if line.find("Access Point") == -1:
                            line = line.lstrip()
                            answer += line + '\n'
                cmd = []
                cmd.append('iw')
                cmd.append('dev')
                cmd.append('wlan0')
                cmd.append('station')
                cmd.append('dump')
                exitCode, stdout, stderr = shellCommand.execute(cmd)
                #get rid of the BSSID mac address
                if len(stdout) > 0:
                    for line in stdout.splitlines():
                        if line.find("Station") == -1:
                            line = line.lstrip()
                            answer += '  ' + line + '\n'
                    #sys.stdout.flush()
            else:
                answer = "Not Connected"
    except:
        pass

    return answer

if __name__ == '__main__':
    #answer = getStationDump()
    print((str(getStationDump())))

