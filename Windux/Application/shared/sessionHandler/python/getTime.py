#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# only works on windows?

import sys
import os
import shellCommand
import string
import datetime

exitCode = 0
if os.name == "nt":
    import ctypes
    class SYSTEMTIME(ctypes.Structure):
        _fields_ = [
            ('wYear', ctypes.c_int16),
            ('wMonth', ctypes.c_int16),
            ('wDayOfWeek', ctypes.c_int16),
            ('wDay', ctypes.c_int16),
            ('wHour', ctypes.c_int16),
            ('wMinute', ctypes.c_int16),
            ('wSecond', ctypes.c_int16),
            ('wMilliseconds', ctypes.c_int16)]

    SystemTime = SYSTEMTIME()
    lpSystemTime = ctypes.pointer(SystemTime)
    ctypes.windll.kernel32.GetLocalTime(lpSystemTime)
    hour = 1
    antimeridian = "AM"
    # The am/pm thing
    if SystemTime.wHour > 11:
        if SystemTime.wHour == 12:
            hour = SystemTime.wHour
        else:
            hour = SystemTime.wHour - 12
        antimeridian = "PM"
    else:
        if SystemTime.wHour == 0:
            hour = 12
        else:
            hour = SystemTime.wHour
    print((str(SystemTime.wYear) + "," + str(SystemTime.wMonth) + "," + str(SystemTime.wDay) + "," + str(hour) + "," + str(SystemTime.wMinute) + "," + antimeridian))

sys.exit(exitCode)

