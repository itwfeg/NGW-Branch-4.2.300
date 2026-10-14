# sets brightness of displays to blackout before a shutdown or reboot

import sys
import os
import shellCommand
import argparse
import sendDeviceEventMessage
import getModelName
import setBrightness

if os.name == 'posix':
    setBacklight = setBrightness.linuxSetBacklight
elif os.name == 'nt':
    setBacklight = setBrightness.ntSetBacklight
else:
    def setBacklight(device,  value):
        return 1

if __name__ == "__main__":
    exitCode = 1

    parser = argparse.ArgumentParser(description="set brightness of specified display")
    parser.add_argument("display",
                        action="store",
                        choices=['customer','operator','both'],
                        help="display device")

    parser.add_argument("brightness",
                        action="store",
                        type=int,
                        choices=list(range(0,256)),
                        help="brightness value")

    args = parser.parse_args()

    #PWM channels 1&2 have been switched as of 4/16/2013
    if args.display == 'operator':
        exitCode = setBacklight(str('0'),  2)
        exitCode = sendDeviceEventMessage.sendMessage(server='localhost',port=5077,message='operatorDisplayBrightness=%s' % (str(args.brightness)))

    if args.display == 'customer':
        exitCode = setBacklight(str('0'),  1)
        exitCode = sendDeviceEventMessage.sendMessage(server='localhost',port=5077,message='customerDisplayBrightness=%s' % (str(args.brightness)))

    if args.display == 'both':
        exitCode = setBacklight(str('0'),  2)
        exitCode = sendDeviceEventMessage.sendMessage(server='localhost',port=5077,message='operatorDisplayBrightness=%s' % (str(args.brightness)))

        if exitCode == 0:
            exitCode = setBacklight(str('0'),  1)
            exitCode = sendDeviceEventMessage.sendMessage(server='localhost',port=5077,message='customerDisplayBrightness=%s' % (str(args.brightness)))

    sys.exit(exitCode)
