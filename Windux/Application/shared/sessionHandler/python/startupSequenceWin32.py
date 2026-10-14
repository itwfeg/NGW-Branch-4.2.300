#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# not tested in python3

# WARNING:
# Do NOT put os.system() calls  in this file if you want the command to run sequentially with other commands
# os.system in startupSequence.py when run from rc.local will background shell commands so the next line
# after os.system call will immmediately execute and your nightmare will begin.
# use shellCommand.execute() call to make sure the command you want to execute is blocked on before next
# line executes
import sys
import os
import file_io
import argparse
import shellCommand
import generateHostname
import getHostname
import setHostname
import time
import datetime
import updateSplashImage
import messageBox
import languageSettings
import subprocess
import configInfo
import getModelName
import shutil
import site
import osUtils
import hwRev
import serialFlash
import winreg
import stat
import ntNetworkUtils
import connectNetworkStartupNew

import bootSettings

import logging

pythonInstallPath = "C:\\Python27"
packagesPath = "C:\\Packages"
pythonPath = os.path.join(configInfo.getRootFolder(),"python")
sitePackages = os.path.join(pythonInstallPath,"lib","site-packages")
featuresPath = os.path.join(configInfo.getRootFolder(),"features")
scaleSettingsFileLocation = os.path.join(configInfo.getRootFolder(),"Db","SettingsWizard","scalesettings")
firstBootFile = "C:\\firstBoot"
upgradeBootFile = "C:\\upgradeBoot"
upgradeCmdFile = "C:\\Updates\\runupdate.cmd"

logging.basicConfig(filename='C:\\Program Files\\Hobart\\startseq.log', filemode='w', level=logging.DEBUG, format='%(asctime)s %(message)s', datefmt='%m/%d/%Y %I:%M:%S %p')
logging.debug('Beginning of startupSequenceWin32 logging')

def convertStr(s):
    return str(int(s) + 104)

def installOptionalFeatures(language):
    # if an upgrade is performed optional features may have to be reinstalled especially if the feature has an
    # install.py script.  upgrade removes files all over filesystem and some optional features like libreoofice
    # install files thru debian package manager and place files all over root filesystem so after upgrade which
    # upgrades entire rootfs they need re-installed
    features = os.listdir(featuresPath)

    for i in range(0, len(features)):

        # check to see if feature has an install.py script that needs re-run
        if os.path.exists(os.path.join(featuresPath, features[i], "install.py")):

            # display message that we are going to run installer for feature since upgrade occurred
            if language == 'spanish':
                message = "Instalar %s" % (features[i])
            elif language == 'french':
                message = "Installation %s" % (features[i])
            else:
                message = "Installing %s" %(features[i])

            exitCode, mbId = messageBox.display(message=message)

            # get current working directory before changing to features directory to run install.py
            currentDir = os.getcwd()

            # change working directory to features
            os.chdir(os.path.join(featuresPath, features[i]))

            # execute install.py
            cmd = []
            cmd.append(os.path.join(pythonInstallPath,"python"))
            cmd.append(os.path.join(featuresPath, features[i], "install.py"))
            exitCode, stdout, stderr = shellCommand.execute(cmd)

            if exitCode != 0:

                # install command failed
                if language == 'spanish':
                    message = "Instalar %s fallado" % (features[i])
                elif language == 'french':
                    message = "Installation %s échoué" % (features[i])
                else:
                    message = "Installing %s failed" % (features[i])

                messageBox.display(message=message, timer=5)

            # restore current working directory before installer run
            os.chdir(currentDir)

            # remove installing message
            messageBox.destroy(mbId)

