#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# WARNING:
# Do NOT put os.system() calls  in this file if you want the command to run sequentially with other commands
# os.system in startupSequence.py when run from rc.local will background shell commands so the next line
# after os.system call will immmediately execute and your nightmare will begin.
# use shellCommand.execute() call to make sure the command you want to execute is blocked on before next
# line executes
import sys
import os
import file_io
import installUtils
import glob
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
import hwRev
import serialFlash
import hwRevFileSync
import installFilePatterns
import re
import subprocess
import updateSplashVersion
import updateSplashDiagnostics
import shutil
import license
import getBoardType
import getModelName
import cpuCount
import scaleSettingsInterface
import web_server
import file_io

print("running on python version: %s" % sys.version)

DEBUG_LOG=True
if DEBUG_LOG:
    import logging

    log_file_path = os.path.join('/', 'usr', 'local', 'hobart', 'startup_sequence.log')
    logging.basicConfig(filename=log_file_path,
                        level=logging.DEBUG,
                        format='[%(levelname)s-%(asctime)s]: %(message)s',
                        datefmt="%Y-%m-%d %H:%M:%S")


# determine recovery partition
RECOVERY_PART = 'mmcblk0p2'
LIVE_PART = 'mmcblk0p1'

with open('/proc/device-tree/compatible', 'r') as board_info:
    board = board_info.read()
    mmc = 'mmcblk0'

    if os.path.exists( os.path.join("/","media","mmcblk0p2","usr","local","hobart") ):
        pass
    elif os.path.exists( os.path.join("/","media","mmcblk1p2","usr","local","hobart") ):
        mmc = 'mmcblk1'
    elif os.path.exists( os.path.join("/","media","mmcblk2p2","usr","local","hobart") ):
        mmc = 'mmcblk2'

    # ARK
    if 'ark' in board:
        RECOVERY_PART = mmc + 'p2'
        LIVE_PART = mmc + 'p3'

    # ARA
    # BCM
    elif 'itw' in board or 'hb6' in board:
        RECOVERY_PART = mmc + 'p2'
        LIVE_PART = mmc + 'p1'

    # unknown
    else:
        print("Unknown Board!")


pythonPath = os.path.join("/","usr","local","hobart","python")
backendPath = os.path.join('/', 'usr', 'local', 'hobart', 'backend')
sitePackages = os.path.join("/","usr","lib","python3.6","site-packages")
firstBootFile = os.path.join("/","etc","firstBoot")
upgradeBootFile = os.path.join("/","etc","upgradeBoot")
wifiPath = os.path.join("/","usr","local","hobart","wifi")
recoveryPartition = os.path.join("/","media",RECOVERY_PART)
boardTesterFile = os.path.join("/","etc","boardtester")
masterBlasterFile = os.path.join("/", "etc", "hobart", "masterblaster")
hflbProgrammerFile = os.path.join("/", "etc", "hobart", "hflbProgrammer")
coronaPsuFile = os.path.join("/","etc","hobart","coronaPsu")
onlineOrderingFile = os.path.join("/", "etc", "hobart", "enable_mysql")
tdccFile = os.path.join("/", "etc", "hobart", "tdcc")
firstTime = False
mbId = None


def change_desktop_wallpaper(filename):

    # if not os.path.exists("/", "usr", "share", "lxde", "wallpapers", "corona_psu_desktop.jpg"):
    #     shutil.copy(os.path.join("/", "usr", "local", "hobart", "images", "corona_psu_desktop.jpg"),
    #                 os.path.join("/", "usr", "share", "lxde", "wallpapers", "corona_psu_desktop.jpg"))

    desktop_config_file = os.path.join("/", "root", ".config", "pcmanfm", "LXDE", "pcmanfm.conf")

    status, lines = file_io.readFromFile(desktop_config_file, removeCR=1)

    if status == 'success':

        file_needs_updated = False
        new_file_lines = []

        for line in lines:

            if 'wallpaper=' in line:

                if 'wallpaper=/usr/share/lxde/wallpapers/%s' % filename not in line:

                    new_file_lines.append('wallpaper=/usr/share/lxde/wallpapers/%s' % filename)
                    file_needs_updated = True

                else:
                    new_file_lines.append(line)

            else:
                new_file_lines.append(line)

        if file_needs_updated:
            file_io.writeToFile(desktop_config_file, new_file_lines)


# Checks for signs of a previously installed HTe Enterprise app hook.
# If the hook is determined to have been installed at some point, but the requisite files are missing, this method
# will scour for the previous installation in /usr/local/hobart/features/, and should the installation script still
# exist, re-install it.

# This is a hack to get around an issue where feature files don't seem to be getting re-installed after an upgrade.
# Since the hook binaries and links are wiped on upgrade, this is especially problematic for app hook feature files.
# When a fix is implemented and verified, feel free to remove this and the accompanying reinstallAppHook().
def appHookNeedsReinstalled():

    needsHook = False
    configAppDataExists = os.path.exists("/usr/local/hobart/apps/hteapphookconfig")  # Preserved on upgrade
    shareDataExists = os.path.exists("/usr/share/hte-apphook")  # Not preserved on upgrade

    if configAppDataExists and not shareDataExists:  # The former implies that the hook was once there...
        needsHook = True

    return needsHook


def reinstallAppHook():

    candidateHookDirs = [file for file in os.listdir("/usr/local/hobart/features/") if file.__contains__("apphook")]
    hookDir = ""

    if len(candidateHookDirs) == 0:  # No hooks we can re-install. Log this, then bail out.
        logging.warning("Hook re-installation method called, but no installable hook exists!")
        return

    elif len(candidateHookDirs) == 1:  # There's only one hook subdirectory, so use that.
        hookDir = candidateHookDirs[0]

    elif len(candidateHookDirs) > 1:  # Multiple hooks, so use the one with the highest version number.
        highestVersionNumber = 0
        for directory in candidateHookDirs:
            if int(directory[:-1]) > highestVersionNumber:
                highestVersionNumber = int(directory[:-1])
                hookDir = directory

    # Assuming no other issues, migrate to the hook directory and get to work!
    logging.warning("Attempting to re-install HTe Enterprise app hook...")

    hookFeatureDir = os.path.join(installUtils.getFeatureRoot(), hookDir)
    hookFeatureInstaller = os.path.join(hookFeatureDir, "install.py")

    os.chdir(hookFeatureDir)
    cmd = ["/usr/bin/python", hookFeatureInstaller]
    shellCommand.execute(cmd)

    sys.exit()  # The command above will cause a reboot, so to prevent boot oddities, we'll stop startupSequence here.
    # Provided it gets this far, anyway.


def isMicroBackend():
    if os.path.exists(os.path.join("/","etc","hobart","blackbox")):
        if os.path.exists(os.path.join("/", "etc", "hobart","enable_mysql")):
            return False
        return True
    if os.path.exists(coronaPsuFile):
        return True
    if os.path.exists(tdccFile):
        return True
    elif os.path.exists(os.path.join("/","etc","hobart","slicer")):
        return True
    elif os.path.exists(os.path.join("/","etc","hobart","testsrv")):
        return True
    return False


def isTouchscreenInstalled():
    if os.path.exists(os.path.join("/","etc","hobart","blackbox")):
        return False
    elif os.path.exists(coronaPsuFile):
        return False
    elif os.path.exists(tdccFile):
        # TODO
        # This may be True if the tdcc application uses a touchscreen leaving it False
        # for now since eGTouchd will use large amount of CPU if no touchscreen present
        return False
    elif os.path.exists(os.path.join("/","etc","hobart","testsrv")) and \
            not os.path.exists(masterBlasterFile) and \
            not os.path.exists(hflbProgrammerFile):
        return False

    return True

def installOptionalFeatures(language):
    FEATURE_UPGRADE_REGEX_COMPILED = re.compile('^.*/%s$' % (installFilePatterns.FEATURE_UPGRADE_FILENAME_REGEX))
    FEATURE_UPGRADE_OLD_REGEX_COMPILED = re.compile('^.*/%s$' % (installFilePatterns.FEATURE_UPGRADE_OLD_FILENAME_REGEX))
    FEATURE_UPGRADE_OSAGNOSTIC_REGEX_COMPILED = re.compile('^.*/%s$' % (installFilePatterns.FEATURE_UPGRADE_OSAGNOSTIC_FILENAME_REGEX))

    # if an upgrade is performed optional features may have to be reinstalled especially if the feature has an
    # install.py script.  upgrade removes files all over filesystem and some optional features like libreoofice
    # install files thru debian package manager and place files all over root filesystem so after upgrade which
    # upgrades entire rootfs they need re-installed
    features = os.listdir(os.path.join("/", "usr", "local", "hobart", "features"))

    for i in range(0, len(features)):
        # extract any tarballs in feature directory, should be only one
        # need to do this since new tarball features installer typically do not do copy anymore
        # since this is implicitly done by the extract of the tarball itself
        tarFiles = glob.glob(os.path.join("/", "usr", "local", "hobart", "features", features[i], "*.tgz"))
        for tarFile in tarFiles:
            if FEATURE_UPGRADE_REGEX_COMPILED.match(tarFile) or FEATURE_UPGRADE_OLD_REGEX_COMPILED.match(tarFile):
                installUtils.extractTarball(tarFile, root='/')
            elif FEATURE_UPGRADE_OSAGNOSTIC_REGEX_COMPILED.match(tarFile):
                installUtils.extractTarball(tarFile, os.path.join(os.sep, "usr", "local"))

        # check to see if feature has an install.py script that needs re-run
        if os.path.exists(os.path.join("/", "usr", "local", "hobart", "features", features[i], "install.py")):
            allowFeature = True

            # WEH-51
            # check for old versions of sm_backend... if installed they will break stuff...
            if features[i] == 'sm_backend':
                installations = os.path.join("/", "usr", "local", "hobart", "features", "FEATURES_INSTALLED")
                allowFeature = False

                # new versions of sm_backend update FEATURES_INSTALLED; so test for it
                if os.path.exists(installations):
                    if 'sm_backend' in open(installations).read():
                        allowFeature = True

            # FT-1771
            # do not install office_viewer or pdf_viewer, they are installed from preinstall
            preinstall_features = []
            preinstall_features.append( 'office_viewer' )
            preinstall_features.append( 'pdf_viewer' )
            preinstall_features.append( 'fbvnc' )
            preinstall_features.append( 'wpasupplicant' )
            preinstall_features.append( 'bluez' )

            if features[i] in preinstall_features:
                allowFeature = False

            # do not install customer application hooks, may not be binary compatible with previous
            # version which would cause crash of chobart_d
            if 'apphook' in features[i]:
                allowFeature = False

            # install feature!
            if allowFeature:
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
                os.chdir(os.path.join("/", "usr", "local", "hobart", "features", features[i]))

                # execute install.py
                cmd = []
                cmd.append("/usr/bin/python")
                cmd.append(os.path.join("/", "usr", "local", "hobart", "features", features[i], "install.py"))
                exitCode, stdout, stderr = shellCommand.executeShellCommandNoPipe(cmd)

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

                # sync the filesystem
                cmd = []
                cmd.append("/bin/sync")
                exitCode, stdout, stderr = shellCommand.execute(cmd)

                # remove installing message
                messageBox.destroy(mbId)

