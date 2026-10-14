#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import shellCommand
import string

speed = "wired negotiated 100"

if os.name == "posix":
    # LINUX OS
    try:
        cmd = []
        cmd.append("mii-tool")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        line = stdout.strip()
        words = line.split()
        speedWord = words[2].split('base')
        speed = speedWord[0]
    except:
        speed = "100"

    print((str(speed)))
    exitCode = 0

else:
    #
    exitCode = 0
    try:
        import wmi

        wmi_obj = wmi.WMI()
        wmi_sql = "select * from Win32_NetworkAdapter where NetEnabled=true"
    
        for item in wmi_obj.query( wmi_sql ):
            print(int(item.Speed) / 1000000)
    except:
        exitCode = 1


sys.exit(exitCode)
