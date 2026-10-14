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

def getBiosVersion(stub):
    answer = "N/A"
    cmd = []
    cmd.append('wmic')
    cmd.append('bios')
    cmd.append('get')
    cmd.append('smbiosbiosversion')
    exitCode, stdout, stderr = shellCommand.execute(cmd)

    lineNumber = 1
    for line in stdout.splitlines():
        line = line.rstrip()
        if lineNumber == 3:
            answer = str(line.rstrip())
        lineNumber = lineNumber + 1
    return answer

if __name__ == "__main__":
    exitCode = 1

    print((getBiosVersion("")))

    sys.exit(0)
