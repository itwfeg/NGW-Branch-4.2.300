#!/usr/bin/env python2
# -*- coding: utf-8 -*-
# @Filename : imx6_sbc_leds.py
# @Creation Date : 2019-10-25-17-33
# @Project: python
# @Organization: ITW Hobart, Inc.
# @AUTHOR : Scott Davis
import argparse
import os
import sys
import threading
import time


class LedControl:

    def __init__(self):

        led_red_device = None
        led_yellow_green_device = None
        led_on_value = 0
        led_off_value = 0

        if 'linux' in sys.platform:

            if os.path.exists(os.path.join('/', 'sys', 'class', 'leds', 'yellow')):
                # ARK board only board that has a yellow led!
                led_yellow_green_device = "/sys/class/leds/yellow/brightness"
                led_red_device = "/sys/class/leds/red/brightness"
                led_on_value = 100
                led_off_value = 0

            elif os.path.exists(os.path.join('/', 'sys', 'class', 'leds', 'green')):
                # ARA board only board that has a green led in the led's class!
                led_yellow_green_device = "/sys/class/leds/green/brightness"
                led_red_device = "/sys/class/leds/red/brightness"
                led_on_value = 100
                led_off_value = 0

            elif os.path.exists(os.path.join('/', 'usr', 'local', 'gpio', 'debug_led_green')):
                # BCM board is only board that has a green debug led!
                led_yellow_green_device = "/usr/local/gpio/debug_led_green/value"
                led_red_device = "/usr/local/gpio/debug_led_red/value"
                led_on_value = 1
                led_off_value = 0

            self.leds = {'red': {'device_name': led_red_device,
                                 'on_value': led_on_value,
                                 'off_value': led_off_value,
                                 'blink': False},
                         'yellow_green': {'device_name': led_yellow_green_device,
                                          'on_value': led_on_value,
                                          'off_value': led_off_value,
                                          'blink': False}}

        else:
            print("Board LED control is only supported in Linux")
            sys.exit(1)

    def led_light_control(self, color, power='on'):
        """
        Turn on/off specified LED on Hobart single board computer
        :param color: red or yellow_green
        :param power: 'on', 'off' to turn on or off LED
        :return: None
        """

        control_value = self.leds[color]['off_value']
        if power == 'on':
            control_value = self.leds[color]['on_value']

        with open(self.leds[color]['device_name'], "wb") as f:
            f.write(bytes(control_value))

    def blink_led(self, color, duration=1, state='on'):
        """
        Blink specified LED on and off
        :param color: red or yellow_green
        :param duration: duration in seconds
        :param state: on or off to start blinking LED
        :param async: run as separate thread and don't block if True, synchronous if False
        :return: None
        """
        if state == 'on':
            self.leds[color]['blink'] = True
            x = threading.Thread(target=self.__led_blink_control, args=(color, duration))
            x.start()

        else:
            self.leds[color]['blink'] = False

    def __led_blink_control(self, color, duration):

        while self.leds[color]['blink']:
            self.led_light_control(color, 'on')
            # print "on"
            time.sleep(duration)
            self.led_light_control(color, 'off')
            # print "off"
            time.sleep(duration)


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Turn on/off specific board LED")
    parser.add_argument("led_color", type=str, help='LED color to manipulate (red or yellow_green or all)')
    parser.add_argument("state", type=str,
                        help="led state (on or off)")
    args = parser.parse_args()

    board_leds = LedControl()
    if args.led_color == 'red' or args.led_color == 'yellow_green':
        board_leds.led_light_control(args.led_color, args.state)
    elif args.led_color == 'all':
        board_leds.led_light_control('red', args.state)
        board_leds.led_light_control('yellow_green', args.state)
    else:
        print("Invalid led_color specified")

    # led_blinker.blink_led(color='red', duration=1, state='on')
    # led_blinker.blink_led(color="yellow_green", duration=1, state='on')
    # time.sleep(30)
    # led_blinker.blink_led(color='red', duration=1, state='off')
    # led_blinker.blink_led(color="yellow_green", duration=1, state='off')
