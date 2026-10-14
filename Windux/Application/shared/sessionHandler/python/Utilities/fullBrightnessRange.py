#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Brighten the screen gradually to full brightness

import os
import sys
import setBrightness
import time

if os.name == 'posix':

    for device in range(1,3):
        for value in range(0,256):
            exitCode = setBrightness.linuxSetBacklight(value,device)
            if exitCode != 0:
                print('Error setting brightness value')
                break
            else:
                print(('brightness value = %d' % (value)))
                time.sleep(0.05)
# Windows
else:
    # TODO need commands on Windows
    pass