def buildRuntimeTree():

    if os.path.exists(os.path.join(pythonPath,"recovery.sh")):
        cmd = []
        cmd.append("/bin/mv")
        cmd.append(os.path.join(pythonPath,"recovery.sh"))
        cmd.append(os.path.join(recoveryPartition,"etc","init.d"))
        exitCode, stdout, stderr = shellCommand.execute(cmd)

    #print 'Compiling python files'
    if os.path.exists(os.path.join(pythonPath,"usbQuery.py")):
        # byte compile all python files
        cmd = []
        cmd.append("/bin/rm")
        cmd.append("-f")
        cmd.append(os.path.join(pythonPath,"*.pyc"))
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        
        cmd = []
        cmd.append("/bin/rm")
        cmd.append("-rf")
        cmd.append(os.path.join(pythonPath,"__pycache__"))
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        cmd = []
        cmd.append("/usr/bin/python3")
        cmd.append("-m")
        cmd.append("compileall")
        cmd.append(pythonPath)
        exitCode, stdout, stderr = shellCommand.execute(cmd, shell=True)
        print("compileall output: %s\n%s" % (stdout, stderr))
        
        import py_compile

        # remove licensing scripts source
        filesToRemove = ['usbQuery.py', 'verifyDongleKey.py', 'crypto.py', 'keyInfo.py', 'csid.py', 'volumeInfo.py', 'license.py', 'setPassword.py']
        for filename in filesToRemove:
            try:
                file_path = os.path.join(pythonPath, filename)
                compiled_path = os.path.join(pythonPath, filename + "c")
                
                py_compile.compile(file_path, compiled_path)
                
                os.remove(os.path.join(pythonPath, filename))
            except Exception as e:
                print("exception: %s" % str(e))
                pass

        cmd = []
        cmd.append('/usr/bin/python3')
        cmd.append('-m')
        cmd.append('compileall')
        cmd.append(backendPath)
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        try:
            file_path = os.path.join(backendPath, 'license.py')
            compiled_path = os.path.join(backendPath, 'license.pyc')
            
            py_compile.compile(file_path, compiled_path)
            
            os.remove(file_path)
        except Exception as e:
            print("exception: %s" % str(e))
            pass

    # fixup tty for ARK board
    if getBoardType.isArkBoard():
        files = ["/media/mmcblk0p2/etc/inittab",
                 "/media/mmcblk0p3/etc/inittab",
                 "/media/mmcblk1p2/etc/inittab",
                 "/media/mmcblk1p3/etc/inittab",
                 "/media/mmcblk2p2/etc/inittab",
                 "/media/mmcblk2p3/etc/inittab"]

        for f in files:
            if os.path.exists(f):
                cmd = []
                cmd.append('/bin/sed')
                cmd.append('-i')
                cmd.append('s/ttymxc1/ttymxc0/g')
                cmd.append(f)
                exitCode, stdout, stderr = shellCommand.execute(cmd)

    # fixup tty for BCM board
    elif getBoardType.isBcmBoard():
        files = ["/media/mmcblk0p2/etc/inittab",
                 "/media/mmcblk0p1/etc/inittab",
                 "/media/mmcblk1p2/etc/inittab",
                 "/media/mmcblk1p1/etc/inittab",
                 "/media/mmcblk2p2/etc/inittab",
                 "/media/mmcblk2p1/etc/inittab"]

        for f in files:
            if os.path.exists(f):
                cmd = []
                cmd.append('/bin/sed')
                cmd.append('-i')
                cmd.append('s/ttymxc1/ttymxc3/g')
                cmd.append(f)
                exitCode, stdout, stderr = shellCommand.execute(cmd)

def setUBootVersion():

    # Pull version from uboot environment
    if not getBoardType.isArkBoard():
        cmd = []
        cmd.append("fw_printenv")
        cmd.append("-n")
        cmd.append("version")
        exitCode, version, stderr = shellCommand.execute(cmd)

    # ARK boots from SD card, so read in uboot from SD and parse out version
    # When no SD exists ARK will fail over to SPI flash part, so upon error attempt to pull version from flash
    else:
        version = 'Unknown'

        files = ["/dev/mmcblk0",
                 "/dev/mtd0ro",
                 "/dev/mmcblk1"]

        tmpfile = '/tmp/uboot.img'
        found = False

        for f in files:
            if not os.path.exists(f):
                continue

            cmd = []
            cmd.append('/bin/dd')
            cmd.append('if=' + f)
            cmd.append('of=' + tmpfile)
            cmd.append('conv=notrunc')
            cmd.append('bs=1K')
            cmd.append('count=600')
            exitCode, stdout, stderr = shellCommand.execute(cmd)

            if os.path.exists(tmpfile):
                if exitCode == 0:
                    with open(tmpfile, 'rb') as img:
                        imgstring = bytes([c for c in img.read() if 32 <= c <= 127]).decode('utf-8')
                        obj = re.search(r'version=\d+\.\d+\.\d+', imgstring, re.MULTILINE | re.DOTALL)

                        if obj:
                            a = obj.group().split('=')

                            if len(a) == 2:
                                version = a[1]
                                found = True

                cmd = []
                cmd.append('/bin/rm')
                cmd.append(tmpfile)
                exitCode, stdout, stderr = shellCommand.execute(cmd)

            if found:
                break

    file_io.writeToFile(os.path.join("/","uBootVersion"),[version])

def checkSerialFlashMFG():
    CONST_PART_SIZE = 4096

    #check the manufacturing section of the serial flash for blank section or incorrect scale sytle setting
    flash = serialFlash.mfgSection()
    scaleStyle = flash.getRawScaleStyle()

     # Determine the board type based on the # of CPU's
    numCpu = cpuCount.getCPUCount()
    numCpu = numCpu.replace( "\n", "" )

    id = "NA"
    if numCpu == "1":
        id = 'Solo'
    elif numCpu == "2":
        id = 'Dual'
    elif numCpu == "4":
        id = 'Quad'

    if flash.isSectionBlank() == True:
         #manufacturing section is blank initalize with default values

        if id == 'Solo':
            flash.setScaleStyle('HTs')
            #set the board assembly for the solo
            flash.setBoardAssembly('00-447777')

        elif id == 'Dual':
            flash.setScaleStyle('HTi')
            #set the board assembly for the dual
            flash.setBoardAssembly('00-447787')

        elif id == 'Quad':
            flash.setScaleStyle('HTx')
            #set the board assembly for the dual
            flash.setBoardAssembly('00-447797')

        #set current date as date of manufacture
        date_cmd = []
        date_cmd.append(os.path.join("/","bin","date"))
        date_cmd.append("+%D")
        exitCode, stdout, stderr = shellCommand.execute(date_cmd)
        date = stdout.replace('/',  '')
        flash.setDateOfManufacturing( date.replace('\n',  '') )

        #get and set the hardware revision that is saved in partition 4 of the serial flash
        partition = []
        if os.path.exists(os.path.join("/","dev", "mtdblock3" )):
            try:

                # open the hardware config partition
                inFile = open(os.path.join("/","dev", "mtd3"), 'rb')

                # read the entire partition
                partition = inFile.read(CONST_PART_SIZE)
                inFile.close()

                start = partition.find( "<BDRev>" )
                end = partition.find( "</BDRev>" )

                if (0 < start) and (start < end) and (0 < end - start - 7):
                    # hardware revision string should be in the first 4 bytes
                    hwRevision = partition[start+7:end]
                    flash.setBoardRevision(hwRevision)

            except IOError as exception_details:
                result = 'error: Reading hardware configuration from partition failed: ' + ', Reason:' + exception_details[1]
                flash.setBoardRevision('0')
        flash.write()
        flash.show()

    if scaleStyle == '0' or  scaleStyle =="":
        #manufacturing section is blank initalize with default values
        if id == 'Solo':
            flash.setScaleStyle('HTs')
        elif id == 'Dual':
            flash.setScaleStyle('HTi')
        elif id == 'Quad':
            flash.setScaleStyle('HTx')

        flash.write()
        flash.show()

