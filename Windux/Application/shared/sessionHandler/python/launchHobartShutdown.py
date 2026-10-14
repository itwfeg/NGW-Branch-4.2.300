#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import subprocess
import shellCommand


exitCode = 0




#start the polling for the shutdown button
cmd = []
#cmd.append("start")
#cmd.append("/MIN")
cmd.append("C:\\Program Files\\Hobart\\bin\\hobartShutdown.exe")
#subprocess.Popen(cmd, shell=True)
shellCommand.executeShellCommandBackground(cmd)
sys.exit(0)

