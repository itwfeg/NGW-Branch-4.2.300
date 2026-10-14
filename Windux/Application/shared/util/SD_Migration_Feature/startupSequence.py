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
import string
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
# import updateSplashVersion
# import updateSplashDiagnostics
import shutil
import license
import getBoardType
import cpuCount

# import logging

# log_file_path = os.path.join('/', 'usr', 'local', 'hobart', 'startup_sequence.log')
# logging.basicConfig(filename=log_file_path,
#                    level=logging.DEBUG,
#                    format='[%(levelname)s-%(asctime)s]: %(message)s',
#                    datefmt="%Y-%m-%d %H:%M:%S")

pythonPath = os.path.join("/", "usr", "local", "hobart", "python")
backendPath = os.path.join('/', 'usr', 'local', 'hobart', 'backend')
sitePackages = os.path.join("/", "usr", "lib", "python2.7", "site-packages")
firstBootFile = os.path.join("/", "etc", "firstBoot")
upgradeBootFile = os.path.join("/", "etc", "upgradeBoot")
wifiPath = os.path.join("/", "usr", "local", "hobart", "wifi")
recoveryPartition = os.path.join("/", "media", "mmcblk0p2")
boardTesterFile = os.path.join("/", "etc", "boardtester")
firstTime = False
mbId = None

cmd = []
cmd.append('readlink')
cmd.append('-f')
cmd.append('/dev/root')
exitCode, stdout, stderr = shellCommand.execute(cmd)

if 'mmcblk1' in stdout:
    recoveryPartition = os.path.join("/", "media", "mmcblk1p2")


def isMicroBackend():
    if os.path.exists(os.path.join("/", "etc", "hobart", "blackbox")):
        return True
    elif os.path.exists(os.path.join("/", "etc", "hobart", "slicer")):
        return True
    elif os.path.exists(os.path.join("/", "etc", "hobart", "testsrv")):
        return True

    return False


def isTouchscreenInstalled():
    if os.path.exists(os.path.join("/", "etc", "hobart", "blackbox")):
        return False
    elif os.path.exists(os.path.join("/", "etc", "hobart", "testsrv")):
        return False

    return True


def installOptionalFeatures(language):
    FEATURE_UPGRADE_REGEX_COMPILED = re.compile('^.*/%s$' % (installFilePatterns.FEATURE_UPGRADE_FILENAME_REGEX))
    FEATURE_UPGRADE_OLD_REGEX_COMPILED = re.compile(
        '^.*/%s$' % (installFilePatterns.FEATURE_UPGRADE_OLD_FILENAME_REGEX))
    FEATURE_UPGRADE_OSAGNOSTIC_REGEX_COMPILED = re.compile(
        '^.*/%s$' % (installFilePatterns.FEATURE_UPGRADE_OSAGNOSTIC_FILENAME_REGEX))

    # if an upgrade is performed optional features may have to be reinstalled especially if the feature has an
    # install.py script.  upgrade removes files all over filesystem and some optional features like libreoofice
    # install files thru debian package manager and place files all over root filesystem so after upgrade which
    # upgrades entire rootfs they need re-installed
    features = os.listdir(os.path.join("/", "usr", "local", "hobart", "features"))

    for i in xrange(0, len(features)):
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
            if features[i] == 'office_viewer':
                allowFeature = False

            if features[i] == 'pdf_viewer':
                allowFeature = False

            # install feature!
            if allowFeature:
                # display message that we are going to run installer for feature since upgrade occurred
                if language == 'spanish':
                    message = "Instalar %s" % (features[i])
                elif language == 'french':
                    message = "Installation %s" % (features[i])
                else:
                    message = "Installing %s" % (features[i])

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
    if os.path.exists(os.path.join(pythonPath, "recovery.sh")):
        cmd = []
        cmd.append("/bin/mv")
        cmd.append(os.path.join(pythonPath, "recovery.sh"))
        cmd.append(os.path.join(recoveryPartition, "etc", "init.d"))
        exitCode, stdout, stderr = shellCommand.execute(cmd)

    # print 'Compiling python files'
    if os.path.exists(os.path.join(pythonPath, "usbQuery.py")):
        # byte compile all python files
        cmd = []
        cmd.append("/bin/rm")
        cmd.append("-f")
        cmd.append(os.path.join(pythonPath, "*.pyc"))
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        cmd = []
        cmd.append("/usr/bin/python")
        cmd.append("-m")
        cmd.append("compileall")
        cmd.append(pythonPath)
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        # remove licensing scripts source
        filesToRemove = ['usbQuery.py', 'verifyDongleKey.py', 'crypto.py', 'keyInfo.py', 'csid.py', 'volumeInfo.py',
                         'license.py', 'setPassword.py']
        for i in xrange(0, len(filesToRemove)):
            try:
                os.remove(os.path.join(pythonPath, filesToRemove[i]))
            except:
                pass

        cmd = []
        cmd.append('/usr/bin/python')
        cmd.append('-m')
        cmd.append('compileall')
        cmd.append(backendPath)
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        try:
            os.remove(os.path.join(backendPath, 'license.py'))
        except:
            pass