def createGPIOTiltRig():
    gpioLinks = []
        
    if getBoardType.isArkBoard():  
        if os.path.exists(os.path.join("/","etc","hobart","autocalibrationrig")):        
            #export the IO
            #gpioLinks.append("echo 48 > /sys/class/gpio/export")
            gpioLinks.append("echo 0 > /sys/class/gpio/export")
            gpioLinks.append("echo 90 > /sys/class/gpio/export")
            #gpioLinks.append("echo 53 > /sys/class/gpio/export")
            gpioLinks.append("echo 175 > /sys/class/gpio/export")
            gpioLinks.append("echo 91 > /sys/class/gpio/export")
            #gpioLinks.append("echo 55 > /sys/class/gpio/export")
            gpioLinks.append("echo 92 > /sys/class/gpio/export")
            #gpioLinks.append("echo 132 > /sys/class/gpio/export")
            gpioLinks.append("echo 89 > /sys/class/gpio/export")
            #gpioLinks.append("echo 33 > /sys/class/gpio/export")
            gpioLinks.append("echo 37 > /sys/class/gpio/export")
            #gpioLinks.append("echo 51 > /sys/class/gpio/export")
            gpioLinks.append("echo 45 > /sys/class/gpio/export")
            #gpioLinks.append("echo 93 > /sys/class/gpio/export")
            gpioLinks.append("echo 46 > /sys/class/gpio/export")

            # create soft links to IO and initialize
            #gpioLinks.append("ln -s /sys/class/gpio/gpio48 /usr/local/gpio/tRPlate0")
            gpioLinks.append("ln -s /sys/class/gpio/gpio0 /usr/local/gpio/tRPlate0")
            gpioLinks.append("ln -s /sys/class/gpio/gpio90 /usr/local/gpio/tRPlate90")
            #gpioLinks.append("ln -s /sys/class/gpio/gpio53 /usr/local/gpio/tRPlate180")
            gpioLinks.append("ln -s /sys/class/gpio/gpio175 /usr/local/gpio/tRPlate180")
            gpioLinks.append("ln -s /sys/class/gpio/gpio91 /usr/local/gpio/tRPlate270")
            #gpioLinks.append("ln -s /sys/class/gpio/gpio55 /usr/local/gpio/tRPlateLevel")
            gpioLinks.append("ln -s /sys/class/gpio/gpio92 /usr/local/gpio/tRPlateLevel")
            #gpioLinks.append("ln -s /sys/class/gpio/gpio132 /usr/local/gpio/tRPlateTilt")
            gpioLinks.append("ln -s /sys/class/gpio/gpio89 /usr/local/gpio/tRPlateTilt")
            #gpioLinks.append("ln -s /sys/class/gpio/gpio33 /usr/local/gpio/tRPlateEngIn")
            gpioLinks.append("ln -s /sys/class/gpio/gpio37 /usr/local/gpio/tRPlateEngIn")
            #gpioLinks.append("ln -s /sys/class/gpio/gpio51 /usr/local/gpio/tRPlateEngOut")
            gpioLinks.append("ln -s /sys/class/gpio/gpio45 /usr/local/gpio/tRPlateEngOut")
            #gpioLinks.append("ln -s /sys/class/gpio/gpio93 /usr/local/gpio/tRDutPwrEn")
            gpioLinks.append("ln -s /sys/class/gpio/gpio46 /usr/local/gpio/tRDutPwrEn")
            gpioLinks.append("echo out > /usr/local/gpio/tRDutPwrEn/direction") 
            gpioLinks.append("echo 0 > /usr/local/gpio/tRDutPwrEn/direction/value")
    for linkCommand in gpioLinks:
        os.system(linkCommand)


def createGPIOlinks():

    # generate links
    gpioLinks = []

    # ARK symlinks
    if getBoardType.isArkBoard():
        gpioLinks.append("ln -s /sys/class/gpio/gpio38  /usr/local/gpio/gcd_backlight_enable")
        gpioLinks.append("ln -s /sys/class/gpio/gpio39  /usr/local/gpio/operator_backlight_enable")
        gpioLinks.append("ln -s /sys/class/gpio/gpio40  /usr/local/gpio/rgb_backlight_enable")
        gpioLinks.append("ln -s /sys/class/gpio/gpio204 /usr/local/gpio/hub1_enable")
        gpioLinks.append("ln -s /sys/class/gpio/gpio105 /usr/local/gpio/oper_display_detect")
        gpioLinks.append("ln -s /sys/class/gpio/gpio174 /usr/local/gpio/gcd_display_detect")

        gpioLinks.append("ln -s /sys/class/gpio/gpio142 /usr/local/gpio/kpc_reset")

        # GPIO Mapping for slicer
        if os.path.exists(os.path.join("/","etc","hobart","slicer")):
            # TODO

            # Copy over background image
            os.system("cp /usr/local/hobart/preinstall/slicer_wallpaper.jpg /usr/share/lxde/wallpapers/hobart_hti.jpg")
        else:
            # check the model
            model = getModelName.getModelName()
            substr = "FS"

            if substr in model.upper():
                message = "(in GPIO) Yes! FS is in Model %s " % model
            else:
                message = "(in GPIO) NO! Why would FS be in Model %s " % model
            print(message)

            # NGW reset lines to peripherals
            gpioLinks.append("ln -s /sys/class/gpio/gpio57 /usr/local/gpio/wrapper_reset")
            gpioLinks.append("ln -s /sys/class/gpio/gpio56 /usr/local/gpio/printer3_reset")
            gpioLinks.append("ln -s /sys/class/gpio/gpio143 /usr/local/gpio/weigher_reset")
            gpioLinks.append("ln -s /sys/class/gpio/gpio87 /usr/local/gpio/printer1_reset")
            gpioLinks.append("ln -s /sys/class/gpio/gpio116 /usr/local/gpio/printer2_reset")

        setupFSWallpaper()
        
    # ARA symlinks
    elif getBoardType.isComboBoard():
        gpioLinks.append("ln -s /sys/class/gpio/gpio70 /usr/local/gpio/debug_led_red")
        gpioLinks.append("ln -s /sys/class/gpio/gpio76 /usr/local/gpio/debug_led_green")
        gpioLinks.append("ln -s /sys/class/gpio/gpio101 /usr/local/gpio/gcd_backlight_enable")
        gpioLinks.append("ln -s /sys/class/gpio/gpio205 /usr/local/gpio/operator_backlight_enable")
        gpioLinks.append("ln -s /dev/null /usr/local/gpio/wifi_power_enable")
        gpioLinks.append("ln -s /dev/null /usr/local/gpio/error_led")
        gpioLinks.append("ln -s /dev/null /usr/local/gpio/low_batt")
        gpioLinks.append("ln -s /dev/null /usr/local/gpio/power_failure")
        gpioLinks.append("ln -s /sys/class/gpio/gpio145 /usr/local/gpio/hub1_enable")
        gpioLinks.append("ln -s /dev/null /usr/local/gpio/hub2_enable")
        gpioLinks.append("ln -s /dev/null /usr/local/gpio/oper_display_enable")
        gpioLinks.append("ln -s /sys/class/gpio/gpio9 /usr/local/gpio/oper_backlight_select")
        gpioLinks.append("ln -s /dev/null /usr/local/gpio/gcd_display_enable")
        gpioLinks.append("ln -s /sys/class/gpio/gpio7 /usr/local/gpio/gcd_backlight_select")
        gpioLinks.append("ln -s /sys/class/gpio/gpio34 /usr/local/gpio/oper_display_detect")
        gpioLinks.append("ln -s /sys/class/gpio/gpio35 /usr/local/gpio/gcd_display_detect")
        gpioLinks.append("ln -s /sys/class/gpio/gpio49 /usr/local/gpio/sata_led")
        gpioLinks.append("ln -s /dev/null /usr/local/gpio/i2c_clk_oper_display")
        gpioLinks.append("ln -s /dev/null /usr/local/gpio/i2c_data_oper_display")
        gpioLinks.append("ln -s /dev/null /usr/local/gpio/i2c_clk_gcd_display")
        gpioLinks.append("ln -s /dev/null /usr/local/gpio/i2c_data_gcd_display")

        ################################################################################################################
        # GPIO TO J24 (JST BM20b-zpdss-tf connector) MAPPING
        # *This is for the ARA board 00-448X0 06/07/2018
        #
        # |__GPIO__|__J24__|
        # |  154   |   5   |
        # |  155   |   6   |
        # |  156   |   7   |
        # |  157   |   8   |
        # |  158   |   9   |
        #
        ################################################################################################################

        # GPIO Mapping for slicer
        if os.path.exists(os.path.join("/","etc","hobart","slicer")):
            gpioLinks.append("ln -s /sys/class/gpio/gpio154 /usr/local/gpio/red_led")
            gpioLinks.append("ln -s /sys/class/gpio/gpio155 /usr/local/gpio/green_led")
            gpioLinks.append("ln -s /sys/class/gpio/gpio156 /usr/local/gpio/blue_led")
            gpioLinks.append("ln -s /sys/class/gpio/gpio10 /usr/local/gpio/on_pause_control")
            gpioLinks.append("ln -s /sys/class/gpio/gpio11 /usr/local/gpio/on_pause")
            gpioLinks.append("ln -s /sys/class/gpio/gpio15 /usr/local/gpio/knife_run")

            # Copy over background image
            os.system("cp /usr/local/hobart/preinstall/slicer_wallpaper.jpg /usr/share/lxde/wallpapers/hobart_hti.jpg")

            setupFSWallpaper()
        
        #NGW reset lines to peripherals
        else:
            gpioLinks.append("ln -s /sys/class/gpio/gpio154 /usr/local/gpio/wrapper_reset")
            gpioLinks.append("ln -s /sys/class/gpio/gpio155 /usr/local/gpio/printer3_reset")
            gpioLinks.append("ln -s /sys/class/gpio/gpio156 /usr/local/gpio/weigher_reset")
            gpioLinks.append("ln -s /sys/class/gpio/gpio157 /usr/local/gpio/printer1_reset")
            gpioLinks.append("ln -s /sys/class/gpio/gpio158 /usr/local/gpio/printer2_reset")

    # BCM symlinks
    elif getBoardType.isBcmBoard():
        gpioLinks.append("ln -s /sys/class/gpio/gpio38 /usr/local/gpio/debug_led_red")
        gpioLinks.append("ln -s /sys/class/gpio/gpio39 /usr/local/gpio/debug_led_green")
        gpioLinks.append("ln -s /sys/class/gpio/gpio49 /usr/local/gpio/gcd_backlight_enable")
        gpioLinks.append("ln -s /sys/class/gpio/gpio50 /usr/local/gpio/operator_backlight_enable")
        gpioLinks.append("ln -s /sys/class/gpio/gpio205 /usr/local/gpio/wifi_power_enable")
        gpioLinks.append("ln -s /sys/class/gpio/gpio4 /usr/local/gpio/error_led")
        gpioLinks.append("ln -s /sys/class/gpio/gpio33 /usr/local/gpio/low_batt")
        gpioLinks.append("ln -s /sys/class/gpio/gpio32 /usr/local/gpio/power_failure")
        gpioLinks.append("ln -s /sys/class/gpio/gpio67 /usr/local/gpio/hub1_enable")
        gpioLinks.append("ln -s /sys/class/gpio/gpio68 /usr/local/gpio/hub2_enable")
        gpioLinks.append("ln -s /sys/class/gpio/gpio54 /usr/local/gpio/oper_display_enable")
        gpioLinks.append("ln -s /sys/class/gpio/gpio52 /usr/local/gpio/oper_backlight_select")
        gpioLinks.append("ln -s /sys/class/gpio/gpio53 /usr/local/gpio/gcd_display_enable")
        gpioLinks.append("ln -s /sys/class/gpio/gpio51 /usr/local/gpio/gcd_backlight_select")
        gpioLinks.append("ln -s /sys/class/gpio/gpio34 /usr/local/gpio/oper_display_detect")
        gpioLinks.append("ln -s /sys/class/gpio/gpio35 /usr/local/gpio/gcd_display_detect")
        gpioLinks.append("ln -s /sys/class/gpio/gpio5 /usr/local/gpio/sata_led")
        gpioLinks.append("ln -s /sys/class/gpio/gpio176 /usr/local/gpio/i2c_clk_oper_display")
        gpioLinks.append("ln -s /sys/class/gpio/gpio167 /usr/local/gpio/i2c_data_oper_display")
        gpioLinks.append("ln -s /sys/class/gpio/gpio169 /usr/local/gpio/i2c_clk_gcd_display")
        gpioLinks.append("ln -s /sys/class/gpio/gpio170 /usr/local/gpio/i2c_data_gcd_display")

        #reset lines
        gpioLinks.append("ln -s /sys/class/gpio/gpio104 /usr/local/gpio/weigher_reset")
        gpioLinks.append("ln -s /sys/class/gpio/gpio108 /usr/local/gpio/printer3_reset")
        gpioLinks.append("ln -s /sys/class/gpio/gpio105 /usr/local/gpio/wrapper_reset")
        gpioLinks.append("ln -s /sys/class/gpio/gpio109 /usr/local/gpio/printer1_reset")
        gpioLinks.append("ln -s /sys/class/gpio/gpio111 /usr/local/gpio/printer2_reset")

    for linkCommand in gpioLinks:
        os.system(linkCommand)


