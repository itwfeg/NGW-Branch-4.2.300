#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import tarfile
import shellCommand
import glob
import py_compile
import string
import file_io
import socket
import datetime
import xml.etree.ElementTree as ET
import osUtils

text_characters = "".join(list(map(chr, list(range(32, 127)))) + list("\n\r\t\b"))

_null_trans = None
if sys.version_info >= (3,0):
    _null_trans = "".maketrans("", "")
else:
    import string
    _null_trans = string.maketrans("", "")

def __istext(s):
    '''
    This routine should not be called directly
    '''
    if b'\0' in s:
        return 0

    if not s:  # Empty files are considered text
        return 1
        
    t = [x for x in list(s) if x in (list(range(32,127)) + list(b"\n\r\t\b"))]

    # Get the non-text characters (maps a character to itself then
    # use the 'remove' option to get rid of the text characters.)
    #t = s.translate(_null_trans, text_characters)

    # If more than 30% non-text characters, then
    # this is considered a binary file
    if float(len(t)/len(s)) > 0.30:
        return 0
    return 1


def istextfile(filename, blocksize = 512):
    '''
    Determine if filename is a text based file
    '''
    return __istext(open(filename, "rb").read(blocksize))


def isbinary(filename):
    '''
    Determine if filename is a binary file
    '''
    return not istextfile(filename)


def getHobartRoot():
    if os.name == 'posix':
        rootDir = os.path.join("/", "usr", "local", "hobart")
    else:
        rootDir = os.path.join("C:\\", "Program Files", "hobart")
    return rootDir


def getFeatureRoot():
    rootDir = os.path.join(getHobartRoot(), "features")
    return rootDir


def getPythonRoot():
    '''
    Gets the root location of the python directory on hobart devices
    '''
    pythonRoot = os.path.join(getHobartRoot(), "python")
    return pythonRoot


def sendMessage(client, port, message):
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_address = (client, port)
        sock.connect(client_address)
        sock.sendall((message + '\n').encode())
        sock.close()
        exitCode = 0
    except:
        exitCode = 1
    return exitCode


def setPythonRoot():
    '''
    Set the PATH environment variable to include python root
    '''
    sys.path.append(getPythonRoot())
    return 0


def chmod(filename, permissions='u=rwx,go=rx'):
    '''
    Change permissions on a file for user, group, and or other
    You can use the intuitive form in default or octal value
    '''
    cmd = []
    cmd.append("/bin/chmod")
    cmd.append(permissions)
    cmd.append(filename)
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    return exitCode


def extractTarballAsIs(featureFilename, root='/'):
    exitCode = 0
    try:
        tar = tarfile.open(featureFilename, "r:gz")
        tar.extractall(root)
        tar.close()
    except:
        exitCode = 1
    return exitCode


def extractTarball(featureFilename, root='/'):
    '''
    Extract the feature tarball including fixing ownership and other particulars
    '''
    exitCode = 0

    licenseFile = os.path.join(getPythonRoot(),"license.py")
    licenseFile = licenseFile[1:]  # get rid of first slash which will not be in tarball name

    # extract the feature tarball all files will be extracted to the correct locations on the filesystem
    tar = tarfile.open(featureFilename, "r:gz")

    # loop thru all tar members
    for tarinfo in tar:

        try:
            tar.extract(tarinfo, root)
        except IOError as e:
            os.remove(os.path.join(root, tarinfo.name))
            tar.extract(tarinfo, root)

        if os.path.dirname(tarinfo.name) == os.path.join("etc", "init.d"):
            chmod(filename=os.path.join(root, tarinfo.name), permissions='u=rwx,go=rx')

        if not tarinfo.isdir():
            if isbinary(os.path.join(root, tarinfo.name)):
                chmod(filename=os.path.join(root, tarinfo.name), permissions='u=rwx,go=rx')

        # compile the license file in case it was found in the tarball
        if tarinfo.name == licenseFile:
            py_compile.compile(os.path.join(getPythonRoot(), "license.py"), os.path.join(getPythonRoot(), "license.pyc"))
            os.remove(os.path.join(getPythonRoot(), "license.py"))

    # close the tarball
    tar.close()
    return exitCode


