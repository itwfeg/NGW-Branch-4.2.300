# -*- coding: utf-8 -*-

import glob
import os
import string
import file_io
import time
import reboot
import select
import struct
import sendDeviceEventMessage
import shellCommand
import languageSettings
import messageBox
import re
import sys
import platform
#Reset button on the Access is mapped to event1. Not sure why need to look into re-map 
#back to event2. change for now and fix later -- ats
#RESET_BUTTON_INPUT_DEVICE = "/dev/input/event2"
RESET_BUTTON_INPUT_DEVICE = "/dev/input/event1"
RESET_TIMER = 2.0 * 60.0 # 2 minutes
FORMAT = 'llHHI'
EVENT_SIZE = struct.calcsize(FORMAT)

def getCurrentProcessList():
    if os.name == "posix":
        processList = {}
        for pid_path in glob.glob('/proc/[0-9]*/'):
            # cmdline represents the command whith which the process was started
            try:
                f = open("%s/cmdline" % pid_path)
                pid = pid_path.split("/")[2] # get the PID
                # we replace the \x00 to spaces to make a prettier output from kernel
                cmdline = f.read().replace("\x00", " ").rstrip()
                f.close()
                tokens = string.split(cmdline, " ")
                cmdline = tokens[0]
                if len(tokens) >= 2:
                    if tokens[0] == '/usr/bin/python' or tokens[0] == 'python' or tokens[0] == 'exec':
                        cmdline = tokens[1]
                processList[cmdline] = pid
            except:
                # processes come and go and one could go bye bye between
                # the glob scan and the file being opened
                pass
    elif os.name == "nt":
        import win32com.client
        processList = {}
        strComputer = "."
        objWMIService = win32com.client.Dispatch("WbemScripting.SWbemLocator")
        objSWbemServices = objWMIService.ConnectServer(strComputer,"root\cimv2")
        colItems = objSWbemServices.ExecQuery("Select * from Win32_Process")
        for objItem in colItems:
            # If we are monitoring a python script, we want to know which script is running
            if objItem.Name == "pythonw.exe":
                if objItem.CommandLine:
                    cmdline = objItem.CommandLine
                    tokens = string.split(cmdline, " ")
                    if len(tokens) >= 2:
                        cmdline = tokens[1]
                        for j in xrange(2, len(tokens)):
                            cmdline = cmdline + ' ' + tokens[j]
            else:
                cmdline = objItem.Name
            pid = objItem.ProcessId
            cmdline = cmdline.lower()
            cmdline = cmdline.strip(" \"")
            processList[cmdline] = pid

    return processList

def getProcessesToMonitorFromXsessiond(exclusionList=[]):
    monitorList = []
    for startupFiles in glob.glob(os.path.join(os.sep, "etc", "X11", "Xsession.d", "*")):
        status,  data = file_io.readFromFile(startupFiles, "r")
        if status == 'success':
            for i in xrange(0, len(data)):
                tokens = string.split(data[i], " ")
                cmdline = tokens[0]
                if len(tokens) >= 2:
                    if tokens[0] == '/usr/bin/python' or tokens[0] == 'python' or tokens[0] == 'exec':
                        cmdline = tokens[1]
                if cmdline not in exclusionList:
                    monitorList.append(cmdline)
    return monitorList

def getProcessesToMonitor(exclusionList=[]):

    # TODO:
    # this must match what is in /etc/init.d/rc.local
    # ultimately need better way
    data = []
    if os.name == "posix":
        # TODO TOUCH
        #data.append("eGTouchD")
        data.append('/usr/local/hobart/python/processDeviceEvents.py')
        data.append('/usr/local/hobart/backend/smBackend.py')
        data.append('/usr/local/hobart/python/sftpUpdate.py')
        data.append('/usr/local/hobart/python/usbDeviceManager.py')
        data.append("/usr/bin/chobart_d")
        data.append("/usr/bin/scaleui")

        # Monitor GCD ... only if Service Counter
        try:

            file = open( "/etc/hobart/scale-type.dat", 'rb' )
            scaleType = file.read()
            scaleType = scaleType.replace( '\n', '' )
            file.close()

            if ( scaleType == 'service counter' ):
                data.append('/usr/bin/gcd')

        except:
            pass

    elif os.name == "nt":
        import configInfo
        data.append(os.path.join(configInfo.getRootFolder(),'python','processDeviceEvents.py'))
        data.append(os.path.join(configInfo.getRootFolder(),'backend','smBackend.py'))
        data.append(os.path.join(configInfo.getRootFolder(),'python','sftpUpdate.py'))
        data.append(os.path.join(configInfo.getRootFolder(),'python','usbDeviceManager.py'))
        data.append("ScaleService.exe")
        data.append("HobartSSHD.exe")
        data.append("HobartHD.exe")
        data.append("scaleui.exe")
        if os.path.exists("C:\\gcddetected"):
            data.append("gcd.exe")

    monitorList = []

    for i in xrange(0, len(data)):
        cmdline = data[i]
        tokens = string.split(data[i], " ")
        if len(tokens) >= 2:
            if tokens[0] == '/usr/bin/python' or tokens[0] == 'python' or tokens[0] == 'exec':
                cmdline = tokens[1]
                for j in xrange(2, len(tokens)):
                    cmdline = cmdline + ' ' + tokens[j]
        if cmdline not in exclusionList:
            # For windows, we are case insensitive for paths.  Make sure all lowercase for strcmps
            if os.name == "nt":
                cmdline = cmdline.lower()
            monitorList.append(cmdline)
    return monitorList