def setUBootVersion():
    cmd = []
    cmd.append("fw_printenv")
    cmd.append("-n")
    cmd.append("version")
    exitCode, version, stderr = shellCommand.execute(cmd)
    file_io.writeToFile(os.path.join("/", "uBootVersion"), [version])


def checkSerialFlashMFG():
    CONST_PART_SIZE = 4096

    # check the manufacturing section of the serial flash for blank section or incorrect scale sytle setting
    flash = serialFlash.mfgSection()
    scaleStyle = flash.getRawScaleStyle()

    # Determine the board type based on the # of CPU's
    numCpu = cpuCount.getCPUCount()
    numCpu = numCpu.replace("\n", "")

    id = "NA"
    if numCpu == "1":
        id = 'Solo'
    elif numCpu == "2":
        id = 'Dual'
    elif numCpu == "4":
        id = 'Quad'

    if flash.isSectionBlank() == True:
        # manufacturing section is blank initalize with default values

        if id == 'Solo':
            flash.setScaleStyle('HTs')
            # set the board assembly for the solo
            flash.setBoardAssembly('00-447777')

        elif id == 'Dual':
            flash.setScaleStyle('HTi')
            # set the board assembly for the dual
            flash.setBoardAssembly('00-447787')

        elif id == 'Quad':
            flash.setScaleStyle('HTx')
            # set the board assembly for the dual
            flash.setBoardAssembly('00-447797')

        # set current date as date of manufacture
        date_cmd = []
        date_cmd.append(os.path.join("/", "bin", "date"))
        date_cmd.append("+%D")
        exitCode, stdout, stderr = shellCommand.execute(date_cmd)
        date = stdout.replace('/', '')
        flash.setDateOfManufacturing(date.replace('\n', ''))

        # get and set the hardware revision that is saved in partition 4 of the serial flash
        partition = []
        if os.path.exists(os.path.join("/", "dev", "mtdblock4")):
            try:

                # open the hardware config partition
                inFile = open(os.path.join("/", "dev", "mtd4"), 'rb')

                # read the entire partition
                partition = inFile.read(CONST_PART_SIZE)

                # hardware revision string should be in the first 4 bytes
                hwRevision = partition[0:3]
                inFile.close()

                flash.setBoardRevision(hwRevision)
            except IOError, exception_details:
                result = 'error: Reading hardware configuration from partition failed: ' + ', Reason:' + \
                         exception_details[1]
                flash.setBoardRevision('0')
        flash.write()
        flash.show()

    if scaleStyle == '0' or scaleStyle == "":
        # manufacturing section is blank initalize with default values
        if id == 'Solo':
            flash.setScaleStyle('HTs')
        elif id == 'Dual':
            flash.setScaleStyle('HTi')
        elif id == 'Quad':
            flash.setScaleStyle('HTx')

        flash.write()
        flash.show()


