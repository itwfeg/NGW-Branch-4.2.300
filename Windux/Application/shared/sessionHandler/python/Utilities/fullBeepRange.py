#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import beep
import time

if os.name == 'posix':
    for value in range(0,255):
        exitCode = beep.linuxBeep(value)
        if exitCode != 0:
            print('Error beeping device')
            break
        else:
            print(('beep value = %d' % (value)))
            time.sleep(1.0)
# Windows
else:
    # TODO need commands on Windows
    pass

sys.exit(exitCode)