def setupFSWallpaper():

    if getBoardType.isArkBoard() or getBoardType.isComboBoard():
        # check the model
        model = getModelName.getModelName()
        substr = "FS"
        
        message = "Model: %s" % model
        print(message)

        if substr in model.upper():
            message = "Yes! FS is in Model %s " % model
        else:
            message = "NO! Why would FS be in Model %s " % model
        print(message)

        if substr in model:
            print("Copy freshserve_wallpaper.jpg to hobart_hti.jpg ")
            os.system("cp /usr/share/lxde/wallpapers/freshserve_wallpaper.jpg /usr/share/lxde/wallpapers/hobart_hti.jpg")
        else:
            print("Continue using original hobart_hti ")

def get_root_device(device_and_partition=False):
    device = None

    cmd = []
    cmd.append('readlink')
    cmd.append('-f')
    cmd.append('/dev/root')
    exitCode, stdout, stderr = shellCommand.execute(cmd)

    if device_and_partition:
        if "mmcblk" in os.path.basename(stdout):
            device = os.path.basename(stdout)
    else:
        if 'mmcblk0' in stdout:
            device = '/dev/mmcblk0'
        elif 'mmcblk1' in stdout:
            device = '/dev/mmcblk1'
        elif 'mmcblk2' in stdout:
            device = '/dev/mmcblk2'

    return device


def automount_secondary_sd_card_slot():

    root_device = get_root_device(device_and_partition=True)

    secondary_usd_card_device = 'mmcblk0p1'

    if root_device[6:7] == '0':
        secondary_usd_card_device = 'mmcblk1p1'

    udev_rules_filename = os.path.join("/", "etc", "udev", "rules.d", "70-hobart-dev.rules")
    status, data = file_io.readFromFile(udev_rules_filename)
    if status == 'success':
        new_lines = []
        for i in range(0, len(data)):
            if '#USD_CARD_ACTION' in data[i]:
                data[i] = data[i].replace('#USD_CARD_ACTION', 'ACTION')
            if '${device_and_partition_number}' in data[i]:
                data[i] = data[i].replace('${device_and_partition_number}', secondary_usd_card_device)
            new_lines.append(data[i])

        status, data = file_io.writeToFile(udev_rules_filename, new_lines)

        if status != 'success':
            return 'error'
        else:
            return 'success'
    else:
        return 'error'


def expandFilesystem():

    if not getBoardType.isArkBoard():
        return

    if not os.path.exists(os.path.join("/","root","expand")):

        device = get_root_device()
        device = get_root_device()

        if device is not None:

            # Device     Boot   Start     End Sectors Size Id Type
            # /dev/sde1          2048   67583   65536  32M  c W95 FAT32 (LBA)
            # /dev/sde2         67584 4261887 4194304   2G 83 Linux
            # /dev/sde3       4261888 8456191 4194304   2G 83 Linux
            with open("/tmp/fdiskcmds", "w") as f:
                f.write("d\n")        # delete
                f.write("3\n")        # third partition
                f.write("n\n")        # new
                f.write("p\n")        # primary type
                f.write("3\n")        # third partition
                f.write("33297\n")    # starting offset
                f.write("\n")         # (default) use all remaining space
                f.write("w\n")        # write table

            os.system("fdisk " + device + " < /tmp/fdiskcmds");
            os.system("touch /root/expand")


def resizeFilesystem():
    if not getBoardType.isArkBoard():
        return

    if not os.path.exists(os.path.join("/","root","resize")):
        os.system("resize2fs /dev/root")
        os.system("touch /root/resize")


def setUbootSplashImage():

    # Determine the board type based on the # of CPU's
    numCpu = cpuCount.getCPUCount()
    numCpu = numCpu.replace( "\n", "" )

    # check the model
    model = getModelName.getModelName()
    substr = "FS"

    print("startupSequence setUbootSpashImage() model: " + model)

    id = "NA"
    if numCpu == "1":
        id = 'Solo'
    elif numCpu == "2":
        id = 'Dual'
    elif numCpu == "4":
        id = 'Quad'

    #SOF - 1511 portion slicer

    if os.path.exists(os.path.join("/","etc","hobart","slicer")):
        result = updateSplashImage.updateSplashImage(os.path.join("/", "uboot", "slicer", "hobart.bmp"))
        # Cannot rely on the u-boot version BMP on the SD card to be correct.  U-boot is installed independently.  Do not do this here.
        #result = updateSplashVersion.updateSplashVersion(os.path.join("/", "uboot", "slicer", "u-version.bmp"))
        result = updateSplashDiagnostics.updateSplashErrorBitmaps( os.path.join("/", "uboot", "slicer", "u-diagnostics.bmp"),  os.path.join("/", "uboot", "slicer", "u-error.bmp"))

    elif os.path.exists(coronaPsuFile):
        result = updateSplashImage.updateSplashImage(os.path.join("/", "uboot", "coronaPsu", "coronapsu.bmp"))
        result = updateSplashDiagnostics.updateSplashErrorBitmaps(
            os.path.join("/", "uboot", "coronaPsu", "u-diagnostics.bmp"),
            os.path.join("/", "uboot", "coronaPsu", "u-missing.bmp"))

    elif os.path.exists(tdccFile):
        result = updateSplashImage.updateSplashImage(os.path.join("/", "uboot", "tdcc", "tdcc.bmp"))
        result = updateSplashDiagnostics.updateSplashErrorBitmaps(
            os.path.join("/", "uboot", "tdcc", "u-diagnostics.bmp"),
            os.path.join("/", "uboot", "tdcc", "u-missing.bmp"))

    elif substr in model.upper():
        result = updateSplashImage.updateSplashImage(os.path.join("/", "uboot", "fs", "fs8.bmp"))
        result = updateSplashDiagnostics.updateSplashErrorBitmaps(
            os.path.join("/", "uboot", "fs", "u-diagnostics.bmp"),
            os.path.join("/", "uboot", "fs", "u-missing.bmp"))
    else:
        if getBoardType.isArkBoard():
            # SOF-3202: Need to update flash with the HTm image
            result = updateSplashImage.updateSplashImage(os.path.join("/", "uboot", "htm8.bmp"))
        else:
            if id == 'Solo':
                result = updateSplashImage.updateSplashImage(os.path.join("/", "uboot", "hts8.bmp"))
            elif id == 'Dual':
                result = updateSplashImage.updateSplashImage(os.path.join("/", "uboot", "hti8.bmp"))
            elif id == 'Quad':
                result = updateSplashImage.updateSplashImage(os.path.join("/", "uboot", "htx8.bmp"))


