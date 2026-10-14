#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import glob
import os
import string
import file_io
import time
import reboot
import select
import struct
import datetime
import sendDeviceEventMessage
import shellCommand
import logging
from logging.handlers import RotatingFileHandler

import languageSettings
import messageBox
import re
import sys
import platform

RESET_BUTTON_INPUT_DEVICE = "/dev/input/resetbutton"
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
                cmdline = f.read().replace(b"\x00", " ").rstrip()
                f.close()
                tokens = cmdline.split(" ")
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
                    tokens = cmdline.split(" ")
                    if len(tokens) >= 2:
                        cmdline = tokens[1]
                        for j in range(2, len(tokens)):
                            cmdline = cmdline + ' ' + tokens[j]
            else:
                cmdline = objItem.Name
            pid = objItem.ProcessId
            cmdline = cmdline.lower()
            cmdline = cmdline.strip(" \"")
            processList[cmdline] = pid

    #print "proc list"
    #print processList
    return processList


def getProcessesToMonitorFromXsessiond(exclusionList=[]):
    monitorList = []
    for startupFiles in glob.glob(os.path.join(os.sep, "etc", "X11", "Xsession.d", "*")):
        status,  data = file_io.readFromFile(startupFiles, "r")
        if status == 'success':
            for i in range(0, len(data)):
                tokens = data[i].split(" ")
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
        # data.append("eGTouchD")
        data.append('/usr/local/hobart/python/processDeviceEvents.py')
        data.append('/usr/local/hobart/backend/smBackend.py')
        data.append('/usr/local/hobart/python/sftpUpdate.py')
        data.append('/usr/local/hobart/python/usbDeviceManager.py')
        data.append("/usr/bin/chobart_d")
        data.append("/usr/bin/scaleui")
        data.append("/usr/bin/gcd")

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

    for i in range(0, len(data)):
        cmdline = data[i]
        tokens = data[i].split(" ")
        if len(tokens) >= 2:
            if tokens[0] == '/usr/bin/python' or tokens[0] == 'python' or tokens[0] == 'exec':
                cmdline = tokens[1]
                for j in range(2, len(tokens)):
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
    if command in processesRunning:
        state = procStatus(processesRunning[command])
        if state != 'Zombie':
            # print 'Process %s with pid %s, state %s is running' % (command, processesRunning[command], state)
            status = True
        else:
            # print 'Process %s with pid %s, state %s is not running' % (command, processesRunning[command], state)
            status = False
    return status


def allProcessesRunning(processesToMonitor):
    status = True
    processesRunning = getCurrentProcessList()

    if os.name == "posix":
        for i in range(0, len(processesToMonitor)):
            tokens = processesToMonitor[i].split(" ")
            # print 'tokens[0] = %s' % (tokens[0])
            if tokens[0] not in processesRunning:
                # print "Process %s not in process table and therefore not running" % (tokens[0])
                status = False
                break
            else:
                state = procStatus(processesRunning[tokens[0]])
                if state != 'Zombie':
                    # print 'Process %s with pid %s, state %s is running' % (tokens[0], processesRunning[tokens[0]],
                    # state)
                    pass
                else:
                    # print 'Process %s with pid %s, state %s is not running' % (tokens[0], processesRunning[tokens[0]],
                    # state)
                    status = False
                    break
    elif os.name == "nt":
        for process in processesToMonitor:
            if process not in processesRunning:
                status = False
                break

    return status


def createInfoLog(max_log_size=(1000*1024), number_backups=3):

    logFilename = None

    if os.name == "posix":
        logger = logging.getLogger('Process Monitor Log')
        logger_stream_handler = logging.StreamHandler()
        logger_stream_handler.setFormatter(logging.Formatter('%(message)s'))
        logger.setLevel(logging.INFO)
        logFilename = os.path.join("/", "usr", "local", "hobart", "process_monitor.log")
        handler = RotatingFileHandler(logFilename, maxBytes=max_log_size, backupCount=number_backups)
        logger.addHandler(handler)
        return logger

    elif os.name == "nt":
        import configInfo
        logFilename = os.path.join(configInfo.getRootFolder(), "log", "info.log")
        if not os.path.exists(os.path.join(configInfo.getRootFolder(), "log")):
            import osUtils
            osUtils.mkDestPath(os.path.join(configInfo.getRootFolder(), "log"))
        return


def writeToInfoLog(logger, line, additionalLines=None):

    if line is None:
        line = ''

    if additionalLines is None:
        additionalLines = []

    try:
        logger.info('====================================================')
        stamp = time.strftime("%a %b %d %Y %H:%M:%S")
        logger.info("%s: %s" % (stamp, line))

        for i in range(0, len(additionalLines)):
            logger.info(additionalLines[i])

    except:
        pass


