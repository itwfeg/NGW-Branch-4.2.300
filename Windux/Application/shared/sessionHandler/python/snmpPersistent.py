#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import shellCommand
import subprocess

cmd = []
cmd.append("pgrep")
cmd.append("-f")
cmd.append("snmpHobart.py")
stdout = "0"
exitCode, stdout, stderr = shellCommand.execute(cmd)

if stdout != "0" and len(stdout):
    print((str(stdout)))
else:
    print("not running")    
    subprocess.Popen(["/usr/bin/python", "/usr/local/hobart/python/snmpHobart.py"])
sys.exit(0)
