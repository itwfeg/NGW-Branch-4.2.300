#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# @Filename : blink_test_leds
# @Creation Date : 2019-10-25-17-33
# @Project: python
# @Organization: ITW Hobart, Inc.
# @AUTHOR : Scott Davis
import os
import sys
import time

def main():
    if sys.platform == 'linux':
        if os.path.exists(os.path.join('/', 'sys', 'class', 'leds', 'yellow')):
            #ARK board only board that has a yellow led!
            cmdLedAOn = "echo 100 > /sys/class/leds/red/brightness"
            cmdLedAOff = "echo 0 > /sys/class/leds/red/brightness"
            cmdLedBOn = "echo 100 > /sys/class/leds/yellow/brightness"
            cmdLedBOff = "echo 0 > /sys/class/leds/yellow/brightness"

        elif os.path.exists(os.path.join('/', 'sys', 'class', 'leds', 'green')):
            #ARA board only board that has a green led in the led's class!
            cmdLedAOn = "echo 100 > /sys/class/leds/green/brightness"
            cmdLedAOff = "echo 0 > /sys/class/leds/green/brightness"
            cmdLedBOn = "echo 100 > /sys/class/leds/red/brightness"
            cmdLedBOff = "echo 0 > /sys/class/leds/red/brightness"

        elif  os.path.exists(os.path.join('/', 'usr', 'local', 'gpio', 'debug_led_green')):
            #BCM board is only board that has a green debug led!
            cmdLedAOn = "echo 1 > /usr/local/gpio/debug_led_green/value"
            cmdLedAOff = "echo 0 > /usr/local/gpio/debug_led_green/value"
            cmdLedBOn = "echo 1 > /usr/local/gpio/debug_led_red/value"
            cmdLedBOff = "echo 0 > /usr/local/gpio/debug_led_red/value"

        while 1:
            #turn off hub leds
            os.system(cmdLedAOn)
            os.system(cmdLedBOn)
            time.sleep(1)
            os.system(cmdLedAOff)
            os.system(cmdLedBOff)
            time.sleep(1)
    else:
        raise RuntimeError("LED testing only supported on Linux")

if __name__ == "__main__":
        main()