def createGPIOlinks():
    # generate links
    gpioLinks = []

    # Combo symlinks
    if getBoardType.isComboBoard():
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

        # GPIO Mapping for slicer
        if os.path.exists(os.path.join("/", "etc", "hobart", "slicer")):
            gpioLinks.append("ln -s /sys/class/gpio/gpio154 /usr/local/gpio/red_led")
            gpioLinks.append("ln -s /sys/class/gpio/gpio155 /usr/local/gpio/green_led")
            gpioLinks.append("ln -s /sys/class/gpio/gpio156 /usr/local/gpio/blue_led")
            gpioLinks.append("ln -s /sys/class/gpio/gpio157 /usr/local/gpio/reserved")
            gpioLinks.append("ln -s /sys/class/gpio/gpio158 /usr/local/gpio/slicer_power")

        # NGW reset lines to peripherals
        else:
            gpioLinks.append("ln -s /sys/class/gpio/gpio154 /usr/local/gpio/weigher_reset")
            gpioLinks.append("ln -s /sys/class/gpio/gpio155 /usr/local/gpio/printer3_reset")
            gpioLinks.append("ln -s /sys/class/gpio/gpio156 /usr/local/gpio/wrapper_reset")
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

        # reset lines
        gpioLinks.append("ln -s /sys/class/gpio/gpio104 /usr/local/gpio/weigher_reset")
        gpioLinks.append("ln -s /sys/class/gpio/gpio108 /usr/local/gpio/printer3_reset")
        gpioLinks.append("ln -s /sys/class/gpio/gpio105 /usr/local/gpio/wrapper_reset")
        gpioLinks.append("ln -s /sys/class/gpio/gpio109 /usr/local/gpio/printer1_reset")
        gpioLinks.append("ln -s /sys/class/gpio/gpio111 /usr/local/gpio/printer2_reset")

    for linkCommand in gpioLinks:
        os.system(linkCommand)


def setUbootSplashImage():
    # Determine the board type based on the # of CPU's
    numCpu = cpuCount.getCPUCount()
    numCpu = numCpu.replace("\n", "")

    id = "NA"
    if numCpu == "1":
        id = 'Solo'
    elif numCpu == "2":
        id = 'Dual'
    elif numCpu == "4":
        id = 'Quad'

    if id == 'Solo':
        result = updateSplashImage.updateSplashImage(os.path.join("/", "uboot", "hts8.bmp"))
    elif id == 'Dual':
        result = updateSplashImage.updateSplashImage(os.path.join("/", "uboot", "hti8.bmp"))
    elif id == 'Quad':
        result = updateSplashImage.updateSplashImage(os.path.join("/", "uboot", "htx8.bmp"))

    # result = updateSplashVersion.updateSplashVersion(os.path.join("/", "uboot", "u-version.bmp"))
    # result = updateSplashDiagnostics.updateSplashDiagnostics(os.path.join("/", "uboot", "u-diagnostics.bmp"))


def startCANInterface():
    cmd = []
    cmd.append("ip")
    cmd.append("link")
    cmd.append("set")
    cmd.append("can0")
    cmd.append("up")
    cmd.append("type")
    cmd.append("can")
    cmd.append("bitrate")
    cmd.append("250000")
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    if exitCode != 0:
        print "Can bus interface failed to initialize."
    else:
        print "Can bus ready for use."


def copyFpcFiles():
    cmd = []
    cmd.append("/bin/cp")
    if getBoardType.isComboBoard():
        cmd.append("/lib/firmware/fpc/sourceFPC2.bin")
    else:
        cmd.append("/lib/firmware/apc/sourceFPC.bin")
    cmd.append("/usr/local/hobart/download")
    exitCode, stdout, stderr = shellCommand.execute(cmd)


