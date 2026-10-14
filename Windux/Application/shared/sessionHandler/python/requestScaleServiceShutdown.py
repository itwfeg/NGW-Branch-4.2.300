#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import socket
import time
import shellCommand
import time

import string
import languageSettings
import messageBox
#import sendDeviceEventMessage

exitCode = 0

cmd = []
cmd.append("sc")
cmd.append("query")
cmd.append("Hobart Scale Service")
#cmd.append("|")
#cmd.append("find")
#cmd.append("STOPPED")
exitCode, stdout, stderr = shellCommand.execute(cmd, shell=True)
#print stdout

if 'RUNNING' in stdout:
    language = languageSettings.currentLanguage()
    message = ""

    if language == 'spanish':
        message = "La balanza esta reiniciándose\npor favor no desconecte la energía."
    elif language == 'french':
        message = "La balance est en train de redémarrer\nveuillez ne pas couper l'alimentation."
    else:
        message = "Scale is rebooting please do not remove power."
    exitCode,  mbId = messageBox.display(message=message, timer=50000)
    server  = 'localhost'
    port    = 6000
    message = 'RT01' + chr(0x1F) + 'DT1' + chr(0x1E) + 'RT4F' + chr(0x1E) + 'RTFF' + chr(0x1E)
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_address = (server, port)
    #print >>sys.stderr, 'connecting to %s port %s' % server_address
    sock.connect(server_address)
    sock.sendall(message.encode())
    time.sleep(10)
    sock.close()
elif 'PENDING' in stdout:
    pass
else:
    #print "down"
    os.system('shutdown /r /t 0')

'''
language = languageSettings.currentLanguage()
message = ""

if language == 'spanish':
    message = "La balanza se esta apagando por favor espere..."
elif language == 'french':
    message = "La balance est en train de s'éteindre, veuillez patienter..."
else:
    message = "Scale is shutting down please wait..."
#exitCode,  mbId = messageBox.display(message=message, timer=50000)
print "A" + str(exitCode)
cmd = []
cmd.append("pythonw")
cmd.append("C:\\Program Files\\Hobart\python\setBrightness.py")
cmd.append("both")
cmd.append("105")
exitCode, stdout, stderr = shellCommand.execute(cmd)
print "B" + str(exitCode)
# Kill GCD
os.system('taskkill /f /im gcd.exe')
print "C"
# Kill ScaleUI
os.system('taskkill /f /im scaleui.exe')
print "D"
# Tell ScaleService to stop
os.system('sc stop "Hobart Scale Service"')
print "E"
cmd = []
cmd.append("pythonw")
cmd.append("C:\\Program Files\\Hobart\python\setBrightness.py")
cmd.append("both")
cmd.append("105")
exitCode, stdout, stderr = shellCommand.execute(cmd)
print "F" + str(exitCode)
time.sleep(10)
os.system('taskkill /F /IM ScaleService.exe')
print "G"
'''
'''
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_address = (server, port)
#print >>sys.stderr, 'connecting to %s port %s' % server_address
sock.connect(server_address)
sock.sendall(message)
time.sleep(10)
sock.close()
'''
exitCode = 0
#print str(exitCode)
sys.exit(exitCode)
