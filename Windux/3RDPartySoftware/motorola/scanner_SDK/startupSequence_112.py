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
import cpuInfo
import wifiDetection
import hwRev
import serialFlash
import hwRevFileSync
import glob


pythonPath = os.path.join("/","usr","local","hobart","python")
sitePackages = os.path.join("/","usr","lib","python2.7","site-packages")
firstBootFile = os.path.join("/","etc","firstBoot")
upgradeBootFile = os.path.join("/","etc","upgradeBoot")
wifiPath = os.path.join("/","usr","local","hobart","wifi")
recoveryPath = os.path.join("/","media","mmcblk0p2","etc","init.d")
firstTime = False
mbId = None

def installOptionalFeatures(language):
    # if an upgrade is performed optional features may have to be reinstalled especially if the feature has an
    # install.py script.  upgrade removes files all over filesystem and some optional features like libreoofice
    # install files thru debian package manager and place files all over root filesystem so after upgrade which
    # upgrades entire rootfs they need re-installed
    features = os.listdir(os.path.join("/", "usr", "local", "hobart", "features"))

    for i in xrange(0, len(features)):

        # check to see if feature has an install.py script that needs re-run
        if os.path.exists(os.path.join("/", "usr", "local", "hobart", "features", features[i], "install.py")):

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
        cmd.append(recoveryPath)
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
        cmd.append("/usr/bin/python")
        cmd.append("-m")
        cmd.append("compileall")
        cmd.append(pythonPath)
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        # remove licensing scripts source
        filesToRemove = ['usbQuery.py', 'verifyDongleKey.py', 'crypto.py', 'keyInfo.py', 'csid.py', 'volumeInfo.py', 'license.py', 'setPassword.py']
        for i in xrange(0,len(filesToRemove)):
            try:
                os.remove(os.path.join(pythonPath,filesToRemove[i]))
            except:
                pass

def setUBootVersion():
    cmd = []
    cmd.append("fw_printenv")
    cmd.append("-n")
    cmd.append("version")
    exitCode, version, stderr = shellCommand.execute(cmd)
    file_io.writeToFile(os.path.join("/","uBootVersion"),[version])

def checkSerialFlashMFG():
    CONST_PART_SIZE = 4096

    #check the manufacturing section of the serial flash for blank section or incorrect scale sytle setting
    flash = serialFlash.mfgSection()
    cpu_info = cpuInfo.cpuinfo()
    scaleStyle = flash.getRawScaleStyle()

    if flash.isSectionBlank() == True:
         #manufacturing section is blank initalize with default values
        id = cpu_info.getChipId()
        if id == 'Solo':
            flash.setScaleStyle('HTs')
            #set the board assembly for the solo
            flash.setBoardAssembly('00-447777')

        if id == 'Dual-Quad':
            flash.setScaleStyle('HTi')
            #set the board assembly for the dual
            flash.setBoardAssembly('00-447787')

        #set current date as date of manufacture
        date_cmd = []
        date_cmd.append(os.path.join("/","bin","date"))
        date_cmd.append("+%D")
        exitCode, stdout, stderr = shellCommand.execute(date_cmd)
        date = stdout.replace('/',  '')
        flash.setDateOfManufacturing( date.replace('\n',  '') )

        #get and set the hardware revision that is saved in partition 4 of the serial flash
        partition = []
        if os.path.exists(os.path.join("/","dev", "mtdblock4" )):
            try:

                # open the hardware config partition
                inFile = open(os.path.join("/","dev", "mtd4"), 'rb')

                # read the entire partition
                partition = inFile.read(CONST_PART_SIZE)

                # hardware revision string should be in the first 4 bytes
                hwRevision = partition[0:3]
                inFile.close()

                flash.setBoardRevision(hwRevision)
            except IOError, exception_details:
                result = 'error: Reading hardware configuration from partition failed: ' + ', Reason:' + exception_details[1]
                flash.setBoardRevision('0')
        flash.write()
        flash.show()

    if scaleStyle == '0' or  scaleStyle =="":
        #manufacturing section is blank initalize with default values
        id = cpu_info.getChipId()
        if id == 'Solo':
            flash.setScaleStyle('HTs')
        if id == 'Dual-Quad':
            flash.setScaleStyle('HTi')

        flash.write()
        flash.show()


def createGPIOlinks():
    gpioLinks = []
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

    for linkCommand in gpioLinks:
        os.system(linkCommand)

