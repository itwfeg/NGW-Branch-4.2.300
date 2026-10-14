import subprocess
import os
import string
import re

# 'Shell' should only be used explicitly in specific instances.
# Using 'Shell' exposes the logic to additional command processing errors.
# convertCmdForShell() attempts to handle those errors, but that overhead should
# be avoided altogether if it's not necesssary.
#
#if os.name == 'nt':
#    shellDefault = True
#else:
shellDefault = False

whiteSpaceRegExp = re.compile('\s')

def convertCmdForShell(cmd):
    convertedCmd = cmd
    # args with spaces must be quoted under windows environments
    if os.name == 'nt' or os.name == 'ce':
        convertedCmd = []
        for arg in cmd:
            strippedArg = arg.strip()
            if whiteSpaceRegExp.search(strippedArg):
                convertedCmd.append('\"' + strippedArg + '\"')
            else:
                convertedCmd.append(strippedArg)
    convertedCmd = string.join(convertedCmd, ' ')
    return convertedCmd

whiteSpaceRegExp = re.compile('\s')

def convertCmdForShell(cmd):
    convertedCmd = cmd
    # args with spaces must be quoted under windows environments
    if os.name == 'nt' or os.name == 'ce':
        convertedCmd = []
        for arg in cmd:
            strippedArg = arg.strip()
            if whiteSpaceRegExp.search(strippedArg):
                convertedCmd.append('\"' + strippedArg + '\"') 
            else:
                convertedCmd.append(strippedArg) 
    convertedCmd = string.join(convertedCmd, ' ')
    return convertedCmd

def noConsoleWindow():
    startupinfo = subprocess.STARTUPINFO()
    startupinfo.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    startupinfo.wShowWindow = subprocess.SW_HIDE
    return startupinfo


def executeShellCommandNoPipe(cmd, shell=shellDefault):
    exitCode = 1
    stdout = ""
    stderr = ""
    try:
        if os.name == 'nt':
            startupData = noConsoleWindow()
        else:
            startupData = None

        if shell:
            cmd = convertCmdForShell(cmd)

        proc = subprocess.Popen(cmd,
                                cwd=os.getcwd(),
                                stdout=open(os.devnull, 'wb'),
                                stderr=open(os.devnull, 'wb'),
                                startupinfo=startupData,
                                shell=shell)

        proc.communicate()
        if stdout == None:
            stdout = ""
        if stderr == None:
            stderr = ""
        exitCode = proc.returncode
    except:
        pass

    return (exitCode,  stdout,  stderr)


def executeShellCommand(cmd,  shell=shellDefault):
    exitCode = 1
    stdout = ""
    stderr = ""
    try:
        if os.name == 'nt':
            startupData = noConsoleWindow()
        else:
            startupData = None

        if shell:
            cmd = convertCmdForShell(cmd)

        proc = subprocess.Popen(cmd,
                                cwd=os.getcwd(),
                                stdin=subprocess.PIPE,
                                stdout=subprocess.PIPE,
                                stderr=subprocess.STDOUT,
                                startupinfo=startupData,
                                shell=shell)

        stdout, stderr = proc.communicate()
        if stdout == None:
            stdout = ""
        if stderr == None:
            stderr = ""
        exitCode = proc.returncode
    except:
        pass

    return (exitCode,  stdout,  stderr)

def executeShellCommandBackground(cmd,  shell=shellDefault):
    exitCode = 1
    stdout = ""
    stderr = ""
    try:
        if os.name == 'nt':
            startupData = noConsoleWindow()
        else:
            startupData = None

        if shell:
            cmd = convertCmdForShell(cmd)

        proc = subprocess.Popen(cmd,
                                cwd=os.getcwd(),
                                stdin=subprocess.PIPE,
                                stdout=subprocess.PIPE,
                                stderr=subprocess.STDOUT,
                                startupinfo=startupData,
                                shell=shell)

        if stdout == None:
            stdout = ""
        if stderr == None:
            stderr = ""
        exitCode = proc.returncode
    except:
        pass

    return (exitCode,  stdout,  stderr)

def executeShellCommand1(cmd, shell=shellDefault):
    exitCode = 1
    stdout = ""
    stderr = ""
    try:
        # We have to force using shell under Windows or
        # command prompt window will flash while executing command
        if os.name == 'nt':
            shell = True

        stdout = subprocess.check_output(args=cmd, cwd=os.getcwd(), shell=shell)
        exitCode = 0
    except:
        pass

    return (exitCode,  stdout,  stderr)

def execute(cmd, shell=False):
    exitCode = 1
    stdout = ""
    stderr = ""
    try:
        if len(cmd) > 0:
            # check for echo to device file command which is a file open and write to device
            if len(cmd) == 4:
                if string.lower(string.strip(cmd[0])) == 'echo' and cmd[2] == '>':
                    exitCode, stdout, stderr = executeDeviceWrite(cmd[3], cmd[1])
                else:
                    exitCode,  stdout,  stderr = executeShellCommand(cmd, shell)
            else:
                exitCode,  stdout,  stderr = executeShellCommand(cmd, shell)
    except:
        pass
    return (exitCode,  stdout,  stderr)

def execCommandGetPID(cmd,  shell=False):
    exitCode = 1
    stdout = ""
    stderr = ""
    proc = None
    try:
        if os.name == 'nt':
            startupData = noConsoleWindow()
        else:
            startupData = None

        if shell:
            cmd = convertCmdForShell(cmd)

        proc = subprocess.Popen(cmd,
                                cwd=os.getcwd(),
                                stdin=subprocess.PIPE,
                                stdout=subprocess.PIPE,
                                stderr=subprocess.STDOUT,
                                startupinfo=startupData,
                                shell=shell)

        #stdout, stderr = proc.communicate()
        if stdout == None:
            stdout = ""
        if stderr == None:
            stderr = ""
        exitCode = proc.returncode
    except Exception as err:
        print ('ERR:', err)

    if (proc is None):
        return (exitCode, stdout, stderr, 0)

    return (exitCode,  stdout,  stderr,  proc.pid)

def executeDeviceWrite(device,value):
    try:
        device = open(device, "w")
        device.write(value)
        device.close()
        exitCode = 0
    except:
        exitCode = 1
    return (exitCode, "", "")