def handleFirstBoot():
    errorLog = open("/var/log/startup" + "-log.txt", "wb")
    errorLog.write("First Boot: \n")

    mbId = None

    # print 'Initializing ld library configuration'
    # needed for pyudev to install
    cmd = []
    cmd.append("/sbin/ldconfig")
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    errorLog.write("ldconfig: " + str(exitCode) + "\n")
    # print 'Initializing Time to Scale Epoch'

    # create GPIO links
    createGPIOlinks()

    # read hwrev from serial flash and update u-boot environment variables
    cmd = []
    cmd.append("hwRev.py")
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    errorLog.write("hwRev: " + str(exitCode) + "\n")
    # Update Uboot splash image
    setUbootSplashImage()

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

        cmd = []
        cmd.append("/bin/sync")
        exitCode, stdout, stderr = shellCommand.execute(cmd)

    # setup touchscreen
    if isTouchscreenInstalled():
        # load touch screen driver for combo board
        if ((getBoardType.isComboBoard()) and (not os.path.exists(os.path.join("/", "dev", "input", "touchscreen")))):

            cmd = []
            cmd.append("/sbin/modprobe")
            cmd.append("crtouch_mt")
            exitCode, stdout, stderr = shellCommand.execute(cmd)

            # ensure module loaded properly next boot
            if (0 == exitCode):
                os.system("echo crtouch_mt > /etc/modules")

        # calibrate touch screen
        # Save touch calibration (BCM)
        if ((not os.path.exists(os.path.join("/", "etc", "eGTouch0.param"))) and (
        not os.path.exists(os.path.join("/", "etc", "pointercal")))):
            cmd = []
            cmd.append("/usr/bin/python")
            cmd.append("/usr/local/hobart/python/calibrateTouchscreen.py")
            exitCode, stdout, stderr = shellCommand.execute(cmd)

    # disable eGTouch if no device present
    if not os.path.exists(os.path.join("/", "dev", "input", "touchscreenusb")):
        os.system('/bin/unlink /etc/rc0.d/*eGTouch')
        os.system('/bin/unlink /etc/rc1.d/*eGTouch')
        os.system('/bin/unlink /etc/rc2.d/*eGTouch')
        os.system('/bin/unlink /etc/rc3.d/*eGTouch')
        os.system('/bin/unlink /etc/rc4.d/*eGTouch')
        os.system('/bin/unlink /etc/rc5.d/*eGTouch')
        os.system('/bin/unlink /etc/rc6.d/*eGTouch')

    # Show first boot message
    language = languageSettings.currentLanguage()

    if language == 'spanish':
        message = "Iniciando el primer inicio de la Escala\nEscala se reiniciará después de la\ninstalación en tiempo de ejecución\nEspere por favor ..."
    elif language == 'french':
        message = "Lancement de la première initialisation de l'échelle\nÉchelle va redémarrer après l'installation Runtime\nAttendez s'il vous plaît..."
    else:
        message = "Initiating First Boot of Scale\nScale will Reboot after Runtime Setup\nPlease wait..."

    exitCode, mbId = messageBox.display(message)

    # check for existance of a hobart user
    cmd = []
    cmd.append("id")
    cmd.append("hobart")
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    errorLog.write("id hobart: " + str(exitCode) + "\n")
    if exitCode != 0:
        # create the hobart user
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
        # set hobart password to the default one
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

    # Bug Fix: ACR-53
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
        if int(datetime.datetime.now().year) < 2017:
            cmd = []
            cmd.append("/usr/bin/python")
            cmd.append("/usr/local/hobart/python/setTime.py")
            cmd.append("--day=01")
            cmd.append("--month=01")
            cmd.append("--year=2017")
            cmd.append("--hour=01")
            cmd.append("--minute=01")
            cmd.append("--second=01")
            cmd.append("--timeZoneText=America/Detroit")
            cmd.append("--reboot=0")
            exitCode, stdout, stderr = shellCommand.execute(cmd)

        # refresh system time from rtc since we may have changed timezone
        else:
            cmd = []
            cmd.append("/sbin/hwclock")
            cmd.append("--utc")
            cmd.append("--hctosys")
            exitCode, stdout, stderr = shellCommand.execute(cmd)

        # print 'Generating hostname'
        # generate hostname for first boot
        hostname = generateHostname.generateHostname()
        setHostname.setHostname(hostname)

    # else upgrade boot file exists so retain all settings prior to upgrade
    else:

        # remove the upgrade boot file so these actions can be triggered
        # on subsequent upgrades
        os.system("/bin/rm -f %s" % (upgradeBootFile))

        if os.path.exists('/usr/local/hobart/features/monolithic_core/recovery.tgz'):
            if mbId != None:
                messageBox.destroy(mbId)

            if language == 'spanish':
                message = "Preparación para actualizar la partición de recuperación de la escala, por favor espere ..."
            elif language == 'french':
                message = "Préparation pour mettre à jour la partition de restauration à grande échelle, s'il vous plaît patienter ..."
            else:
                message = "Preparing to upgrade scale recovery partition, please wait..."

            exitCode, mbId = messageBox.display(message)

            recoveryRootDirs = os.listdir(recoveryPartition)

            for curdir in recoveryRootDirs:
                exitCode = os.system("/bin/rm -rf %s" % (os.path.join(recoveryPartition, curdir)))

            messageBox.destroy(mbId)

            if language == 'spanish':
                message = "Partición de recuperación escala refinación, esto puede tardar varios minutos ..."
            elif language == 'french':
                message = "Partition de récupération à grande échelle de mise à niveau, cela peut prendre plusieurs minutes ..."
            else:
                message = "Upgrading scale recovery partition, this may take several minutes..."

            exitCode, mbId = messageBox.display(message)

            # extract the recovery tarball to the recovery partition
            cmd = []
            cmd.append("tar")
            cmd.append("xzf")
            cmd.append("/usr/local/hobart/features/monolithic_core/recovery.tgz")
            cmd.append('-C')
            cmd.append(recoveryPartition)
            exitCode, stdout, stderr = shellCommand.execute(cmd)

            errorLog.write('Removing messageBox with id {0}\n'.format(mbId))
            messageBox.destroy(mbId)
            mbId = None

        if os.path.exists('/usr/local/hobart/features/monolithic_core'):
            errorLog.write('Removing monolithic_core installer files\n')
            exitCode = installUtils.removeInstallerFiles("monolithic_core")

        # re-install any optional features found under /usr/local/hobart/features
        installOptionalFeatures(language)

    # print 'Starting crond'
    cmd = []
    cmd.append('/usr/bin/crontab')
    cmd.append("/usr/local/hobart/python/cronjobs")
    exitCode, stdout, stderr = shellCommand.execute(cmd)

    # installPackages()
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
                    cmd = []
                    cmd.append("/usr/bin/python")
                    cmd.append("installUpgrade.py")
                    cmd.append(os.path.join(root, f))
                    exitCode, stdout, stderr = shellCommand.execute(cmd)

        # install Telit software
        if (os.path.exists(os.path.join("/", "etc", "hobart", "telit")) and os.path.exists(
                os.path.join("/", "usr", "local", "hobart", "preinstall", "dw.tar.gz"))):
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

        # remove preinstall directory
        shutil.rmtree('/usr/local/hobart/preinstall')

        # restore current working
        os.chdir(currentDir)

    # create first boot file so this does section does not run again
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
        except Exception, e:
            print e
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

    else:
        cmd = []
        cmd.append("echo")
        cmd.append("1")
        cmd.append(">")
        cmd.append("/usr/local/gpio/hub1_enable/value")
        exitCode, stdout, stderr = shellCommand.execute(cmd)

    # if not first boot file exists which means scale has not gone
    # thru first boot installation/configuration
    if not os.path.exists(firstBootFile):
        handleFirstBoot()

        # reboot the device
        cmd = []
        cmd.append("/sbin/init")
        cmd.append("6")
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        # exit this script since depending upon timing the statements
        # below could run and we do not want that to happen
        sys.exit(0)

    # install if needed
    buildRuntimeTree()

    # startup networking
    os.system("/usr/bin/python %s &" % (os.path.join(pythonPath, "connectNetworkStartupNew.py")))

    # Do not check the manufacturing section of the serial flash.
    # checkSerialFlashMFG()
    # check the status of the serial flash
    hwRevFileSync.sync()

    # replay USB events from powerup for udev. (udev missing some USB device nodes after powerup)
    cmd = []
    cmd.append("/sbin/udevadm")
    cmd.append("trigger")
    cmd.append("--action=add")
    cmd.append("--subsystem-match=usb")
    exitCode, stdout, stderr = shellCommand.execute(cmd)

    # startup CAN interface
    startCANInterface()

    # read the flash key buffer
    xmlLicenseBuffer = license.readFlashKeyBuffer()

    if xmlLicenseBuffer == None:
        xmlLicenseBuffer = license.xmlCreateInitialLicenseBuffer()

    # print 'Execute touchscreen device management'
    os.system("/usr/bin/python %s &" % (os.path.join(pythonPath, "processDeviceEvents.py")))

    # print 'Execute flash drive management'
    os.system("/usr/bin/python %s &" % (os.path.join(pythonPath, "usbDeviceManager.py")))

    if isMicroBackend():

        # print 'Execute Micro SM Management backend'
        os.system("/usr/bin/python %s > /tmp/sm.log 2>&1 &" % (os.path.join(backendPath, "microSmBackend.py")))

        # print 'Start networking configurator'
        os.system("/usr/bin/python %s &" % (os.path.join(backendPath, "autoconfig.py")))

    else:

        # print 'Execute Scale Management backend'
        os.system("/usr/bin/python %s > /tmp/sm.log 2>&1 &" % (os.path.join(backendPath, "smBackend.py")))

        # print 'Execute Update Via SFTP Listener'
        os.system("/usr/bin/python %s &" % (os.path.join(pythonPath, "sftpUpdate.py")))

    # launch VNC server if licensed
    if license.isProductLicensed(xmlLicenseBuffer, 'SCALE_REMOTE_MONITOR'):

        os.system(
            "/usr/bin/x11vnc -shared -forever -display :0 -tightfilexfer -nopw -localhost -rfbport 5900 -o /var/log/x11vnc0.log &")

        if os.path.exists("/usr/bin/framebuffer-vncserver"):
            os.system(
                "/usr/bin/framebuffer-vncserver -f /dev/fb2 -alwaysshared -listen localhost -p 5901 -listenv6 ::1 -rfbportv6 5901 &")

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

    # get the u-boot version
    # os.system('source /usr/local/hobart/python/getUbootVersion.sh')
    setUBootVersion()


