#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import string
from time import ctime
import argparse
import os
import shellCommand

def setHardwareClock(date, time):
    cmd = []
    cmd.append("/sbin/hwclock")
    cmd.append("--set")
    cmd.append("--date")
    cmd.append(date + " " + time)
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    return exitCode

def setWindowsHardwareClock(date, time):
    # This has to be done after a local system time is already set correctly to your new time
    cmd = []
    cmd.append("w32tm")
    cmd.append("/resync")
    cmd.append("/nowait")
    #cmd.append(date + " " + time)
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    return exitCode

if __name__ == "__main__":
    exitCode = 1

    parser = argparse.ArgumentParser(description="set hardware clock date/time")

    parser.add_argument("date",
                        action="store",
                        help="date in the format MM/DD/YYYY")

    parser.add_argument("time",
                        action="store",
                        help="date in the format HH:MM:SS")

    args = parser.parse_args()

    if os.name == "posix":
        exitCode = setHardwareClock(args.date, args.time)
    else:
        exitCode = setWindowsHardwareClock(args.date,  args.time)
        
    sys.exit(exitCode)

