#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import shellCommand
from serialFlash import *
import updateSplashImage
import updateSplashVersion
import updateSplashDiagnostics
import file_io
import getBoardType
import cpuCount
import datetime
import re


def getUbootUsdCardVersion():
    ubootVersionFile = os.path.join(os.sep, "uboot", "uboot-version.txt")
    status, data = file_io.readFromFile(ubootVersionFile)

    if status == 'success':
        ubootVersion = data[0]
        return ubootVersion

    return None

def ubootNeedsUpgraded():
    # get the Uboot version installed on the uSD card
    version1 = getUbootUsdCardVersion()
    if version1 == None:
        version1 = '0.0.0'

    # get the Uboot version programmed in the flash part
    version2 = getUbootVersionInFlashPart()
    if version2 == None:
        version2 = '0.0.0'

    version1_list = version1.split('.')
    version2_list = version2.split('.')
    
    if (3 != len(version1_list) or 3 != len(version2_list)):
        return True

    versionA = int(version1_list[0]) * 1000000 + int(version1_list[1]) * 1000 + int(version1_list[2])
    versionB = int(version2_list[0]) * 1000000 + int(version2_list[1]) * 1000 + int(version2_list[2])
    if versionA <= versionB:
        return False

    return True

def getUbootVersionInFlashPart():
    
    if getBoardType.isArkBoard():

        uversion = 'Unknown'

        files = ["/dev/mtd0ro"]

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

                        obj = re.search(r'version=\d+\.\d+\.\d+', str(img.read()), re.MULTILINE | re.DOTALL)

                        if obj:
                            a = obj.group().split('=')

                            if len(a) == 2:
                                uversion = a[1]
                                #print "uversion found, uversion=" + uversion
                                found = True

                cmd = []
                cmd.append('/bin/rm')
                cmd.append(tmpfile)
                exitCode, stdout, stderr = shellCommand.execute(cmd)

            if found:
                return uversion
                
        return None

    else:

        # get the Uboot version located in the flash part that is currently running in RAM
        cmd = []
        cmd.append("fw_printenv")
        cmd.append("version")
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        if exitCode == 0:

            uversion = stdout.replace("version=",  "")
            uversion = uversion.replace("\n",  "")
            return uversion

    return None

