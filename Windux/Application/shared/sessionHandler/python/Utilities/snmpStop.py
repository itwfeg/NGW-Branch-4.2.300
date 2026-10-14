#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os, signal
import shellCommand
import subprocess

def check_kill_process(pstring):
    for line in os.popen("ps ax | grep " + pstring + " | grep -v grep"):
        fields = line.split()
        pid = fields[0]
        os.kill(int(pid), signal.SIGKILL)

snmpProcess = "/usr/local/hobart/python/snmpHobart.py"

check_kill_process(snmpProcess)
