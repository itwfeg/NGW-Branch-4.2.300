#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import shellCommand
import string
import time

timezone = ""
timezoneOffset = 0

if os.name == "posix":
    # LINUX OS

    try:
        cmd = []
        cmd.append("date")
        cmd.append("+%Z")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        timezone = stdout.strip()
    except:
        timezone = "None"

    if timezone != "None":
        try:
            cmd = []
            cmd.append("date")
            cmd.append("+%z")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            timezone = stdout.strip()
            # this will break with a timezone that isn't offset by full hours
            timezoneOffset = int(float(timezone) / 100)
        except:
            raise
            timezoneOffset = 0

    print(timezoneOffset)
    
    exitCode = 0

else:
    # Windows
    
    # The timezone property gives the offset of the local timezone from UTC in seconds, so we need to convert this to hours.
    # Also, this value is positive west of UTC and negative east of UTC.  This is backwards to what we are used to, so the +/- sign needs to be flipped.
    timezoneOffset = ( time.timezone / 60 / 60 ) * -1

    print((str( timezoneOffset )))

    exitCode = 0

sys.exit(exitCode)
