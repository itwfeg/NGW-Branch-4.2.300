#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import shellCommand
import subprocess
import snmpRemoveCronJob

try:    
    snmpRemoveCronJob.removeCronJob()

    cmd = []
    cmd.append("pkill")
    cmd.append("-9")
    cmd.append("-f")
    cmd.append("snmpHobart.py")
    exitCode, stdout, stderr = shellCommand.execute(cmd)
except Exception as error:
    import traceback
    traceback.print_exc(error)
    sys.exit(1)

sys.exit(0)