def startCANInterface():
    
    #check the manufacturing section of the serial flash for blank section or incorrect scale sytle setting
    flash = serialFlash.mfgSection()
    scaleStyle = flash.getRawScaleStyle()

    cmd = []
    cmd.append("ip")
    cmd.append("link")
    cmd.append("set")
    cmd.append("can0")
    cmd.append("up")
    cmd.append("type")
    cmd.append("can")
    cmd.append("bitrate")
    
    if scaleStyle is None:
        cmd.append("1000000")
    else:
        if(scaleStyle.startswith("NGW")):
            cmd.append("250000")
        else:
            cmd.append("1000000")

    exitCode, stdout, stderr = shellCommand.execute(cmd)
    if exitCode != 0:
        print("Can bus interface failed to initialize.")
    else:
        print("Can bus ready for use.")

    cmd = []
    cmd.append("ip")
    cmd.append("link")
    cmd.append("set")
    cmd.append("can1")
    cmd.append("up")
    cmd.append("type")
    cmd.append("can")
    cmd.append("bitrate")

    if scaleStyle is None:
        cmd.append("1000000")
    else:
        if(scaleStyle.startswith("NGW")):
            cmd.append("250000")
        else:
            cmd.append("1000000")

    exitCode, stdout, stderr = shellCommand.execute(cmd)
    if exitCode != 0:
        print("Can bus interface 1 failed to initialize.\n")

def copyFpcFiles():

    cmd = []
    cmd.append("/bin/cp")
    if getBoardType.isComboBoard():
        cmd.append("/lib/firmware/fpc/sourceFPC2.bin")
    elif getBoardType.isBcmBoard():
        cmd.append("/lib/firmware/apc/sourceFPC.bin")
    # SOF-2727: Copy to pending directory instead of download directory
    cmd.append("/usr/local/hobart/pending")
    exitCode, stdout, stderr = shellCommand.execute(cmd)

    # SOF-4449 Allow weigher firmware on wrapper to be upgraded on first boot
    cmd = []
    cmd.append("/bin/cp")
    cmd.append("/lib/firmware/weigher/XPEWeigher.bin")
    cmd.append("/usr/local/hobart/pending")
    exitCode, stdout, stderr = shellCommand.execute(cmd)

def handlePreInstallFeatures():

    if os.path.exists('/usr/local/hobart/preinstall'):
        # get current working directory
        currentDir = os.getcwd()

        # change working directory to python
        os.chdir(os.path.join("/", "usr", "local", "hobart", "python"))

        # iterate over files in preinstall
        for root, dirs, files in os.walk('/usr/local/hobart/preinstall'):
            for f in files:
                if f.endswith('.tgz'):

                    # install the feature
                    os.system( "/usr/bin/python installUpgrade.py " + os.path.join(root, f) )

        # install Telit software

        # TODO
        # I think we need to rmtree the top level folder under /opt for the dw.tar.gz tarball so it can
        # be upodated
        if (os.path.exists(os.path.join("/","etc","hobart","telit")) and os.path.exists(os.path.join("/","usr","local","hobart","preinstall","dw.tar.gz"))):
            cmd = []
            cmd.append("tar")
            cmd.append("xvf")
            cmd.append("/usr/local/hobart/preinstall/dw.tar.gz")
            cmd.append("-C")
            cmd.append("/opt")
            exitCode, stdout, stderr = shellCommand.execute(cmd)

            cmd = []
            cmd.append("dwconfig")
            exitCode, stdout, stderr = shellCommand.execute(cmd)

        # install HFLB Programmer software
        hflbProgrammerRoot = os.path.join("/", "usr", "local", "hobart", "hflbProgrammer")

        if hflbProgrammerFile and \
                os.path.exists(
                    os.path.join("/", "usr", "local", "hobart", "preinstall", "hflb_programmer_runtime.tar.gz")):

            # if the HFLB programmer location does not exisit
            if os.path.exists(hflbProgrammerRoot):
                # else remove any previous installation
                shutil.rmtree(hflbProgrammerRoot)

            # remake HFLB programmer directory tree
            os.makedirs(hflbProgrammerRoot, 0o755)

            cmd = []
            cmd.append("tar")
            cmd.append("xvzf")
            cmd.append(
                os.path.join("/", "usr", "local", "hobart", "preinstall", "hflb_programmer_runtime.tar.gz"))
            cmd.append("-C")
            cmd.append(hflbProgrammerRoot)
            exitCode, stdout, stderr = shellCommand.execute(cmd)

            os.system("/bin/chown -R root.root %s" % hflbProgrammerRoot)

        # install corona PillarSync web and serial port application
        coronaPsuRoot = os.path.join("/", "usr", "local", "hobart", "coronaPsu")

        if coronaPsuFile and \
                os.path.exists(
                    os.path.join("/", "usr", "local", "hobart", "preinstall", "pillar_sync_runtime.tar.gz")):

            # if the corona psu location does not exisit
            if os.path.exists(coronaPsuRoot):
                # else remove any previous installation
                shutil.rmtree(coronaPsuRoot)

            # remake corona directory tree
            os.makedirs(coronaPsuRoot, 0o755)

            # install the corona psu files
            cmd = []
            cmd.append("tar")
            cmd.append("xvzf")
            cmd.append(
                os.path.join("/", "usr", "local", "hobart", "preinstall", "pillar_sync_runtime.tar.gz"))
            cmd.append("-C")
            cmd.append(coronaPsuRoot)
            exitCode, stdout, stderr = shellCommand.execute(cmd)

            os.system("/bin/chown -R root.root %s" % coronaPsuRoot)

        # remove preinstall directory
        shutil.rmtree('/usr/local/hobart/preinstall')

        # restore current working
        os.chdir(currentDir)