def setUbootSplashImage():
    cpu_info = cpuInfo.cpuinfo()
    id = cpu_info.getChipId()
    if id == 'Solo':
        result = updateSplashImage.updateSplashImage(os.path.join("/", "uboot", "hts8.bmp"))
    else:
        result = updateSplashImage.updateSplashImage(os.path.join("/", "uboot", "hti8.bmp"))

def processStartupSequence():

    errorLog = open( "/var/log/startup" + "-log.txt",  "wb")
    # process any first boot stuff

    #get our processor info Dual-Quad or Solo
    cpu_info = cpuInfo.cpuinfo()

    #print 'Setup clock'
    os.system("/bin/echo ds1307 0x68 > /sys/class/i2c-adapter/i2c-0/new_device")
    cmd = []
    cmd.append("/bin/ln")
    cmd.append("-s")
    cmd.append("/dev/rtc1")
    cmd.append("/dev/rtc")
    exitCode, stdout, stderr = shellCommand.execute(cmd)

    # default logic that RTC on board has been preset to a date later than the EPOCH which is 1/1/2013
    rtcPreset = True

    # if not first boot file exists which means scale has not gone
    # thru first boot installation/configuration
    if not os.path.exists(firstBootFile):
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

        # read hwrev from serial flash and update u-boot environment variables
        cmd = []
        cmd.append("hwRev.py")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        errorLog.write("hwRev: " + str(exitCode) + "\n")
        # Update Uboot splash image
        setUbootSplashImage()

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
            cmd.append('"Hobart"')
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

        # Multiple boots required of scale since RTC not preset to right time
        language = languageSettings.currentLanguage()

        # if this is not an upgrade
        if not os.path.exists(upgradeBootFile):

            # timezone and system time never set so default to
            # EST and scale epoch time 01:01:01

            # check year, if it before 2013 then set it else do nothing
            # TO do this first set the system time from the hardware clock
            # and then read the year, then it will reset this to January 1, 2013
            # if the hardware clock year is before 2013, most likely 1970
            cmd = []
            cmd.append("/sbin/hwclock")
            cmd.append("-s")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            errorLog.write("set system clock: " + str(exitCode) + "\n")

            cmd = []
            cmd.append("/bin/date")
            cmd.append("+%Z")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            timezone = string.upper(string.strip(stdout))
            rtcPreset = False


            # need to check timezone of RTC from BCM
            if int(datetime.datetime.now().year) < 2013:


                if language == 'spanish':
                    message = "Iniciando el primer inicio de la Escala\nEscala se reiniciará después de la\ninstalación en tiempo de ejecución\nEspere por favor ..."
                elif language == 'french':
                    message = "Lancement de la première initialisation de l'échelle\nÉchelle va redémarrer après l'installation Runtime\nAttendez s'il vous plaît..."
                else:
                    message = "Initiating First Boot of Scale\nScale will Reboot after Runtime Setup\nPlease wait..."

                exitCode,  mbId = messageBox.display(message)

                rtcPreset = False

                cmd = []
                cmd.append("ln")
                cmd.append("-sf")
                cmd.append("/usr/share/zoneinfo/America/Detroit")
                cmd.append("/etc/localtime")

                exitCode, stdout, stderr = shellCommand.execute(cmd)

                if exitCode != 0:
                    sys.exit(exitCode)

                elif exitCode == 0:
                    cmd = []
                    cmd.append("echo")
                    cmd.append("America/Detroit")
                    cmd.append(">")
                    cmd.append("/etc/timezone")

                    exitCode, stdout, stderr = shellCommand.execute(cmd)

                    if exitCode != 0:
                        sys.exit(exitCode)

                    elif exitCode == 0:
                        os.environ['TZ'] = "America/Detroit"

                        cmd = []
                        cmd.append("export")
                        cmd.append("TZ=America/Detroit")

                        exitCode, stdout, stderr = shellCommand.execute(cmd, True)
                        if exitCode != 0:
                            sys.exit(exitCode)

                        elif exitCode == 0:
                            if int(datetime.datetime.now().year) < 2013:
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

                                # TODO: Verify do not need lines below
                                #os.system('/bin/date --set="01 January 2013 01:01:01"')
                                #time.sleep(3)
                                #os.system("/sbin/hwclock --utc --systohc")
            else:
                # BCM has preset the time zone to UTC so needs fixed
                # Fix timezone since RTC has been preset by BCM
                cmd = []
                cmd.append("ln")
                cmd.append("-sf")
                cmd.append("/usr/share/zoneinfo/America/Detroit")
                cmd.append("/etc/localtime")

                exitCode, stdout, stderr = shellCommand.execute(cmd)

                if exitCode != 0:
                    sys.exit(exitCode)

                elif exitCode == 0:
                    cmd = []
                    cmd.append("echo")
                    cmd.append("America/Detroit")
                    cmd.append(">")
                    cmd.append("/etc/timezone")

                    exitCode, stdout, stderr = shellCommand.execute(cmd)

                    if exitCode != 0:
                        sys.exit(exitCode)

                    elif exitCode == 0:
                        os.environ['TZ'] = "America/Detroit"

                        cmd = []
                        cmd.append("export")
                        cmd.append("TZ=America/Detroit")

                        exitCode, stdout, stderr = shellCommand.execute(cmd, True)
                        if exitCode != 0:
                            sys.exit(exitCode)
                        else:
                            cmd = []
                            cmd.append("/sbin/hwclock")
                            cmd.append("-s")
                            cmd.append("--utc")
                            exitCode, stdout, stderr = shellCommand.execute(cmd)

            #print 'Generating hostname'
            # generate hostname for first boot
            hostname = generateHostname.generateHostname()
            setHostname.setHostname(hostname)

            #print 'Starting crond'
            cmd = []
            cmd.append('/usr/bin/crontab')
            cmd.append("/usr/local/hobart/python/cronjobs")
            exitCode, stdout, stderr = shellCommand.execute(cmd)

        # else upgrade boot file exists so retain all settings prior to upgrade
        else:
            # Reboot required on an upgrade until startupSequence.py
            # successfully eliminates second boot
            rtcPreset = False
            # remove the upgrade boot file so these actions can be triggered
            # on subsequent upgrades
            os.system("/bin/rm -f %s" % (upgradeBootFile))

            if os.path.exists('/usr/local/hobart/feature/monolithic_core/recovery.tgz'):
                if mbId != None:
                    messageBox.destroy(mbId)

                if language == 'spanish':
                    message = "Preparación para actualizar la partición de recuperación de la escala, por favor espere ..."
                elif language == 'french':
                    message = "Préparation pour mettre à jour la partition de restauration à grande échelle, s'il vous plaît patienter ..."
                else:
                    message = "Preparing to upgrade scale recovery partition, please wait..."

                exitCode, mbId = messageBox.display(message)

                recoveryPartition = "/media/mmcblk0p2/"
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
                cmd.append("/media/mmcblk0p2")
                exitCode, stdout, stderr = shellCommand.execute(cmd)

                messageBox.destroy(mbId)
                # Set mbId to None so we don't try to kill it twice below.
                mbId = None

                featureName = 'monolithic_core'
                if os.name == 'posix':
                    featurePath = os.path.join('/', 'usr', 'local', 'hobart', 'features', featureName)
                else:
                    featurePath = os.path.join('C:\\', 'Program Files', 'hobart', 'features', featureName)

                filesToRemove = glob.glob(os.path.join(featurePath, '*'))
                for filePath in filesToRemove:
                    try:
                        os.remove(filePath)
                    except:
                        pass

                try:
                    os.rmdir(featurePath)
                except:
                    pass

            # re-install any optional features found under /usr/local/hobart/features
            installOptionalFeatures(language)

            # use current hardware clock setting since it should have been set before upgrade
            # and update logical system time
            cmd = []
            cmd.append("/sbin/hwclock")
            cmd.append("-s")
            cmd.append("--utc")
            exitCode, stdout, stderr = shellCommand.execute(cmd)

        # print 'Starting crond'
        cmd = []
        cmd.append('/usr/bin/crontab')
        cmd.append("/usr/local/hobart/python/cronjobs")
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        #installPackages()

        buildRuntimeTree()

        # create first boot file so this does section does not run again
        cmd = []
        cmd.append("/bin/touch")
        cmd.append(firstBootFile)
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        # remove the message box display first boot message
        if mbId != None:
            messageBox.destroy(mbId)

        cmd = []
        cmd.append("/bin/sync")
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        # if the RTC is not preset to right date time
        if not rtcPreset:

            # reboot the device
            cmd=[]
            cmd.append("/sbin/init")
            cmd.append("6")
            exitCode, stdout, stderr = shellCommand.execute(cmd)

            # exit this script since depending upon timing the statements
            # below could run and we do not want that to happen
            sys.exit(0)


    cmd = []
    cmd.append("/sbin/hwclock")
    cmd.append("-s")
    cmd.append("--utc")
    exitCode, stdout, stderr = shellCommand.execute(cmd)

    # install if needed
    #installPackages()

    # install if needed
    buildRuntimeTree()

    #print 'Start Touchscreen Driver'
    cmd = []
    cmd.append("/usr/bin/eGTouchD")
    exitCode, stdout, stderr = shellCommand.execute(cmd)

    #check the status of the serial flash
    checkSerialFlashMFG()
    hwRevFileSync.sync()

    #replay USB events from powerup for udev. (udev missing some USB device nodes after powerup)
    cmd = []
    cmd.append("/sbin/udevadm")
    cmd.append("trigger")
    cmd.append("--action=add")
    cmd.append("--subsystem-match=usb")
    exitCode, stdout, stderr = shellCommand.execute(cmd)

    wifiDetect = wifiDetection.wifiDetect()
    if wifiDetect.isWifiInstalled():
        #print 'Load wifi modules'
        wifiModules = ['compat.ko', 'cfg80211.ko', 'mac80211.ko', 'ath.ko', 'ath9k_hw.ko', 'ath9k_common.ko', 'ath9k.ko']
        for i in xrange(0,len(wifiModules)):
            cmd = []
            cmd.append("/sbin/insmod")
            cmd.append(os.path.join(wifiPath,wifiModules[i]))
            if 'ath9k.ko'==wifiModules[i]:
                cmd.append('nohwcrypt=1')
            exitCode, stdout, stderr = shellCommand.execute(cmd)

    #Load Bluetooth driver for Hti Dual-Quad only!
    if cpu_info.getChipId() == 'Dual-Quad' and wifiDetect.isWifiInstalled():
        cmd = []
        cmd.append("/sbin/insmod")
        cmd.append(os.path.join("/","lib","firmware","ath3k.ko"))
        exitCode, stdout, stderr = shellCommand.execute(cmd)

    #print 'Patch touchscreen and X'
    cmd = []
    cmd.append("/usr/bin/xinput")
    cmd.append("set-prop")
    cmd.append("eGalaxTouch Virtual Device for Single")
    cmd.append("Coordinate Transformation Matrix")
    cmd.append("0.5")
    cmd.append("0")
    cmd.append("0")
    cmd.append("0")
    cmd.append("0.5")
    cmd.append("0")
    cmd.append("0")
    cmd.append("0")
    cmd.append("1.0")
    exitCode, stdout, stderr = shellCommand.execute(cmd)

    cmd = []
    cmd.append("/usr/bin/xinput")
    cmd.append("set-prop")
    cmd.append("eGalaxTouch Virtual Device for Single")
    cmd.append("Coordinate Transformation Matrix")
    cmd.append("1.0")
    cmd.append("0")
    cmd.append("0")
    cmd.append("0")
    cmd.append("1.0")
    cmd.append("0")
    cmd.append("0")
    cmd.append("0")
    cmd.append("1.0")
    exitCode, stdout, stderr = shellCommand.execute(cmd)

    # read the flash key buffer
    import license
    xmlLicenseBuffer = license.readFlashKeyBuffer()

    if xmlLicenseBuffer == None:
        xmlLicenseBuffer = license.xmlCreateInitialLicenseBuffer()

    #print 'Execute touchscreen device management'
    os.system("/usr/bin/python %s &" % (os.path.join(pythonPath,"processDeviceEvents.py")))

    #print 'Execute flash drive management'
    os.system("/usr/bin/python %s &" % (os.path.join(pythonPath,"usbDeviceManager.py")))

    #print 'Execute Scale Management backend'
    os.system("/usr/bin/python %s &" % (os.path.join(pythonPath,"smBackend.py")))

    #print 'Execute Update Via SFTP Listener'
    os.system("/usr/bin/python %s &" % (os.path.join(pythonPath,"sftpUpdate.py")))

    # launch VNC server if licensed
    if license.isProductLicensed(xmlLicenseBuffer, 'SCALE_REMOTE_MONITOR'):
        os.system("/usr/bin/x11vnc -forever -display :0 -tightfilexfer -nopw -localhost &")

    #get the u-boot version
    #os.system('source /usr/local/hobart/python/getUbootVersion.sh')
    setUBootVersion()
    errorLog.flush()
    errorLog.close()

if __name__ == "__main__":
    processStartupSequence()
    sys.exit(0)
