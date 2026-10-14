#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import shutil
import subprocess
import shellCommand
import hashlib
import string
import re
import file_io

IMAGE_FILES = ['jpg', 'png', 'bmp', 'gif']
AUDIO_FILES = ['aac', 'm4a']
VIDEO_FILES = ['mp4', 'avi']

def filenameWithoutExtension(filePath):
    try:
        fileBasename = os.path.basename(filePath)
        filenameWithoutExtension = os.path.splitext(fileBasename)[0]
        return filenameWithoutExtension
    except:
        return None

def winProcessRunningNumInstances(name):
    import wmi
    c = wmi.WMI()
    procName = re.compile(name.upper())
    numInstances = 0
    for process in c.Win32_Process():
        if procName.match(process.Name.upper()):
            numInstances = numInstances + 1
    return numInstances

def winProcessRunning(name):
    import wmi
    c = wmi.WMI()
    procName = re.compile(name.upper())
    for process in c.Win32_Process():
        # print process.ProcessId, process.Name
        if procName.match(process.Name.upper()):
            return True
    return False

def winRunElevated(cmd):
    if os.name == 'nt':
        return shellCommand.execute(cmd)
    else:
        return command.execute(cmd)

def copyFileElevated(src, dest):
    shutil.copy(src, dest)

def stopProcessWindowTitle(windowTitle):
    cmd = []
    cmd.append("tasklist")
    cmd.append("/FI")
    cmd.append("WINDOWTITLE eq %s" % (windowTitle))
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    lines = stdout.split(os.linesep)
    foundDashes = False
    pid = None
    for i in range(0, len(lines)):
        if foundDashes:
            processTokens = lines[i].split()
            pid = processTokens[1]
            break
        elif lines[i].strip() != '':
            if len(lines[i]) > 3:
                if lines[i][:3] == '===':
                    foundDashes = True

    if pid != None:
        killProcess(pid)

# TODO:
# Does not work but leaving here to find out why some day
def stopProcessCommandLine(commandLineRegEx):
    if os.name == 'posix':
        # TODO:
        # Add support for linux when required
        pass
    else:
        import psutil
        foundOne = False
        procCommand = re.compile(commandLineRegEx)
        allprocesses = list(psutil.process_iter())
        for process in allprocesses:
            # print process.ProcessId, process.Name
            try:
                commandLine = process.cmdline()
                print(commandLine)
                if len(commandLine) > 0:
                    for j in range(0, len(commandLine)):

                        if commandLine[j] is not None and commandLine[j] != '':
                            if procCommand.match(commandLine[j]):
                                print(('Found process named: %s id: %s, terminating it now.' % (process.pid, process.name())))
                                try:
                                    process.terminate()
                                except:
                                    print(('Could not terminate process %s due to Windows COM error, try again' % (process.name())))
                                foundOne = True
            except:
                pass
        if not foundOne:
            print(('Did not find any processes that matched %s' % (commandLineRegEx)))

def setNameValuePairBasedOnMatch(line, match, nameValuePair):
    pattern = re.compile(match)
    if pattern.match(line):
        line = nameValuePair
    return line

def assignVariableInFile(filename, match, nameValuePair):
    status, data = file_io.readFromFile(filename)
    if status == 'success':
        newLines = []
        for i in range(0, len(data)):
            newLines.append(setNameValuePairBasedOnMatch(data[i].rstrip('\r'), match, nameValuePair))

        status, data = file_io.writeToFile(filename, newLines)
        if status != 'success':
            return ('error', None)
        else:
            return ('success', None)
    else:
        return ('error', None)

def stopProcess(name):
    if os.name == 'nt':
        import wmi
        c = wmi.WMI()
        foundOne = False
        procName = re.compile(name.upper())
        for process in c.Win32_Process():
            # print process.ProcessId, process.Name
            if procName.match(process.Name.upper()):
                print(('Found process named: %s id: %s, terminating it now.' % (process.Name, process.ProcessId)))
                try:
                    process.Terminate()
                except:
                    print(('Could not terminate process %s due to Windows COM error, try again' % (process.Name)))
                foundOne = True
        if not foundOne:
            print(('Did not find any processes that matched %s' % (name)))
    else:
        # TODO:
        # Add support for linux when required
        pass

