#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# beeps for the BB3 (online ordering)

import sys
import os
import shellCommand
import argparse
import time
from random import randrange

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

def nasty_noise():
    for i in range(0,500):
        linuxBeep(randrange(50, 250))
    linuxBeep(0)

def error(count=5):
    for i in range(0,count):
        nasty_noise()
        time.sleep(0.5)

def attack():
    for i in list(range(0,20,5))[::1]:
        linuxBeep(i)
        time.sleep(0.05)
    linuxBeep(250)

def decay():
    for i in list(range(0,20,2))[::-1]:
        linuxBeep(i)
        time.sleep(0.05)
    linuxBeep(0)

def smooth(count=3):
    for i in range(0,count):
        attack()
        decay()

def blips(count=7):
    for i in range(0,count):
        linuxBeep(250)
        time.sleep(0.075)
        linuxBeep(5)
        time.sleep(0.075)
    linuxBeep(0)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="various notification beeps for the black box")
    parser.add_argument("beep_type", choices=["smooth","error","blips"], help="type of beep")
    parser.add_argument("beep_count", help="number of times to beep")

    args = parser.parse_args()
    
    count = int(args.beep_count)
    
    if args.beep_type == "smooth":
        beep = smooth
    elif args.beep_type == "error":
        beep = error
    elif args.beep_type == "blips":
        beep = blips
        
    beep(count)
    
    linuxBeep(0)