def installLicense(licenseFile):
    # read the flash key buffer
    xmlLicenseBuffer = license.readFlashKeyBuffer()

    if xmlLicenseBuffer == None:
        xmlLicenseBuffer = license.xmlCreateInitialLicenseBuffer()
    # get elements in license file
    status, individualName, companyName, companyAddress, productNames, licenseType, licenseActivationDate, licenseAllotment, licenseTimeout, licenseKey = license.getLicenseElements(
        licenseFile)

    addLicense = False
    # see if products in license are already licensed
    for i in xrange(0, len(productNames)):
        if not license.isProductLicensed(xmlLicenseBuffer, productNames[i]):
            # did not find product name so add license
            addLicense = True

    myExitCode = 0
    # at least one product not licensed in flash so need to add license
    if addLicense:
        # update xml license buffer with new key
        xmlLicenseBuffer = license.xmlAddLicenseToBuffer(xmlLicenseBuffer, individualName, companyName, companyAddress,
                                                         productNames, licenseType, licenseActivationDate,
                                                         licenseAllotment, licenseTimeout, licenseKey)
        # write the flash key buffer
        status, message = license.writeFlashKeyBuffer(xmlLicenseBuffer)
        if status == 'success':
            message = "License key installed."
        else:
            message = 'License key could not be installed.'
        myExitCode, mbId = messageBox.display(message, timer=2)
        messageBox.destroy(mbId)
    return myExitCode


if __name__ == "__main__":
    processStartupSequence()
    sys.exit(0)
