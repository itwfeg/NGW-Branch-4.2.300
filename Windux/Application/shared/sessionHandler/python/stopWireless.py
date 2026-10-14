#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys, os, string
from optparse import OptionParser
import shlex
import time, subprocess
import shellCommand
import socket, struct
import signal
if os.name == "posix":
    import fcntl

    # stop wireless
    '''try:
        cmd = []
        cmd.append("ifconfig")
        cmd.append(deviceName)
        cmd.append("down")
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        cmd = []
        cmd.append("ifconfig")
        cmd.append(deviceName)
        cmd.append("up")
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        cmd = []
        cmd.append("killall")
        cmd.append("wpa_cli")
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        cmd = []
        cmd.append("killall")
        cmd.append("wpa_supplicant")
        exitCode, stdout, stderr = shellCommand.execute(cmd)

    except:
        pass
    '''

    # Change this to your process name
    processname = 'wpa_supplicant'
    processname2 = '/usr/sbin/wpa_cli'
    for line in os.popen("ps -ef"):
        fields = line.split()
        pid = fields[1]
        process = fields[7]
        print(line)
        print(process)
        if process.find(processname) > -1 or process.find(processname2) > -1:
            print("XXXFOUND IT XXXX")
            # Kill the Process. Change signal.SIGHUP to signal.SIGKILL if you like
            os.kill(int(pid), 9)
            #cmd = []
            #cmd.append("/usr/bin/kill")
            #cmd.append("-9")
            #cmd.append(pid)
            #exitCode, stdout, stderr = shellCommand.execute(cmd)

            # Do something else here
            print("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\nKilling wpa_supplicant\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXx")

            # Hop out of loop

else:
    TBD = "windows"

sys.exit(0)
