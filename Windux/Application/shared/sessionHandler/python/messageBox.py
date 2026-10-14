#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import string
import shellCommand
import osUtils
import os
import configInfo
import deviceType


def display(message, width=1024, height=600, fontFamily="Sans", fontSize=24, timer=None):
    if os.name == 'posix':
        mbExeFullPath = os.path.join(os.sep, "usr", "bin", "mb")
    else:
        mbExeFullPath = configInfo.getRootFolder() + os.path.join(os.sep, "mb")

    if not os.path.exists(mbExeFullPath):
        return -1, None

    myEnv = os.environ
    myEnv['DISPLAY'] = ":0"
    myEnv['LC_ALL'] = "en_US.utf-8"

    cmd = []
    cmd.append(mbExeFullPath)
    cmd.append("-full")
    cmd.append('-msg')
    cmd.append(message.encode('utf-8'))

    if timer != None:
        cmd.append("-timer")
        timeValue = 1000 * timer
        cmd.append(str(timeValue))

    cmd.append("-fontFamily")
    cmd.append(fontFamily)

    if not (deviceType.is_device_type("/", "scale") or deviceType.is_device_type("/", "ngw")):
        ## override font size to 10 point if message box displayed on portion scale slicer */
        cmd.append("-nologo")

    if deviceType.is_device_type("/","slicer"):
        fontSize = 10

    cmd.append("-fontPointSize")
    cmd.append(str(fontSize))
    cmd.append("&")


    exitCode, stdout, stderr, pid = shellCommand.execCommandGetPID(cmd,False)

    return exitCode, pid


def destroy(pid):
    # do not try and kill a process with id 0 or we will be killing everything
    if pid != 0:
        try:
            osUtils.killProcess(pid)
        except:
            pass


if __name__ == "__main__":
    import time
    exitCode,  mbId = display("hello\nhello2")
    print(mbId)
    time.sleep(5)
    destroy(mbId)
