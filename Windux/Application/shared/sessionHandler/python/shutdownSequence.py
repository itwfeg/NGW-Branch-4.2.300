#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import string
import languageSettings
import messageBox
import time

pythonPath = os.path.join("/","usr","local","hobart","python")
appsPath = os.path.join("/","usr","local","hobart","apps")

def get_process(name):
    pids = []
    process = None
    for i in os.listdir('/proc'):
        if i.isdigit():
            pids.append(i)

    for pid in pids:
        try:
            proc = open(os.path.join('/proc', pid, 'cmdline'), 'r').readline()
            if name in proc:
                process = pid
                break
        except:
            pass

    return process          

def is_running(pid):
    return os.path.exists("/proc/%s" % str(pid))

def processShutdownSequence(runlevel):

    language = languageSettings.currentLanguage()
    message = ""

    if int(runlevel) == 0:

        if language == 'spanish':
            message = "La balanza se esta apagando por favor espere..."
        elif language == 'french':
            message = "La balance est en train de s'éteindre, veuillez patienter..."
        else:
            message = "Scale is shutting down please wait..."
    else:

        if language == 'spanish':
            message = "La balanza esta reiniciándose\npor favor no desconecte la energía."
        elif language == 'french':
            message = "La balance est en train de redémarrer\nveuillez ne pas couper l'alimentation."
        else:
            message = "Scale is rebooting please do not remove power."

    exitCode,  mbId = messageBox.display(message=message, timer=75000)

    if os.name == 'posix':
        os.system('/usr/bin/pkill -f "/usr/bin/python /usr/local/hobart/python/processMonitor.py"')

        os.system("/usr/bin/python /usr/local/hobart/python/cleanWirelessDevice.py")

        os.system("/usr/bin/kill -9 $(/usr/bin/pgrep scaleui)")

        # try to terminate chobart_d nicely
        pid = get_process("chobart_d")

        print( "term chobart_d..." )
        os.system("/usr/bin/kill $(/usr/bin/pgrep chobart_d)")

        if pid is not None:
            print( "waiting on chobart_d " + str(pid) )

            # wait until the process end
            for _ in range( 0, 60 ):
                if is_running( pid ):
                    print( "waiting..." )
                    time.sleep( 1 )

        if is_running(pid):
            print( "chobart_d still active!" )
            os.system("/usr/bin/kill -9 $(/usr/bin/pgrep chobart_d)")

        os.system("/usr/bin/kill -9 $(/usr/bin/pgrep gcd)")

        # hard reset weigher / printer controller (FPC)
        os.system("/bin/echo 0 >> /usr/local/gpio/hub1_enable/value")

		# hard reset NGW wrapper
        os.system("/bin/echo 0 >> /usr/local/gpio/wrapper_reset/value")
 
        # shutdown processDeviceEvents
        os.system('/usr/bin/pkill -f "/usr/bin/python /usr/local/hobart/python/processDeviceEvents.py"')
        # shutdown installer
        os.system('/usr/bin/pkill -f "/usr/bin/python /usr/local/hobart/python/usbDeviceManager.py"')
        # shutdown smBackend
        os.system('/usr/bin/pkill -f "/usr/bin/python /usr/local/hobart/backend/smBackend.py"')
        # shutdown sftp update
        os.system('/usr/bin/pkill -f "/usr/bin/python /usr/local/hobart/python/sftpUpdate.py"')

        # TODO TOUCH
        # shutdown touchscreen controller
        #os.system('/usr/bin/pkill eGTouchD')

        # shutdown app daemon processes
        os.system("/usr/bin/python %s --folder %s &" % (os.path.join(pythonPath,"appsShutdown.py"), appsPath))

        #shutdown second core to serialize the reset of shutdown /reboot process
        #echo 0 >>/sys/devices/system/cpu/cpu1/online

        os.system('/bin/echo "rc.local Stop" >> /root/logs/shutdownlog')

        # sync filesystem
        os.system('/bin/sync')
    elif os.name == 'nt':

        # Shutdown scripts
        os.system('taskkill /f /im python.exe')

        # Kill GCD
        os.system('taskkill /f /im gcd.exe')

        # Kill ScaleUI
        os.system('taskkill /f /im scaleui.exe')

        if int(runlevel) == 0:
            os.system('shutdown /t 0')
        else:
            os.system('shutdown /r /t 0')

if __name__ == "__main__":
    processShutdownSequence(sys.argv[1])
    sys.exit(0)
