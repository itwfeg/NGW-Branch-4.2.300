# -*- coding: utf-8 -*-

import sys
import os
import string
import shutil
import stat

dev_testing = False

if os.name == 'posix':
    # Change below to where HTi looks for it
    scaleSettingsFileLocation = '/usr/local/hobart/db/settingswizard/scalesettings'
elif os.name == 'nt':
    # Change below to where HTx looks for it (soon it will be C:\Program Files\Hobart\scalesettings)
    scaleSettingsFileLocation = 'C:\Hobart\Db\SettingsWizard\scalesettings'

    if os.path.exists(scaleSettingsFileLocation):
        os.chmod(scaleSettingsFileLocation, stat.S_IWRITE)

def getScaleSettingsFileLocation():
    return scaleSettingsFileLocation

def scaleSettings_file_exists():
    try:
        with open(scaleSettingsFileLocation) as f:
            return True
    except IOError:
        return False

def getValue(key):
    answer = None
    #Loop through scalesettings looking for key
    if scaleSettings_file_exists():
        try:
            scaleFileContents = open(scaleSettingsFileLocation, "r")
            for line in scaleFileContents:
                keyValuePair = line.split('=')
                if keyValuePair[0] == key:
                    # 0 = wire      1 = wifi
                    answer = str(keyValuePair[1].rstrip())
                    break
            scaleFileContents.close()
        except:
            pass
    return answer

def setValue(key, value):
    success = False
    if scaleSettings_file_exists():
        try:
            #Read the contents and replace inline
            import fileinput
            for line in fileinput.FileInput(scaleSettingsFileLocation, inplace=1):
                if line.startswith(key + '='):
                    line = key + '=' + value
                    success = True
                print((line.rstrip()))
        except:
            success = False
    return success