def diskUsage(path):
    """Return disk usage for mount point path associated with path in a tuple
    (total, used, free) in bytes
    block size = 1024
    """
    block_size = 1024
    total = None
    used = None
    free = None

    if os.name == 'posix':
        st = os.statvfs(path)
        free = (st.f_bavail * st.f_frsize)
        total = (st.f_blocks * st.f_frsize)
        used = (st.f_blocks - st.f_bfree) * st.f_frsize
#
    else:
        return (-1, -1, -1)
#
    return (total / block_size, used / block_size, free / block_size)

def memInfo():
    """
    Get the memory info from system
    Specifically total memory, used memory, free memory in kB
    """
    memTotal = 0
    memFree = 0
    memUsed = 0
    if os.name == 'posix':
        memFreeLine = re.compile("^MemFree:\s+[0-9]+\s+kB$")
        memTotalLine = re.compile("^MemTotal:\s+[0-9]+\s+kB$")
        status, data = file_io.readFromFile(os.path.join("/", "proc", "meminfo"))
        if status == 'success':
            for i in range(0, len(data)):
                if memFreeLine.match(data[i]):
                    tokens = data[i].split(' ')
                    memFree = int(tokens[-2])
                elif memTotalLine.match(data[i]):
                    tokens = data[i].split(' ')
                    memTotal = int(tokens[-2])
        memUsed = memTotal - memFree
    else:
        print('Unsupported')
    return (memTotal, memUsed, memFree)

def applicationPath(file=None):
    # determine if application is a script file or frozen exe
    frozen = False
    if getattr(sys, 'frozen', False):
        application_path = os.path.dirname(sys.executable)
        frozen = True
    else:
        if file == None:
            if __file__:
                application_path = os.path.dirname(__file__)
        else:
            application_path = os.path.dirname(file)
    return (frozen, application_path)

def getImmediateDirectories(dir):
    if os.path.isdir(dir):
        return [name for name in os.listdir(dir)
            if os.path.isdir(os.path.join(dir, name))]

def mkDestPath(path):
    if not os.path.exists(path):
        os.makedirs(path)

def rmDestPath(path):
    if os.path.exists(path):
        shutil.rmtree(path=path, ignore_errors=True)

def copytree(src, dst, symlinks=False, ignore=None):
    if not os.path.exists(dst):
        os.makedirs(dst)
    if os.path.exists(src):
        for item in os.listdir(src):
            s = os.path.join(src, item)
            d = os.path.join(dst, item)
            if os.path.isdir(s):
                copytree(s, d, symlinks, ignore)
            else:
                if not os.path.exists(d) or os.stat(src).st_mtime - os.stat(dst).st_mtime > 1:
                    shutil.copy2(s, d)

def killProcess(pid):
    if os.name == 'nt':
        subprocess.Popen("taskkill /F /T /PID %i" % int(pid), shell=True)
    else:
        import signal
        os.kill(pid, signal.SIGKILL)

def fileType(file):
    filename, extension = os.path.splitext(str(file))
    extension = extension[1:]
    if extension in IMAGE_FILES:
        return 'image'
    elif extension in VIDEO_FILES:
        return 'video'
    elif extension in AUDIO_FILES:
        return 'audio'

def createThumbNailofVideo(videoName, resolutionWidth, resolutionHeight, thumbNailFilename):
    cmd = []
    cmd.append("ffmpeg")
    cmd.append("-y")
    cmd.append("-i")
    cmd.append(videoName)
    cmd.append("-vframes")
    cmd.append("1")
    cmd.append("-s")
    cmd.append("%dx%d" % (resolutionWidth, resolutionHeight))
    cmd.append("-ss")
    cmd.append("1")
    cmd.append(thumbNailFilename)

    shellCommand.execute(cmd)