if __name__ == "__main__":

    # set this to true record state of a scale periodically regardless of whether
    # a process is no longer running
    periodicRecorderEnabled = False
    # set monitor time to number of seconds to monitor processes if they are running
    # or to periodically record state of scale
    monitorTime = 60.0

    logger = createInfoLog()

    event_input_device = None

    # sleep N seconds before process monitoring starts
    # to allow all processes to come up and to monitor
    # at 1 minute intervals
    # time.sleep(monitorTime)

    shutdown = False

    exclusionList = []
    exclusionList.append("/usr/bin/xinput")
    exclusionList.append("startlxde")

    processesToMonitor = getProcessesToMonitor(exclusionList)
    # print processesToMonitor

    # for curCmdLine in processesRunning.keys():
    #    print "pid = %s, command = %s" % (processesRunning[curCmdLine],  curCmdLine)

    inputs = None

    if os.name == "posix":
        event_input_device = open(RESET_BUTTON_INPUT_DEVICE, "rb")
        inputs = [event_input_device]
    elif os.name == "nt":
        import time
        # TODO - how to detect reset button
        inputs = []

    outputs = []

    previous_process_issues = []

    while not shutdown:

        if os.name == "posix":
            readable, writable, exceptional = select.select(inputs, outputs, inputs, monitorTime)
            for s in readable:
                if s is event_input_device:
                    # Shutdown reset button pressed
                    # Check if any processes dead and if so
                    # do immediate shutdown since scale application is dead
                    event = event_input_device.read(EVENT_SIZE)
                    (tv_sec, tv_usec, ev_type, code, ev_value) = struct.unpack(FORMAT, event)
                    value = '%d' % ev_type
                    if value == '1':
                        writeToInfoLog(logger, 'RESET BUTTON PRESSED!')
                        # tell OS to reboot scale
                        reboot.reboot()
                        shutdown = True
                    else:
                        continue
            for s in exceptional:
                if s is event_input_device:
                    # Try to re-open input device
                    print('input device in exception state!')
                    if os.path.exists(RESET_BUTTON_INPUT_DEVICE):
                        event_input_device = open(RESET_BUTTON_INPUT_DEVICE, "rb")
                        inputs = [event_input_device]
        else:
            time.sleep(monitorTime)

        if not allProcessesRunning(processesToMonitor) or periodicRecorderEnabled:

            #print "check process list"
            #print processesToMonitor

            processes = []
            for i in range(0, len(processesToMonitor)):
                if not processIsRunning(processesToMonitor[i]):
                    processes.append(processesToMonitor[i])

            #print 'prev and current'
            #print previous_process_issues
            #print processes

            if len(processes) > 0 or periodicRecorderEnabled:

                if (previous_process_issues != processes) or periodicRecorderEnabled:

                    #print "log issue"
                    # record processes that are not running
                    writeToInfoLog(logger, "processes not running output:", processes)

                    # list open files
                    # lsof

                    # list open sockets
                    # netstat -an

                    # list kernel modules
                    # lsmod

                    # check memory fragmentation
                    # cat /proc/buddyinfo

                    # check files that are created when videos are played
                    # look for oper and cust video playback files

                    commandsToLog = [['/bin/dmesg'],
                                     ['/usr/bin/ps', '-aux'],
                                     ['/usr/bin/top', '-b', '-n', '1'],
                                     ['/usr/bin/lsof'],
                                     ['/bin/netstat', '-an'],
                                     ['/sbin/lsmod'],
                                     ['/bin/df', '-k', '/media/mmcblk1p1'],
                                     ['/bin/df', '-k', '/media/mmcblk1p2'],
                                     ['/bin/cat', '/proc/buddyinfo'],
                                     ['/bin/cat', '/proc/mounts']]

                    for commandSequence in commandsToLog:
                        # record command output
                        #print commandSequence
                        exitCode, stdout, stderr = shellCommand.execute(commandSequence)
                        #print exitCode
                        if exitCode == 0:
                            stdout_split_at_newline = stdout.splitlines()
                            writeToInfoLog(logger, '%s output:' % " ".join(commandSequence), stdout_split_at_newline)

                    # record certain tmp files if they exist
                    files_to_log = ['/tmp/startScaleApplication.log',
                                    '/tmp/gcd.txt',
                                    '/tmp/gcdCom.txt'
                                    '/tmp/scaleui.txt',
                                    '/tmp/scaleuiComm.txt'
                                    '/tmp/sm.log']
                    for log_file in files_to_log:
                        if os.path.exists(log_file):
                            exitCode, stdout, stderr = shellCommand.execute(['/bin/cp', '-f', log_file, '/usr/local/hobart'])

            previous_process_issues = processes
        else:
            previous_process_issues = []
