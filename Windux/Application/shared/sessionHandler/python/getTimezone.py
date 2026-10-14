#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import shellCommand

def getTimeZone():
    timezone = ""

    if os.name == "posix":
        # LINUX OS
        try:
            cmd = []
            cmd.append("/bin/date")
            cmd.append("+%Z")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            timezone = stdout.strip()
        except:
            timezone = "None"

        exitCode = 0
        return (exitCode, timezone)

    elif os.name == "nt":

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
        #print("__________________")
        #print SystemTime.wHour, SystemTime.wMinute
        #print("__________________")

        import time
        import datetime
        #time.tzset()

        #import tzlocal
        #print datetime.datetime.now()
        #print datetime.datetime.now(tz=Eastern Daylight Time)

        currentTimeZoneSpelledOut = time.strftime("%Z")
        print(currentTimeZoneSpelledOut)
        exitCode = 0

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
    #print("__________________")
    #print SystemTime.wHour, SystemTime.wMinute
    #print("__________________")

    import time
    import datetime
    #time.tzset()

    #import tzlocal
    #print datetime.datetime.now()
    #print datetime.datetime.now(tz=Eastern Daylight Time)

    currentTimeZoneSpelledOut = time.strftime("%Z")
    print(currentTimeZoneSpelledOut)
    exitCode = 0

if __name__ == '__main__':
    exitCode, timezone = getTimeZone()
    print((str(timezone)))
    sys.exit(exitCode)