def procStatus(pid):
    if os.name == "posix":
        for line in open("/proc/%d/status" % int(pid)).readlines():
            if line.startswith("State:"):
                state = line.split(":",1)[1].strip().split(' ')[0]
                if state == 'Z':
                    return 'Zombie'
                elif state == 'D':
                    return 'Uninterruptible Sleep'
                elif state == 'R':
                    return 'Running or Runnable'
                elif state == 'S':
                    return 'Interruptible sleep'
                elif state == 'T':
                    return 'Stopped'
                elif state == 'W':
                    return 'Paging'
                elif state == 'X':
                    return 'Dead'
    elif os.name == "nt":
        import win32com.client
        processList = {}
        strComputer = "."
        objWMIService = win32com.client.Dispatch("WbemScripting.SWbemLocator")
        objSWbemServices = objWMIService.ConnectServer(strComputer,"root\cimv2")
        colItems = objSWbemServices.ExecQuery("Select * from Win32_Process WHERE ProcessId = " + str(pid))
        # With Windows, can only tell if it is running, unless we create a watchdog mechanism
        for objItem in colItems:
            return 'Running or Runnable'
    return None

def processIsRunning(command):
    status = False
    processesRunning = getCurrentProcessList()
    if processesRunning.has_key(command):
        state = procStatus(processesRunning[command])
        if state != 'Zombie':
            #print 'Process %s with pid %s, state %s is running' % (command, processesRunning[command], state)
            status = True
        else:
            #print 'Process %s with pid %s, state %s is not running' % (command, processesRunning[command], state)
            status = False
    return status

def allProcessesRunning(processesToMonitor):
    status = True
    processesRunning = getCurrentProcessList()

    if os.name == "posix":
        for i in xrange(0, len(processesToMonitor)):
            tokens = string.split(processesToMonitor[i], " ")
            #print 'tokens[0] = %s' % (tokens[0])
            if not processesRunning.has_key(tokens[0]):
                #print "Process %s not in process table and therefore not running" % (tokens[0])
                status = False
                break
            else:
                state = procStatus(processesRunning[tokens[0]])
                if state != 'Zombie':
                    #print 'Process %s with pid %s, state %s is running' % (tokens[0], processesRunning[tokens[0]], state)
                    pass
                else:
                    #print 'Process %s with pid %s, state %s is not running' % (tokens[0], processesRunning[tokens[0]], state)
                    status = False
                    break
    elif os.name == "nt":
        for process in processesToMonitor:
            if not processesRunning.has_key(process):
                status = False
                break

    return status

def isPrepack():
    if  platform.system() == 'Linux':
        #check to see if config file is present
        if os.path.exists(os.path.join("/", "etc", "hobart",  "config_linux.dat")):
            fileIn = open(os.path.join("/", "etc", "hobart",  "config_linux.dat"), 'rb')
            fileData = fileIn.readlines()
            fileIn.close()

            p = re.compile('scaleModel=([0-9])')

            for line in fileData:
                if p.match(line) != None and p.match(line).groups()[0] == '2':
                    return True

    #default to service scale
    return False

def writeToInfoLog(line, additionalLines=None):
    if line is None:
        line = ''
    if additionalLines is None:
        additionalLines = []

    if os.name == "posix":
        logFilename = os.path.join("/","usr","local","hobart","info.log")
    elif os.name == "nt":
        import configInfo
        logFilename = os.path.join(configInfo.getRootFolder(),"log","info.log")
        if not os.path.exists(os.path.join(configInfo.getRootFolder(),"log")):
            import osUtils
            osUtils.mkDestPath(os.path.join(configInfo.getRootFolder(),"log"))

    try:
        outputFile = open(logFilename, 'a')

        stamp = time.strftime("%a %b %d %H:%M:%S %Y")
        outputFile.write("%s %s\n" % (stamp, line))

        for i in xrange(0,len(additionalLines)):
            outputFile.write("   * %s\n" % (additionalLines[i]))

        outputFile.close()

    except IOError, exception_details:
        print exception_details


if __name__ == "__main__":

    monitorTime = 60.0

    # sleep N seconds before process monitoring starts
    # to allow all processes to come up and to monitor
    # at 1 minute intervals
    #time.sleep(monitorTime)

    shutdown = False

    exclusionList = []
    exclusionList.append("/usr/bin/xinput")
    exclusionList.append("startlxde")

    if isPrepack():
        exclusionList.append('/usr/bin/gcd')

    processesToMonitor = getProcessesToMonitor(exclusionList)
    #print processesToMonitor

    #for curCmdLine in processesRunning.keys():
    #    print "pid = %s, command = %s" % (processesRunning[curCmdLine],  curCmdLine)

    if os.name == "posix":
        event_input_device = open(RESET_BUTTON_INPUT_DEVICE, "rb")
        inputs = [event_input_device]
    elif os.name == "nt":
        import time
        # TODO - how to detect reset button
        inputs = []

    outputs = []

    while not shutdown:

        if os.name == "posix":
            readable, writable, exceptional = select.select(inputs, outputs, inputs, monitorTime)
            for s in readable:
                if s is event_input_device:
                    # Shutdown reset button pressed
                    # Check if any processes dead and if so
                    # do immediate shutdown since scale application is dead
                    event = event_input_device.read(EVENT_SIZE)
                    (tv_sec, tv_usec, type, code, value) = struct.unpack(FORMAT, event)
                    value = '%d' % (type)
                    if value == '1':
                        writeToInfoLog('RESET BUTTON PRESSED!')
                        # tell OS to reboot scale
                        reboot.reboot()
                        shutdown = True
                    else:
                        continue
        else:
            time.sleep(monitorTime)

        if not allProcessesRunning(processesToMonitor):

            processes = []
            for i in xrange(0,len(processesToMonitor)):
                if not processIsRunning(processesToMonitor[i]):
                    processes.append(processesToMonitor[i])

            writeToInfoLog("The following processes are not running:", processes)