def handleFirstBoot():

    errorLog = open( "/var/log/startup" + "-log.txt",  "w")
    errorLog.write("First Boot: \n")

    mbId = None

    #print 'Initializing ld library configuration'
    # needed for pyudev to install
    cmd = []
    cmd.append("/sbin/ldconfig")
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    errorLog.write("ldconfig: " + str(exitCode) + "\n")
    #print 'Initializing Time to Scale Epoch'

    # create GPIO links
    createGPIOlinks()

    # setup udev automounter to mount and unmount any uSD card in the
    # secondary slot
    automount_secondary_sd_card_slot()

    # expand filesystem
    expandFilesystem()

    # read hwrev from serial flash and update u-boot environment variables
    cmd = []
    cmd.append("hwRev.py")
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    errorLog.write("hwRev: " + str(exitCode) + "\n")

    # Update Uboot splash image
    setUbootSplashImage()

    if os.path.exists(onlineOrderingFile):
        errorLog.write("online_ordering_desktop desktop")
        change_desktop_wallpaper("online_ordering_desktop.jpg")

    elif os.path.exists(coronaPsuFile):
        errorLog.write("corona_psu_desktop desktop")
        change_desktop_wallpaper("corona_psu_desktop.jpg")

    elif os.path.exists(os.path.join(tdccFile)):
        change_desktop_wallpaper("tdcc_desktop.jpg")

    setupFSWallpaper()

    # setup wired DHCP as default network config
    if isMicroBackend():
        cmd = []
        cmd.append("/bin/cp")
        cmd.append("/etc/network/interfacesWIRE-DHCP")
        cmd.append("/etc/network/interfaces")
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        cmd = []
        cmd.append("/bin/cp")
        cmd.append("/usr/local/hobart/db/settingswizard/scalesettingsBB")
        cmd.append("/usr/local/hobart/db/settingswizard/scalesettings")
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        # added so the slicer display will dim after 10 min
        if os.path.exists(os.path.join("/", "etc", "hobart", "slicer")):
            scaleSettingsInterface.setValue('dimDisplayTimeout','10')
            errorLog.write("Slicer Brighteness Timeout Set: \n")

        cmd = []
        cmd.append("/bin/sync")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
    
    # setup touchscreen
    if isTouchscreenInstalled():

        #SOF-2943 Moved rotation into xorg.conf
        #force rotate portion slicer screen for calibrateTouchscreen into proper offset
        if(os.path.exists(os.path.join("/","etc","hobart","slicer"))):
            os.system('DISPLAY=:0 /usr/bin/xrandr -o right')

        # load touch screen driver for combo board
        if ((getBoardType.isComboBoard()) and (not os.path.exists(os.path.join("/","dev","input","touchscreen")))):

            cmd = []
            cmd.append("/sbin/modprobe")
            cmd.append("crtouch_mt")
            exitCode, stdout, stderr = shellCommand.execute(cmd)

            # ensure module loaded properly next boot
            if (0 == exitCode):
                os.system("echo crtouch_mt > /etc/modules")

        #Slicer only: If there is no touchscreen connected, send an error message and wait for input
        if os.path.exists("/etc/hobart/slicer"):
            MSG_PID = 0
            TOUCHSCREEN_INPUT_DEVICE = "/dev/input/touchscreen"
        
            device_name = "/dev/input/touchscreenusb"
            mouse0_device_name = "/dev/input/mouse0"
            mouse1_device_name = "/dev/input/mouse1"

            if os.path.exists( device_name ):
                TOUCHSCREEN_INPUT_DEVICE = device_name
        
            if (os.path.exists( mouse0_device_name ) and os.path.exists( mouse1_device_name )):
                TOUCHSCREEN_INPUT_DEVICE = mouse1_device_name
    
            if (TOUCHSCREEN_INPUT_DEVICE == "/dev/input/touchscreen"):
                print( "Startup: Touchscreen not connected \n" )
                time.sleep(1)
        
                msg = "Touchscreen not detected. Please contact\nyour local Hobart-authorized service office."
                msg += "\n\nL'ecran tactile non detecte. Contactez\nle bureau de service Hobart autorise local."
                MSG_EXITCODE, MSG_PID = messageBox.display(msg)
                print("\nTouchscreen not connected: " + str(MSG_EXITCODE) + " " + str(MSG_PID))

            while (TOUCHSCREEN_INPUT_DEVICE == "/dev/input/touchscreen"):
                time.sleep(1) #Keep checking every second until the correct touchscreen is found

                if os.path.exists( device_name ):
                    TOUCHSCREEN_INPUT_DEVICE = device_name

                if (os.path.exists( mouse0_device_name ) and os.path.exists( mouse1_device_name )):
            	    TOUCHSCREEN_INPUT_DEVICE = mouse1_device_name

            if(MSG_PID != 0):
                messageBox.destroy(MSG_PID)
                print( "Touchscreen reconnected!" )

        # calibrate touch screen
        # Save touch calibration (BCM)
        if ((not os.path.exists(os.path.join("/","etc","eGTouch0.param"))) and (not os.path.exists(os.path.join("/","etc","pointercal")))):
            cmd = []
            cmd.append("/usr/bin/python")
            cmd.append("/usr/local/hobart/python/calibrateTouchscreen.py")
            exitCode, stdout, stderr = shellCommand.execute(cmd)

    # disable eGTouch if no device present
    if not os.path.exists(os.path.join("/","dev","input","touchscreenusb")):
        if not os.path.exists(os.path.join("/","etc","hobart","slicer")):
            os.system('/bin/unlink /etc/rc0.d/*eGTouch')
            os.system('/bin/unlink /etc/rc1.d/*eGTouch')
            os.system('/bin/unlink /etc/rc2.d/*eGTouch')
            os.system('/bin/unlink /etc/rc3.d/*eGTouch')
            os.system('/bin/unlink /etc/rc4.d/*eGTouch')
            os.system('/bin/unlink /etc/rc5.d/*eGTouch')
            os.system('/bin/unlink /etc/rc6.d/*eGTouch')
    
    print( "touchscreen installed" )
    
    # Show first boot message
    language = languageSettings.currentLanguage()

    if language == 'spanish':
        message = "Iniciando el primer inicio de la Escala\nEscala se reiniciará después de la\ninstalación en tiempo de ejecución\nEspere por favor ..."
    elif language == 'french':
        message = "Lancement de la première initialisation de l'échelle\nÉchelle va redémarrer après l'installation Runtime\nAttendez s'il vous plaît..."
    else:
        message = "Initiating First Boot\nDevice will Reboot after Runtime Setup\nPlease wait..."

    exitCode, mbId = messageBox.display(message)

    #check for existance of a hobart user
    cmd = []
    cmd.append("id")
    cmd.append("hobart")
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    errorLog.write("id hobart: " + str(exitCode) + "\n")
    if exitCode != 0:

        #create the hobart user
        cmd = []
        cmd.append("useradd")
        cmd.append("-c")
        cmd.append("Hobart")
        cmd.append("-d")
        cmd.append("/home/hobart")
        cmd.append("-m")
        cmd.append("-k")
        cmd.append("/etc/skel")
        cmd.append("-s")
        cmd.append("/bin/sh")
        cmd.append("-u")
        cmd.append("0")
        cmd.append("-o")
        cmd.append("-g")
        cmd.append("root")
        cmd.append("hobart")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        errorLog.write("hobart user: " + str(exitCode) + "\n")
        #set hobart password to the default one
        cmd = []
        cmd.append("usermod")
        cmd.append("-p")
        cmd.append("xZvxPWRl22dJs")
        cmd.append("hobart")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        errorLog.write("set hobart password: " + str(exitCode) + "\n")

    # Make sure the /home/hobart directory is present after an upgrade
    cmd = []
    cmd.append("/bin/mkdir")
    cmd.append("-p")
    cmd.append("/home/hobart")
    exitCode, stdout, stderr = shellCommand.execute(cmd)

    #Bug Fix: ACR-53
    copyFpcFiles()

    # if this is not an upgrade
    if not os.path.exists(upgradeBootFile):

        # set default timezone and time
        cmd = []
        cmd.append("ln")
        cmd.append("-sf")
        cmd.append("/usr/share/zoneinfo/America/Detroit")
        cmd.append("/etc/localtime")

        exitCode, stdout, stderr = shellCommand.execute(cmd)
        if exitCode != 0:
            sys.exit(exitCode)

        cmd = []
        cmd.append("echo")
        cmd.append("America/Detroit")
        cmd.append(">")
        cmd.append("/etc/timezone")

        exitCode, stdout, stderr = shellCommand.execute(cmd)
        if exitCode != 0:
            sys.exit(exitCode)

        os.environ['TZ'] = "America/Detroit"

        cmd = []
        cmd.append("export")
        cmd.append("TZ=America/Detroit")

        exitCode, stdout, stderr = shellCommand.execute(cmd, True)
        if exitCode != 0:
            sys.exit(exitCode)

        # force time change to something more current
        if int(datetime.datetime.now().year) < 2022:
            cmd = []
            cmd.append("/usr/bin/python")
            cmd.append("/usr/local/hobart/python/setTime.py")
            cmd.append("--day=01")
            cmd.append("--month=01")
            cmd.append("--year=2022")
            cmd.append("--hour=08")
            cmd.append("--minute=08")
            cmd.append("--second=00")
            cmd.append("--timeZoneText=America/Detroit")
            cmd.append("--reboot=0")
            exitCode, stdout, stderr = shellCommand.execute(cmd)

        # refresh system time from rtc since we may have changed timezone
        else:
            cmd = []
            cmd.append("/sbin/hwclock")
            cmd.append("--utc")
            cmd.append("--systohc")
            exitCode, stdout, stderr = shellCommand.execute(cmd)

        #print 'Generating hostname'
        # generate hostname for first boot
        hostname = generateHostname.generateHostname()
        setHostname.setHostname(hostname)

    # else upgrade boot file exists so retain all settings prior to upgrade
    else:

        # remove the upgrade boot file so these actions can be triggered
        # on subsequent upgrades
        os.system("/bin/rm -f %s" % (upgradeBootFile))

        if os.path.exists('/usr/local/hobart/features/monolithic_core/recovery.tgz'):
            os.system("printf \"Begin Time: \" >> /mono-recovery.log")
            os.system("date >> /mono-recovery.log")
            os.system("printf \"\n\" >> /mono-recovery.log")
            os.system("printf \"Pre-Extract dump\n\" >> /mono-recovery.log")
            os.system("printf \"fw_printenv: \n\" >> /mono-recovery.log")
            os.system("fw_printenv >> /mono-recovery.log")
            os.system("printf \"ls -laR primary:\n\" >> /mono-recovery.log")
            os.system("ls -laR $PRIMARYMOUNT >> /mono-recovery.log")
            os.system("printf \"ls -laR recovery:\n\" >> /mono-recovery.log")
            os.system("ls -laR $RECOVERYMOUNT >> /mono-recovery.log")
            os.system("printf \"dmesg:\n\" >> /mono-recovery.log")
            os.system("dmesg >> /mono-recovery.log")
            os.system("printf \"buddyinfo:\n\" >> /mono-recovery.log")
            os.system("cat /proc/buddyinfo >> /mono-recovery.log")
            os.system("printf \"free - m:\n\" >> /mono-recovery.log")
            os.system("free -m >> /mono-recovery.log")
            os.system("printf \"df:\n\" >> /mono-recovery.log")
            os.system("df -h >> /mono-recovery.log")
            os.system("printf \"ps:\n\" >> /mono-recovery.log")
            os.system("ps >> /mono-recovery.log")

            if mbId != None:
                messageBox.destroy(mbId)

            if language == 'spanish':
                message = "Preparación para actualizar la partición de recuperación de la escala, por favor espere ..."
            elif language == 'french':
                message = "Préparation pour mettre à jour la partition de restauration à grande échelle, s'il vous plaît patienter ..."
            else:
                message = "Preparing to upgrade recovery partition, please wait..."

            exitCode, mbId = messageBox.display(message)

            recoveryRootDirs = os.listdir(recoveryPartition)

            for curdir in recoveryRootDirs:
                exitCode = os.system("/bin/rm -rf %s >> /mono-recovery.log" % (os.path.join(recoveryPartition, curdir)))

            messageBox.destroy(mbId)

            if language == 'spanish':
                message = "Partición de recuperación escala refinación, esto puede tardar varios minutos ..."
            elif language == 'french':
                message = "Partition de récupération à grande échelle de mise à niveau, cela peut prendre plusieurs minutes ..."
            else:
                message = "Upgrading recovery partition, this may take several minutes..."

            exitCode, mbId = messageBox.display(message)
            # test extract the tarball
            cmd = []
            cmd.append("tar")
            cmd.append("ztvf")
            cmd.append("/usr/local/hobart/features/monolithic_core/recovery.tgz")
            # cmd.append(">>")
            # cmd.append("/mono-recovery.log")
            # cmd.append("2>&1")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            os.system("echo \"Tar test Exit code: {0}\" >> /mono-recovery.log".format(exitCode))
            os.system("echo \"Tar test stdout: {0}\" >> /mono-recovery.log".format(stdout))
            os.system("echo \"Tar test stderr: {0}\" >> /mono-recovery.log".format(stderr))
            # extract the recovery tarball to the recovery partition
            os.system("printf \"Start Tar Extract:\" >> /mono-recovery.log")
            os.system("date >> /mono-recovery.log")
            os.system("printf \"\n\" >> /mono-recovery.log")
            cmd = []
            cmd.append("tar")
            cmd.append("xzf")
            cmd.append("/usr/local/hobart/features/monolithic_core/recovery.tgz")
            cmd.append('-C')
            cmd.append(recoveryPartition)
            # cmd.append('>>')
            # cmd.append('/mono-recovery.log')
            # cmd.append('2>&1')
            exitCode, stdout, stderr = shellCommand.execute(cmd)

            os.system("printf \"Tar Extract finished:\" >> /mono-recovery.log")
            os.system("date >> /mono-recovery.log")
            os.system("printf \"\n\" >> /mono-recovery.log")

            os.system("bash /usr/local/hobart/python/dropCaches.sh")

            os.system("echo \"Tar Exit code: {0}\" >> /mono-recovery.log".format(exitCode))
            os.system("echo \"Tar stdout: {0}\" >> /mono-recovery.log".format(stdout))
            os.system("echo \"Tar stderr: {0}\" >> /mono-recovery.log".format(stderr))

            recoveryMount = os.path.join('/', 'dev', os.path.basename(recoveryPartition))
            os.system("printf \"Unmounting Recovery Partition:\n\" >> /mono-recovery.log")
            os.system("umount {0} >> /mono-recovery.log".format(recoveryMount))
            os.system("printf \"Running fsck {0}\n\" >> /mono-recovery.log".format(recoveryMount))
            os.system("fsck -pvf {0} >> /mono-recovery.log 2>&1".format(recoveryMount))
            os.system("printf \"Mounting Primary Partition:\n\" >> /mono-recovery.log")
            os.system("mount {0} {1}".format(recoveryMount, recoveryPartition))
            errorLog.write('Removing messageBox with id {0}\n'.format(mbId))
            messageBox.destroy(mbId)
            mbId = None

        if os.path.exists('/usr/local/hobart/features/monolithic_core'):
            errorLog.write('Removing monolithic_core installer files\n')
            exitCode = installUtils.removeInstallerFiles("monolithic_core")

        # re-install any optional features found under /usr/local/hobart/features
        installOptionalFeatures(language)

    # SOF-4279
    # Had to rewrite how we handle cron jobs during upgrade process. In old process we would backup the crontab
    # from root folder, then backup cronjobs from python folder.  We then restored both files and this script would
    # then overwrite the root crontab with whatever was in the cronjobs python file.
    #
    # Lots of problems with this approach:
    # 1) The root crontab file is always overwritten... so existing cron is always lost
    # 2) The cronjobs file is overwritten during upgrade with the old one... so any new cron you want from install
    #    of new feature is lost as well.
    #
    # Net effect is your stuck with whatever was in that original cronjobs file from old version. This is absolute
    # rubbish and unusable.
    #
    # New approach is we backup the root crontab and restore on upggrade. The python cronjobs file is gone... we
    # cant use this because of old code. So now we will generate a new cronjobs file (see below) in first boot and
    # then merge it with the existing crontab (see below).
    #
    # The new net effect is existing crontab are maintained and new cronjobs are created. The only downside is new
    # cronjobs need to be put here instead of in a file.
    
    # SOF-4279
    # generate file with new cronjobs
    with open( "/tmp/cronjobs", "w" ) as jobs: #SOF-4623: Changed "wb" to "w" in order to treat as string-write rather than bytes-write mode
        jobs.write( "*/20 * * * * /bin/bash /usr/local/hobart/python/dropCaches.sh -f\n" )
        jobs.write( "*/1 * * * * /bin/bash /usr/local/hobart/python/dropCaches.sh\n" )
        jobs.write( "* * * * * /usr/bin/python /usr/local/hobart/python/snmpPersistent.py\n" )
        jobs.write( "0 0 * * * /usr/bin/python /usr/local/hobart/python/createThumbnailImages.py\n" )

    # SOF-4279
    # merge existing (restored) crontab and new cronjobs together
    if os.path.exists( '/var/spool/cron/crontabs/root' ):
        os.system( "sort /var/spool/cron/crontabs/root /tmp/cronjobs | uniq > /tmp/crontab" )
    else:
        os.system( "cp /tmp/cronjobs /tmp/crontab" )

    # install cron jobs
    if os.path.exists( '/tmp/crontab' ):
        cmd = []
        cmd.append( '/usr/bin/crontab' )
        cmd.append( '/tmp/crontab' )
        exitCode, stdout, stderr = shellCommand.execute( cmd )

    #installPackages()
    buildRuntimeTree()

    # import certificates
    cmd = []
    cmd.append("/bin/cp")
    cmd.append("/etc/hobart/HobartRootCA.crt")
    cmd.append("/usr/local/share/ca-certificates")
    exitCode, stdout, stderr = shellCommand.execute(cmd)

    cmd = []
    cmd.append("/usr/sbin/update-ca-certificates")
    exitCode, stdout, stderr = shellCommand.execute(cmd)

    # preinstall features
    handlePreInstallFeatures()

    if os.path.exists("/etc/hobart/slicer"):
        # These drivers hijack the slicer control gpio and are not needed so we just remove them
        # SOF-2862: Directory name different for 4.9 vs. 3.14 kernel - modified to use wildcard instead.
        os.system("rm -f /lib/modules/*/kernel/drivers/media/platform/mxc/capture/tc358743_h2c_bridge.ko")
        os.system("rm -f /lib/modules/*/kernel/drivers/media/platform/mxc/capture/v4l2-int-device.ko")
        os.system("rm -f /lib/modules/*/kernel/drivers/media/platform/mxc/capture/mxc_v4l2_capture.ko")

    # create first boot file so this section does not run again
    cmd = []
    cmd.append("/bin/touch")
    cmd.append(firstBootFile)
    exitCode, stdout, stderr = shellCommand.execute(cmd)

    if os.path.exists(os.path.join('/', 'migration', 'MIGRATED')):
        exit_code, pid = messageBox.display("SD Migration Detected, Preparing files")
        if os.path.exists(os.path.join("/", "migration", "mtd6")):
            shutil.copyfile(os.path.join("/", "migration", "mtd6"), "/dev/mtdblock6")
        os.remove(os.path.join('/', 'migration', "MIGRATED"))
        try:
            with open(os.path.join('/', 'migration', 'hostname'), 'r') as host_file:
                hostname = host_file.readline()
                setHostname.setHostname(hostname)
        except Exception as e:
            print(e)
            pass
        messageBox.destroy(pid)

    # remove the message box display first boot message
    if mbId != None:
        messageBox.destroy(mbId)

    cmd = []
    cmd.append("/bin/sync")
    exitCode, stdout, stderr = shellCommand.execute(cmd)

    errorLog.flush()
    errorLog.close()

