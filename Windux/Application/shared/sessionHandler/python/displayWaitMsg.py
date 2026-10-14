#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import messageBox
import time

def showBox(msg):
    exitCode, mbId = messageBox.display(message=msg, width=200, height=100, fontFamily="Sans",  fontSize=32,  timer=30)
    print(mbId)
    time.sleep(1)

if __name__ == "__main__":
    showBox("Maintenance reboot required.\n\n  Please Wait...")
