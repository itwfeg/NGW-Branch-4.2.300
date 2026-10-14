#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# windows only, see messageBox for linux version

import os
import sys

import messageBox
import sendDeviceEventMessage
import shellCommand

def showBox(msg):
    exitCode, mbId = messageBox.display(message=msg)

def destroy():

    cmd = []
    cmd.append("taskkill")
    cmd.append("/F")
    cmd.append("/IM")
    cmd.append("mb.exe")

    exitCode, stdout, stderr = shellCommand.execute(cmd)


# Instantiate the Server and Begin Event Listener Reactor
if __name__ == "__main__":
    showBox("Scale is updating. Please Wait...")
    sys.exit(0)