def processStartupSequence():

    if appHookNeedsReinstalled():
        reinstallAppHook()

    logging.info("before get board type")

    if getBoardType.isComboBoard():
        if not os.path.exists("/etc/hobart/scale-type.dat"):
            cmd = []
            cmd.append("echo")
            cmd.append("1")
            cmd.append(">")
            cmd.append("/usr/local/gpio/hub1_enable/value")
            exitCode, stdout, stderr = shellCommand.execute(cmd)


        else:
            cmd = []
            cmd.append("echo")
            cmd.append("0")
            cmd.append(">")
            cmd.append("/usr/local/gpio/hub1_enable/value")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
 
            cmd = []
            cmd.append("echo")
            cmd.append("1")
            cmd.append(">")
            cmd.append("/usr/local/gpio/wrapper_reset/value")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
           
    else:
        cmd = []
        cmd.append("echo")
        cmd.append("1")
        cmd.append(">")
        cmd.append("/usr/local/gpio/hub1_enable/value")
        exitCode, stdout, stderr = shellCommand.execute(cmd)

    logging.info("before board tester file check")

    # BOARD TESTER
    if (os.path.exists(boardTesterFile)):
        # SOF-560
        # init mtd sections
        cmd = []
        cmd.append("/usr/bin/python")
        cmd.append(os.path.join(pythonPath,"initmtd.py"))
        cmd.append("--all")
        exitCode, stdout, stderr = shellCommand.execute(cmd)

    logging.info("before first boot file check")

    # if not first boot file exists which means scale has not gone
    # thru first boot installation/configuration
    if not os.path.exists(firstBootFile):
        logging.info("before handleFirstBoot")

        if os.path.exists(os.path.join("/", "etc", "hobart", "enable_mysql")):  # print 'Execute Scale Management backend'
            # logging.info("start smBackend")
            # os.system("/usr/bin/python %s > /tmp/sm.log 2>&1 &" % (os.path.join(backendPath, "smBackend.py")))                           # ORIGINAL 4.2.300 call   
            # os.system("nice -n 1 /usr/bin/python -m cProfile %s > /tmp/sm.log 2>&1 &" % (os.path.join(backendPath,"smBackend.py")))      # OPTIMIZED AND gets profile information
            os.system("nice -n 1 /usr/bin/python %s > /tmp/sm.log 2>&1 &" % (os.path.join(backendPath,"smBackend.py")))                  # OPTIMIZED 4.2.300 call
            
        handleFirstBoot()

        # reboot the device
        cmd=[]
        cmd.append("/sbin/init")
        cmd.append("6")
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        # exit this script since depending upon timing the statements
        # below could run and we do not want that to happen

        sys.exit(0)
    else:
        logging.info("else firstBootFile")
    
    #this needs to run on every boot of the tilt rig fixture.
    createGPIOTiltRig()

    logging.info("before build runtime tree")

    # install if needed
    buildRuntimeTree()

    logging.info("before resize filesystem")
    # resize filesystem
    resizeFilesystem()

    # Reset KPC if ARK board and set governor to ondemand
    if getBoardType.isArkBoard():
        os.system("echo 1 > /usr/local/gpio/kpc_reset/value")
        os.system("echo ondemand > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor")
        
    logging.info("before startup networking")

    # startup networking
    os.system("nice -n 1 /usr/bin/python %s &" % (os.path.join(pythonPath,"connectNetworkStartupNew.py")))

    #Do not check the manufacturing section of the serial flash.
    #checkSerialFlashMFG()
    #check the status of the serial flash
    hwRevFileSync.sync()

    #replay USB events from powerup for udev. (udev missing some USB device nodes after powerup)
    cmd = []
    cmd.append("/sbin/udevadm")
    cmd.append("trigger")
    cmd.append("--action=add")
    cmd.append("--subsystem-match=usb")
    exitCode, stdout, stderr = shellCommand.execute(cmd)

    logging.info("before start can interface")

    #startup CAN interface
    startCANInterface()

    if os.path.exists(os.path.join("/","etc","hobart","slicer")):
        os.system("echo out > /usr/local/gpio/on_pause_control/direction")


    logging.info("read the flashkey buffer")

    # read the flash key buffer
    xmlLicenseBuffer = license.readFlashKeyBuffer()

    if xmlLicenseBuffer == None:
        xmlLicenseBuffer = license.xmlCreateInitialLicenseBuffer()

    # preinstall features
    if os.path.exists("/etc/hobart/blackbox"):
        handlePreInstallFeatures()

    logging.info("startup python background tasks")

    setupFSWallpaper()
    
    #print 'Execute touchscreen device management'
    os.system("nice -n 1 /usr/bin/python %s &" % (os.path.join(pythonPath,"processDeviceEvents.py")))

    #print 'Execute flash drive management'
    os.system("/usr/bin/python %s &" % (os.path.join(pythonPath,"usbDeviceManager.py")))

    # startup snmp
    try:
        os.system("nice -n 1/usr/bin/python %s &" % (os.path.join(pythonPath,"snmpHobart.py")))
    except:
        pass

    logging.info("before check if is micro backend")

    if isMicroBackend():
        # print 'Execute Micro SM Management backend'
        os.system("nice -n 1 /usr/bin/python %s > /tmp/sm.log 2>&1 &" % (os.path.join(backendPath,"microSmBackend.py")))

        # print 'Start networking configurator'
        os.system("/usr/bin/python %s &" % (os.path.join(backendPath,"autoconfig.py")))

    elif os.path.exists(onlineOrderingFile):
       os.system("nice -n 1/usr/bin/python %s > /tmp/sm.log 2>&1 &" % (os.path.join(backendPath,"smBackend.py")))
       os.system("/usr/bin/python %s &" % (os.path.join(backendPath,"autoconfig.py")))

    elif os.path.exists(masterBlasterFile) or os.path.exists(hflbProgrammerFile) or os.path.exists(coronaPsuFile):
        #print 'Start networking configurator'
        os.system("/usr/bin/python %s &" % (os.path.join(backendPath,"autoconfig.py")))
    else:
        # print 'Execute Scale Management backend'
        # os.system("nice -n 1 /usr/bin/python -m cProfile %s > /tmp/sm.log 2>&1 &" % (os.path.join(backendPath,"smBackend.py")))   # OPTIMIZED and PROFILED
        os.system("nice -n 1 /usr/bin/python %s > /tmp/sm.log 2>&1 &" % (os.path.join(backendPath,"smBackend.py")))

        # print 'Execute Update Via SFTP Listener'
        # os.system("/usr/bin/python %s &" % (os.path.join(pythonPath,"sftpUpdate.py")))                             # ORIG 4.2.300 call
        # os.system("nice -n 12 /usr/bin/python -m cProfile %s &" % (os.path.join(pythonPath,"sftpUpdate.py")))        # OPTIMIZED and PROFILED call added in 4.2.300
        os.system("nice -n 1 /usr/bin/python %s &" % (os.path.join(pythonPath,"sftpUpdate.py")))                  # OPTIMIZED call added in 4.2.300
          

    logging.info("before launch vncserver")

    # launch VNC server if licensed
    # SOF-1983 remote monitor is enabled without license as per marketing...
    if True: # license.isProductLicensed(xmlLicenseBuffer, 'SCALE_REMOTE_MONITOR') or os.path.exists(masterBlasterFile):

        os.system("/usr/bin/x11vnc -shared -forever -display :0 -tightfilexfer -nopw -localhost -rfbport 5900 -o /var/log/x11vnc0.log &")

        if os.path.exists("/usr/bin/framebuffer-vncserver"):
            os.system("/usr/bin/framebuffer-vncserver -f /dev/fb2 -alwaysshared -listen localhost -p 5901 -listenv6 ::1 -rfbportv6 5901 &")

        if os.path.exists(os.path.join("/", "etc", "init.d", "xrdp")):
            cmd = []
            cmd.append("/bin/rm")
            cmd.append("/var/run/xrdp.pid")
            cmd.append("/var/run/xrdp-sesman.pid")
            exitCode, stdout, stderr = shellCommand.execute(cmd)

            cmd = []
            cmd.append("/etc/init.d/xrdp")
            cmd.append("start")
            exitCode, stdout, stderr = shellCommand.execute(cmd)

    # scan USB bus
    device_re = re.compile("Bus\s+(?P<bus>\d+)\s+Device\s+(?P<device>\d+).+ID\s(?P<id>\w+:\w+)\s(?P<tag>.+)$", re.I)
    df = subprocess.check_output("lsusb")
    df = df.decode('utf-8')
    devices = []

    for i in df.split('\n'):
        if i:
            info = device_re.match(i)
            if info:
                dinfo = info.groupdict()
                dinfo['device'] = '/dev/bus/usb/%s/%s' % (dinfo.pop('bus'), dinfo.pop('device'))
                devices.append(dinfo)

    for d in devices:

        # MTQ-LVW3 Cellular modem attached
        # connect modem to cell network
        if (('1bc7:0036' == d['id']) and ('Telit' in d['tag'])):
            cmd = []
            cmd.append('/usr/sbin/pppd')
            cmd.append('call')
            cmd.append('lvw2')
            exitCode, stdout, stderr = shellCommand.execute(cmd)

    #get the u-boot version
    #os.system('source /usr/local/hobart/python/getUbootVersion.sh')
    setUBootVersion()
    if (os.path.exists(masterBlasterFile)):
        os.system("python /usr/local/hobart/python/masterblasterApp.py &")

    elif os.path.exists(hflbProgrammerFile):
        os.system("python /usr/local/hobart/hflbProgrammer/hflbProgrammer.py &")

    elif os.path.exists(coronaPsuFile):

        logging.info("launching modbus consumer")
        os.system("/usr/bin/python /usr/local/hobart/coronaPsu/web2py/applications/PillarSync/modules/modbus_consumer.py &")

        logging.info('launching web server')
        corona_web_server = web_server.Web2Py(application='coronaPsu',
                                              admin_password='itw1pillar2',
                                              server_key_prefix_name='pillartech',
                                              server_certificate_life_in_years=10,
                                              server_certificate_state='Wisconsin',
                                              server_certificate_location='475 Industrial Drive, Hartland WI 53029-0110',
                                              server_certificate_business_unit='Pillar Technologies',
                                              server_certificate_company_name="ITW",
                                              server_certificate_country="US",
                                              server_certificate_domain_name='pillartech.com')
        logging.info('generate key if needed')
        corona_web_server.generate_certificate()
        logging.info('start web server')
        corona_web_server.run()
        logging.info("done launching web server")

    # Reset KPC if ARK board
    if getBoardType.isArkBoard():
        os.system("echo 0 > /usr/local/gpio/kpc_reset/value")

