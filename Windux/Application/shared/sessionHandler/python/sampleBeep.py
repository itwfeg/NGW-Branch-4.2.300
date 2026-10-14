#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import shellCommand
import argparse
import time
import sendDeviceEventMessage

def linuxBeep(value):
    cmd = []
    cmd.append("echo")
    cmd.append(str(value))
    cmd.append(">")
    if os.path.isfile("/sys/class/leds/buzzer/brightness"):
        cmd.append("/sys/class/leds/buzzer/brightness")
    elif os.path.isfile("/sys/class/backlight/buzzer.27/brightness"):
        cmd.append("/sys/class/backlight/buzzer.27/brightness")
    elif os.path.isfile("/sys/class/backlight/buzzer.23/brightness"):
        cmd.append("/sys/class/backlight/buzzer.23/brightness")
    elif os.path.isfile("/sys/class/backlight/buzzer.22/brightness"):
        cmd.append("/sys/class/backlight/buzzer.22/brightness")
    elif os.path.isfile("/sys/class/backlight/buzzer/brightness"):
        cmd.append("/sys/class/backlight/buzzer/brightness")
    else:
        cmd.append("/dev/null")
    exitCode, stdout, stderr = shellCommand.execute(cmd=cmd)
    return exitCode

if __name__ == "__main__":
    exitCode = 1

    parser = argparse.ArgumentParser(description="sound operator piezo beeper at specified volume level")
    parser.add_argument("volume",
                        action="store",
                        type=int,
                        choices=list(range(0,255)),
                        help="piezo beeper volume as a percentage")

    args = parser.parse_args()

    # Linux command to beep piezo beeper
    # Windows touchscreen driver will handle this directly
    if os.name == "posix":
        exitCode = linuxBeep(str(args.volume))

    exitCode = sendDeviceEventMessage.sendMessage(server='localhost',port=5077,message='beeperVolume=%s' % (str(args.volume)))

    # Linux command to beep piezo beeper
    # Windows touchscreen driver will handle this directly
    if os.name == "posix":
        time.sleep(0.5)
        exitCode = linuxBeep(str("0"))

    sys.exit(exitCode)