def installPackages():

    # install chilkat
    if not os.path.exists(os.path.join(sitePackages,"chilkat.py")):
        #print 'Installing chilkat'
        cmd = []
        cmd.append(os.path.join(pythonInstallPath,"python"))
        cmd.append(os.path.join(pythonInstallPath, "lib", "zipfile.py"))
        cmd.append("-e")
        cmd.append(os.path.join(packagesPath, "chilkat-9.5.0-python-2.7-win32.zip"))
        cmd.append(packagesPath)
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        extractedChilkatPath = os.path.join(packagesPath,"chilkat-9.5.0-python-2.7-win32")

        shutil.copy(os.path.join(extractedChilkatPath,"chilkat.py"),os.path.join(sitePackages,"chilkat.py"))
        shutil.copy(os.path.join(extractedChilkatPath,"_chilkat.pyd"),os.path.join(sitePackages,"_chilkat.pyd"))

        shutil.rmtree(extractedChilkatPath, True)

    # install zope
    if not os.path.exists(os.path.join(sitePackages,"zope","interface","__init__.py")):
        #print 'Installing zope'
        cmd = []
        cmd.append(os.path.join(pythonInstallPath,"python"))
        cmd.append(os.path.join(pythonInstallPath, "lib", "zipfile.py"))
        cmd.append("-e")
        cmd.append(os.path.join(packagesPath, "zope.interface-4.1.1.win32-py2.7.exe"))
        cmd.append(os.path.join(packagesPath, "zope"))
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        extractedZopePath = os.path.join(packagesPath,"zope","platlib")

        if not os.path.exists(os.path.join(sitePackages,"zope")):
            shutil.copytree(os.path.join(extractedZopePath,"zope"),os.path.join(sitePackages,"zope"))
        if not os.path.exists(os.path.join(sitePackages,"zope.interface-4.1.1-py2.7.egg-info")):
            shutil.copytree(os.path.join(extractedZopePath,"zope.interface-4.1.1-py2.7.egg-info"),os.path.join(sitePackages,"zope.interface-4.1.1-py2.7.egg-info"))
        shutil.copy(os.path.join(extractedZopePath,"zope.interface-4.1.1-py2.7-nspkg.pth"),os.path.join(sitePackages,"zope.interface-4.1.1-py2.7-nspkg.pth"))

        shutil.rmtree(extractedZopePath, True)

def buildRuntimeTree():

    #print 'Compiling python files'
    if os.path.exists(os.path.join(pythonPath,"usbQuery.py")):
        # byte compile all python files
        cmd = []
        cmd.append("/bin/rm")
        cmd.append("-f")
        cmd.append(os.path.join(pythonPath,"*.pyc"))
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        cmd = []
        cmd.append(os.path.join(pythonInstallPath,"python"))
        cmd.append("-m")
        cmd.append("compileall")
        cmd.append(pythonPath)
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        # remove licensing scripts source
        filesToRemove = ['usbQuery.py', 'verifyDongleKey.py', 'crypto.py', 'keyInfo.py', 'csid.py', 'volumeInfo.py', 'license.py', 'setPassword.py']
        for i in range(0,len(filesToRemove)):
            try:
                os.remove(os.path.join(pythonPath,filesToRemove[i]))
            except:
                pass


def setUBootVersion():
    import wmi
    import re
    strComputer = "."
    c = wmi.WMI()
    for objItem in c.query("select * from Win32_BIOS"):
        try:
            f = open("C:\\uBootVersion",  'w')
            strList = ""
            try :
                for objElem in objItem.BIOSVersion :
                    ver = re.search("Phoenix.*v([0-9]{1,3}\.[0-9]{1,3})",  str(objElem))
                    if ver:
                        strList = strList + ver.group(1) + "\n"
            except:
                strList = strList + 'null'
            # print strList
            f.write(strList)

            f.close()
            pass
        except:
            pass

def stopPeripherals():
    cmd = []
    cmd.append(os.path.join(configInfo.getRootFolder(), "bin", "usbpwrtest.exe"))
    cmd.append("0")
    cmd.append("0")
    try:
        osUtils.winRunElevated(cmd)
    except:
        pass
def startPeripherals():
    cmd = []
    cmd.append(os.path.join(configInfo.getRootFolder(), "bin", "usbpwrtest.exe"))
    cmd.append("0")
    cmd.append("1")
    try:
        osUtils.winRunElevated(cmd)
    except:
        pass
    cmd = []
    cmd.append(os.path.join(configInfo.getRootFolder(), "bin", "usbpwrtest.exe"))
    cmd.append("1")
    cmd.append("1")
    try:
        osUtils.winRunElevated(cmd)
    except:
        pass
