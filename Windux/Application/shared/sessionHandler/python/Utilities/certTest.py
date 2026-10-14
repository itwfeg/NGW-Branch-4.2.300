#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import shellCommand


cmd = []
cmd.append("certmgr.exe")
exitCode, stdout, stderr = shellCommand.execute(cmd)

print(stdout)

sys.exit(0)
