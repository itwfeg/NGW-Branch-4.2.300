#!/usr/bin/env python
# -*- coding: utf-8 -*-

# -*- coding: utf-8 -*-

import os
import sys

import messageBox

def showBox(msg):
    exitCode, mbId = messageBox.display(message=msg, width=200, height=100, fontFamily="Sans",  fontSize=32,  timer=30)

if __name__ == "__main__":
    showBox("Maintenance reboot required.\n\n  Please Wait...")
    sys.exit(0)