def installLicense(licenseFile):
    # read the flash key buffer
    xmlLicenseBuffer = license.readFlashKeyBuffer()

    if xmlLicenseBuffer == None:
         xmlLicenseBuffer = license.xmlCreateInitialLicenseBuffer()
     # get elements in license file
    status, individualName, companyName, companyAddress, productNames, licenseType, licenseActivationDate, licenseAllotment, licenseTimeout, licenseKey = license.getLicenseElements(licenseFile)

    addLicense = False
    # see if products in license are already licensed
    for i in range(0, len(productNames)):
        if not license.isProductLicensed(xmlLicenseBuffer, productNames[i]):
            # did not find product name so add license
            addLicense = True

    myExitCode = 0
    # at least one product not licensed in flash so need to add license
    if addLicense:
        # update xml license buffer with new key
        xmlLicenseBuffer = license.xmlAddLicenseToBuffer(xmlLicenseBuffer, individualName, companyName, companyAddress, productNames, licenseType, licenseActivationDate, licenseAllotment, licenseTimeout, licenseKey)
        # write the flash key buffer
        status,  message = license.writeFlashKeyBuffer(xmlLicenseBuffer)
        if status == 'success':
            message = "License key installed."
        else:
            message = 'License key could not be installed.'
        myExitCode,  mbId = messageBox.display(message, timer=2)
        messageBox.destroy(mbId)
    return myExitCode

if __name__ == "__main__":
    processStartupSequence()
    sys.exit(0)