def processStartupSequence():

    pythonInstallPath = "C:\\Python27"
    packagesPath = "C:\\Packages"
    pythonPath = os.path.join(configInfo.getRootFolder(),"python")
    sitePackages = os.path.join(pythonInstallPath,"lib","site-packages")
    # process any first boot stuff
    #print 'Startup System'

    logging.debug('Beginning of processStartupSequence()')



    # if not first boot file exists which means scale has not gone
    # thru first boot installation/configuration
    if not os.path.exists(firstBootFile):
        logging.debug('BEGIN FIRSTBOOT')
        #print 'First Boot of Scale'
        if os.name == 'posix':
            mbExeFullPath = os.path.join(os.sep, "usr", "bin", "scaleModelChoice")
        else:
            logging.debug('FIRSTBOOT: Before setting EXTENDED desktop')

            # HTA-154 Set Extended Desktop before starting scaleModelChoice,
            # just like before starting scaleui
            cmd = []
            cmd.append( "C:\\Windows\\System32\\DisplaySwitch.exe" )
            cmd.append( "/extend" )
            logging.debug('EXTEND desktop cmd=%s', cmd )
            subprocess.Popen(cmd)

            logging.debug('FIRSTBOOT:  After setting EXTENDED desktop')
        
            logging.debug('Before scaleModelChoice')
            mbExeFullPath = configInfo.getRootFolder() + os.path.join(os.sep, "scaleModelChoice.exe")
            logging.debug('After scaleModelChoice')

        cmd = []
        cmd.append(mbExeFullPath)
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        language = languageSettings.currentLanguage()
        if language == 'spanish':
            message = "Iniciando el primer inicio de la Escala\nEscala se reiniciará después de la\ninstalación en tiempo de ejecución\nEspere por favor ..."
        elif language == 'french':
            message = "Lancement de la première initialisation de l'échelle\nÉchelle va redémarrer après l'installation Runtime\nAttendez s'il vous plaît..."
        else:
            message = "Initiating First Boot of Scale\nScale will Reboot after Runtime Setup\nPlease wait..."

        exitCode,  mbId = messageBox.display(message)

        # install pywin if it's not there
        if not os.path.exists(os.path.join(sitePackages,"pythoncom.py")):
            #print 'Installing pywin'
            cmd = []
            cmd.append(os.path.join(pythonInstallPath, "python"))
            cmd.append(os.path.join(pythonInstallPath, "lib", "zipfile.py"))
            cmd.append("-e")
            cmd.append(os.path.join(packagesPath, "pywin32-218.win32-py2.7.exe"))
            extractedPywinPath = os.path.join(packagesPath, "Pywin32-218")
            cmd.append(extractedPywinPath)
            osUtils.winRunElevated(cmd)

            extractedPywinPathPlatlib = os.path.join(extractedPywinPath, "PLATLIB")

            try:
                f = open(os.path.join(extractedPywinPath, "install.cmd"),  'w')
                f.write('xcopy /E /Y /Q ' + os.path.join(extractedPywinPathPlatlib,'*.*') + ' ' + sitePackages + '\n')
                f.write('xcopy /E /Y /Q ' + os.path.join(extractedPywinPathPlatlib, 'pywin32_system32','*.*') + ' C:\\Windows\\System32\n')
                f.write('pushd ' + os.path.join(extractedPywinPath, "SCRIPTS") + '\n')
                f.write(os.path.join(pythonInstallPath, "python") + ' pywin32_postinstall.py -install\n')
                f.write('popd\n')

                f.close()
            except:
                pass
            cmd = []
            cmd.append('cmd')
            cmd.append('/c')
            cmd.append(os.path.join(extractedPywinPath, "install.cmd"))
            osUtils.winRunElevated(cmd)

            # refresh module path given new path configuration file
            site.addsitedir(sitePackages)

            shutil.rmtree(extractedPywinPath, True)

        # install python wmi module if it's not there
        if not os.path.exists(os.path.join(sitePackages,"wmi.py")):
            #print 'Python WMI module'
            cmd = []
            cmd.append(os.path.join(pythonInstallPath, "python"))
            cmd.append(os.path.join(pythonInstallPath, "lib", "zipfile.py"))
            cmd.append("-e")
            cmd.append(os.path.join(packagesPath, "WMI-1.4.9.zip"))
            cmd.append(packagesPath)
            osUtils.winRunElevated(cmd)

            extractedWMIPath = os.path.join(packagesPath, "WMI-1.4.9")

            try:
                f = open(os.path.join(extractedWMIPath, "install.cmd"),  'w')
                f.write('pushd ' + extractedWMIPath + '\n')
                f.write(os.path.join(pythonInstallPath, "python") + ' setup.py install\n')
                f.write('popd\n')

                f.close()
            except:
                pass
            cmd = []
            cmd.append('cmd')
            cmd.append('/c')
            cmd.append(os.path.join(extractedWMIPath, "install.cmd"))
            osUtils.winRunElevated(cmd)

            shutil.rmtree(extractedWMIPath, True)

        # if this is not an upgrade
        if not os.path.exists(upgradeBootFile):

            # timezone and system time never set so default to
            # EST and scale epoch time 01:01:01

            # check year, if it before 2013 then set it else do nothing
            # TO do this first set the system time from the hardware clock
            # and then read the year, then it will reset this to January 1, 2013
            # if the hardware clock year is before 2013, most likely 1970
            cmd = []
            cmd.append("tzutil")
            cmd.append("/g")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            timezone = stdout.strip()

            if int(datetime.datetime.now().year) < 2013 or timezone != "Eastern Standard Time":
                cmd = []
                cmd.append("tzutil")
                cmd.append("/s")
                cmd.append("Eastern Standard Time")
                exitCode, stdout, stderr = shellCommand.execute(cmd)
                timezone = stdout.strip()

                cmd = []
                cmd.append("/usr/bin/python")
                cmd.append("/usr/local/hobart/python/setTime.py")
                cmd.append("--day=01")
                cmd.append("--month=01")
                cmd.append("--year=2013")
                cmd.append("--hour=01")
                cmd.append("--minute=01")
                cmd.append("--second=01")
                cmd.append("--timeZoneText=America/Detroit")
                cmd.append("--reboot=0")
                exitCode, stdout, stderr = shellCommand.execute(cmd)

            # print 'Generating hostname'
            # generate hostname for first boot
            hostname = generateHostname.generateHostname()
            setHostname.setHostname(hostname)

            # Ensure scalesettings DB is writeable
            if os.path.exists(scaleSettingsFileLocation):
                os.chmod(scaleSettingsFileLocation, stat.S_IWRITE)

            # Configure allowed applications with AppLocker
            # print 'Configuring AppLocker'
            cmd = []
            cmd.append('powershell')
            cmd.append('-Command')
            cmd.append('&{Import-Module AppLocker ; Set-AppLockerPolicy -XmlPolicy \"' + os.path.join(configInfo.getRootFolder(), 'bin', 'AppLockerConfig.xml') + '\"}')
            osUtils.winRunElevated(cmd)

            # Turn on "appidsvc"
            cmd = []
            cmd.append('sc')
            cmd.append('config')
            cmd.append('appidsvc')
            cmd.append('type=')
            cmd.append('share')
            cmd.append('start=')
            cmd.append('auto')
            osUtils.winRunElevated(cmd)

            try:
                # Turn off AutoPlay for insertion of USB thumb drives
                hkcuKey = winreg.ConnectRegistry(None,  winreg.HKEY_CURRENT_USER)
                hklmKey = winreg.ConnectRegistry(None,  winreg.HKEY_LOCAL_MACHINE)
                autoPlayKey = winreg.OpenKey(hkcuKey,  "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\AutoplayHandlers", 0, winreg.KEY_ALL_ACCESS)
                winreg.SetValueEx(autoPlayKey, "DisableAutoplay", None, winreg.REG_DWORD, 0x1)
                winreg.CloseKey(autoPlayKey);

                # HW-43 Turn off options in Ctrl+Alt+Del
                key = winreg.CreateKey(hklmKey, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System")
                winreg.SetValueEx(key, "HideFastUserSwitching", None, winreg.REG_DWORD, 0x1)
                winreg.CloseKey(key);

                key = winreg.CreateKey(hkcuKey, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System")
                winreg.SetValueEx(key, "DisableLockWorkstation", None, winreg.REG_DWORD, 0x1)
                winreg.SetValueEx(key, "DisableTaskMgr", None, winreg.REG_DWORD, 0x1)
                winreg.SetValueEx(key, "DisableChangePassword", None, winreg.REG_DWORD, 0x1)
                winreg.CloseKey(key);

                key = winreg.CreateKey(hkcuKey, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer")
                winreg.SetValueEx(key, "NoLogoff", None, winreg.REG_DWORD, 0x1)
                winreg.CloseKey(key);

                # HW-43 Turn off Ctrl+Alt+Arrows hotkeys utilized by Intel drivers
                key = winreg.CreateKey(hklmKey, "SOFTWARE\\Intel\\Display\\igfxcui\\HotKeys")
                winreg.SetValueEx(key, "Enable", None, winreg.REG_DWORD, 0x0)
                winreg.CloseKey(key);
            except:
                pass

        # else upgrade boot file exists so retain all settings prior to upgrade
        else:
            # remove the upgrade boot file so these actions can be triggered
            # on subsequent upgrades
            try:
                os.remove(upgradeBootFile)
            except:
                pass

            # re-install any optional features found under /usr/local/hobart/features
            installOptionalFeatures(language)

        # print 'Starting crond'
        cmd = []
        cmd.append('/usr/bin/crontab')
        cmd.append("/usr/local/hobart/python/cronjobs")
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        installPackages()

        buildRuntimeTree()

        # install .NET 4.0 redist if it's not already
        # check if .NET 4.0 redist already there
        dotNetDetected = False
        try:
            hklmKey = winreg.ConnectRegistry(None,  winreg.HKEY_LOCAL_MACHINE)
            dotNet4FullSetupKey = winreg.OpenKey(hklmKey,  "SOFTWARE\\Microsoft\\NET Framework Setup\\NDP\\v4\\Full")
            value,  valueType = winreg.QueryValueEx(dotNet4FullSetupKey, "TargetVersion")
            if value != None:
                dotNetDetected = True
        except:
            pass
        # install .NET 4.0 if it's not detected
        if not dotNetDetected:

            cmd = []
            cmd.append(os.path.join(packagesPath, "dotNetFx40_Full_x86_x64.exe"))
            cmd.append("/q")
            osUtils.winRunElevated(cmd)

        # Install/reintall Hobart Services
        # Stop, delete and unregister "Hobart Scale Service"
        cmd = []
        cmd.append("sc")
        cmd.append("stop")
        cmd.append("Hobart Scale Service")
        osUtils.winRunElevated(cmd)

        cmd = []
        cmd.append("sc")
        cmd.append("delete")
        cmd.append("Hobart Scale Service")
        osUtils.winRunElevated(cmd)

        # Stop, delete and unregister "Hobart SSHD"
        cmd = []
        cmd.append("sc")
        cmd.append("stop")
        cmd.append("Hobart SSHD")
        osUtils.winRunElevated(cmd)

        cmd = []
        cmd.append("sc")
        cmd.append("delete")
        cmd.append("Hobart SSHD")
        osUtils.winRunElevated(cmd)

        installutilPath = "C:\\Windows\\Microsoft.NET\\Framework\\v4.0.30319\\installutil.exe"
        try:
            hklmKey = winreg.ConnectRegistry(None,  winreg.HKEY_LOCAL_MACHINE)
            dotNet4FullSetupKey = winreg.OpenKey(hklmKey,  "SOFTWARE\\Microsoft\\NET Framework Setup\\NDP\\v4\\Full")
            value,  valueType = winreg.QueryValueEx(dotNet4FullSetupKey, "InstallPath")
            if value != None:
                installutilPath = os.path.join(value, "installutil.exe")
        except:
            pass
        cmd = []
        cmd.append(installutilPath)
        cmd.append("/u")
        cmd.append(os.path.join(configInfo.getRootFolder(), "HobartSSHD.exe"))
        osUtils.winRunElevated(cmd)

        cmd = []
        cmd.append("regsvr32")
        cmd.append("/s")
        cmd.append("/u")
        cmd.append(os.path.join(configInfo.getRootFolder(), "wodSSHD.dll"))
        osUtils.winRunElevated(cmd)

        # Create Scale Service
        cmd = []
        cmd.append("sc")
        cmd.append("create")
        cmd.append("Hobart Scale Service")
        cmd.append("type=")
        cmd.append("share")
        cmd.append("start=")
        cmd.append("auto")
        cmd.append("binpath=")
        cmd.append(os.path.join(configInfo.getRootFolder(), "scaleservice.exe"))
        osUtils.winRunElevated(cmd)

        # Register SSHD
        cmd = []
        cmd.append("regsvr32")
        cmd.append("/s")
        cmd.append(os.path.join(configInfo.getRootFolder(), "wodSSHD.dll"))
        osUtils.winRunElevated(cmd)

        # Create SSHD
        cmd = []
        cmd.append("C:\\Windows\\Microsoft.NET\\Framework\\v4.0.30319\\installutil.exe")
        cmd.append(os.path.join(configInfo.getRootFolder(), "HobartSSHD.exe"))
        osUtils.winRunElevated(cmd)

        try:
            # setup desktop wallpaper
            hkcuKey = winreg.ConnectRegistry(None,  winreg.HKEY_CURRENT_USER)
            desktopKey = winreg.OpenKey(hkcuKey,  "Control Panel\\Desktop", 0, winreg.KEY_ALL_ACCESS)
            winreg.SetValueEx(desktopKey, "Wallpaper", None, winreg.REG_SZ, "C:\\windows\\system32\\oobe\\info\\backgrounds\\backgrounddefault.jpg")
            winreg.CloseKey(desktopKey)

            # setup taskbar auto hide
            stuckRects2Key = winreg.CreateKeyEx(hkcuKey,  "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\StuckRects2", 0, winreg.KEY_ALL_ACCESS)
            settingsArray = None
            try:
                (value,  valueType) = winreg.QueryValueEx(stuckRects2Key, "Settings")
                settingsArray = bytearray(value)
                settingsArray[8] = 3
            except:
                settingsArray = bytearray([0x28,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x03,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x3e,0x00,0x00,0x00,0x28,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x04,0x00,0x00,0x80,0x07,0x00,0x00,0x38,0x04,0x00,0x00])
            winreg.SetValueEx(stuckRects2Key, "Settings", None, winreg.REG_BINARY, str(settingsArray))
            winreg.FlushKey(stuckRects2Key)
            winreg.CloseKey(stuckRects2Key)

            # hide desktop icons
            hkcuKey = winreg.ConnectRegistry(None,  winreg.HKEY_CURRENT_USER)
            advancedKey = winreg.OpenKey(hkcuKey,  "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", 0, winreg.KEY_ALL_ACCESS)
            winreg.SetValueEx(advancedKey, "HideIcons", None, winreg.REG_DWORD, 0x1)
            winreg.FlushKey(advancedKey)
            winreg.CloseKey(advancedKey)

            # restart explorer so settings take effect
            cmd = []
            cmd.append('taskkill')
            cmd.append('/f')
            cmd.append('/im')
            cmd.append('explorer.exe')
            subprocess.Popen(cmd)

            cmd = []
            cmd.append('explorer')
            subprocess.Popen(cmd)
        except:
            pass

        # create first boot file so this does section does not run again
        try:
            cmd = []
            cmd.append("cmd")
            cmd.append("/c")
            cmd.append("echo firstBootDone > " + firstBootFile)
            osUtils.winRunElevated(cmd)
        except:
            pass

        # remove the message box display first boot message
        messageBox.destroy(mbId)

        cmd = []
        cmd.append(os.path.join(configInfo.getRootFolder(),"bin","FBWFenable.CMD"))
        osUtils.winRunElevated(cmd)

        cmd = []
        cmd.append("shutdown")
        cmd.append("/r")
        cmd.append("/t")
        cmd.append("1")
        osUtils.winRunElevated(cmd)

        # exit this script since depending upon timing the statements
        # below could run and we do not want that to happen
        sys.exit(0)

    logging.debug('END FIRSTBOOT')
    # On Windows, for an upgrade, we create the file "runupdate.cmd", reboot to turn off FBWF and run runupdate.cmd to continue update
    # This checks for the existence of this script.  If the script exists, run it and exit startupSequence.py
    if os.path.exists(upgradeCmdFile):
        language = languageSettings.currentLanguage()

        # display message that we are going to run installer for feature since upgrade occurred
        if language == 'spanish':
            message = "Instalar..."
        elif language == 'french':
            message = "Installation..."
        else:
            message = "Installing..."

        exitCode, mbId = messageBox.display(message=message)

        # execute upgradeCmdFile
        cmd = []
        cmd.append(upgradeCmdFile)
        osUtils.winRunElevated(cmd)

        # remove installing message
        messageBox.destroy(mbId)

        # exit script
        sys.exit(0)

    # install if needed
    installPackages()

    # install if needed
    logging.debug('Before buildRuntimeTree')
    buildRuntimeTree()
    logging.debug('After buildRuntimeTree')

    logging.debug('Before start PERIPHERALS')
    stopPeripherals()
    time.sleep( 5 )
    startPeripherals()
    logging.debug('After start PERIPHERALS')

    #print 'See if the DHCP Client service needs started'
    #hobartSettingsFile = "C:\Program Files\Hobart\Db\SettingsWizard\scalesettings"
    #startDHCP = False
    #bootSettings = {}
    #if os.path.exists(hobartSettingsFile):

    #    try:
    #        dbFile = open(hobartSettingsFile, "r")

    #        for line in dbFile:
    #            line = string.strip(line)
    #            keyValuePair = line.split('=')
    #            if keyValuePair[0] == "isStatic":
    #                bootSettings['isStatic'] = str(keyValuePair[1])
    #                break

    #        dbFile.close()
    #    except:
    #        pass

    #if str(bootSettings['isStatic']) == "1":
    #    startDHCP = False
    #else:
    #    startDHCP = True
    # Look at this stub later when there is time
    #ntNetworkUtils.enableDHCPClientService(startDHCP)


    # start network connection
    #cmd = []
    #cmd.append(os.path.join(pythonInstallPath, "pythonw"))
    #cmd.append(os.path.join(pythonPath, "connectStartupNetwork.py"))
    #subprocess.Popen(cmd)

    #print 'Execute touchscreen device management'
    pythonInstallPath = "C:\\Python27"
    packagesPath = "C:\\Packages"
    pythonPath = os.path.join(configInfo.getRootFolder(),"python")
    backendPath = os.path.join(configInfo.getRootFolder(), 'Backend')
    sitePackages = os.path.join(pythonInstallPath,"lib","site-packages")
    featuresPath = os.path.join(configInfo.getRootFolder(),"features")

    logging.debug('before processDeviceEvents')
    cmd = []
    cmd.append(os.path.join(pythonInstallPath, "pythonw"))
    cmd.append(os.path.join(pythonPath, "processDeviceEvents.py"))
    subprocess.Popen(cmd)
    logging.debug('after processDeviceEvents')

    cmd = []
    cmd.append(os.path.join(pythonInstallPath, "pythonw"))
    cmd.append(os.path.join(pythonPath, "winEventHandler.py"))
    subprocess.Popen(cmd)

    #print 'Execute flash drive management'
    cmd = []
    cmd.append(os.path.join(pythonInstallPath, "pythonw"))
    cmd.append(os.path.join(pythonPath, "usbDeviceManager.py"))
    subprocess.Popen(cmd)

    logging.debug('after usbDeviceManager')

    #print 'Execute Scale Management backend'

    # TODO - WINDOWS - Add once PY script ported
    cmd = []
    cmd.append(os.path.join(pythonInstallPath, "pythonw"))
    cmd.append(os.path.join(backendPath, "smBackend.py"))
    subprocess.Popen(cmd)
    pass

    #print 'Execute Update Via SFTP Listener'
    cmd = []
    cmd.append(os.path.join(pythonInstallPath, "pythonw"))
    cmd.append(os.path.join(pythonPath, "sftpUpdate.py"))
    subprocess.Popen(cmd)

    logging.debug('after sftpUpdate')


    # read the flash key buffer
    import license

    try:
        xmlLicenseBuffer = license.readFlashKeyBuffer()

        logging.debug('SUCCESS - after read license')
    except:
        logging.debug('FAILURE - on read license, xmlLicenseBuffer set to None')
        xmlLicenseBuffer = None

    if xmlLicenseBuffer == None:
        try:
            xmlLicenseBuffer = license.xmlCreateInitialLicenseBuffer()
            logging.debug('SUCCESS - after xmlCreateLicenseBuffer')
        except:
            logging.debug('FAILURE - on xmlCreateLicenseBuffer')
            pass

    logging.debug('xmlLicenseBuffer=%s', xmlLicenseBuffer )

    ##########################
    ##                      ##
    ##    TightVNC Server   ##
    ##                      ##
    ##########################
    # launch VNC server if licensed
    logging.debug('before checking license for SCALE_REMOTE_MONITOR')

    isLicensed = False

    try:
        isLicensed = license.isProductLicensed( xmlLicenseBuffer, 'SCALE_REMOTE_MONITOR' )
        logging.debug( 'SUCCESS - able to parse xmlLicenseBuffer for SCALE_REMOTE_MONITOR' )

    except:
        logging.debug( 'FAILURE - not able to parse xmlLicenseBuffer for SCALE_REMOTE_MONITOR' )
        pass


    try:
        if isLicensed:
            logging.debug( 'STARTING VNC Service' )
            # 'Start the TightVNC Service'
            cmd = []
            cmd.append('sc')
            cmd.append('start')
            cmd.append('tvnserver')
            osUtils.winRunElevated( cmd )
            logging.debug( 'STARTED VNC Service' )
        else:
            logging.debug( 'STOPPING VNC Service' )
            # Let's be safe and stop the VNC Service in case it started somehow or
            # was configured to start outside of Hobart Application
            cmd = []
            cmd.append('sc')
            cmd.append('stop')
            cmd.append('tvnserver')
            osUtils.winRunElevated( cmd )
            logging.debug( 'STOPPED VNC Service' )
    except:
        logging.debug( 'Failure starting / stopping VNC Service' )
        pass

    ##########################

    # DO NOT EDIT OR REMOVE THE LINE BELOW: IT WILL BE EDITED BY SED TO ENABLE SQUISH
    #_START_SQUISH_subprocess.Popen(os.path.join(configInfo.getRootFolder(), 'startsquishserver.cmd'))

    #get the u-boot version
    #os.system('source /usr/local/hobart/python/getUbootVersion.sh')
    setUBootVersion()

    time.sleep( 3 )

    #print 'Execute Hobart Daemon'
    #cmd = []
    #cmd.append('sc')
    #cmd.append('start')
    #cmd.append('Hobart Scale Service')
    #osUtils.winRunElevated(cmd)

    #cmd = []
    #cmd.append('sc')
    #cmd.append('start')
    #cmd.append('Hobart SSHD')
    #osUtils.winRunElevated(cmd)

    # TODO:
    # No longer need delay
    # give all process startup above 5 seconds to execute and get to running state before
    # launching scaleui
    #time.sleep(10.0)

    # HW-1: Windows key should be disabled on HTx scale
    #
    # Are you an esteemed hobart developer wondering where your taskbar went??
    # Do you want your taskbar back again??
    #
    # SCP into your scale and drop a file named 'engdebug' into the root folder
    # - or -
    # Drop a file named 'engdebug' onto your USB stick and insert while booting
    #
    # Please do not comment this... QA is testing...

    logging.debug('Before setting EXTENDED desktop')

    cmd = []
    cmd.append( "C:\\Windows\\System32\\DisplaySwitch.exe" )
    cmd.append( "/extend" )
    logging.debug('EXTEND desktop cmd=%s', cmd )
    subprocess.Popen(cmd)

    logging.debug('After setting EXTENDED desktop')

    logging.debug('Before start SCALEUI')

    #print 'Execute Scale UI'
    cmd = []
    cmd.append(os.path.join(configInfo.getRootFolder(), 'scaleui'))
    if not os.path.exists("c:\\Program Files\\Hobart\\engdebug"):
        if not os.path.exists("d:\\engdebug"):
            if not os.path.exists("e:\\engdebug"):
                cmd.append('-killexplorer')
    subprocess.Popen(cmd)

    logging.debug('After start SCALEUI')

    time.sleep( 3)

    # Are we Prepack or ServiceCounter?
    ourScaleModel = configInfo.getValue('scaleModel')

    if ourScaleModel == "3":
        logging.debug('Before start GCD')

        #print 'Execute GCD'
        # Run gcd if GCD connected
        cmd = []
        cmd.append(os.path.join(configInfo.getRootFolder(), "bin", "GCDDetect.cmd"))
        osUtils.winRunElevated(cmd)
        if os.path.exists("C:\\gcdDetected"):
            subprocess.Popen(os.path.join(configInfo.getRootFolder(), 'gcd'))

        logging.debug('After start GCD')


    #print 'Execute Process Monitor'
    cmd = []
    cmd.append(os.path.join(pythonInstallPath, "pythonw"))
    cmd.append(os.path.join(pythonPath, "processMonitor.py"))
    subprocess.Popen(cmd)

    #print 'Starting crond'
    cmd = []
    cmd.append('sc')
    cmd.append('start')
    cmd.append('Schedule')
    osUtils.winRunElevated(cmd)

    #logging.debug('Before start PERIPHERALS')
    #stopPeripherals()
    #time.sleep( 5 )
    #startPeripherals()
    #logging.debug('After start PERIPHERALS')

    #start the polling for the shutdown button
    cmd = []
    #Next two lines terminate the .exe instead of letting it run. Still working on this.
    #cmd.append("start")
    #cmd.append("/MIN")
    #cmd.append("C:\\Program Files\\Hobart\\bin\\hobartShutdown.exe")
    cmd.append(os.path.join(pythonInstallPath, "pythonw"))
    cmd.append(os.path.join(pythonPath, "launchHobartShutdown.py"))
    subprocess.Popen(cmd)

    # HTA-92 Call connectNetworkStartupNew.py from here instead of from
    # a batch file to re-connect to the network
    connectNetworkStartupNew.connectToNetwork()

if __name__ == "__main__":
    processStartupSequence()
    sys.exit(0)
