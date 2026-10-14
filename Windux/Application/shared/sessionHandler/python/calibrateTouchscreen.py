#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Will launch the touchscreen calibration utility and close

import sys
import os
import shellCommand

rebootRequired = False
cmd = []

calibration = ''

if os.name == "posix":
    # egalax/bcm calibration
    if os.path.exists(os.path.join("/","dev","input","touchscreenusb")):
        os.environ['DISPLAY'] = ':0'
        cmd.append("/usr/bin/eCalib")
        #cmd.append("Cal")
        cmd.append("Linz9")

        calibration = '/etc/eGTouch0.param'

    # combo calibration
    elif os.path.exists(os.path.join("/","dev","input","touchscreen")):
        os.environ['TSLIB_TSEVENTTYPE'] = 'INPUT'
        os.environ['TSLIB_CONSOLEDEVICE'] = 'none'
        os.environ['TSLIB_FBDEVICE'] = '/dev/fb0'
        os.environ['TSLIB_TSDEVICE'] = '/dev/input/touchscreen'
        os.environ['TSLIB_CALIBFILE'] = '/etc/pointercal'
        os.environ['TSLIB_CONFFILE'] = '/etc/ts.conf'
        os.environ['TSLIB_PLUGINDIR'] = '/usr/lib/ts'
        cmd.append("/usr/bin/ts_calibrate")

        calibration = '/etc/pointercal'

        # reboot is required for settings to take effect
        # not during first boot!
        if os.path.exists(os.path.join("/","etc","firstBoot")):
            rebootRequired = True

else:
    cmd.append("C:\\Program Files\\eGalaxTouch\\xAuto4PtsCal.exe")

# calibrate!
complete = False

while not complete:
    print('calibrating...')
    exitCode, stdout, stderr = shellCommand.execute(cmd=cmd)

    if not calibration:
        complete = True
    elif os.path.exists(calibration):
        complete = True

if os.name == "posix":
    cmd = []
    cmd.append("/bin/sync")
    exitCode, stdout, stderr = shellCommand.execute(cmd)

# reboot if needed
if rebootRequired:
    if os.name == "posix":
        os.system("/sbin/reboot")
    else:
        # TODO
        pass

sys.exit(exitCode)