def removeInstallerFiles(featureName, root='/'):
    '''
    WARNING:
    This routine is here in case you want to remove installer files but it is not
    recommended since these files must be present to reinstall the feature during first
    boot after upgrade is performed.

    You had better make sure you know what you are doing if you call this!
    Remove all files under feature directory that were used to install the feature
    '''
    if os.name == 'posix':
        featurePath = os.path.join(root, "usr", "local", "hobart", "features", featureName)
    else:
        featurePath = os.path.join(root, "Program Files", "hobart", "features", featureName)

    filesToRemove = glob.glob(os.path.join(featurePath, "*"))

    for curFile in filesToRemove:
        # remove the feature tarball and install files
        try:
            os.remove(curFile)
        except:
            pass

    if os.name == 'posix':
        os.system('rm -rf {0}'.format(featurePath))
    else:
        os.system('rmdir /s /q {0}'.format(featurePath))

    return 0


def xmlPrettyPrint(rootElement):
    __indent(rootElement)
    ET.dump(rootElement)


def __indent(elem, level=0, more_sibs=False):
    i = "\n"
    if level:
        i += (level-1) * '  '
#
    num_kids = len(elem)

    if num_kids:
        if not elem.text or not elem.text.strip():
            elem.text = i + "  "
            if level:
                elem.text += '  '
#
        count = 0

        for kid in elem:
            __indent(kid, level+1, count < num_kids - 1)
            count += 1
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
            if more_sibs:
                elem.tail += '  '
#
    else:
        if level and (not elem.tail or not elem.tail.strip()):
            elem.tail = i
            if more_sibs:
                elem.tail += '  '


def xmlStringToElementTree(xmlLicenseBuffer):
    return ET.fromstring(xmlLicenseBuffer)


def getInstalledFeatures(xmlFeatureBuffer=None):
    features = {}

    if os.name == 'posix':
        feature_installation_path = '/usr/local/hobart/features/'
    else:
        feature_installation_path = 'C:/Program Files/Hobart/features/'

    for subdirectory in osUtils.listSubdirectories(feature_installation_path):
        feature_name = os.path.basename(subdirectory)
        features[feature_name] = {'name': feature_name,
                                  'version': 'UNKNOWN',
                                  'activation_date': 'UNKNOWN',
                                 }

    if os.name == 'posix':
        featureFilename = os.path.join(os.sep, "usr", "local", "hobart", 'features', 'FEATURES_INSTALLED')
    else:
        featureFilename = os.path.join("C:\\", "Program Files", "hobart", 'features', 'FEATURES_INSTALLED')

    if os.path.exists(featureFilename):
        with open(featureFilename) as featureFile:
            xmlFeatureBuffer = featureFile.read()
    else:
        return features

    element_tree = ET.fromstring(xmlFeatureBuffer)

    for feature_element in element_tree.iter('feature'):
        feature_data = _featureVersionData(feature_element)
        features[feature_data['name']] = feature_data

    return features


def _featureVersionData(feature_element):
    feature_data = {'name': feature_element.get('name'),
                    'version': feature_element.get('version'),
                    'activation_date': feature_element.get('activation_date'),
                   }

    return feature_data


def featureVersionData(xmlFeatureBuffer, feature):
    if not xmlFeatureBuffer:
        raise ValueError('Invalid XML buffer data provided.')

    element_tree = ET.fromstring(xmlFeatureBuffer)

    for feature_element in element_tree.iter('feature'):
        feature_name = feature_element.get('name')

        if feature_name == feature:
            return _featureVersionData(feature_element)

    raise ValueError('Feature {0} is not installed.'.format(feature))


def isFeatureInstalled(xmlFeatureBuffer, feature):
    foundFeature = False

    if xmlFeatureBuffer != None:
        hobartEl = ET.fromstring(xmlFeatureBuffer)
        for featureEl in hobartEl.iter('feature'):
            # print featureEl.get('name')
            if str(featureEl.get('name')) == feature:
               foundFeature = True
#
    return foundFeature


def __xmlRemoveFeature(feature, xmlLicenseBuffer):
    hobartEl = xmlStringToElementTree(xmlLicenseBuffer)
    for featureEl in hobartEl.iter('feature'):
        if str(featureEl.get('name')) == feature:
            hobartEl.remove(featureEl)
#
    return ET.tostring(hobartEl)


