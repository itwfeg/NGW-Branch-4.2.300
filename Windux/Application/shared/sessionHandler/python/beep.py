#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import shellCommand
import argparse
import getModelName

def linuxBeep(value):
    cmd = []
    cmd.append("echo")
    cmd.append(str(value))
    cmd.append(">")
    if os.path.isfile("/sys/class/leds/buzzer/brightness"):
        cmd.append("/sys/class/leds/buzzer/brightness")
    elif os.path.isfile("/sys/class/backlight/buzzer.23/brightness"):
        cmd.append("/sys/class/backlight/buzzer.23/brightness")
    elif os.path.isfile("/sys/class/backlight/buzzer.22/brightness"):
        cmd.append("/sys/class/backlight/buzzer.22/brightness")
    else:
        cmd.append("/dev/null")
    exitCode, stdout, stderr = shellCommand.execute(cmd=cmd)
    return exitCode
    
# Windows command to set frequency and duration of piezo beeper.
# It is anticipated that the EGalax touch driver will handle playing the sound    import configInfo
# This does not play the beep sound. It merely sets the proper frequency and duration of the beep sound.
# Setting frequency and duration to 0 mutes the beep.
def ntSetBeep(on):
    import configInfo
    import osUtils
    cmd = []
    touchBeepCtrlPath = configInfo.getRootFolder() + os.path.join(os.sep, 'bin', 'TouchBeepCtrl.exe')
    cmd.append(touchBeepCtrlPath)
    print(touchBeepCtrlPath)
    
    if on:
        # This is the frequency and duration that most closely emulates the HTi beep sound
        cmd.append("640")
        cmd.append("50")
    else:
        # Setting frequency and duration to 0 mutes the beep
        cmd.append("0")
        cmd.append("0")
    osUtils.winRunElevated(cmd)
    return 0
    
if __name__ == "__main__":
    exitCode = 1

    parser = argparse.ArgumentParser(description="sound operator piezo beeper at specified volume level")
    parser.add_argument("volume",
                        action="store",
                        type=int,
                        choices=list(range(0,255)),
                        help="piezo beeper volume as a percentage")

    args = parser.parse_args()

    if os.name == 'posix':
        exitCode = linuxBeep(str(args.volume))
    elif os.name == 'nt':
        exitCode = ntSetBeep(int(args.volume) > 0)
    else:
        exitCode = 1

    sys.exit(exitCode)