def listSubdirectories(directoryPath):
    filenames = os.listdir(directoryPath)

    for filename in filenames:
        fullPath = os.path.join(directoryPath, filename)
        if os.path.isdir(fullPath):
            yield fullPath


def readUbootVersion():
    with open('/uBootVersion') as uboot_version_file:
        uboot_version = uboot_version_file.read().rstrip()

    return uboot_version


def readDNSSuffixes():
    if os.name == 'posix':
        scale_settings_path = '/usr/local/hobart/db/settingswizard/scalesettings'
    elif os.name == 'nt':
        scale_settings_path = 'C:/Program Files/Hobart/db/settingswizard/scalesettings'
    else:
        raise OSError('Could not determine OS to find scalesettings file')

    if not os.path.exists(scale_settings_path):
        raise OSError('scalesettings file not found at {0}'.format(scale_settings_path))

    suffixes = []

    with open(scale_settings_path) as scale_settings_file:
        for line in scale_settings_file:
            if line.startswith('DNSSuffix'):
                variable, suffix = line.split('=')
                if not suffix.rstrip():
                   continue

                suffixes.append(suffix.rstrip())

    return suffixes


def getDirs(dirPath):
    fileList = []
    for root, dirs, files in os.walk(dirPath):
#        for dir in dirs:
#            thatDirList = getDirs(os.path.join(root,  dir))
#            for fileName in thatDirList:
#                fileList.append(fileName)
        for fileName in files:
            filePath = os.path.join(root, fileName)
            fileList.append(filePath)
    return fileList

def getNewFiles(localFilePaths, updateFilePaths):
    newFilePaths = []
    for localFilePath, updateFilePath in zip(localFilePaths, updateFilePaths):
        if isNewer(localFilePath, updateFilePath):
            newFilePaths.append(updateFilePath)
    return newFilePaths

def isNewer(localFilePath, updateFilePath):
    localMd5String = ''
    updateMd5String = ''
    with open(localFilePath) as localFileObject:
        localMd5String = getMd5(localFileObject)
    with open(updateFilePath) as updateFileObject:
        updateMd5String = getMd5(updateFileObject)
    if localMd5String == updateMd5String:
        return False
    return True

def copyFile(sourceFilePath, destinationFilePath, forced=False):
    outputDir = os.path.dirname(destinationFilePath)

    try:
        shutil.copy(sourceFilePath, destinationFilePath)
    except Exception as error:
        if 'No such file' in str(error):
            if not os.path.isdir(outputDir) and forced:
                mkDestPath(outputDir)
                shutil.copy(sourceFilePath, destinationFilePath)
                return

        raise error

def moveFile(sourceFilePath, destinationFilePath, forced=False):
    outputDir = os.path.dirname(destinationFilePath)
    try:
        shutil.move(sourceFilePath, destinationFilePath)
#
    except Exception as error:
        if 'No such file' in str(error):
            if not os.path.isdir(outputDir) and forced:
                mkDestPath(outputDir)
                shutil.move(sourceFilePath, destinationFilePath)

def clearDir(dirPath):
    try:
        shutil.rmtree(dirPath)
        os.makedirs(dirPath)
    except:
        pass

def getMd5(f):
    md5 = hashlib.md5()
    for chunk in iter(lambda: f.read(8192), b''):
        md5.update(chunk)
    return md5.hexdigest()

def copyFileObj(file, filePath):
    shutil.copyfileobj(file, filePath)
#
#
def process_count(process_name):
    import psutil

    process_count = 0
    java_binary = 'javaw.exe'

    for process in psutil.process_iter():
        try:
            process_info = process.as_dict(attrs=['pid', 'name', 'exe', 'cmdline', 'cwd'])
#
        except (psutil.NoSuchProcess, psutil.AccessDenied):
            continue
#
        if not process_info['name']:
            continue
#
        if process_info['name'] == process_name:
            process_count += 1
