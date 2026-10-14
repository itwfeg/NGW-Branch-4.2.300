#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import socket
import time
import shellCommand
import time
import osUtils
import configInfo
import string
import languageSettings
import messageBox

language = languageSettings.currentLanguage()
message = ""

if language == 'spanish':
    message = "La balanza se esta apagando por favor espere..."
elif language == 'french':
    message = "La balance est en train de s'éteindre, veuillez patienter..."
else:
    message = "Scale is shutting down please wait..."
exitCode,  mbId = messageBox.display(message=message, timer=50000)
time.sleep(5)

#print "A" + str(exitCode)
os.system('"C:\\Program Files\\Hobart\\bin\\BL_EN.exe" 1 0')
os.system('"C:\\Program Files\\Hobart\\bin\\BL_EN.exe" 2 0')
cmd = []
cmd.append(os.path.join(configInfo.getRootFolder(), "bin", "usbpwrtest.exe"))
cmd.append("0")
cmd.append("0")
osUtils.winRunElevated(cmd)
#print "B" + str(exitCode)
# Kill GCD
os.system('taskkill /f /im gcd.exe')
#print "C"
# Kill ScaleUI
os.system('taskkill /f /im scaleui.exe')
#print "D"
# Tell ScaleService to stop
os.system('sc stop "Hobart Scale Service"')
#print "F" + str(exitCode)
time.sleep(10)
os.system('taskkill /F /IM ScaleService.exe')
#print "G"
sys.exit(exitCode)