def updateUboot():

   # get the Uboot version installed on the uSD card
    uSdUbootVersion = getUbootUsdCardVersion()
    if uSdUbootVersion == None:
        uSdUbootVersion = '0.0.0'

    # get the Uboot version programmed in the flash part
    uversion = getUbootVersionInFlashPart()
    if uversion == None:
        uversion = '0.0.0'

    # determine if the uSD card version is later version than the one in flash
    upgrade_required = ubootNeedsUpgraded()

    # open a log file
    logFile = open("/usr/local/hobart/ubootUpdate.log", 'a+')

    # if a Uboot upgrade is required
    if not upgrade_required:
        logFile.write("U-boot upgrade not required. \n")
        return 0

    # start a log of this update
    logFile.write("\n\n **** Upgrade of u-boot from version " + uversion + " to version %s **** \n" % (uSdUbootVersion))

    now = datetime.datetime.now()
    logTime = now.strftime("%m-%d-%Y %H:%M:%S %p")
    logFile.write("\nBegin: " + logTime + "\n\n")

    logFile.flush()

    #get the software version, scale serial number and scale style to record to upgrade log.
    softVer = open("/SOFTWARE_VERSION_INFO", 'rb')
    logFile.write(" **** Software Version: " + str(softVer.readline()) +  " **** \n")
    softVer.close()

    mfgs = mfgSection()
    name = mfgs.getScaleStyle()
    logFile.write(" **** Scale Style: " + name + " **** \n")
    logFile.write(" **** Scale Serial Number: " + mfgs.getScaleSerialNumber() + " **** \n")
    logFile.write(" **** Date of Manufacture: " + mfgs.getDateOfManufacturing() + " **** \n")
    logFile.write("\n")
    logFile.write("\n")
    logFile.flush()

    if getBoardType.isArkBoard():
        ubootSoloFilename = '/u-boot-s-ark.imx'
        ubootDualFilename = '/u-boot-ark.imx'
        ubootQuadFilename = '/u-boot-q-ark.imx'
    elif getBoardType.isComboBoard():
        ubootSoloFilename = '/u-boot-s-itw.imx'
        ubootDualFilename = '/u-boot-itw.imx'
        ubootQuadFilename = '/u-boot-q-itw.imx'
    elif getBoardType.isBcmBoard():
        ubootSoloFilename = '/u-boot-s.imx'
        ubootDualFilename = '/u-boot.imx'
        ubootQuadFilename = '/u-boot-q.imx'
    else:
        return 1

    # Determine the board type based on the # of CPU's
    numCpu = cpuCount.getCPUCount()
    numCpu = numCpu.replace( "\n", "" )

    # board is a solo
    if numCpu == "1":
        ubootFilename = ubootSoloFilename
        splashFilename = os.path.join(os.sep, "uboot", "hts8.bmp")
    # board is a dual
    elif numCpu == "2":
        ubootFilename = ubootDualFilename
        splashFilename = os.path.join(os.sep, "uboot", "hti8.bmp")
    # board is a quad
    elif numCpu == "4":
        ubootFilename = ubootQuadFilename
        splashFilename = os.path.join(os.sep, "uboot", "htx8.bmp")
    else:
        logFile.write("Unknown Board, numCpu: " + numCpu + "\n")
        return 1

    if getBoardType.isArkBoard():
        # SOF-3203: Need to update flash with the HTm image
        splashFilename = os.path.join(os.sep, "uboot", "htm8.bmp")

    versionFilename = os.path.join(os.sep, "uboot", "u-version.bmp")
    diagFilename = os.path.join(os.sep, "uboot", "u-diagnostics.bmp")
    missingFilename = os.path.join(os.sep, "uboot", "u-missing.bmp")

    if not os.path.exists(ubootFilename):
        logFile.write("Missing uboot firmware!\n")
        return 1
    elif not os.path.exists(splashFilename):
        logFile.write("Missing splash image!\n")
        return 1
    elif not os.path.exists(versionFilename):
        logFile.write("Missing version image!\n")
        return 1
    elif not os.path.exists(diagFilename):
        logFile.write("Missing diag image!\n")
        return 1
    elif not os.path.exists(missingFilename):
        logFile.write("Missing missing image!\n")
        return 1

    # generate padded uboot image from uboot on uSD card
    ubootImage = os.path.join(os.sep, "tmp", "u-boot.imx")

    if os.path.exists(ubootImage):
        os.remove(ubootImage)

    try:
        # read image files
        with open( ubootFilename, 'rb' ) as f:
            image = f.read()

        now = datetime.datetime.now()
        logTime = now.strftime("%m-%d-%Y %H:%M:%S %p")

        logFile.write(logTime + " Image length: " + str(len(image)) + "\n")

        # write image file
        with open( ubootImage, 'wb' ) as f:
            f.write( b'\xFF' * 786432 )

            f.seek( 1024 )
            f.write( image )

    except Exception as e:
        logFile.write("Failed to generate image: Reason - " + str(e))
        return 1

    # update image
    result = updateSplashImage.updateSplashImage(splashFilename)

    now = datetime.datetime.now()
    logTime = now.strftime("%m-%d-%Y %H:%M:%S %p")

    if result == 'success':
        logFile.write(logTime + " Splash image updated \n")
    else:
        logFile.write(logTime + " Splash image failed to update! \n")

    # update version
    result = updateSplashVersion.updateSplashVersion(versionFilename)

    now = datetime.datetime.now()
    logTime = now.strftime("%m-%d-%Y %H:%M:%S %p")

    if result == 'success':
        logFile.write(logTime + " Version image updated \n")
    else:
        logFile.write(logTime + " Version image failed to update! \n")
        return 1

    # update diag and missing
    result = updateSplashDiagnostics.updateSplashErrorBitmaps(diagFilename, missingFilename)

    now = datetime.datetime.now()
    logTime = now.strftime("%m-%d-%Y %H:%M:%S %p")

    if(result == 'success'):
        logFile.write(logTime + " U-boot diagnostic images updated \n")
    else:
        logFile.write(logTime + " U-boot diagnostic images failed to update! \n")

    logFile.flush()

    # flash the new uboot image  section of the serial flash
    cmd = []
    cmd.append("flashcp")
    cmd.append(ubootImage)
    cmd.append("/dev/mtd0")
    exitCode, stdout, stderr = shellCommand.execute(cmd)

    now = datetime.datetime.now()
    logTime = now.strftime("%m-%d-%Y %H:%M:%S %p")

    if exitCode != 0:
        logFile.write(logTime + " U-boot image failed to write to section /dev/mtd0: " + stderr + "\n")
        return exitCode
    else:
        logFile.write(logTime + " U-boot image section programmed \n")

    # erase u-boot image section of the serial flash.
    cmd = []
    cmd.append("flash_eraseall")
    cmd.append("/dev/mtd1")
    exitCode, stdout, stderr = shellCommand.execute(cmd)

    now = datetime.datetime.now()
    logTime = now.strftime("%m-%d-%Y %H:%M:%S %p")
    
    if exitCode != 0:
        logFile.write(logTime + " Erase of /dev/mtd1 section failed: " + stderr + "\n")
        return exitCode
    else:
        logFile.write(logTime + " /dev/mtd1 section erased \n")

    now = datetime.datetime.now()
    logTime = now.strftime("%m-%d-%Y %H:%M:%S %p")
    logFile.write("\nComplete: " + logTime + "\n\n")

    logFile.flush()
    logFile.close()
    return 0

if __name__ == "__main__":
    exitCode = updateUboot()
    sys.exit(exitCode)