def addFeatureToInstalledList(featureName, version):
    feature = {}
    feature['name'] = featureName
    feature['version'] = version
    dateTimeNow = datetime.datetime.now()
    feature['activation_date'] = dateTimeNow.strftime("%m-%d-%Y %H:%M:%S")

    if os.name == 'posix':
        featureFilename = os.path.join(os.sep, "usr", "local", "hobart", 'features', 'FEATURES_INSTALLED')
    else:
        featureFilename = os.path.join("C:\\", "Program Files", "hobart", 'features', 'FEATURES_INSTALLED')
#
    status, buffer = file_io.readFileIntoBuff(featureFilename)

    if status == 'success':
        if isFeatureInstalled(buffer, feature['name']):
            buffer = __xmlRemoveFeature(feature['name'], buffer)
#
        buffer = __xmlAddFeatureToBuffer(buffer, feature)
        status, detail = file_io.writeBufferToFile(featureFilename, buffer, mode='w')
    else:
        buffer = __xmlCreateInitialFeatureBuffer()
        buffer = __xmlAddFeatureToBuffer(buffer, feature)
        status, detail = file_io.writeBufferToFile(featureFilename, buffer, mode='w')
#
    if status == 'success':
        return 0
    else:
        return 1


def __xmlCreateInitialFeatureBuffer():
    hobartEl = ET.Element("hobart")
    return ET.tostring(hobartEl)


def __xmlAddFeatureToBuffer(xmlFeatureBuffer, feature):
    hobartEl = ET.fromstring(xmlFeatureBuffer)
    featureEl = ET.SubElement(hobartEl, 'feature')
    featureEl.set("name", feature['name'])
    featureEl.set("version", feature['version'])
    featureEl.set("activation_date", feature['activation_date'] )
    return ET.tostring(hobartEl)


def addDebianApplicationStartup(commandLine, user):
    xsessionFile = os.path.join(os.sep, "home", user, ".xsessionrc")
    status, lines = file_io.readFromFile(xsessionFile)
    if status == 'success':
        if commandLine in lines:
            return 'success'
        else:
            lines = lines + [commandLine]
    else:
        lines = ['#!/bin/sh -f', commandLine]
    status, details = file_io.writeToFile(xsessionFile, lines)
    if status == 'success':
        return 0
    else:
        return 1


def addInitdScript(featureName, sequence='99'):
    '''
    Install an init.d script in the appropriate init.d location and use typical start/kill
    links
    sequence is where in the start/stop the script should run
    '''

    # create S and K links to smBackend.sh startup script
    cmd = []
    cmd.append("/usr/sbin/update-rc.d")
    cmd.append(featureName)
    cmd.append("start")
    cmd.append(sequence)
    cmd.append("3")
    cmd.append("4")
    cmd.append("5")
    cmd.append(".")
    cmd.append("stop")
    cmd.append(sequence)
    cmd.append("0")
    cmd.append("1")
    cmd.append("6")
    cmd.append(".")

    exitCode, stdout, stderr = shellCommand.execute(cmd)
    return exitCode


def executeInstallScript(featureName, root='/'):
    # run installer script if one exists in the feature upgrade
    if os.name == 'posix':
        if os.path.exists(os.path.join(root,"usr", "local", "hobart", "features", featureName, "install.py")):
            exitCode = os.system('cd %s; PYTHONPATH=$PYTHONPATH:/usr/local/hobart/python /usr/bin/python install.py' % (os.path.join(root, "usr", "local", "hobart", "features", featureName)))
        else:
            exitCode = 0
    else:
        if os.path.exists(os.path.join(root, "Program Files", "hobart", "features", featureName, "install.py")):
            cmd = []
            cmd.append("c:\\python27\\python.exe")
            cmd.append(os.path.join(root, "Program Files", "hobart", "features", featureName, "install.py"))
            exitCode, stdout, stderr = osUtils.winRunElevated(cmd)
    return exitCode


def syncFilesystem():
    '''
    Synchronize filesystem RAM with the uSD card
    '''
    if os.name == 'posix':
        os.system("/bin/sync")
    return 0