#
        if process_info['name'] == java_binary:
            if check_java_process(process_id=process_info['pid'], jars=[process_name]):
                process_count += 1
#
    return process_count
#
#
def check_java_process(process_id, jars):
    cmd = ('tasklist',
           '/fi',
           'pid eq {process_id}'.format(process_id=process_id),
           '/v',
           'fo',
           'csv')
#
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    tasks = stdout.split('\r\n')

    for task in tasks:
        tokens = task.split(',')

        if not tokens:
            return False
#
        if not tokens[0]:
            return False
#
        if tokens[0] == '"Image Name"':
            return False
#
        jar_name = tokens[-1][1:-1]
        if jar_name in jars:
            return True
#
    return False
#
#
def process_running(process_name):
    import psutil

    for process in psutil.process_iter():
        try:
            process_info = process.as_dict(attrs=['pid', 'name', 'exe', 'cmdline', 'cwd'])
#
        except (psutil.NoSuchProcess, psutil.AccessDenied):
            continue
#
        if not process_info['name']:
            continue
#
        if process_info['name'] == process_name:
            return True
#
        elif check_java_process(process_id=process_info['pid'], jars=[process_name]):
            return True
#
    return False
#
#
def processes_running(process_names):
    """ Given an iterable of binary executable names, and an iterable of java jar files,
        this routine determines if any of the processes given are running.
        Much faster than using WMI (like the other routines in this module).
        Uses psutil but still needs tasklist for java .jars as psutil doesn't retrieve the
            entire command line.
#
        NOTE: Only works on Windows due to use of tasklist;
            Cannot be used on scale unless psutil is built and installed in Timesys RFS.
    """
    import psutil
    java_binary = 'javaw.exe'

    for process in psutil.process_iter():
        try:
            process_info = process.as_dict(attrs=['pid', 'name', 'exe', 'cmdline', 'cwd'])
#
        except (psutil.NoSuchProcess, psutil.AccessDenied):
            continue
#
        if not process_info['name']:
            continue
#
        if process_info['name'] in process_names:
            return True
#
        elif process_info['name'] == java_binary:
            if check_java_process(process_id=process_info['pid'], jars=process_names):
                return True
#
    return False
#
#
def stop_processes(process_names):
    """ Given an iterable of binary executable names, and an iterable of java jar files,
        this routine will terminate all process names given that are running.
        Much faster than using WMI (like the other routines in this module).
        Uses psutil but still needs tasklist for java .jars as psutil doesn't retrieve the
            entire command line.
#
        NOTE: Only works on Windows due to use of tasklist;
            Cannot be used on scale unless psutil is built and installed in Timesys RFS.
    """
    import psutil

    java_binary = 'javaw.exe'

    for process in psutil.process_iter():
        try:
            process_info = process.as_dict(attrs=['pid', 'name', 'exe', 'cmdline', 'cwd'])
#
        except (psutil.NoSuchProcess, psutil.AccessDenied):
            with open('sm-debug.log', 'a') as log_file:
                log_file.write('Cannot access process, this could be a permission error or the process may have been terminated.\n')
#
            continue
#
        if not process_info['name']:
            continue
#
        if process_info['name'] in process_names:
            with open('sm-debug.log', 'a') as log_file:
                log_file.write('Killing process named: ' + str(process_info['name']))
                log_file.write(' because ' + process_info['name'] + ' in [' + ', '.join(process_names) + ']' + '\n')
#
            process.terminate()
#
        elif process_info['name'] == java_binary:
            if check_java_process(process_id=process_info['pid'], jars=process_names):
                process.terminate()
#
        else:
            with open('sm-debug.log', 'a') as log_file:
                log_file.write('Ignoring process named: ' + str(process_info['name']) + '\n')
#
    return False
#
#
if __name__ == '__main__':
    process_names = ('sm.exe',
                     'rsa.exe',
                     'HTeSmSshServer.exe',
                     'flashkeyEditor.exe',
                     'Hobart Label Designer',
                     'New Hobart VNC Connection')
#
    stop_processes(process_names=process_names)

