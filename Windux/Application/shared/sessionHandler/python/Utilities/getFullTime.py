#!/usr/bin/env python3
# -*- coding: utf-8 -*-

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
    print((str(SystemTime.wYear) + "," + str(SystemTime.wMonth).zfill(2) + "," + str(SystemTime.wDayOfWeek).zfill(1) + "," + str(SystemTime.wDay).zfill(2) + "," + str(SystemTime.wHour).zfill(2) + "," + str(SystemTime.wMinute).zfill(2) + "," + str(SystemTime.wMinute).zfill(2) + "," + str(SystemTime.wSecond).zfill(2) + "," + str(SystemTime.wMilliseconds).zfill(3)))

sys.exit(exitCode)