def ubootSetBootParameters(partition=1, action='upgrade_scale'):

    exitCode = 0

    cmd = []
    cmd.append("fw_setenv")
    cmd.append("recovery_action")
    cmd.append(action)
    code, stdout, stderr = shellCommand.execute(cmd)

    exitCode |= code

    cmd = []
    cmd.append("fw_setenv")
    cmd.append("active_partition")
    cmd.append(str(partition))
    code, stdout, stderr = shellCommand.execute(cmd)

    # check for new SD format
    cmd = []
    cmd.append('readlink')
    cmd.append('-f')
    cmd.append('/dev/root')
    exitCode, stdout, stderr = shellCommand.execute(cmd)

    recovery = None

    if 'mmcblk0p3' in stdout:
        recovery = os.path.join(os.sep, 'media', 'mmcblk0p1', 'recovery')
    elif 'mmcblk1p3' in stdout:
        recovery = os.path.join(os.sep, 'media', 'mmcblk1p1', 'recovery')
    elif 'mmcblk2p3' in stdout:
        recovery = os.path.join(os.sep, 'media', 'mmcblk2p1', 'recovery')

    if recovery is not None:
        with open(recovery, 'w') as recovery_file:
            recovery_file.write(action)

    exitCode |= code
    return code


def reboot(sync=True):
    '''
    Reboot the device with optional filesystem RAM sync to uSD card
    '''
    if sync:
        syncFilesystem()
    os.system("/sbin/reboot")
    return 0


def startApplication(featureFilename):
    '''
    Launch application in the background.
    '''
    cmd = []
    if os.name == 'posix':
        if os.path.splitext(featureFilename)[1] == '.py':
            cmd.append('/usr/bin/python')
            cmd.append(os.path.join(getPythonRoot(), featureFilename))
        else:
            cmd.append(featureFilename)
#
        cmd.append("&")
        # NOTE:
        # shellCommand.execute() does not background things correctly
        exitCode = os.system(" ".join(cmd))
    else:
        if os.path.splitext(featureFilename)[1] == '.py':
            cmd.append("c:\\python27\\python.exe")
            cmd.append(os.path.join(getPythonRoot(), featureFilename))
#
        else:
            cmd.append(featureFilename)
#
        osUtils.winRunElevated(cmd)
        exitCode = 0

    return exitCode


def stopApplication(featureFilename):
    '''
    Stop an application from running executableFullPathFilename must be the
    same path used when starting the application. Also if the filename is path
    to a python script the python interpreter is used to find the process to kill
    a message to stop the application ideally one would send a message that exits
    the program versus a kill
    '''
    cmd = []
    cmd.append("/usr/bin/pkill")
    cmd.append("-f")

    if os.path.splitext(featureFilename)[1] == '.py':
        cmd.append('/usr/bin/python' + ' ' + os.path.join(getPythonRoot(), featureFilename))
    else:
        cmd.append(featureFilename)
#
    try:
        exitCode, stdout, stderr = shellCommand.execute(cmd)
    except:
        exitCode, stdout, stderr = (0, '', '')
#
    if os.path.splitext(featureFilename)[1] == '.py' and exitCode == 1:
        cmd = []
        cmd.append('/usr/bin/pkill')
        cmd.append('-f')

        if os.path.splitext(featureFilename)[1] == '.py':
            cmd.append('python' + ' ' + os.path.basename(featureFilename))
#
        try:
            exitCode, stdout, stderr = shellCommand.execute(cmd)
        except:
            exitCode, stdout, stderr = (0, '', '')
#
    return exitCode


class retainFile:
    def __init__(self, filepath):
        self.filepath = filepath
        self.backupext = '.bak'

    def backup(self):
        if os.path.exists(self.filepath + self.backupext):
            os.remove(self.filepath + self.backupext)
        if os.path.exists(self.filepath):
            osUtils.copyFile(self.filepath, self.filepath + self.backupext, forced=False)
        return 0

    def restore(self):
        if os.path.exists(self.filepath + self.backupext):
            osUtils.copyFile(self.filepath + self.backupext, self.filepath, forced=False)
        if os.path.exists(self.filepath + self.backupext):
            os.remove(self.filepath + self.backupext)
        return 0


if __name__ == '__main__':
    addFeatureToInstalledList("kiosk_now_serving", "1.99.0")
    addFeatureToInstalledList("kiosk_now_serving", "2.10.0")

