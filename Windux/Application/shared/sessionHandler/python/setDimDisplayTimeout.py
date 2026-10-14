#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# sets dimmer timeout of operator display

import sys
import os
import shellCommand
import argparse
import sendDeviceEventMessage

if __name__ == "__main__":
    exitCode = 1

    parser = argparse.ArgumentParser(description="set dim timeout of operator display")
    parser.add_argument("timeout",
                        action="store",
                        type=int,
                        choices=list(range(0,11)),
                        help="dimmer timeout value")

    args = parser.parse_args()

    if os.name == "posix":
        #PWM channels 1&2 have been switched as of 4/16/2013
        #exitCode = sendDeviceEventMessage.sendMessage(server='localhost',port=5077,message='stopTimer')
        #if exiCode == 0:
        exitCode = sendDeviceEventMessage.sendMessage(server='localhost',port=5077,message='dimDisplayTimeout=%s' % (str(args.timeout)))
        #    if exitCode == 0:
        #        exitCode = sendDeviceEventMessage.sendMessage(server='localhost',port=5077,message='startTimer')

    else:
        exitCode = sendDeviceEventMessage.sendMessage(server='localhost',port=5077,message='dimDisplayTimeout=%s' % (str(args.timeout)))

    sys.exit(exitCode)
