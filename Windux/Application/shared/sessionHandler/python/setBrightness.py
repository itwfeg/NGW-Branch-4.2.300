#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# sets brightness of display

import sys
import os
import shellCommand
import argparse
import sendDeviceEventMessage
import re

def linuxSetBacklight(value, device):
    exitCode = 0
    
    if device == 2:
        exitCode = sendDeviceEventMessage.sendMessage(server='localhost',port=5077,message='operatorDisplayBrightness=%s' % (str(value)))
    elif device == 1:
        exitCode = sendDeviceEventMessage.sendMessage(server='localhost',port=5077,message='customerDisplayBrightness=%s' % (str(value)))
        
    return exitCode

def isWindowsPrepack():
    if sys.platform == 'win32':
        #check to see if config file is present
        if os.path.exists(os.path.join("C:\Program Files", "Hobart")):
            fileIn = open(os.path.join("C:\Program Files", "Hobart", "configc.dat"), 'rb')
            fileData = fileIn.readlines()
            fileIn.close()

            p = re.compile('scaleModel=([0-9])')

            for line in fileData:
                if p.match(line) != None and p.match(line).groups()[0] == '2':
                    return True

    #default to service scale
    return False

def ntSetBacklight(duty, device):
    exitCode = 0
    #print("duty   is " + str(duty))
    #print("device is " + str(device))
    # EXAMPLE COMMAND LINE: setBrightness.py customer 4
    # DEVELOPER NOTE
    # device:
    #          1 = INTEL LVDS (customer)
    #          2 = CH7511     (operator)
    
    # duty (brightness)
    #          1 = 30%
    #          2 = 40%
    #          3 = 50%
    #          4 = 60%
    #          5 = 70%
    #          6 = 80%
    #          7 = 90%
    #          8 = 100%
    
    import configInfo
    import osUtils
    
    panel = ''
    cmd = []
    cmd.append(configInfo.getRootFolder() + os.path.join(os.sep, 'bin', 'PanelBLCtrlTest'))
    if device != '0':
        cmd.append(device)

    adjustedduty = int(duty)
    if int(duty) > 100:
        adjustedduty = 100
        #print "A"

    if device == '1':
        #print "B"
        if int(duty) < 40:
            adjustedduty = 40
        opDuty =  str((int(adjustedduty) / 10) - 2)
        cmd.append(opDuty)
        #print(cmd)
        exitCode, stdout, stderr = osUtils.winRunElevated(cmd)

    elif device == '2':
        #print "C"
        # HW-27 Per hardware team request, we can only set between 60% and 100% on operator display
        # Spread settings out between 10% and 100%
        # 90% & 100% --> 8, 70% & 80% --> 7, 50% & 60% --> 6,  30% & 40% --> 5, 10% & 20% --> 4 
        if int(duty) < 30:
            adjustedduty = 10
        opDuty = str((int(adjustedduty) + 10) / 20 + 3)
        cmd.append(opDuty)
        print(cmd)
        exitCode, stdout, stderr = osUtils.winRunElevated(cmd)
    elif device == '0':
        #print "both dimmed"
        # customer first
        cmd.append("1")
        if int(duty) < 40:
            adjustedduty = 40
        opDuty =  str((int(adjustedduty) / 10) - 0)
        cmd.append(opDuty)
        #print "====================="
        #print(cmd)
        exitCode, stdout, stderr = osUtils.winRunElevated(cmd)
        cmd = []
        cmd.append(configInfo.getRootFolder() + os.path.join(os.sep, 'bin', 'PanelBLCtrlTest'))
        cmd.append("2")
        cmd.append(opDuty)
        #print(cmd)
        exitCode, stdout, stderr = osUtils.winRunElevated(cmd)
        #print "====================="

        # operator second

    return exitCode    
    
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

    if os.name == "posix":
        device = 0
        
        if args.display == 'operator' or args.display == 'both' or args.display == '0':
            device = 1
        else:
            device = 2
            
        linuxSetBacklight(args.brightness, device)
            
    else:
        # In SetupUtility.cpp, value passed to setBrightness is Percentage + 104 (see setupUtility::setBrightnessBut_click())
        # So subtract 104
        illumination = args.brightness - 100
        
        if args.display == "operator":
            # Operator only
            if isWindowsPrepack():
                exitCode = ntSetBacklight(str(illumination), '1')
            else:
                exitCode = ntSetBacklight(str(illumination), '2')
            exitCode = sendDeviceEventMessage.sendMessage(server='localhost',port=5077,message='operatorDisplayBrightness=%s' % (str(args.brightness)))

        if args.display == "customer":
            # Customer only
            if isWindowsPrepack(): 
                exitCode = ntSetBacklight(str(illumination),  '2')
            else:
                exitCode = ntSetBacklight(str(illumination),  '1')
            exitCode = sendDeviceEventMessage.sendMessage(server='localhost',port=5077,message='customerDisplayBrightness=%s' % (str(args.brightness)))

        if args.display == "both" or args.display == "0":
            # Both displays
            exitCode = ntSetBacklight(str(illumination),  '2')
            exitCode = ntSetBacklight(str(illumination),  '1')
            exitCode = sendDeviceEventMessage.sendMessage(server='localhost',port=5077,message='operatorDisplayBrightness=%s' % (str(args.brightness)))
            exitCode = sendDeviceEventMessage.sendMessage(server='localhost',port=5077,message='customerDisplayBrightness=%s' % (str(args.brightness)))

            
    sys.exit(exitCode)
