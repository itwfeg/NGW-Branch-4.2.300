#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import os
import re
import osUtils
import sys
import shellCommand
import tarfile
import glob
import shutil
import sendDeviceEventMessage
import socket
import languageSettings
import select
import messageBox
import usbQuery
import configInfo
import stat
import license
import md5checksum
from sftpProtocol import SFTPClient
import fileSysStatistics
import installUtils
import installFilePatterns
import subprocess
import tempfile
import updateSplashDiagnostics
import updateSplashVersion
import getBoardType
import time
import logging
import deviceType
MONOLITHIC_CORE_PREINSTALL_VERSION="06032015_114048"

UPGRADE_LOG_FILENAME="/installUpgrade.log"

mbprev = None

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
        logging.info("Unknown Board!")
        print("Unknown Board!")

def show_message(message, width=1024, height=600, fontFamily="Sans", fontSize=24, timer=None):
    global mbprev

    exitCode, mb = messageBox.display(message, width=width, height=height, fontFamily=fontFamily, fontSize=fontSize, timer=timer)

    if mbprev:
        messageBox.destroy(mbprev)

    mbprev = mb
    return exitCode, mb

def osVersionMajorMinor():
    os_info = os.uname()[2]
    os_version = None
    os_version_string = os_info.split('-')[0]
    os_version_tokens = os_version_string.split('.')
    if len(os_version_tokens) >= 2:
        try:
            os_version = float(os_version_tokens[0] + '.' + os_version_tokens[1])
        except:
            os_version = None
    logging.info("osVersionMajorMinor: %s" % os_version)
    return os_version

def cleanupFeature(filepath, destination="/"):
    """ Before installing a new feature upgrade, iterate through the feature archive and clear out any potential remnants left over by 
        previous versions of the same feature to ensure that they do not break the newest install.
    """
    try:
        archive = tarfile.open(filepath)

        for tarinfo in archive.getmembers():
            directories = os.path.dirname(tarinfo.name).split(os.sep)
            directory_name = destination + os.path.dirname(tarinfo.name)

            if directory_name.startswith("/usr/local/hobart/features"):
                if os.path.exists(directory_name):
                    logging.info("CleanupFeature: " + directory_name)
                    shutil.rmtree(directory_name)

        os.system("/bin/sync")
    except Exception as err:
        logging.info("cleanupFeature exception")
        logging.exception(err)

def extractOSAgnosticArchive(archive):
    if os.name == 'posix':
        baseDirectory = '/usr/local/'
    else:
        baseDirectory = 'C:/Program Files/'

    logging.info("extractOSAgnosticArchive: start")
    archive.extractall(baseDirectory)
    logging.info("extractOSAgnosticArchive: done")

    # copy the hobart directory from the proper baseFolder out to the baseDirectory it belongs.
    # for root, dirs, files in os.walk(os.path.join(temporaryDirectory, baseFolder, 'hobart')):
        # destinationDirectory = os.path.join(baseDirectory, root.split(baseFolder)[1])

        # if not os.path.exists(destinationDirectory):
            # os.makedirs(destinationDirectory)
        # for filename in files:
            # shutil.copy2(os.path.join(root, filename), os.path.join(destinationDirectory, filename))

def extractMultimediaArchive(archive):
    # get base folder for extraction
    if os.name == 'posix':
        baseDirectory = '/usr/local/hobart'
    else:
        baseDirectory = 'C:/Program Files/Hobart'

    # setup allowed prefixes
    prefixes = []
    prefixes.append('audio/')
    prefixes.append('images/')
    prefixes.append('playlists/')
    prefixes.append('videos/')

    # only extract multimedia files
    subdir_and_files = []

    for tarinfo in archive.getmembers():
        for prefix in prefixes:
            if tarinfo.name.startswith(prefix) or tarinfo.name.startswith("./%s" % prefix):
                subdir_and_files.append(tarinfo)
            elif os.name != 'posix':
                windowsPrefix = prefix[0].upper() + prefix[1:]
                if tarinfo.name.startswith(windowsPrefix) or tarinfo.name.startswith("./%s" % windowsPrefix):
                    subdir_and_files.append(tarinfo)

    # extract!
    logging.info("extractMultimediaArchive: files: %s" % subdir_and_files)
    archive.extractall(baseDirectory, subdir_and_files)
    logging.info("extractMultimediaArchive: done")

def installFPC(upgradeFile):
    logging.info("installFPC: start")
    try:
        # detect board type and copy file to download directory which will trigger chobart_d to install

        if os.name == 'posix':
            # extract both binaries to the firmware directory	        
            cmd = []
            cmd.append("/bin/tar")
            cmd.append("xvzf")
            cmd.append(upgradeFile)
            cmd.append("-C")
            cmd.append(os.path.join('/', 'lib', 'firmware'))
            exitCode, stdout,stderr = shellCommand.execute(cmd)

            #determine our board type
            board = "unknown"

            if getBoardType.isComboBoard():
                board = "combo"
            if getBoardType.isBcmBoard():
                board = "bcm"
            if getBoardType.isArkBoard():
                board = "ark"

            #-------------------------------------------------------------------
            # SOF-4447: Need to be able to upgrade weigher firmware on a wrapper
            #-------------------------------------------------------------------
            file = open( "/etc/hobart/scale-type.dat", 'r' )
            scaleType = file.read()
            scaleType = scaleType.replace( '\n', '' )
            file.close()

            if ( scaleType == 'pre-pack' ):
                cmd = []
                cmd.append("/bin/mv")		   
                cmd.append("/lib/firmware/XPEWeigher.bin")
                cmd.append(os.path.join('/', 'usr', 'local', 'hobart', 'download'))
                exitCode, stdout, stderr =shellCommand.execute(cmd)
            
            #copy the correct binary to the firmware folder and download directory
            if board == "combo":
                cmd = []
                cmd.append("/bin/cp")
                cmd.append("/lib/firmware/sourceFPC2.bin")
                cmd.append(os.path.join('/', 'lib', 'firmware', 'fpc'))
                exitCode, stdout,stderr = shellCommand.execute(cmd)

                cmd = []
                cmd.append("/bin/mv")		   
                cmd.append("/lib/firmware/sourceFPC2.bin")
                cmd.append(os.path.join('/', 'usr', 'local', 'hobart', 'download'))
                exitCode, stdout, stderr =shellCommand.execute(cmd)

            elif board == "bcm":
                cmd = []
                cmd.append("/bin/cp")
                cmd.append("/lib/firmware/sourceFPC.bin")
                cmd.append(os.path.join('/', 'lib', 'firmware', 'apc'))
                exitCode, stdout,stderr = shellCommand.execute(cmd)

                cmd = []
                cmd.append("/bin/mv")		   
                cmd.append("/lib/firmware/sourceFPC.bin")
                cmd.append(os.path.join('/', 'usr', 'local', 'hobart', 'download'))
                exitCode, stdout, stderr =shellCommand.execute(cmd)

            elif board == "ark":
                cmd = []
                cmd.append("/bin/cp")
                cmd.append("/lib/firmware/sourceKPCApp.bin")
                cmd.append(os.path.join('/', 'lib', 'firmware', 'kpc', 'app'))
                exitCode, stdout,stderr = shellCommand.execute(cmd)

                cmd = []
                cmd.append("/bin/mv")
                cmd.append("/lib/firmware/sourceKPCApp.bin")
                cmd.append(os.path.join('/', 'usr', 'local', 'hobart', 'download'))
                exitCode, stdout, stderr =shellCommand.execute(cmd)
                
                # Also copy the firmware for Free-Standing Self-Serve
                cmd = []
                cmd.append("/bin/cp")
                cmd.append("/lib/firmware/sourceFsssApp.bin")
                cmd.append(os.path.join('/', 'lib', 'firmware', 'fsss', 'app'))
                exitCode, stdout,stderr = shellCommand.execute(cmd)

                cmd = []
                cmd.append("/bin/mv")
                cmd.append("/lib/firmware/sourceFsssApp.bin")
                cmd.append(os.path.join('/', 'usr', 'local', 'hobart', 'download'))
                exitCode, stdout, stderr =shellCommand.execute(cmd)

            cmd = []
            cmd.append("bin/sync")
            shellCommand.execute(cmd)

        elif os.name == 'nt':
            tar = tarfile.open(upgradeFile, 'r')
            tar.extractall(os.path.join("c:\\","Program Files","Hobart","Download"))

        sendDeviceEventMessage.sendMessage(server='localhost', port=6000, message="RTDO" + chr(0x1F) + "TNCheckForDownload" + chr(0x1E))
        logging.info("installFPC: done")
        return exitCode

    except Exception as e:
        logging.info("installFPC: failed!")
        logging.execption(e)
        return 1

def installPrePackDevice(upgradeFile):
    logging.info("installPrePackDevice: start")
    try:
        if os.name == 'posix':
            tmpDir = "/lib/firmware/tmp"
        elif os.name == 'nt':
            tmpDir = os.path.join("C:\\", "Program Files", "Hobart", "tmp")
        #make the temporary dir to extract the tarball to
        if os.path.exists(tmpDir):
            shutil.rmtree(tmpDir)
        os.makedirs(tmpDir)
        if os.name == 'posix':
            cmd = []
            cmd.append("/bin/tar")
            cmd.append("xvzf")
            cmd.append(upgradeFile)
            cmd.append("-C")
            cmd.append(tmpDir)
            exitCode, stdout, stderr =shellCommand.execute(cmd)
            #print 'Prepack:  After extract exitCode = ' + str(exitCode) + ',  stdout = ' + str(stdout) + ',  stderr = ' + str(stderr)
            #sync
            cmd = []
            cmd.append("/bin/sync")
            shellCommand.execute(cmd)
        elif os.name == 'nt':
            tar = tarfile.open(upgradeFile, 'r')
            #print "Opened tar file: " + upgradeFile + "."
            tar.extractall(tmpDir)
            #print "Extracted files to " + tmpDir + "."
            exitCode = 0					# What should this be and does extractall return anything???
        #send signal to start upgrade from usb for prepack devices
        sendDeviceEventMessage.sendMessage(server="localhost", port=6000, message="RTDO" + chr(0x1F) + "TNPrePackUpgrade" + tmpDir + chr(0x1E))
        logging.info("installPrePackDevice: done")
        return exitCode

    except Exception as e:
        logging.info("installPrePackDevice: failed!")
        logging.exception(e)
        return 1

def installPrePackDeviceNGW(upgradeFile, machine='ngw', isNGW2P=False):

    logging.info("installPrePackDeviceNGW: start")
    try:
        source = "source.bin"
        version= "version.ini"

        if os.name == 'posix':
            tmpDir = "/lib/firmware/tmp/"
            destination = "/lib/firmware/hobart/" + machine + "/wrapper/"
        elif os.name == 'nt':
            tmpDir = os.path.join("C:\\", "Program Files", "Hobart", "tmp")
            destination = os.path.join("C:\\", "Program Files", "Hobart", machine.upper(),  "Wrapper")

        #make the temporary dir to extract the tarball to
        if os.path.exists(tmpDir):
            shutil.rmtree(tmpDir)
        os.makedirs(tmpDir)

        if os.name == 'posix':
            cmd = []
            cmd.append("/bin/tar")
            cmd.append("xvzf")
            cmd.append(upgradeFile)
            cmd.append("-C")
            cmd.append(tmpDir)
            exitCode, stdout, stderr =shellCommand.execute(cmd)


            node = ""
            node = tmpDir + "node1/" + source
            destNode = ""
            destNode = destination  + "node1"
            cmd = []
            cmd.append("/bin/cp")
            cmd.append(node)
            cmd.append(destNode)
            exitCode, stdout, stderr =shellCommand.execute(cmd)

            node = ""
            node = tmpDir + "node1/" + version
            cmd = []
            cmd.append("/bin/cp")
            cmd.append(node)
            cmd.append(destNode)
            exitCode, stdout, stderr =shellCommand.execute(cmd)

            node = ""
            node = tmpDir + "node2/" + source
            destNode = ""
            destNode = destination  + "node2"
            cmd = []
            cmd.append("/bin/cp")
            cmd.append(node)
            cmd.append(destNode)
            exitCode, stdout, stderr =shellCommand.execute(cmd)

            node = ""
            node = tmpDir + "node2/" + version
            cmd = []
            cmd.append("/bin/cp")
            cmd.append(node)
            cmd.append(destNode)
            exitCode, stdout, stderr =shellCommand.execute(cmd)

            node = ""
            node = tmpDir + "node3/" + source
            destNode = ""
            destNode = destination  + "node3"
            cmd = []
            cmd.append("/bin/mv")
            cmd.append(node)
            cmd.append(destNode)
            shellCommand.execute(cmd)

            node = ""
            node = tmpDir + "node3/" + version
            cmd = []
            cmd.append("/bin/mv")
            cmd.append(node)
            cmd.append(destNode)
            exitCode, stdout, stderr =shellCommand.execute(cmd)

            node = ""
            node = tmpDir + "node4/" + source
            destNode = ""
            destNode = destination  + "node4"
            cmd = []
            cmd.append("/bin/mv")
            cmd.append(node)
            cmd.append(destNode)
            exitCode, stdout, stderr =shellCommand.execute(cmd)

            node = ""
            node = tmpDir + "node4/" + version
            cmd = []
            cmd.append("/bin/mv")
            cmd.append(node)
            cmd.append(destNode)
            exitCode, stdout, stderr =shellCommand.execute(cmd)

            node = ""
            node = tmpDir + "node5/" + source
            destNode = ""
            destNode = destination  + "node5"
            cmd = []
            cmd.append("/bin/mv")
            cmd.append(node)
            cmd.append(destNode)
            exitCode, stdout, stderr =shellCommand.execute(cmd)


            node = ""
            node = tmpDir + "node5/" + version
            cmd = []
            cmd.append("/bin/mv")
            cmd.append(node)
            cmd.append(destNode)
            exitCode, stdout, stderr =shellCommand.execute(cmd)

            node = ""
            node = tmpDir + "node6/" + source
            if os.path.exists(node):
                destNode = ""
                destNode = destination  + "node6"
                cmd = []
                cmd.append("/bin/mv")
                cmd.append(node)
                cmd.append(destNode)
                exitCode, stdout, stderr =shellCommand.execute(cmd)

            node = ""
            node = tmpDir + "node6/" + version
            if os.path.exists(node):
                cmd = []
                cmd.append("/bin/mv")
                cmd.append(node)
                cmd.append(destNode)
                exitCode, stdout, stderr =shellCommand.execute(cmd)

            # SOF-4509: Also install TOF App and Boot firmware for 2nd Label Applier
            if( isNGW2P ):
                destination = "/lib/firmware/hobart/" + machine + "/wrapper/tof/"
                src = ""
                src = tmpDir + "tof/app/sourceTOFApp.bin"
                if os.path.exists(src):
                    dest = ""
                    dest = destination  + "app/"
                    cmd = []
                    cmd.append("/bin/mv")
                    cmd.append(src)
                    cmd.append(dest)
                    exitCode, stdout, stderr =shellCommand.execute(cmd)

                src = ""
                src = tmpDir + "tof/app/" + version
                if os.path.exists(src):
                    dest = ""
                    dest = destination  + "app/"
                    cmd = []
                    cmd.append("/bin/mv")
                    cmd.append(src)
                    cmd.append(dest)
                    exitCode, stdout, stderr =shellCommand.execute(cmd)

                src = ""
                src = tmpDir + "tof/boot/sourceTOFBoot.bin"
                if os.path.exists(src):
                    dest = ""
                    dest = destination  + "boot/"
                    cmd = []
                    cmd.append("/bin/mv")
                    cmd.append(src)
                    cmd.append(dest)
                    exitCode, stdout, stderr =shellCommand.execute(cmd)
                
                src = ""
                src = tmpDir + "tof/boot/" + version
                if os.path.exists(src):
                    dest = ""
                    dest = destination  + "boot/"
                    cmd = []
                    cmd.append("/bin/mv")
                    cmd.append(src)
                    cmd.append(dest)
                    exitCode, stdout, stderr =shellCommand.execute(cmd)
            
        elif os.name == 'nt':
            #TO DO:
            tar = tarfile.open(upgradeFile, 'r')
            #print "Opened tar file: " + upgradeFile + "."
            tar.extractall(tmpDir)
            #print "Extracted files to " + tmpDir + "."
            exitCode = 0					# What should this be and does extractall return anything???

        #send signal to start upgrade from usb for prepack devices
        sendDeviceEventMessage.sendMessage(server="localhost", port=6000, message="RTDO" + chr(0x1F) + "TNPrePackUpgradeAWS3" + tmpDir + chr(0x1E))
        logging.info("installPrePackDeviceNGW: done")
        return exitCode

    except Exception as e:
        logging.info("installPrePackDeviceNGW: failed!")
        logging.exception(e)
        return 1

def installLicense(licenseFile, language, serverHost=None, username=None, password=None, uploadFolder=None):

    # See if the USB drive mounted contains a feature license key and if so activate license
    try:
        serialNumber = None
        if serverHost == None:
            # get list of attached USB drives, only one USB thumb drive can be inserted in scale when installing licenses
            usbDevices = usbQuery.UsbDeviceQuery()
            devices = usbDevices.findDevicesInClass(usbDevices.DEVICE_TYPE_USB)
            for dev in devices:
                serialNumber = usbDevices.getSerialNumber(dev)
                # only compute serial number for first USB device found
                break

        else:

            ipAddressRegExp = re.compile("\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}")

            # If serverHost is not an IP address get IP by hostName
            if ipAddressRegExp.match(serverHost):
                serialNumber = serverHost
            else:
                # get ip address of hostname
                serialNumber = socket.gethostbyname(serverHost)

        # do not compute mac address since this is a scale device
        macAddress = license.NULL_MAC_ADDRESS

        # if the USB thumb drive has a serial number
        if serialNumber == None:
            if language == 'spanish':
                message = "Error de validación de número de serie."
            elif language == 'french':
                message = "Erreur lors de la validation du numéro de série."
            else:
                message = "Error during serial number validation."

            exitCode,  mbId = show_message(message, timer=5)
            logging.info("installLicense: %s" % message)
            return 1

        # get elements in license file
        status, individualName, companyName, companyAddress, productNames, licenseType, licenseActivationDate, licenseAllotment, licenseTimeout, licenseKey = license.getLicenseElements(licenseFile)

        # if read of license file and elements extracted ok
        if status != 'success':
            if language == 'spanish':
                message = "No se pudo analizar el archivo de licencia."
            elif language == 'french':
                message = "Impossible d'analyser le fichier de licence."
            else:
                message = "Could not parse license file."
            logging.info("installLicense: %s" % message)
            exitCode,  mbId = show_message(message, timer=5)
            return 1

        # if current license file being processed contains a scale product/feature
        if not license.containsScaleProducts(productNames):
            if language == 'spanish':
                message = "Licencia no contiene productos escala."
            elif language == 'french':
                message = "Licence ne contient pas de produits á grande échelle."
            else:
                message = "License does not contain products."
            logging.info("installLicense: %s" % message)
            exitCode,  mbId = show_message(message, timer=5)
            return 1

        needToReboot = False

        # for all products/features in license
        for i in range(0, len(productNames)):

            # get the type of feature and if a scale feature process it
            if license.getFeatureType(productNames[i]) == 'scale':

                # check if license file on thumb drive is valid, mac address of 0 is used for scale products
                if not license.validLicenseKey(licenseFile, productNames[i], license.NULL_MAC_ADDRESS, serialNumber):
                    if language == 'spanish':
                        message = "Clave de licencia no es válida."
                    elif language == 'french':
                        message = "La clé de licence n'est pas valide."
                    else:
                        message = "License key is not valid."
                    logging.info("installLicense: %s" % message)
                    exitCode,  mbId = show_message(message, timer=5)
                    return 1

                # read the flash key buffer
                xmlLicenseBuffer = license.readFlashKeyBuffer()

                if xmlLicenseBuffer == None:
                    xmlLicenseBuffer = license.xmlCreateInitialLicenseBuffer()

                addLicense = False

                # see if products in license are already licensed
                for i in range(0, len(productNames)):
                    if not license.isProductLicensed(xmlLicenseBuffer, productNames[i]):
                        # did not find product name so add license
                        addLicense = True
                    if productNames[i] in license.HTS_DISABLED_PRODUCTS:
                        needToReboot = True
                    if 'SCALE_REMOTE_MONITOR' == productNames[i]:
                        needToReboot = True
                    # SOF-3479
                    if 'SCALE_ENHANCED_CENTERSCREEN' == productNames[i]:
                        needToReboot = True

                # at least one product not licensed in flash so need to add license
                if not addLicense:
                    if language == 'spanish':
                        message = "Este producto ya tiene licencia."
                    elif language == 'french':
                        message = "Ce produit est déjá autorisé."
                    else:
                        message = "This product is already licensed."

                    logging.info("installLicense: %s" % message)
                    exitCode, mbId = show_message(message, timer=5)
                    return 1

                # update xml license buffer with new key
                xmlLicenseBuffer = license.xmlAddLicenseToBuffer(xmlLicenseBuffer, individualName, companyName, companyAddress, productNames, licenseType, licenseActivationDate, licenseAllotment, licenseTimeout, licenseKey)

                # set number of licenses to the integer equivalent
                numLicenses = int(licenseAllotment)

                # if the number of licenses > 0
                if numLicenses <= 0:
                    if language == 'spanish':
                        message = "Adjudicación de licencias es 0, no se puede instalar la licencia."
                    elif language == 'french':
                        message = "Licence attribution est de 0, ne peut pas installer la licence."
                    else:
                        message = "Licenses allotment is 0, can not install license"

                    logging.info("installLicense: %s" % message)
                    exitCode, mbId = show_message(message, timer=5)                    
                    return 1

                numLicenses = numLicenses - 1
                licenseAllotment = str(numLicenses)

                # write the flash key buffer
                status,  message = license.writeFlashKeyBuffer(xmlLicenseBuffer)

                if status != 'success':
                    if language == 'spanish':
                        message = "Error al escribir licencia para flash."
                    elif language == 'french':
                        message = "Erreur d'écriture licence á clignoter."
                    else:
                        message = "Error writing license to flash."
                        
                    logging.info("installLicense: %s" % message)
                    exitCode, mbId = show_message(message, timer=5)
                    return 1

                # rewrite the license file on the thumb drive or in local folder for transmission to server
                # so that one license is used
                status, reason, licenseData = license.createLicenseFile(licenseFile, individualName, companyName,
                                                                        companyAddress, productNames, licenseType,
                                                                        licenseActivationDate, licenseAllotment,
                                                                        licenseTimeout, serialNumber=None,
                                                                        licenseKey=licenseKey)

                if status != 'success':
                    if language == 'spanish':
                        message = "Un error al actualizar el archivo de licencia."
                    elif language == 'french':
                        message = "Erreur mise á jour le fichier de licence."
                    else:
                        message = "Error updating license file."

                    logging.info("installLicense: %s" % message)
                    exitCode, mbId = show_message(message, timer=5)
                    return 1

                # if license is a server ip address liceense type
                if licenseType == 'SERVER_IP_ADDRESS':

                    # SFTP the license file back to server
                    SFTPClnt = SFTPClient(username=username, password=password, host=serverHost)
                    connectResult = SFTPClnt.connect()

                    # if connection via SFTP was not successful
                    if not connectResult == "success" and not connectResult == "alreadyConnected":
                        print(('Failed to connect to SFTPClient@%s with user:%s pass:%s' % (serverHost, username, password)))
                        if language == 'spanish':
                            message = "Error de conexión al servidor de licencias."
                        elif language == 'french':
                            message = "Erreur de connexion au serveur de licence."
                        else:
                            message = "Error connecting to license server."

                        logging.info("installLicense: %s" % message)
                        exitCode, mbId = show_message(message, timer=5)
                        return 1

                    # upload file and check status
                    elif SFTPClnt.uploadFile(uploadFolder, licenseFile).split('|')[0] != 'success':
                        if language == 'spanish':
                            message = "Error al actualizar el servidor de licencias."
                        elif language == 'french':
                            message = "Erreur mise á jour de serveur de licences."
                        else:
                            message = "Error updating license server."

                        logging.info("installLicense: %s" % message)
                        exitCode, mbId = show_message(message, timer=5)
                        return 1

                # license was added or no more licenses issue
                # so break out of processing products loop
                break

        message = ""
        if language == 'spanish':
            message = "Clave de licencia instalada."
        elif language == 'french':
            message = "La clé de licence installée."
        else:
            message = "License key installed."

        if needToReboot:
            os.system("/sbin/reboot")

        logging.info("installLicense: %s" % message)
        exitCode, mbId = show_message(message, timer=5)
        return 0

    except Exception as err:
        if language == 'spanish':
            message = "Un fatal error al actualizar el archivo de licencia."
        elif language == 'french':
            message = "Erreur fatale mise á jour le fichier de licence."
        else:
            message = "Fatal error updating license file."

        logging.info("installLicense: failed!")
        logging.exception(err)
        exitCode, mbId = show_message(message, timer=5)
        return 1


def cleanupPackageUpgradeSource(removeUpgrade, upgradeFilename):
    logging.info("cleanupPackageUpgradeSource: start")
    upgrade_name = os.path.splitext(os.path.basename(upgradeFilename))[0]
    directory_name = os.path.basename(os.path.dirname(upgradeFilename))
    match = re.match(r"^ht\w-.+", upgrade_name)
    # check to verify the given path is one of our upgrade paths by checking if the filename starts with ht*-
    if not (match is None):
        # upgrade from package
        if upgrade_name == directory_name:
            if removeUpgrade:
                package_path = os.path.join(os.path.dirname(os.path.dirname(upgradeFilename)), upgrade_name + '.package.tgz')
                if os.path.isfile(package_path):
                    try:
                        os.remove(package_path)
                    except Exception as err:
                        print(('error removing upgrade source:', str(err)))
                        logging.info("cleanupPackageUpgradeSource: failed to remove upgrade source")
                        logging.exception(err)


            isThumbDrive=False
            if upgradeFilename.startswith('/media'):
                isThumbDrive=True
            if upgradeFilename.startswith('D:\\'):
                isThumbDrive=True
            if upgradeFilename.startswith('E:\\'):
                isThumbDrive=True

            # remove package folder
            if not isThumbDrive:
                directory_path = os.path.dirname(upgradeFilename)
                try:
                    shutil.rmtree(directory_path)
                except Exception as err:
                    print(('error removing upgrade package directory:', str(err)))
                    logging.info("cleanupPackageUpgradeSource: failed to remove upgrade package directory")
                    logging.exception(err)

        # upgrade from tarball
        elif removeUpgrade:
            if os.path.isfile(upgradeFilename):
                try:
                    os.remove(upgradeFilename)
                except Exception as err:
                    logging.info("cleanupPackageUpgradeSource: failed to remove upgrade tarball")
                    logging.exception(err)
                    print((str(err)))

            md5Path = md5checksum.md5PathFromFilename(upgradeFilename)

            if os.path.exists(md5Path):
                try:
                    os.remove(md5Path)
                except Exception as err:
                    logging.info("cleanupPackageUpgradeSource: failed to remove upgrade md5")
                    logging.exception(err)
                    print(('error removing old upgrade md5:', str(err)))
    else:
        logging.info("cleanupPackageUpgradeSource: Cleanup given non hobart upgrade file, No files Removed")
        print("Cleanup given non hobart upgrade file, No files Removed!")
        print(("Upgrade: {0}".format(upgrade_name)))
        print(("Dir: {0}".format(directory_name)))
        print(("Match: {0}".format(match)))


def displayNotEnoughSpaceMessage(language, whichPartition, availableSpace, neededSpace):
    if language == 'spanish':
        if whichPartition == 'recovery':
            message = "Partición de recuperación está lleno. La actualización ha sido abortada. Espacio disponible: "
        elif whichPartition == 'live':
            message = "Partición en vivo está lleno. La actualización ha sido abortada. Espacio disponible: "
        message += str( availableSpace )  + " Bytes \nCantidad necesaria: "
        message += str( neededSpace )  +  " Bytes"

    elif language == 'french':
        if whichPartition == 'recovery':
            message = "Partition de récupération est plein. La mise á niveau a été abandonnée. L'espace disponible: "
        elif whichPartition == 'live':
            message = "Partition Live est plein. La mise á niveau a été abandonnée. L'espace disponible: "
        message += str( availableSpace )  + " Octets \nQuantité nécessaire: "
        message += str( neededSpace ) +  " Octets"
    else:
        if whichPartition == 'recovery':
            message = "Recovery partition is full. The upgrade has been aborted. \nSpace available: "
        elif whichPartition == 'live':
            message = "Live partition is full. The upgrade has been aborted. \nSpace available: "
        message += str( availableSpace )  +   " Bytes \nAmount needed: "
        message += str( neededSpace ) + " Bytes"

    logging.info("displayNotEnoughSpaceMessage: %s" % message)
    exitCode,  mbId = show_message(message=message,timer=60)


def saveRestoreCronJobs(restoreScriptFile, restorePath):
    logging.info("saveRestoreCronJobs: start")
    return
    cmd = []
    cmd.append('schtasks')
    cmd.append('/Query')
    cmd.append('/FO')
    cmd.append('LIST')
    exitCode, stdout, stderror = shellCommand.execute(cmd=cmd)

    if exitCode == 0:
        tasks = re.split('\r\n', stdout)

        for i in range(0, len(tasks)):
            SPLIT_TASKNAME = 'TaskName:[ ]+'

            taskname = re.split(SPLIT_TASKNAME, tasks[i])

            if len(taskname) > 1:

                # TODO - need to make sure there are no '\' in name
                # TODO - need to check exitCode
                # Do not save off the standard Microsoft schedules
                if not re.search('Microsoft', taskname[1]):
                    # Note: We need to use Popen as shellCommand.Execute doubles up on any '\' in the text of the params
                    p = subprocess.Popen('schtasks /Query /TN \"' + taskname[1] + '\" /XML', stdout=subprocess.PIPE)
                    out = p.stdout.read()

                    try:
                        f = open(os.path.join(restorePath, taskname[1] + '.xml'), 'w')
                        f.write(out)
                        f.close()

                        restoreScriptFile.write('schtasks /Create /TN \"' + taskname[1] + '\" /XML \"' + os.path.join(restorePath, taskname[1] + '.xml\n'))
                    except Exception as e:
                        logging.info("saveRestoreCronJobs: failed")
                        logging.exception(e)
                        pass


def installUpgrade(upgradeFilename, serverHost=None,  username=None,  password=None,  uploadFolder=None, removeUpgrade=False):
    logging.info("installUpgrade with file: %s" % upgradeFilename)
    return installUpgradeNonTerminating(upgradeFilename,serverHost,username,password,uploadFolder,removeUpgrade)


def backupCriticalFiles(upgradeFilename, resources, restoreScript, scaleAppRestore, mbId,
                        language, db, dbBackupInPlace, live, liveHobart, isPatchUpgrade, removeUpgrade, dbRestore, coreTGZ,
                        removeResources=True):
    logging.info("backupCriticalFiles: start")
    # remove anything in recovery resources directory
    if removeResources:
        if os.name == 'posix':
            os.system("/bin/rm -rf %s" % (os.path.join(resources, "*")))
        elif os.name == 'nt':
            filelist = os.listdir(resources)
            for f in filelist:
                try:
                    os.chmod(os.path.join(resources,f), stat.S_IWRITE)
                    os.remove(os.path.join(resources,f))
                except:
                    logging.info("backupCriticalFiles: failed to remove: %s" % f)
                    print(('error: Cannot remove file: ' + f))

    try:
        # open a restore script file for stagingRecover.sh to call to restore after upgrade
        restoreScriptFile = open(restoreScript, 'w')
        if os.name == 'posix':
            restoreScriptFile.write('#!/bin/bash\n')
            restoreScriptFile.write('PATH=/usr/bin:/bin:/usr/sbin:/sbin\n')

    except IOError as exception_details:
        logging.info("backupCriticalFiles: failed to open file: %s" % restoreScript)
        logging.exception(exception_details)
        print(('error: Can not open file: ' + restoreScript + ', Reason:' + exception_details[1]))
        return 1

    # if rootfs.tar.gz already in recovery area
    if os.path.exists(os.path.join(scaleAppRestore, coreTGZ)):
        try:
            # delete it
            os.chmod(os.path.join(scaleAppRestore, coreTGZ), stat.S_IWRITE)
            os.remove(os.path.join(scaleAppRestore, coreTGZ))
        except exception_details:
            logging.info("backupCriticalFiles: failed to delete backup file")
            logging.exception(exception_details)
            print(('error: Cannot remove backup file, Reason: ' + exception_details[1]))

    messageBox.destroy(mbId)

    if language == 'spanish':
        message = "Actualización de Aplicación y SO\n\nCopiando actualización desde dispositivo fuente\nPor favor espere"
    elif language == 'french':
        message = "Mise á niveau de l\'application et du SE (étape 1) \n\n Copie de la mise á niveau á partir du \ndispositif source Veuillez patienter"
    else:
        message = "Application and OS Upgrade (Stage 1)\n\nCopying Upgrade from Source Device\nPlease wait..."

    logging.info("backupCriticalFiles: %s" % message)
    exitCode,  mbId = show_message(message)

    # copy the file via shell, shutil is VERY SLOW on large files and this is large
    if os.name == 'posix':
        os.system('/bin/cp -f %s %s' % (upgradeFilename, os.path.join(scaleAppRestore,"hte.tar.gz")))
    elif os.name == 'nt':
        if not os.path.exists(scaleAppRestore):
            os.mkdir(scaleAppRestore)
        shutil.copy(upgradeFilename, os.path.join(scaleAppRestore, coreTGZ))
        os.chmod(os.path.join(scaleAppRestore, coreTGZ), stat.S_IWRITE)

    # if we are told to remove the upgrade file source
    cleanupPackageUpgradeSource(removeUpgrade, upgradeFilename)

    messageBox.destroy(mbId)

    if language == 'spanish':
        message = "Actualización de Aplicación y SO (Etapa 1)\n\nRespaldando la Base de Datos y Configuración..."
    elif language == 'french':
        message = "Mise á niveau de l\'application et du SE (étape 1)\n\nSauvegarde de la Base de Données et de Configuration..."
    else:
        message = "Application and OS Upgrade (Stage 1)\n\nBacking up DB and Configuration..."

    logging.info("backupCriticalFiles: %s" % message)
    exitCode,  mbId = show_message(message)

    if os.name == 'posix':
        # backup the database, use wildcard for last character since db is evolving
        #matches = glob.glob(os.path.join(db,"scale","dbfiles","dbft?.db"))
        #if len(matches) > 0:
        #    shutil.copy(matches[0], dbRestore)
        #    restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join(dbRestore,os.path.basename(matches[0])), os.path.join(db, "scale","dbfiles")))

        if os.path.exists(os.path.join(db,"settingswizard")):
            shutil.copytree(
                os.path.join(db, "settingswizard"), os.path.join(resources,"settingswizard"), symlinks=False, ignore=None)
            restoreScriptFile.write('/bin/cp -f --recursive %s %s\n' % (os.path.join(resources,"settingswizard"), db))

        if os.path.exists(os.path.join("/","var","spool", "cron","crontabs", "root")):
            # cronjob backup
            shutil.copy(os.path.join("/","var","spool", "cron","crontabs", "root"),
                        resources)

            # SOF-4279
            # fix pathing for restore of crontabs
            restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join(resources,"root"), os.path.join(live,"var","spool", "cron","crontabs", "root")))

        if deviceType.is_device_type(live, "scale") or deviceType.is_device_type(live, 'ngw'):
            logging.info("backupCriticalFiles: running on a scale or ngw")
            # if DB backup implementation in place
            if dbBackupInPlace:

                # need to export sql db from old db to new format
                # send message to chobart_d to backup DB to HTDF
                sendDeviceEventMessage.sendMessage(server='localhost', port=6000, message="RTDO" + chr(0x1F) + "TNBackupEntireDB" + chr(0x1E))

                server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

                server_address = ('localhost', 5079)
                server.bind(server_address)
                server.listen(5)

                inputs = [server]
                outputs = []

                # timeout waiting from message from chobart_d
                # after 20 minutes
                timeout = 20.0 * 60.0

                # wait for chobart_d to respond with message
                messageReceived = False

                logging.info("backupCriticalFiles: waiting for chobart_d...")
                while not messageReceived:

                    readable, writable, exceptional = select.select(inputs, outputs, inputs, timeout)

                    # check if timeout reached
                    if readable == [] and writable == [] and exceptional == []:
                        # timeout reached so backup of db not possible
                        break
                    else:
                        # received message from chobart_d
                        for s in readable:

                            # socket message coming in to process
                            if s is server:

                                # accept network connection
                                connection, client_address = s.accept()
                                data = connection.recv(1024).decode('utf-8')
                                connection.close()
                                messageReceived = True

                                if data == 'htdfBackupSuccess':
                                    logging.info("backupCriticalFiles: htdfBackupSuccess message received")
                                    if os.path.exists(os.path.join("/","etc","hobart", "verIndData")):
                                        shutil.copy(os.path.join("/","etc","hobart", "verIndData"), resources)
                                        restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join(resources, "verIndData"), os.path.join(live,"etc", "hobart", "verIndData")))
                if not messageReceived:
                    logging.info("backupCriticalFiles: no DB received from chobart_d, no DB backup made!")

                    # close server
                server.close()
                logging.info("backupCriticalFiles: done waiting for chobart_d")

        if os.path.exists(os.path.join("/","etc","timezone")):
            # backup time data
            shutil.copy(os.path.join("/","etc","timezone"),
                        resources)

            restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join(resources, "timezone"), os.path.join(live,"etc", "timezone")))

        if os.path.exists(os.path.join("/","root","ssactive.lock")):
            # self service mode
            shutil.copy(os.path.join("/","root","ssactive.lock"),
                        resources)

            restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join(resources, "ssactive.lock"), os.path.join(live,"root", "ssactive.lock")))

        if os.path.exists(os.path.join("/","usr","local", "hobart", "info.log")):
            # health agent file
            shutil.copy(os.path.join("/","usr","local", "hobart", "info.log"),
                        resources)

            restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join(resources, "info.log"), os.path.join(live,"usr", "local", "hobart", "info.log")))

        # SOF-4279
        # removed part dealing with cronjobs... we are already restoring existing cron above.
        # the proper solution is to restore (i.e. the code above) then merge in new cronjobs

        if os.path.exists(os.path.join("/","usr","local", "hobart", "layouts", "storelogo.png")):
            # backup store logo
            shutil.copy(os.path.join("/","usr","local", "hobart", "layouts", "storelogo.png"),
                        resources)

            restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join(resources, "storelogo.png"), os.path.join(live,"usr", "local", "hobart", "layouts", "storelogo.png")))

        if os.path.exists(os.path.join("/","usr","local", "hobart", "layouts", "background_pick5.png")):
            # backup store logo
            shutil.copy(os.path.join("/","usr","local", "hobart", "layouts", "background_pick5.png"), resources)

            restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join(resources, "background_pick5.png"), os.path.join(live,"usr", "local", "hobart", "layouts", "background_pick5.png")))

        if os.path.exists(os.path.join("/","usr","local", "hobart", "playlists", "playlist.txt")):
            # backup playlist.txt
            shutil.copy(os.path.join("/","usr","local", "hobart", "playlists", "playlist.txt"),
                        resources)

            restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join(resources, "playlist.txt"), os.path.join(live,"usr", "local", "hobart", "playlists", "playlist.txt")))

        cmd = []
        cmd.append('/usr/bin/readlink')
        cmd.append('/etc/localtime')
        exitCode,stdout,stderr = shellCommand.execute(cmd)

        if exitCode == 0:
            # remove current /etc/localtime
            restoreScriptFile.write("/bin/rm -f %s\n" % (os.path.join(live,"etc","localtime")))
            # write link command to relink /etc/localtime to correct timezone
            restoreScriptFile.write('/bin/ln -s %s %s\n' % (os.path.join(live,stdout.strip()),os.path.join(live,"etc","localtime")))

        if os.path.exists(os.path.join("/","etc","hostname")):
            # backup hostname
            shutil.copy(os.path.join("/","etc","hostname"),
                        resources)

            restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join(resources, "hostname"), os.path.join(live,"etc", "hostname")))

        #-----------------
        # Backup sshd keys
        #-----------------
        if os.path.exists(os.path.join("/","etc","openssh","ssh_host_dsa_key")):
            shutil.copy(os.path.join("/","etc","openssh","ssh_host_dsa_key"),resources)
            restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join(resources, "ssh_host_dsa_key"), os.path.join(live,"etc", "openssh","ssh_host_dsa_key")))

        if os.path.exists(os.path.join("/","etc","openssh","ssh_host_dsa_key.pub")):
            shutil.copy(os.path.join("/","etc","openssh","ssh_host_dsa_key.pub"),resources)
            restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join(resources, "ssh_host_dsa_key.pub"), os.path.join(live,"etc", "openssh","ssh_host_dsa_key.pub")))

        if os.path.exists(os.path.join("/","etc","openssh","ssh_host_ecdsa_key")):
            shutil.copy(os.path.join("/","etc","openssh","ssh_host_ecdsa_key"),resources)
            restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join(resources, "ssh_host_ecdsa_key"), os.path.join(live,"etc", "openssh","ssh_host_ecdsa_key")))

        if os.path.exists(os.path.join("/","etc","openssh","ssh_host_ecdsa_key.pub")):
            shutil.copy(os.path.join("/","etc","openssh","ssh_host_ecdsa_key.pub"),resources)
            restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join(resources, "ssh_host_ecdsa_key.pub"), os.path.join(live,"etc", "openssh","ssh_host_ecdsa_key.pub")))

        if os.path.exists(os.path.join("/","etc","openssh","ssh_host_ed25519_key")):
            shutil.copy(os.path.join("/","etc","openssh","ssh_host_ed25519_key"),resources)
            restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join(resources, "ssh_host_ed25519_key"), os.path.join(live,"etc", "openssh","ssh_host_ed25519_key")))

        if os.path.exists(os.path.join("/","etc","openssh","ssh_host_ed25519_key.pub")):
            shutil.copy(os.path.join("/","etc","openssh","ssh_host_ed25519_key.pub"),resources)
            restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join(resources, "ssh_host_ed25519_key.pub"), os.path.join(live,"etc", "openssh","ssh_host_ed25519_key.pub")))

        if os.path.exists(os.path.join("/","etc","openssh","ssh_host_rsa_key")):
            shutil.copy(os.path.join("/","etc","openssh","ssh_host_rsa_key"),resources)
            restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join(resources, "ssh_host_rsa_key"), os.path.join(live,"etc", "openssh","ssh_host_rsa_key")))

        if os.path.exists(os.path.join("/","etc","openssh","ssh_host_rsa_key.pub")):
            shutil.copy(os.path.join("/","etc","openssh","ssh_host_rsa_key.pub"),resources)
            restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join(resources, "ssh_host_rsa_key.pub"), os.path.join(live,"etc", "openssh","ssh_host_rsa_key.pub")))

        if ((os.path.exists(os.path.join("/","etc","passwd"))) and (os.path.exists(os.path.join("/","etc","shadow")))):
            # backup passwords
            shutil.copy(os.path.join("/","etc","passwd"),resources)
            shutil.copy(os.path.join("/","etc","shadow"),resources)

            restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join(resources, "passwd"), os.path.join(live,"etc", "passwd")))
            restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join(resources, "shadow"), os.path.join(live,"etc", "shadow")))

        if (os.path.exists(os.path.join("/","usr","local", "hobart","hobart.key"))):
            # backup password of user hobart
            shutil.copy(os.path.join("/","usr","local", "hobart","hobart.key"),resources)

            restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join(resources, "hobart.key"), os.path.join(liveHobart,"hobart.key")))

        # copy language file if it exists
        if os.path.exists(os.path.join("/","usr","local", "hobart","lang")):
            # backup language file
            shutil.copy(os.path.join("/","usr","local", "hobart","lang"),
                        resources)

            restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join(resources, "lang"), os.path.join(liveHobart,"lang")))

        # if this is a slicer
        if deviceType.is_device_type(live, 'slicer'):
            logging.info("backupCriticalFiles: running on a slicer")

            if os.path.isdir(os.path.join(liveHobart, "slicer")):

                # Save entire /usr/local/hobart/slicer directory
                shutil.copytree(os.path.join(liveHobart, "slicer"),
                                os.path.join(resources, "slicer"),
                                symlinks=False,
                                ignore=None)

                restoreScriptFile.write(
                    '/bin/cp -f --recursive %s %s\n' % (os.path.join(resources, "slicer"), liveHobart))

            osVersion = osVersionMajorMinor()

            # if os version of linux is less than or equal to 3.14
            if osVersion is not None and osVersion <= 3.14:

                # need to update xorg.conf on recovery partition so upgrade will work for a slicer

                if os.path.exists(os.path.join(RECOVERY_PART, "etc", "X11", "xorg.conf")):
                    # remove default recovery partition xorg.conf and rewrite it with PSS version
                    os.remove(os.path.join(RECOVERY_PART, "etc", "X11", "xorg.conf"))

                # install PSS X11 portrait display X11 config in recovery partition since it is not a part of recovery kernel RFS
                shutil.copy(os.path.join(live, "etc", "X11", "xorg.conf-slicer"),
                            os.path.join(RECOVERY_PART, "etc", "X11", "xorg.conf"))

                # install PSS X11 portrait display X11 config in recovery partition resources directory so it can be later copied to live partition
                shutil.copy(os.path.join(live, "etc", "X11", "xorg.conf-slicer"), resources)

                # after live partition is created cp PSS X11 portrait X11 config to the xorg.conf file that X11 reads
                restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join(resources, "xorg.conf-slicer"), os.path.join(live, "etc", "X11", "xorg.conf")))

            # create /etc/hobart/slicer file on recovery partition if it does not exist
            if not os.path.exists(os.path.join(RECOVERY_PART, "etc", "hobart", "slicer")):
                if not os.path.exists(os.path.join(RECOVERY_PART,"etc","hobart")):
                    os.mkdir(os.path.join(RECOVERY_PART,"etc","hobart"))
                os.system('/bin/touch %s\n' % (os.path.join(RECOVERY_PART, "etc", "hobart", "slicer")))

        # copy lang file to recovery partition
        if os.path.exists( os.path.join("/","media",RECOVERY_PART,"usr","local","hobart") ):
            shutil.copy( os.path.join("/","usr","local","hobart","lang"), os.path.join("/","media",RECOVERY_PART,"usr","local","hobart","lang") )

        # backup configc.dat
        if os.path.exists(os.path.join("/","etc","hobart", "configc.dat")):
            shutil.copy(os.path.join("/","etc","hobart", "configc.dat"), resources)

            restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join(resources, "configc.dat"), os.path.join(live,"etc", "hobart", "configc.dat")))

        # HT-442: Backup scale-type.dat
        if os.path.exists(os.path.join("/","etc","hobart", "scale-type.dat")):
            shutil.copy(os.path.join("/","etc","hobart", "scale-type.dat"), resources)

            restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join(resources, "scale-type.dat"), os.path.join(live,"etc", "hobart", "scale-type.dat")))
            
        # backup historically referenced actuators timings file, actuatorTimings.log
        if os.path.exists(os.path.join("/", "usr", "local", "hobart", "actuatorTimings.log")):
            shutil.copy(os.path.join("/", "usr", "local", "hobart", "actuatorTimings.log"),resources)

            restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join(resources, "actuatorTimings.log"), os.path.join(live, "usr", "local", "hobart", "actuatorTimings.log")))

        # save network interfaces
        if os.path.isdir(os.path.join("/","etc","network")):
            # Save entire /etc/network directory
            shutil.copytree(os.path.join("/","etc","network"),os.path.join(resources,"network"), symlinks=False, ignore=None)
            restoreScriptFile.write('/bin/cp -f --recursive %s %s\n' % (os.path.join(resources, "network"), os.path.join(live, "etc")))

        if os.path.exists(os.path.join("/","etc","resolv.conf")):
            # save DNS network settings
            shutil.copy(os.path.join("/","etc","resolv.conf"),
                        resources)

        restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join(resources, "resolv.conf"), os.path.join(live,"etc", "resolv.conf")))

        # Save touch calibration (BCM)
        if os.path.exists(os.path.join("/","etc","eGTouch0.param")):
            # save display calibration data
            shutil.copy(os.path.join("/","etc","eGTouch0.param"), resources)
            restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join(resources, "eGTouch0.param"), os.path.join(live,"etc", "eGTouch0.param")))

        # Save touch calibration (Boundary)
        if os.path.exists(os.path.join("/","etc","pointercal")):
            # save display calibration data
            shutil.copy(os.path.join("/","etc","pointercal"), resources)
            restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join(resources, "pointercal"), os.path.join(live,"etc", "pointercal")))

        # Save BB configuration
        if deviceType.is_device_type(live, 'blackbox'):
            restoreScriptFile.write( '/bin/touch %s\n' % (os.path.join( live, "etc", "hobart", "blackbox" )) )

        if deviceType.is_device_type(live, 'coronaPsu'):
            restoreScriptFile.write( '/bin/touch %s\n' % (os.path.join( live, "etc", "hobart", "coronaPsu" )) )

        # unique to Portion Scale Slicer
        if deviceType.is_device_type(live, 'slicer'):
            restoreScriptFile.write( '/bin/touch %s\n' % (os.path.join( live, "etc", "hobart", "slicer" )) )

        if os.path.exists( os.path.join( "/", "etc", "hobart", "engdebug" ) ):
            restoreScriptFile.write( '/bin/touch %s\n' % (os.path.join( live, "etc", "hobart", "engdebug" )) )

        if deviceType.is_device_type(live, 'hdmiin'):
            restoreScriptFile.write( '/bin/touch %s\n' % (os.path.join( live, "etc", "hobart", "hdmiin" )) )

        if deviceType.is_device_type(live, 'telit'):
            restoreScriptFile.write( '/bin/touch %s\n' % (os.path.join( live, "etc", "hobart", "telit" )) )

            # backup current telit software
            os.system( "/usr/bin/tar -cvzf %s -C /opt dw" % (os.path.join( resources, "dw.tar.gz" )) )

            # restore telit software after upgrade (preinstall process will extract it)
            restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join( resources, "dw.tar.gz" ), os.path.join( live, "usr", "local", "hobart", "preinstall" )) )

        if deviceType.is_device_type(live, "testsrv"):
            restoreScriptFile.write( '/bin/touch %s\n' % (os.path.join( live, "etc", "hobart", "testsrv" )) )

            # backup server config
            shutil.copy( os.path.join( "/", "usr", "local", "hobart", "scale_information.sqlite" ), resources )
            shutil.copy( os.path.join( "/", "usr", "local", "hobart", "backend", "mac_address.dat" ), resources )

            # restore server config after upgrade
            restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join( resources, "scale_information.sqlite" ), os.path.join( live, "usr", "local", "hobart", "scale_information.sqlite" )) )
            restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join( resources, "mac_address.dat" ), os.path.join( live, "usr", "local", "hobart", "backend", "mac_address.dat" )) )
            
        # online ordering database backup
        if os.path.exists(os.path.join("/","var","mysql", "online_ordering")):
            os.system("mysqldump --user=hobart --password=hobart --databases online_ordering > /var/mysql/online_ordering/online_ordering_dump.sql")
            shutil.copytree(os.path.join("/","var","mysql"), resources)
            
            restoreScriptFile.write('/bin/cp -r %s %s\n' % (os.path.join(resources, "mysql"), os.path.join(live, "var")))

        # create upgrade boot file for startup sequence on subsequent reboot so scale settings before
        # upgrade are used versus setting default time based on scale epoch
        restoreScriptFile.write("/bin/touch %s\n" % (os.path.join(live,"etc","upgradeBoot")))

        # copy wireless settings if configured
        if os.path.exists(os.path.join("/","etc","wpa_supplicant.conf")):
            shutil.copy(os.path.join("/","etc","wpa_supplicant.conf"),
                        resources)
            restoreScriptFile.write('/bin/cp -f %s %s\n' % (os.path.join(resources, "wpa_supplicant.conf"), os.path.join(live,"etc", "wpa_supplicant.conf")))

        # if a monolithic_core startupSequence.py exists copy it to resources so it can be restored to live for subsequent upgrade.
        local_path = os.path.join("/", "usr", "local", "hobart", "features", "monolithic_core", "startupSequence.py")
        resources_path = os.path.join(resources, 'startupSequence.py')
        if os.path.exists(local_path):
            if os.path.exists(resources_path):
                os.remove(resources_path)

            shutil.copy(local_path, resources_path)

            restoreScriptFile.write('/bin/cp -f %s %s\n' % (resources_path, os.path.join(live, 'usr', 'local', 'hobart', 'python', 'startupSequence.py')))

        # close restore script
        restoreScriptFile.close()

        # make script executable
        os.system("/bin/chmod ugo+rx %s" % (restoreScript))

    elif os.name == 'nt':
        # backup the database, use wildcard for last character since db is evolving
        matches = glob.glob(os.path.join(db,"scale","dbfiles","dbft?.db"))
        if len(matches) > 0:
            for par, dir, files in os.walk(dbRestore):
                for myfile in files:
                    os.chmod(os.path.join(par, myfile), stat.S_IWRITE)
            shutil.copy(matches[0], dbRestore)
            restoreScriptFile.write('xcopy /y /r \"%s\" \"%s\"\n' % (os.path.join(dbRestore,os.path.basename(matches[0])), os.path.join(db, "scale","dbfiles")))

        if os.path.exists(os.path.join(db,"settingswizard","scalesettings")):
            # backup scale settings
            shutil.copyfile(os.path.join(db,"settingswizard","scalesettings"),
                        os.path.join(resources,"scalesettings"))

            restoreScriptFile.write('xcopy /y /r \"%s\" \"%s\"\n' % (os.path.join(resources,"scalesettings"), os.path.join(db,"settingswizard","scalesettings")))

        if os.path.exists(os.path.join(db,"settingswizard","lastNetworkRun.txt")):
            # backup scale settings
            shutil.copyfile(os.path.join(db,"settingswizard","lastNetworkRun.txt"),
                        os.path.join(resources,"lastNetworkRun.txt"))
            restoreScriptFile.write('xcopy /y /r \"%s\" \"%s\"\n' % (os.path.join(resources,"lastNetworkRun.txt"), os.path.join(db,"settingswizard","lastNetworkRun.txt")))

        # Save/restore Network Settings

        if os.path.exists(os.path.join(db,"settingswizard","networkSettings.cfg")):
            # backup scale settings
            shutil.copyfile(os.path.join(db,"settingswizard","networkSettings.cfg"),
                        os.path.join(resources,"networkSettings.cfg"))

            restoreScriptFile.write('xcopy /y /r \"%s\" \"%s\"\n' % (os.path.join(resources,"networkSettings.cfg"), os.path.join(db,"settingswizard","networkSettings.cfg")))

        if os.path.exists(os.path.join(db,"settingswizard","EAPUserName.xml")):
            # backup network settings
            shutil.copyfile(os.path.join(db,"settingswizard","EAPUserName.xml"),
                        os.path.join(resources,"EAPUserName.xml"))

            restoreScriptFile.write('xcopy /y /r \"%s\" \"%s\"\n' % (os.path.join(resources,"EAPUserName.xml"), os.path.join(db,"settingswizard","EAPUserName.xml")))

        if os.path.exists(os.path.join(db,"settingswizard","WirelessProfile.xml")):
            # backup network settings
            shutil.copyfile(os.path.join(db,"settingswizard","WirelessProfile.xml"),
                        os.path.join(resources,"WirelessProfile.xml"))

            restoreScriptFile.write('xcopy /y /r \"%s\" \"%s\"\n' % (os.path.join(resources,"WirelessProfile.xml"), os.path.join(db,"settingswizard","WirelessProfile.xml")))

        # Get timezone and save to restoreScriptFile
        cmd = []
        cmd.append("tzutil")
        cmd.append("/g")

        exitCode, stdout, stderr = shellCommand.execute(cmd)

        if (exitCode == 0):
            restoreScriptFile.write('tzutil /s \"%s\"\n' % stdout)

        infologpath = os.path.join(configInfo.getRootFolder(), "log")
        if os.path.exists(os.path.join(infologpath, "info.log")):
            # health agent file
            shutil.copyfile(os.path.join(infologpath, "info.log"),
                        os.path.join(resources,"info.log"))

            restoreScriptFile.write('xcopy /y /r \"%s\" \"%s\"\n' % (os.path.join(resources, "info.log"), os.path.join(infologpath, "info.log")))

        saveRestoreCronJobs(restoreScriptFile, resources)

        if os.path.exists(os.path.join(configInfo.getRootFolder(), "layouts", "storelogo.png")):
            # backup store logo
            shutil.copyfile(os.path.join(configInfo.getRootFolder(), "layouts", "storelogo.png"),
                        os.path.join(resources,"storelogo.png"))

            restoreScriptFile.write('xcopy /y /r \"%s\" \"%s\"\n' % (os.path.join(resources, "storelogo.png"), os.path.join(configInfo.getRootFolder(), "layouts", "storelogo.png")))

        if os.path.exists(os.path.join(configInfo.getRootFolder(), "layouts", "background_pick5.png")):
            # backup store logo
            shutil.copyfile(os.path.join(configInfo.getRootFolder(), "layouts", "background_pick5.png"),
                        os.path.join(resources,"background_pick5.png"))

            restoreScriptFile.write('xcopy /y /r \"%s\" \"%s\"\n' % (os.path.join(resources, "background_pick5.png"), os.path.join(configInfo.getRootFolder(), "layouts", "background_pick5.png")))

        if os.path.exists(os.path.join(configInfo.getRootFolder(), "playlists", "playlist.txt")):
            # backup playlist.txt
            shutil.copyfile(os.path.join(configInfo.getRootFolder(), "playlists", "playlist.txt"),
                        os.path.join(resources,"playlist.txt"))

            restoreScriptFile.write('xcopy /y /r \"%s\" \"%s\"\n' % (os.path.join(resources, "playlist.txt"), os.path.join(configInfo.getRootFolder(), "playlists", "playlist.txt")))

        import getHostname
        exitCode, hostname = getHostname.getHostname()
        restoreScriptFile.write('c:\\Python27\python \"%s\" %s\n' % (os.path.join(configInfo.getRootFolder(), 'python', "setHostname.py"), hostname))

        if os.path.exists(os.path.join(configInfo.getRootFolder(),"hobsshd.rsa")):
            # backup sshd keys
            shutil.copyfile(os.path.join(configInfo.getRootFolder(),"hobsshd.rsa"),os.path.join(resources, "hobsshd.rsa"))

            restoreScriptFile.write('xcopy /y /r \"%s\" \"%s\"\n' % (os.path.join(resources, "hobsshd.rsa"), os.path.join(configInfo.getRootFolder())))

        # copy language file if it exists
        if os.path.exists(os.path.join(configInfo.getRootFolder(),"lang")):
            # backup language file
            shutil.copyfile(os.path.join(configInfo.getRootFolder(),"lang"),
                        os.path.join(resources, "lang"))

            restoreScriptFile.write('xcopy /y /r \"%s\" \"%s\"\n' % (os.path.join(resources, "lang"), os.path.join(configInfo.getRootFolder(),"lang")))

        if os.path.exists(os.path.join(configInfo.getRootFolder(), "configc.dat")):
            # backup configc.dat
            shutil.copyfile(os.path.join(configInfo.getRootFolder(), "configc.dat"),
                        os.path.join(resources, "configc.dat"))

            restoreScriptFile.write('xcopy /y /r \"%s\" \"%s\"\n' % (os.path.join(resources, "configc.dat"), os.path.join(configInfo.getRootFolder(), "configc.dat")))

        # HT-442: Backup scale-type.dat
        if os.path.exists(os.path.join(configInfo.getRootFolder(), "scale-type.dat")):
            shutil.copyfile(os.path.join(configInfo.getRootFolder(), "scale-type.dat"),
                        os.path.join(resources, "scale-type.dat"))

            restoreScriptFile.write('xcopy /y /r \"%s\" \"%s\"\n' % (os.path.join(resources, "scale-type.dat"), os.path.join(configInfo.getRootFolder(), "scale-type.dat")))

        # TODO - Verify with BCD this is where the calibration info is saved
        cmd = []
        cmd.append("reg")
        cmd.append("query")
        cmd.append("HKLM\\System\\CurrentControlSet\\services\\xtouch\\DeviceDefault")
        cmd.append("/v")
        cmd.append("CalInfo")

        exitCode, stdout, stderr = shellCommand.execute(cmd)

        # Extract the binary data from the output for reg query
        if exitCode == 0:
            tokens = stdout.split('REG_BINARY')
            if len(tokens) > 1:
                strBinaryData = tokens[1]
                strBinaryData = strBinaryData.strip()

                restoreScriptFile.write('reg add HKLM\\System\\CurrentControlSet\\services\\xtouch\\DeviceDefault /v CalInfo /f /t REG_BINARY /d %s\n' % strBinaryData)

        # create upgrade boot file for startup sequence on subsequent reboot so scale settings before
        # upgrade are used versus setting default time based on scale epoch
        restoreScriptFile.write('echo \"Upgrade\" > c:\\upgradeBoot')
        restoreScriptFile.write('del firstBoot')

        # restoreScriptFile.write("/bin/touch %s\n" % (os.path.join(live,"etc","upgradeBoot")))

        # close restore script
        restoreScriptFile.close()

    messageBox.destroy(mbId)

    if language == 'spanish':
        message = "Actualización de Aplicación y SO (Etapa 1)\n\nInicializando Etapa 2..."
    elif language == 'french':
        message = "Mise á niveau de l\'application et du SE (étape 1)\n\nInitialisation étape 2..."
    else:
        message = "Application and OS Upgrade (Stage 1)\n\nInitiating Stage 2..."

    logging.info("backupCriticalFiles: %s" % message)
    exitCode,  mbId = show_message(message)

    if os.name == 'posix':
        os.system("/bin/sync")

    logging.info("backupCriticalFiles: done (%s)" % exitCode)
    return exitCode


def enoughFreeSpaceToInstall(upgradeSize, partition):
    # get free space on partition
    partitionAvailableSpace = fileSysStatistics.availableSpace(partition)

    if upgradeSize >= partitionAvailableSpace:
        logging.info("enoughFreeSpaceToInstall: no")
        return (False, partitionAvailableSpace)
    else:
        logging.info("enoughFreeSpaceToInstall: yes")
        return (True, partitionAvailableSpace)


def installUpgradeNonTerminating(upgradeFilename, serverHost=None,  username=None,  password=None,  uploadFolder=None, removeUpgrade=False):
    logging.info("installUpgradeNonTerminating: start")
    if os.name == 'posix':

        dbBackupInPlace = deviceType.is_device_type("/", "scale") or deviceType.is_device_type("/", "ngw")

    elif os.name == 'nt':
        dbBackupInPlace = False

    # if specified upgrade filename exists
    if os.path.isfile(upgradeFilename):

        # get current scale language
        language = languageSettings.currentLanguage()

        regexPattern = '^.*%s$'       # HTA-91: Accept anything up to our filename string

        CORE_UPGRADE_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.CORE_UPGRADE_FILENAME_REGEX))
        APPLICATION_UPGRADE_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.APPLICATION_UPGRADE_FILENAME_REGEX))
        BOOTLOADER_UPGRADE_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.BOOTLOADER_UPGRADE_FILENAME_REGEX))
        RECOVERY_UPGRADE_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.RECOVERY_UPGRADE_FILENAME_REGEX))
        LANGUAGE_UPGRADE_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.LANGUAGE_UPGRADE_FILENAME_REGEX))
        FPC_UPGRADE_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.FPC_UPGRADE_FILENAME_REGEX))
        AWS_UPGRADE_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.AWS_UPGRADE_FILENAME_REGEX))
        AWS3_UPGRADE_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.AWS3_UPGRADE_FILENAME_REGEX))
        NGW_UPGRADE_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.NGW_UPGRADE_FILENAME_REGEX))
        NGW2P_UPGRADE_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.NGW2P_UPGRADE_FILENAME_REGEX))
        NGS_UPGRADE_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.NGS_UPGRADE_FILENAME_REGEX))
        EWS_UPGRADE_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.EWS_UPGRADE_FILENAME_REGEX))
        CLA_UPGRADE_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.CLA_UPGRADE_FILENAME_REGEX))
        XPE_UPGRADE_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.XPE_UPGRADE_FILENAME_REGEX))
        EVO_UPGRADE_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.EVO_UPGRADE_FILENAME_REGEX))
        WGH_UPGRADE_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.WGH_UPGRADE_FILENAME_REGEX))
        FEATURE_UPGRADE_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.FEATURE_UPGRADE_FILENAME_REGEX))
        LICENSE_INSTALL_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.LICENSE_INSTALL_FILENAME_REGEX))

        PATCH_UPGRADE_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.PATCH_UPGRADE_FILENAME_REGEX))
        BACKEND_UPGRADE_FILENAME_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.BACKEND_UPGRADE_FILENAME_REGEX))
        BACKEND_UPGRADE_NEW_FILENAME_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.BACKEND_UPGRADE_NEW_FILENAME_REGEX))

        CORE_UPGRADE_OLD_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.CORE_UPGRADE_OLD_FILENAME_REGEX))
        BOOTLOADER_UPGRADE_OLD_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.BOOTLOADER_UPGRADE_OLD_FILENAME_REGEX))
        RECOVERY_UPGRADE_OLD_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.RECOVERY_UPGRADE_OLD_FILENAME_REGEX))
        LANGUAGE_UPGRADE_OLD_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.LANGUAGE_UPGRADE_OLD_FILENAME_REGEX))
        FPC_UPGRADE_OLD_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.FPC_UPGRADE_OLD_FILENAME_REGEX))
        AWS_UPGRADE_OLD_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.AWS_UPGRADE_OLD_FILENAME_REGEX))
        AWS3_UPGRADE_OLD_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.AWS3_UPGRADE_OLD_FILENAME_REGEX))
        EWS_UPGRADE_OLD_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.EWS_UPGRADE_OLD_FILENAME_REGEX))
        CLA_UPGRADE_OLD_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.CLA_UPGRADE_OLD_FILENAME_REGEX))
        XPE_UPGRADE_OLD_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.XPE_UPGRADE_OLD_FILENAME_REGEX))
        EVO_UPGRADE_OLD_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.EVO_UPGRADE_OLD_FILENAME_REGEX))
        WGH_UPGRADE_OLD_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.WGH_UPGRADE_OLD_FILENAME_REGEX))
        FEATURE_UPGRADE_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.FEATURE_UPGRADE_FILENAME_REGEX))
        FEATURE_UPGRADE_OLD_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.FEATURE_UPGRADE_OLD_FILENAME_REGEX))
        FEATURE_UPGRADE_OSAGNOSTIC_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.FEATURE_UPGRADE_OSAGNOSTIC_FILENAME_REGEX))
        MONOLITHIC_UPGRADE_REGEX_COMPILED = re.compile(regexPattern % (installFilePatterns.MONOLITHIC_UPGRADE_FILENAME_REGEX))
        HTE_MULTIMEDIA_FILENAME_COMPILED = re.compile(regexPattern % (installFilePatterns.HTE_MULTIMEDIA_FILENAME_REGEX))

        KPC_BOOT_UPGRADE_FILENAME_COMPILED = re.compile(regexPattern % (installFilePatterns.KPC_BOOT_UPGRADE_FILENAME_REGEX))
        FSSS_BOOT_UPGRADE_FILENAME_COMPILED = re.compile(regexPattern % (installFilePatterns.FSSS_BOOT_UPGRADE_FILENAME_REGEX))
        HFPD_UPGRADE_FILENAME_COMPILED = re.compile(regexPattern % (installFilePatterns.HFPD_UPGRADE_FILENAME_REGEX) )

        if os.name == 'posix':
            recoveryPartition = os.path.join("/","media",RECOVERY_PART)
            live = os.path.join("/","media",LIVE_PART)

            recovery = os.path.join(recoveryPartition,"usr","local","hobart")
            resources = os.path.join(recovery,"resources")
            restoreScript = os.path.join(resources,"restore.sh")
            scaleAppRestore = os.path.join(recovery,"scaleApp")
            coreTGZ = "hte.tar.gz"

            liveHobart = os.path.join(live,"usr","local","hobart")
            db = os.path.join(liveHobart, "db")
            dbRestore = os.path.join(recovery,"db")
        elif os.name == 'nt':
            recoveryPartition = os.path.join("R:\\")
            recovery = os.path.join(recoveryPartition,"Program Files","Hobart")
            resources = os.path.join(recovery,"resources")
            restoreScript = os.path.join(resources,"restore.cmd")
            scaleAppRestore = os.path.join(recovery,"scaleApp")
            coreTGZ = "hte.tgz"

            live = os.path.join("c:\\")
            liveHobart = os.path.join(live,"Program Files","Hobart")
            db = os.path.join(liveHobart, "db")
            dbRestore = os.path.join(recovery,"db")

            # Create key directories in recovery partition, if they do not already exist
            if not os.path.exists(recovery):
                os.makedirs(recovery)

            if not os.path.exists(resources):
                os.makedirs(resources)

            if not os.path.exists(dbRestore):
                os.makedirs(dbRestore)

            if not os.path.exists(os.path.join(recovery,'bios')):
                os.makedirs(os.path.join(recovery,'bios'))
                
        logging.info("installUpgradeNonTerminating: got files")

        # check filename to see if we need to check checksum of file
        fileComponents = os.path.splitext(os.path.basename(upgradeFilename))
        packagePath = ''
        # make sure there is a basename and extension in filename
        if len(fileComponents) == 2:

            # check for feature package
            if '.package.tgz' in upgradeFilename:

                # create folder for package extraction
                packagePath = upgradeFilename.replace('.package.tgz','')
                try:
                    if os.path.isdir(packagePath):
                        shutil.rmtree(packagePath)
                    os.makedirs(packagePath)
                except Exception as e:
                    packagePath = ''
                    logging.info("installUpgradeNonTerminating: failed to create extraction folder")
                    logging.exception(e)
                    pass

                # extract package
                if 0 < len(packagePath) and os.path.isdir(packagePath):
                    if language == 'spanish':
                        message = "Desembalaje de archivo ..."
                    elif language == 'french':
                        message = "Déballage fichier ..."
                    else:
                        message = "Unpacking file..."

                    exitCode,  mbId1 = show_message(message=message)
                    logging.info("installUpgradeNonTerminating: %s" % message)

                    try:
                        tar = tarfile.open(upgradeFilename, 'r')
                        tar.extractall(packagePath)
                        logging.info("installUpgradeNonTerminating: tarball extracted")
                    except Exception as e:
                        logging.info("installUpgradeNonTerminating: failed to extract tarball!")
                        logging.exception(e)
                        pass

                    messageBox.destroy(mbId1)

                    path, filename = os.path.split(upgradeFilename)
                    filename = os.path.join(packagePath, filename.replace('.package.tgz','.tgz'))

                    # new upgrade filename is extracted upgrade filename
                    if os.path.exists(filename):
                        upgradeFilename = filename

            logging.info("installUpgradeNonTerminating: checking file...")

            # if a license file is not being installed must check md5 of file
            if fileComponents[1] != '.lic':
                allowMd5Checksum = False

                # allow md5 checksum for hte multimedia exports only
                if HTE_MULTIMEDIA_FILENAME_COMPILED.match(upgradeFilename):
                    allowMd5Checksum = True

                # allow md5 checksum for first boots
                if os.name == 'posix':
                    if not os.path.exists(os.path.join("/","etc","firstBoot")):
                        allowMd5Checksum = True

                if language == 'spanish':
                    message = "Comprobación de integridad del archivo de actualización ..."
                elif language == 'french':
                    message = "Vérification de l'intégrité du fichier de mise á jour ..."
                else:
                    message = "Checking integrity of upgrade file..."

                logging.info("installUpgradeNonTerminating: %s" % message)
                exitCode,  mbId1 = show_message(message=message)

                # file is signed with certificate
                if os.path.exists(upgradeFilename+'.crt') and os.path.exists(upgradeFilename+'.signed'):
                    logging.info("installUpgradeNonTerminating: verifying signed/cert")
                    validateScript = ''
                    exitCode = -1

                    if os.name == 'posix':
                        validateScript = os.path.join(configInfo.getRootFolder(), "python","verifyFeatureInstaller.sh")
                    elif os.name == 'nt':
                        validateScript = os.path.join(configInfo.getRootFolder(), "sigcerttool.exe")

                    # invoke validation script
                    if 0 < len(validateScript) and os.path.exists(validateScript):
                        cmd = []
                        cmd.append(validateScript)
                        cmd.append(upgradeFilename)

                        exitCode, stdout, stderr = shellCommand.execute(cmd)

                    print(exitCode)

                    # check expired certificate
                    if exitCode == 3 or exitCode == 4 or exitCode == 5:
                        messageBox.destroy(mbId1)

                        if language == 'spanish':
                            message = "El firmware contiene un certificado caducado o no válido. \n\n La instalación se canceló"
                        elif language == 'french':
                            message = "Le micrologiciel contient un certificat expiré ou non valide. \n\n L'installation a été abandonnée."
                        else:
                            message = "Firmware contains and expired or invalid certificate.\n\nInstallation aborted."

                        exitCode,  mbId = show_message(message=message, timer=5)

                        logging.info("installUpgradeNonTerminating: %s" % message)
                        return

                    # check validation result
                    elif exitCode != 0:
                        messageBox.destroy(mbId1)

                        if language == 'spanish':
                            message = "Archivo de actualización de firmware está dañado. \n\n La instalación no coincide la suma de comprobación abortado."
                        elif language == 'french':
                            message = "Fichier de mise á jour du micrologiciel est corrompu. \n\n L'installation contrôle d'incompatibilité annulée."
                        else:
                            message = "Firmware update file is corrupt. Checksum mismatch installation aborted."

                        logging.info("installUpgradeNonTerminating: %s" % message)
                        exitCode,  mbId = show_message(message=message, timer=5)

                        return

                # file has MD5 checksum attached
                elif allowMd5Checksum and md5checksum.md5FilenameFromInstallerFilenameExists(upgradeFilename):
                    if not md5checksum.md5ValidBasedOnChecksumFile(upgradeFilename):

                        messageBox.destroy(mbId1)

                        if language == 'spanish':
                            message = "Archivo de actualización de firmware está dañado, la instalación \n\n no coincide la suma de comprobación abortado."
                        elif language == 'french':
                            message = "Fichier de mise á jour du micrologiciel est corrompu, l'installation \n\n contrôle d'incompatibilité annulée."
                        else:
                            message = "Firmware update file is corrupt, checksum mismatch installation aborted."

                        logging.info("installUpgradeNonTerminating: %s" % message)
                        exitCode,  mbId = show_message(message=message, timer=5)

                        cleanupPackageUpgradeSource(removeUpgrade, upgradeFilename)
                        return

                # no checksum
                else:
                    messageBox.destroy(mbId1)

                    if language == 'spanish':
                        message = "Archivo Checksum para la actualización de firmware no se encuentra, la instalación abortada."
                    elif language == 'french':
                        message = "Fichier checksum pour la mise á jour du firmware n'est pas trouvé, l'installation abandonnée."
                    else:
                        message = "Checksum file for firmware update not found, installation aborted."

                    logging.info("installUpgradeNonTerminating: %s" % message)
                    exitCode,  mbId = show_message(message=message,timer=5)

                    cleanupPackageUpgradeSource(removeUpgrade, upgradeFilename)
                    return

                messageBox.destroy(mbId1)

        else:
            if language == 'spanish':
                message = "Nombre del archivo de firmware no tiene extensión, instalación abortada."
            elif language == 'french':
                message = "Firmware nom de fichier n'a pas d'extension, l'installation avortée."
            else:
                message = "Firmware filename has no extension, installation aborted."
                
            logging.info("installUpgradeNonTerminating: %s" % message)
            exitCode,  mbId = show_message(message=message, timer=5)
            cleanupPackageUpgradeSource(removeUpgrade, upgradeFilename)
            return

        # if upgrade file is core a complete scale app/OS upgrade then
        if CORE_UPGRADE_REGEX_COMPILED.match(upgradeFilename) or CORE_UPGRADE_OLD_REGEX_COMPILED.match(upgradeFilename) or PATCH_UPGRADE_REGEX_COMPILED.match(upgradeFilename):
            logging.info("installUpgradeNonTerminating: core/app/OS upgrade file")
            isPatchUpgrade = False

            if PATCH_UPGRADE_REGEX_COMPILED.match(upgradeFilename):
                isPatchUpgrade = True

            # Remove any stale upgrade tarballs from the recovery partition before doing our size calculations.
            if os.path.exists(os.path.join(scaleAppRestore, coreTGZ)):
                os.remove(os.path.join(scaleAppRestore, coreTGZ))

            # get size of resources directory we are about to remove
            resourcesDirSize = fileSysStatistics.directorySize(resources)

            # get size of last upgrade recovery tarball in recovery partition that we are about to remove
            recoveryTarballSize = fileSysStatistics.fileSize(os.path.join(scaleAppRestore, coreTGZ))

            # get size of tarball we are about to extract and install
            upgradeFileSize = fileSysStatistics.fileSize(upgradeFilename)

            # get free space on recovery partition
            recoveryPartitionAvailableSpace = fileSysStatistics.availableSpace(recoveryPartition)

            # get the used space on the recovery partition
            recoveryPartitionUsedSpace = fileSysStatistics.usedSpace(recoveryPartition)

            # get the live /usr/local/hobart directory size since it is not removed when this upgrade occurs
            # which means that when the upgrade is extracted the contents of this directory is retained
            # with potentially overwrites of files based on what is in tarball for application/OS upgrade
            liveHobartDirSize = fileSysStatistics.directorySize(liveHobart)

            # get the live partition size
            livePartitionTotalSize = fileSysStatistics.totalSize(live)

            # TODO:
            # I am estimating that the worst case HTDF file backup of a DB is 50 MB
            # is that enough?
            # db backup HTDF file worst case size 50 MB
            dbBackupHtdfWorstCaseFileSize = 50 * 1024 * 1024

            print(('bytes needed: ' + str(upgradeFileSize + dbBackupHtdfWorstCaseFileSize )))
            print(('bytes available: ' + str(recoveryPartitionAvailableSpace + resourcesDirSize + recoveryTarballSize)))

            # TODO:
            # Assume worst case tar ball extract size is 2 times the tarball size compressed
            # May not be the greatest assumption
            # check if recovery partition has enough free space
            enoughSpace, partitionFreeSpace = enoughFreeSpaceToInstall(upgradeFileSize + dbBackupHtdfWorstCaseFileSize, recoveryPartition)

            if not enoughSpace:
                logging.info("installUpgradeNonTerminating: not enough space!")
                displayNotEnoughSpaceMessage(language=language,
                                             whichPartition='recovery',
                                             availableSpace=partitionFreeSpace + resourcesDirSize + recoveryTarballSize,
                                             neededSpace=upgradeFileSize + dbBackupHtdfWorstCaseFileSize)
                return 1

            print(('partition needed: ' + str((upgradeFileSize * 2) + liveHobartDirSize + dbBackupHtdfWorstCaseFileSize)))
            print(('partition available: ' + str(livePartitionTotalSize)))

            enoughSpace = (upgradeFileSize * 2) + liveHobartDirSize + dbBackupHtdfWorstCaseFileSize < livePartitionTotalSize
            partitionFreeSpace = livePartitionTotalSize

            if not enoughSpace:
                displayNotEnoughSpaceMessage(language=language,
                                             whichPartition='live',
                                             availableSpace=partitionFreeSpace,
                                             neededSpace=((upgradeFileSize * 2) + liveHobartDirSize + dbBackupHtdfWorstCaseFileSize))
                logging.info("installUpgradeNonTerminating: not enough space!")
                return 1

            if language == 'spanish':
                message = "Actualización de Aplicación y SO (Etapa 1)\n\nInicializando..."
            elif language == 'french':
                message = "Mise á niveau de l\'application et du SE (étape 1)\n\nInitialisation..."
            else:
                message = "Application and OS Upgrade (Stage 1)\n\nInitiating..."

            logging.info("installUpgradeNonTerminating: %s" % message)
            exitCode,  mbId = show_message(message)

            backupCriticalFiles(upgradeFilename, resources, restoreScript, scaleAppRestore, mbId,
                                language, db, dbBackupInPlace, live, liveHobart, isPatchUpgrade, removeUpgrade, dbRestore, coreTGZ, True)
            # if told to remove the upgrade
            cleanupPackageUpgradeSource(removeUpgrade, upgradeFilename)

            # stage the upgrade
            if os.name == 'posix':
                cmd = []
                cmd.append(os.path.join("/", "etc","init.d","stageRecovery.sh"))
                if isPatchUpgrade:
                    cmd.append("patch_scale")
                else:
                    cmd.append("upgrade_scale")

                exitCode, stdout, stderr = shellCommand.execute(cmd)
            elif os.name == 'nt':
                cmd = []
                cmd.append(os.path.join(configInfo.getRootFolder(), "bin", "stageRecovery.cmd"))
                cmd.append("upgrade_scale")
                cmd.append(os.path.join(scaleAppRestore, coreTGZ))
                osUtils.winRunElevated(cmd)
            messageBox.destroy(mbId)
            
            logging.info("installUpgradeNonTerminating: done")
            return exitCode

        elif APPLICATION_UPGRADE_REGEX_COMPILED.match(upgradeFilename):
            logging.info("installUpgradeNonTerminating: application upgrade")

            baseVersionFile = os.path.join(os.path.dirname(upgradeFilename), "base")
            versionFlag = False

            # open up version info files and compare versions to see if we should upgrade
            try:
                with open(baseVersionFile) as base:
                    match = re.search(r"[0-9]\.[0-9]\.[0-9]{1,3}", base.readline())
                    baseVersion = match.group(0).split('.')

                    with open(os.path.join('/', 'SOFTWARE_VERSION_INFO')) as versionInfoFile:
                    # really confusing line but all it does is get the version major rev, minor rev, and build as a list
                        scaleVersion = versionInfoFile.readline().strip().split(':')[1].strip().split('.')
                        # 3.X.X can only work on 3.Y.Y and 4.X.X can only work on 4.Y.Y etc.
                        if( int(baseVersion[0]) != int(scaleVersion[0]) ):
                            versionFlag = True
                        elif (int(baseVersion[0]) == int(scaleVersion[0]) and
                              int(baseVersion[1]) > int(scaleVersion[1])):
                            versionFlag = True
                        elif (int(baseVersion[0]) == int(scaleVersion[0]) and
                              int(baseVersion[1]) == int(scaleVersion[1]) and
                              int(baseVersion[2]) > int(scaleVersion[2])):
                            versionFlag = True

                        if versionFlag:
                            print("Upgrade not compatible with current application version, aborting.")
                            print(("Base Ver: {}".format(baseVersion)))
                            print(("Scale Ver: {}".format(scaleVersion)))

                            if language == 'spanish':
                                message = "Upgrade not compatible with current application version, aborting." #TODO: JHC Translate
                            elif language == 'french':
                                message = "Upgrade not compatible with current application version, aborting." #TODO: JHC Translate
                            else:
                                message = "Upgrade not compatible with current application version, aborting."
                                
                            logging.info("installUpgradeNonTerminating: %s" % message)
                            exitCode, mbId = show_message(message=message, timer=5)

                            cleanupPackageUpgradeSource(removeUpgrade, upgradeFilename)
                            time.sleep(2)
                            return 1

            except Exception as e:
                logging.info("installUpgradeNonTerminating: failed!")
                logging.exception(e)
                print (e)
                print (baseVersionFile)
                #print (os.getcwd())
                #print(upgradeFilename)
                print("Where did my base file go? or was it SOFTWARE_VERSION_INFO???")

                if not versionFlag:
                    if language == 'spanish':
                        message = "Upgrade Version File Missing."  # TODO: JHC Translate
                    elif language == 'french':
                        message = "Upgrade Version File Missing."  # TODO: JHC Translate
                    else:
                        message = "Upgrade Version File Missing."
                    logging.info("installUpgradeNonTerminating: %s" % message)
                    exitCode, mbId = show_message(message=message, timer=5)

                    cleanupPackageUpgradeSource(removeUpgrade, upgradeFilename)
                    time.sleep(4)
                    return 1
                return 1
                # Remove any stale upgrade tarballs from the recovery partition before doing our size calculations.
            if os.path.exists(os.path.join(scaleAppRestore, coreTGZ)):
                os.remove(os.path.join(scaleAppRestore, coreTGZ))

            # get size of resources directory we are about to remove
            resourcesDirSize = fileSysStatistics.directorySize(resources)

            # get size of last upgrade recovery tarball in recovery partition that we are about to remove
            recoveryTarballSize = fileSysStatistics.fileSize(os.path.join(scaleAppRestore, coreTGZ))

            # get size of tarball we are about to extract and install
            upgradeFileSize = fileSysStatistics.fileSize(upgradeFilename)

            # get free space on recovery partition
            recoveryPartitionAvailableSpace = fileSysStatistics.availableSpace(recoveryPartition)

            # get the used space on the recovery partition
            recoveryPartitionUsedSpace = fileSysStatistics.usedSpace(recoveryPartition)

            # get the live /usr/local/hobart directory size since it is not removed when this upgrade occurs
            # which means that when the upgrade is extracted the contents of this directory is retained
            # with potentially overwrites of files based on what is in tarball for application/OS upgrade
            liveHobartDirSize = fileSysStatistics.directorySize(liveHobart)

            # get the live partition size
            livePartitionTotalSize = fileSysStatistics.totalSize(live)

            # TODO:
            # I am estimating that the worst case HTDF file backup of a DB is 50 MB
            # is that enough?
            # db backup HTDF file worst case size 50 MB
            dbBackupHtdfWorstCaseFileSize = 50 * 1024 * 1024

            print(('bytes needed: ' + str(upgradeFileSize + dbBackupHtdfWorstCaseFileSize)))
            print(('bytes available: ' + str(recoveryPartitionAvailableSpace + resourcesDirSize + recoveryTarballSize)))

            # TODO:
            # Assume worst case tar ball extract size is 2 times the tarball size compressed
            # May not be the greatest assumption
            # check if recovery partition has enough free space
            enoughSpace, partitionFreeSpace = enoughFreeSpaceToInstall(upgradeFileSize + dbBackupHtdfWorstCaseFileSize,
                                                                       recoveryPartition)

            if not enoughSpace:
                displayNotEnoughSpaceMessage(language=language,
                                             whichPartition='recovery',
                                             availableSpace=partitionFreeSpace + resourcesDirSize + recoveryTarballSize,
                                             neededSpace=upgradeFileSize + dbBackupHtdfWorstCaseFileSize)
                logging.info("installUpgradeNonTerminating: not enough space!")
                return 1

            print(('partition needed: ' + str((upgradeFileSize * 2) + liveHobartDirSize + dbBackupHtdfWorstCaseFileSize)))
            print(('partition available: ' + str(livePartitionTotalSize)))

            enoughSpace = (upgradeFileSize * 2) + liveHobartDirSize + dbBackupHtdfWorstCaseFileSize < livePartitionTotalSize
            partitionFreeSpace = livePartitionTotalSize

            if not enoughSpace:
                displayNotEnoughSpaceMessage(language=language,
                                             whichPartition='live',
                                             availableSpace=partitionFreeSpace,
                                             neededSpace=((
                                                                      upgradeFileSize * 2) + liveHobartDirSize + dbBackupHtdfWorstCaseFileSize))
                logging.info("installUpgradeNonTerminating: not enough space!")
                return 1

            if language == 'spanish':
                message = "Actualización de Aplicación y SO (Etapa 1)\n\nInicializando..."
            elif language == 'french':
                message = "Mise á niveau de l\'application et du SE (étape 1)\n\nInitialisation..."
            else:
                message = "Application and OS Upgrade (Stage 1)\n\nInitiating..."
                
            logging.info("installUpgradeNonTerminating: %s" % message)

            exitCode, mbId = show_message(message)

            backupCriticalFiles(upgradeFilename, resources, restoreScript, scaleAppRestore, mbId,
                                language, db, dbBackupInPlace, live, liveHobart, False, removeUpgrade,
                                dbRestore, coreTGZ, True)
            # if told to remove the upgrade
            cleanupPackageUpgradeSource(removeUpgrade, upgradeFilename)

            # stage the upgrade
            if os.name == 'posix':
                cmd = []
                cmd.append(os.path.join("/", "etc", "init.d", "stageRecovery.sh"))
                cmd.append("app_upgrade")

                exitCode, stdout, stderr = shellCommand.execute(cmd)
            elif os.name == 'nt':
                cmd = []
                cmd.append(os.path.join(configInfo.getRootFolder(), "bin", "stageRecovery.cmd"))
                cmd.append("upgrade_scale")
                cmd.append(os.path.join(scaleAppRestore, coreTGZ))
                osUtils.winRunElevated(cmd)
            messageBox.destroy(mbId)
            logging.info("installUpgradeNonTerminating: staging done")
            return exitCode

        # u-boot upgrade
        elif BOOTLOADER_UPGRADE_REGEX_COMPILED.match(upgradeFilename) or BOOTLOADER_UPGRADE_OLD_REGEX_COMPILED.match(upgradeFilename):
            logging.info("installUpgradeNonTerminating: uboot upgrade")
            # get size of resources directory we are about to remove
            if os.name == 'posix':
                bootloaderDirSize = fileSysStatistics.directorySize(os.path.join(recovery,"u-boot"))
            elif os.name == 'nt':
                bootloaderDirSize = fileSysStatistics.directorySize(os.path.join(recovery,'bios'))

            # get size of tarball we are about to extract and install
            upgradeFileSize = fileSysStatistics.fileSize(upgradeFilename)

            enoughSpace, partitionFreeSpace = enoughFreeSpaceToInstall(upgradeFileSize + bootloaderDirSize, recoveryPartition)
            if not enoughSpace:
                displayNotEnoughSpaceMessage(language=language,
                                             whichPartition='recovery',
                                             availableSpace=partitionFreeSpace + bootloaderDirSize,
                                             neededSpace=upgradeFileSize+bootloaderDirSize)
                logging.info("installUpgradeNonTerminating: not enough space!")
                return 1

            if language == 'spanish':
                message = "Actualización de Bootloader (Etapa 1)\n\nCopiando actualización desde dispositivo fuente..."
            elif language == 'french':
                message = "Mise á niveau de Bootloader (étape 1)\n\nCopie de la mise á niveau á partir du dispositif source..."
            else:
                message = "Boot Loader Upgrade (Stage 1)\n\nCopying Upgrade from Source Device..."

            logging.info("installUpgradeNonTerminating: %s" % message)
            exitCode,  mbId = show_message(message)

            if os.name == 'posix':
                if os.path.exists(os.path.join(recovery,"u-boot","hte-uboot.tgz")):
                    os.remove(os.path.join(recovery,"u-boot","hte-uboot.tgz"))

                if os.path.exists(os.path.join(recovery,"u-boot","u-boot.imx")):
                    os.remove(os.path.join(recovery,"u-boot","u-boot.imx"))

                os.system('/bin/cp %s %s' % (upgradeFilename, os.path.join(recovery,"u-boot","hte-uboot.tgz")))

                # if told to remove the upgrade
                cleanupPackageUpgradeSource(removeUpgrade, upgradeFilename)

                os.system('/bin/tar xvzf %s -C %s' % (os.path.join(recovery,"u-boot","hte-uboot.tgz"),
                                os.path.join(recovery,"u-boot")))
                os.remove(os.path.join(recovery,"u-boot",'hte-uboot.tgz'))

                #apply new u-boot bitmap files to serial flash partitions 2 and 5 (u-boot version >= 2.1.1).
                if os.path.exists(os.path.join(recovery,"u-boot","u-version.bmp")):
                    #apply the new version bitmap
                    result = updateSplashVersion.updateSplashVersion(os.path.join(recovery,"u-boot", "u-version.bmp"))
                    if 'success' != result:
                        logging.info("installUpgradeNonTerminating: error: Cannot upgrade version bitmap file to /dev/mtd2!")
                        print('error: Cannot upgrade version bitmap file to /dev/mtd2!')

                if os.path.exists(os.path.join(recovery,"u-boot","u-diagnostics.bmp")) and os.path.exists(os.path.join(recovery,"u-boot","u-missing.bmp")):
                    result = updateSplashDiagnostics.updateSplashErrorBitmaps(os.path.join(recovery,"u-boot", "u-diagnostics.bmp"), os.path.join(recovery,"u-boot", "u-missing.bmp"))
                    if 'success' != result:
                        logging.info("installUpgradeNonTerminating: error: Cannot upgrade bitmap files to /dev/mtd5!")
                        print('error: Cannot upgrade bitmap files to /dev/mtd5!')
                elif os.path.exists(os.path.join(recovery,"u-boot","u-diagnostics.bmp")):
                    result = updateSplashDiagnostics.updateSplashDiagnostics(os.path.join(recovery,"u-boot", "u-diagnostics.bmp"))
                    if 'success' != result:
                        logging.info("installUpgradeNonTerminating: error: Cannot upgrade diagnostics bitmap file to /dev/mtd5!")
                        print('error: Cannot upgrade diagnostics bitmap file to /dev/mtd5!')

            elif os.name == 'nt':
                biosRecoveryPath = os.path.join(recovery, 'bios')
                if os.path.exists(os.path.join(biosRecoveryPath,'htx-bios.tgz')):
                    try:
                        os.chmod(os.path.join(biosRecoveryPath,'htx-bios.tgz'), stat.S_IWRITE)
                        os.remove(os.path.join(biosRecoveryPath,'htx-bios.tgz'))
                    except IOError as exception_details:
                        print(('error: Cannot remove file: ' + os.path.join(biosRecoveryPath,'htx-bios.tgz') + ', Reason: ' + exception_details[1]))

                if os.path.exists(os.path.join(biosRecoveryPath,'bios.bin')):
                    try:
                        os.chmod(os.path.join(biosRecoveryPath,'bios.bin'), stat.S_IWRITE)
                        os.remove(os.path.join(biosRecoveryPath,'bios.bin'))
                    except exception_details:
                        print(('error: Cannot remove file: ' + os.path.join(biosRecoveryPath,'bios.bin') + ', Reason: ' + exception_details[1]))

                shutil.copy(upgradeFilename, os.path.join(biosRecoveryPath, 'htx-bios.tgz'))

                # if told to remove the upgrade
                cleanupPackageUpgradeSource(removeUpgrade, upgradeFilename)

                try:
                    tar = tarfile.open(os.path.join(biosRecoveryPath, 'htx-bios.tgz'), 'r')
                    tar.extractall(biosRecoveryPath)
                    os.chmod(os.path.join(recovery, 'bios','htx-bios.tgz'), stat.S_IWRITE)
                    os.remove(os.path.join(recovery, 'bios', 'htx-bios.tgz'))
                except Exception as e:
                    logging.exception(e)
                    raise

            messageBox.destroy(mbId)

            if language == 'spanish':
                message = "Actualización de Bootloader (Etapa 1)\n\nInicializando Etapa 2..."
            elif language == 'french':
                message = "Mise á niveau de Bootloader (étape 1)\n\nInitialisation étape 2..."
            else:
                message = "Boot Loader Upgrade (Stage 1)\n\nInitiating Stage 2..."
                
            logging.info("installUpgradeNonTerminating: %s" % message)

            exitCode,  mbId = show_message(message)

            # copy lang file to recovery partition
            if os.name == 'posix':
                if os.path.exists( os.path.join("/","media",RECOVERY_PART,"usr","local","hobart") ):
                    shutil.copy( os.path.join("/","usr","local","hobart","lang"), os.path.join("/","media",RECOVERY_PART,"usr","local","hobart","lang") )

                os.system("/bin/sync")

                cmd = []
                cmd.append(os.path.join("/", "etc","init.d","stageRecovery.sh"))
                cmd.append("upgrade_u-boot")
                exitCode, stdout, stderr = shellCommand.execute(cmd)

            elif os.name == 'nt':
                shutil.copy(os.path.join(liveHobart, 'lang'), os.path.join(recovery, 'lang'))

                cmd = []
                cmd.append(os.path.join(configInfo.getRootFolder(), "bin", "stageRecovery.cmd"))
                cmd.append("upgrade_bios")
                cmd.append(os.path.join(recovery,'bios','bios.bin'))
                osUtils.winRunElevated(cmd)

            messageBox.destroy(mbId)
            
            logging.info("installUpgradeNonTerminating: done: %s" % exitCode)
            return exitCode

        # language upgrade
        elif LANGUAGE_UPGRADE_REGEX_COMPILED.match(upgradeFilename) or LANGUAGE_UPGRADE_OLD_REGEX_COMPILED.match(upgradeFilename):
            logging.info("installUpgradeNonTerminating: language upgrade")
            if language == 'spanish':
                message = "Actualización de idioma\n\nCopiando actualización desde dispositivo fuente..."
            elif language == 'french':
                message = "Mise á niveau de langue\n\nCopie de la mise á niveau á partir du dispositif source..."
            else:
                message = "Language Upgrade\n\nCopying Upgrade from Source Device..."
            
            logging.info("installUpgradeNonTerminating: %s" % message)

            exitCode,  mbId = show_message(message)

            if os.name == 'posix':
                os.system('/bin/tar xvzf %s -C %s' % (upgradeFilename,  live))

                # if told to remove the upgrade
                cleanupPackageUpgradeSource(removeUpgrade, upgradeFilename)

                os.system("/bin/sync")

            elif os.name == 'nt':
                try:

                    destPath = os.path.join( liveHobart, "Videos" )

                    tar = tarfile.open( upgradeFilename, 'r' )

                    # HW-103
                    # Even though the training video upgrade files are considered to be
                    # platform-independent, they contain the full path for the Linux scale
                    # i.e.  /usr/local/hobart/videos/Super_OpScreen_Video.mp4
                    # Need to extract the videos one-at-a-time using the basename
                    for member in tar.getmembers():
                        if member.isreg():  # regular file
                            member.name = os.path.basename( member.name )
                            tar.extract( member, destPath )

                except Exception as e:
                    logging.exception(e)
                    raise

                # if told to remove the upgrade
                cleanupPackageUpgradeSource(removeUpgrade, upgradeFilename)

            messageBox.destroy(mbId)

            if language == 'spanish':
                message = "Instalación de la actualización del idioma es completa."
            elif language == 'french':
                message = "L\'installation de la mise á niveau de la langue est terminé."
            else:
                message = "Installation of Language Upgrade is complete."
                
            logging.info("installUpgradeNonTerminating: %s" % message)

            exitCode,  mbId = show_message(message=message,  timer=5)
            return 0

        # feature upgrade
        elif FEATURE_UPGRADE_REGEX_COMPILED.match(upgradeFilename) or \
            FEATURE_UPGRADE_OLD_REGEX_COMPILED.match(upgradeFilename) or \
            FEATURE_UPGRADE_OSAGNOSTIC_REGEX_COMPILED.match(upgradeFilename):
            logging.info("installUpgradeNonTerminating: feature upgrade")

            # get size of tarball we are about to extract and install
            upgradeFileSize = fileSysStatistics.fileSize(upgradeFilename)

            enoughSpace, partitionFreeSpace = enoughFreeSpaceToInstall(upgradeFileSize, live)
            if not enoughSpace:
                displayNotEnoughSpaceMessage(language=language,
                                             whichPartition='live',
                                             availableSpace=partitionFreeSpace,
                                             neededSpace=upgradeFileSize)
                logging.info("installUpgradeNonTerminating: not enough space!")
                return 1

            # do not display message box if this is a SM backend upgrade
            if not BACKEND_UPGRADE_FILENAME_REGEX_COMPILED.match(upgradeFilename) and \
                not BACKEND_UPGRADE_NEW_FILENAME_REGEX_COMPILED.match(upgradeFilename):
                if language == 'spanish':
                    message = "Actualización de caracterÃ­stica\n\nCopiando actualización desde dispositivo fuente..."
                elif language == 'french':
                    message = "Mise á niveau de caractéristique\n\nCopie de la mise á niveau á partir du dispositif source..."
                else:
                    message = "Feature Upgrade\n\nCopying Upgrade from Source Device..."
                    
                logging.info("installUpgradeNonTerminating: %s" % message)

                exitCode,  mbId = show_message(message)

            # pick hte-feature_{name} out of .tgz tarball name by splitting it at dash
            upgradeFileElements = os.path.basename(upgradeFilename).split("-")

            if FEATURE_UPGRADE_OLD_REGEX_COMPILED.match(upgradeFilename):
                # pick feature name out of feature section of file name by splitting at _
                featureNameElements = upgradeFileElements[1].split('_')
                # set feature name to all the feature name elements except feature_
                featureName = "_".join(featureNameElements[1:])
            else:
                featureName = upgradeFileElements[3]

            # determine if feature already is installed and if so remove feature folder
            if os.name == 'posix':
                if MONOLITHIC_UPGRADE_REGEX_COMPILED.match(upgradeFilename):
                    logging.info("installUpgradeNonTerminating: monolithic upgrade")
                    # extract the main tarball with live, recovery, and uboot tarballs in it and an install file
                    installUtils.extractTarball(upgradeFilename, live)
                    # execute the install file to extract all tarballs and update uboot, recovery, and live
                    installUtils.executeInstallScript(featureName, live)

                    cleanupPackageUpgradeSource(removeUpgrade, upgradeFilename)

                    installUtils.reboot()
                else:
                    if FEATURE_UPGRADE_OSAGNOSTIC_REGEX_COMPILED.match(upgradeFilename):
                        logging.info("installUpgradeNonTerminating: feature upgrade OS agnostic")
                        # any os feature is installed at /usr/local since the prefix directory is os specific
                        cleanupFeature(upgradeFilename, "/usr/local/")
                        with tarfile.open(upgradeFilename) as archive:
                            extractOSAgnosticArchive(archive)
                    else:
                        logging.info("installUpgradeNonTerminating: other feature upgrade")
                        cleanupFeature(upgradeFilename, "/")
                        installUtils.extractTarball(upgradeFilename, live)

                    # if told to remove the upgrade
                    cleanupPackageUpgradeSource(removeUpgrade, upgradeFilename)

                    installUtils.executeInstallScript(featureName, live)

            elif os.name == 'nt':
                feature_install_path = '/'.join(('C:', 'Program Files', 'Hobart', 'features', featureName))
                if os.path.exists(feature_install_path):
                    os.system('rmdir /s /q "{0}"'.format(feature_install_path))

                if FEATURE_UPGRADE_OSAGNOSTIC_REGEX_COMPILED.match(upgradeFilename):
                    with tarfile.open(upgradeFilename) as archive:
                        extractOSAgnosticArchive(archive)
                else:
                    installUtils.extractTarball(upgradeFilename, os.path.join(live))

                # if told to remove the upgrade
                cleanupPackageUpgradeSource(removeUpgrade, upgradeFilename)

                installUtils.executeInstallScript(featureName, live)

            # do not display message box if this is a SM backend upgrade
            if not BACKEND_UPGRADE_FILENAME_REGEX_COMPILED.match(upgradeFilename) and \
                not BACKEND_UPGRADE_NEW_FILENAME_REGEX_COMPILED.match(upgradeFilename):

                messageBox.destroy(mbId)

                if language == 'spanish':
                    message = "Instalación de la actualización del caracterÃ­stica es completa."
                elif language == 'french':
                    message = "L\'installation de la mise á niveau de la caractéristique est terminé."
                else:
                    message = "Installation of Feature Upgrade is complete."
                    
                logging.info("installUpgradeNonTerminating: %s" % message)

                exitCode,  mbId = show_message(message=message,  timer=5)

            return 0

        # multimedia tarball from HTe
        elif HTE_MULTIMEDIA_FILENAME_COMPILED.match(upgradeFilename):
            logging.info("installUpgradeNonTerminating: multimedia tarball")

            # get size of tarball we are about to extract and install
            upgradeFileSize = fileSysStatistics.fileSize(upgradeFilename)

            enoughSpace, partitionFreeSpace = enoughFreeSpaceToInstall(upgradeFileSize, live)
            if not enoughSpace:
                displayNotEnoughSpaceMessage(language=language,
                                             whichPartition='live',
                                             availableSpace=partitionFreeSpace,
                                             neededSpace=upgradeFileSize)
                logging.info("installUpgradeNonTerminating: not enough space!")
                return 1

            # extract multimedia files
            try:
                with tarfile.open(upgradeFilename) as archive:
                    extractMultimediaArchive(archive)
            except Exception as e:
                logging.info("installUpgradeNonTerminating: failed to extract!")
                logging.exception(e)
                pass

            # remove the source upgrade file since we have already extracted the feature
            cleanupPackageUpgradeSource(removeUpgrade, upgradeFilename)

            return 0

        elif LICENSE_INSTALL_REGEX_COMPILED.match(upgradeFilename):
            logging.info("installUpgradeNonTerminating: license install")

            #if language == 'spanish':
            #    message = "Actualización de la licencia\n\nCopiando actualización desde dispositivo fuente..."
            #elif language == 'french':
            #    message = "mise á niveau de licence\n\nCopie de la mise á niveau á partir du dispositif source..."
            #else:
            #    message = "License Upgrade\n\nCopying Upgrade from Source Device..."
            #
            #exitCode, mbId = show_message(message, timer=5)

            exitCode = installLicense(licenseFile=upgradeFilename, language=language, serverHost=serverHost, username=username, password=password, uploadFolder=uploadFolder)

            # if told to remove the source upgrade
            cleanupPackageUpgradeSource(removeUpgrade, upgradeFilename)

            if os.name == 'posix':
                os.system("/bin/sync")
                
            logging.info("installUpgradeNonTerminating: license install done (%s)" % exitCode)

            return exitCode

        elif FPC_UPGRADE_REGEX_COMPILED.match(upgradeFilename) or FPC_UPGRADE_OLD_REGEX_COMPILED.match(upgradeFilename):
            logging.info("installUpgradeNonTerminating: FPC upgrade")

            if language == 'spanish':
                message = "Actualización de la FPC\n\nCopiando actualización desde dispositivo fuente..."
            elif language == 'french':
                message = "mise á niveau de FPC\n\nCopie de la mise á niveau á partir du dispositif source..."
            else:
                message = "FPC Upgrade\n\nCopying Upgrade from Source Device..."

            exitCode,  mbId = show_message(message, timer=5)

            exitCode = installFPC(upgradeFilename)

            # if told to remove the source upgrade
            cleanupPackageUpgradeSource(removeUpgrade, upgradeFilename)
            
            logging.info("installUpgradeNonTerminating: FPC upgrade done (%s)" % exitCode)

            return exitCode

        elif KPC_BOOT_UPGRADE_FILENAME_COMPILED.match(upgradeFilename):
            logging.info("installUpgradeNonTerminating: KPC upgrade")

            if language == 'spanish':
                message = "Actualización de la KPC\n\nCopiando actualización desde dispositivo fuente..."
            elif language == 'french':
                message = "mise á niveau de KPC\n\nCopie de la mise á niveau á partir du dispositif source..."
            else:
                message = "KPC Upgrade\n\nCopying Upgrade from Source Device..."
                
            logging.info("installUpgradeNonTerminating: %s" % message)

            exitCode, mbId = show_message(message, timer=5)

            # detect board type and copy file to download directory which will trigger chobart_d to install
            if os.name == 'posix':
                # extract both binaries to the firmware directory
                cmd = []
                cmd.append("/bin/tar")
                cmd.append("xvzf")
                cmd.append(upgradeFilename)
                cmd.append("-C")
                cmd.append(os.path.join('/', 'lib', 'firmware'))
                exitCode, stdout, stderr = shellCommand.execute(cmd)

                # determine our board type
                if getBoardType.isArkBoard():
                    # copy the correct binary to the firmware folder and download directory
                    cmd = []
                    cmd.append("/bin/cp")
                    cmd.append("/lib/firmware/sourceKPCBoot.bin")
                    cmd.append(os.path.join('/', 'lib', 'firmware', 'kpc', 'boot'))
                    exitCode, stdout, stderr = shellCommand.execute(cmd)

                    cmd = []
                    cmd.append("/bin/mv")
                    cmd.append("/lib/firmware/sourceKPCBoot.bin")
                    cmd.append(os.path.join('/', 'usr', 'local', 'hobart', 'download'))
                    exitCode, stdout, stderr = shellCommand.execute(cmd)
                else:
                    exitCode = 1
                cmd = []
                cmd.append("bin/sync")
                shellCommand.execute(cmd)
                
                logging.info("installUpgradeNonTerminating: copied KPC file (%s)" % exitCode)

            sendDeviceEventMessage.sendMessage(server='localhost', port=6000,
                                               message="RTDO" + chr(0x1F) + "TNCheckForDownload" + chr(0x1E))

            # if told to remove the source upgrade
            cleanupPackageUpgradeSource(removeUpgrade, upgradeFilename)
            return exitCode

        elif FSSS_BOOT_UPGRADE_FILENAME_COMPILED.match(upgradeFilename):

            if language == 'spanish':
                message = "Actualización de la KPC\n\nCopiando actualización desde dispositivo fuente..."
            elif language == 'french':
                message = "mise á niveau de KPC\n\nCopie de la mise á niveau á partir du dispositif source..."
            else:
                message = "FSSS Upgrade\n\nCopying Upgrade from Source Device..."

            exitCode, mbId = messageBox.display(message, timer=5)

            # detect board type and copy file to download directory which will trigger chobart_d to install
            if os.name == 'posix':
                # extract both binaries to the firmware directory
                cmd = []
                cmd.append("/bin/tar")
                cmd.append("xvzf")
                cmd.append(upgradeFilename)
                cmd.append("-C")
                cmd.append(os.path.join('/', 'lib', 'firmware'))
                exitCode, stdout, stderr = shellCommand.execute(cmd)

                # determine our board type
                if getBoardType.isArkBoard():
                    # copy the correct binary to the firmware folder and download directory
                    cmd = []
                    cmd.append("/bin/cp")
                    cmd.append("/lib/firmware/sourceFsssBoot.bin")
                    cmd.append(os.path.join('/', 'lib', 'firmware', 'fsss', 'boot'))
                    exitCode, stdout, stderr = shellCommand.execute(cmd)

                    cmd = []
                    cmd.append("/bin/mv")
                    cmd.append("/lib/firmware/sourceFsssBoot.bin")
                    cmd.append(os.path.join('/', 'usr', 'local', 'hobart', 'download'))
                    exitCode, stdout, stderr = shellCommand.execute(cmd)
                else:
                    exitCode = 1
                cmd = []
                cmd.append("bin/sync")
                shellCommand.execute(cmd)

            sendDeviceEventMessage.sendMessage(server='localhost', port=6000,
                                               message="RTDO" + chr(0x1F) + "TNCheckForDownload" + chr(0x1E))

            # if told to remove the source upgrade
            cleanupPackageUpgradeSource(removeUpgrade, upgradeFilename)
            return exitCode

        elif HFPD_UPGRADE_FILENAME_COMPILED.match(upgradeFilename):
            logging.info("installUpgradeNonTerminating: HFPD upgrade")
            if language == 'spanish':
                message = "Actualización de la KPC\n\nCopiando actualización desde dispositivo fuente..."
            elif language == 'french':
                message = "mise á niveau de KPC\n\nCopie de la mise á niveau á partir du dispositif source..."
            else:
                message = "Handsfree Pager Device Upgrade\n\nCopying Upgrade from Source Device..."

            exitCode, mbId = show_message(message, timer=5)

            # detect board type and copy file to download directory which will trigger chobart_d to install
            if os.name == 'posix':
                # extract both binaries to the firmware directory
                cmd = []
                cmd.append("/bin/tar")
                cmd.append("xvzf")
                cmd.append(upgradeFilename)
                cmd.append("-C")
                cmd.append(os.path.join('/', 'lib', 'firmware', 'hfpd'))
                exitCode, stdout, stderr = shellCommand.execute(cmd)
                
                logging.info("installUpgradeNonTerminating: HFPD upgrade done (%s)" % exitCode)

        elif CLA_UPGRADE_REGEX_COMPILED.match(upgradeFilename) or CLA_UPGRADE_OLD_REGEX_COMPILED.match(upgradeFilename):
            logging.info("installUpgradeNonTerminating: CLA upgrade")
            if language == "spanish":
                message = "Actualización de la CLA\n\nCopiando actualización desde dispositivo fuente..."
            elif language == "french":
                message = "mise á niveau de CLA\n\nCopie de la mise á niveau á partir du dispositif source..."
            else:
                message = "CLA Upgrade\n\nCopying Upgrade from Source Device..."
            exitCode,  mbId = show_message(message, timer=5)
            exitCode = installPrePackDevice(upgradeFilename)
            # if told to remove the source upgrade
            cleanupPackageUpgradeSource(removeUpgrade, upgradeFilename)
            return exitCode
        elif AWS_UPGRADE_REGEX_COMPILED.match(upgradeFilename) or AWS_UPGRADE_OLD_REGEX_COMPILED.match(upgradeFilename):
            logging.info("installUpgradeNonTerminating: AWS upgrade")
            if language == "spanish":
                message = "Actualización de la AWS\n\nCopiando actualización desde dispositivo fuente..."
            elif language == "french":
                message = "mise á niveau de AWS\n\nCopie de la mise á niveau á partir du dispositif source..."
            else:
                message = "AWS Upgrade\n\nCopying Upgrade from Source Device..."
            exitCode,  mbId = show_message(message, timer=5)
            exitCode = installPrePackDevice(upgradeFilename)
            # if told to remove the source upgrade
            cleanupPackageUpgradeSource(removeUpgrade, upgradeFilename)
            return exitCode
        elif AWS3_UPGRADE_REGEX_COMPILED.match(upgradeFilename) or AWS3_UPGRADE_OLD_REGEX_COMPILED.match(upgradeFilename):
            logging.info("installUpgradeNonTerminating: AWS3 upgrade")
            if language == "spanish":
                message = "Actualización de la AWS3\n\nCopiando actualización desde dispositivo fuente..."
            elif language == "french":
                message = "mise á niveau de AWS3\n\nCopie de la mise á niveau á partir du dispositif source..."
            else:
                message = "AWS3 Upgrade\n\nCopying Upgrade from Source Device..."
            exitCode,  mbId = show_message(message, timer=5)
            exitCode = installPrePackDeviceNGW(upgradeFilename)
            # if told to remove the source upgrade
            cleanupPackageUpgradeSource(removeUpgrade, upgradeFilename)
            return exitCode
        elif NGW_UPGRADE_REGEX_COMPILED.match(upgradeFilename):
            logging.info("installUpgradeNonTerminating: NGW upgrade")
            if language == "spanish":
                message = "Actualización de la NGW\n\nCopiando actualización desde dispositivo fuente..."
            elif language == "french":
                message = "mise á niveau de NGW\n\nCopie de la mise á niveau á partir du dispositif source..."
            else:
                message = "NGW Upgrade\n\nCopying Upgrade from Source Device..."
            exitCode,  mbId = show_message(message, timer=5)
            exitCode = installPrePackDeviceNGW(upgradeFilename)
            # if told to remove the source upgrade
            cleanupPackageUpgradeSource(removeUpgrade, upgradeFilename)
            return exitCode
        elif NGW2P_UPGRADE_REGEX_COMPILED.match(upgradeFilename):
            # SOF-4509: NGW with 2nd Label Applier
            if language == "spanish":
                message = "Actualización de la NGW\n\nCopiando actualización desde dispositivo fuente..."
            elif language == "french":
                message = "mise á niveau de NGW\n\nCopie de la mise á niveau á partir du dispositif source..."
            else:
                message = "NGW Upgrade for 2nd Label Applier\n\nCopying Upgrade from Source Device..."
            exitCode,  mbId = show_message(message, timer=5)
            exitCode = installPrePackDeviceNGW( upgradeFilename, 'ngw', True )
            # if told to remove the source upgrade
            cleanupPackageUpgradeSource(removeUpgrade, upgradeFilename)
            return exitCode
        elif NGS_UPGRADE_REGEX_COMPILED.match(upgradeFilename):
            logging.info("installUpgradeNonTerminating: NGS upgrade")
            if language == "spanish":
                message = "Actualización de la NGS\n\nCopiando actualización desde dispositivo fuente..."
            elif language == "french":
                message = "mise á niveau de NGS\n\nCopie de la mise á niveau á partir du dispositif source..."
            else:
                message = "NGS Upgrade\n\nCopying Upgrade from Source Device..."
            exitCode,  mbId = show_message(message, timer=5)
            exitCode = installPrePackDeviceNGW(upgradeFilename, 'ngs')
            # if told to remove the source upgrade
            cleanupPackageUpgradeSource(removeUpgrade, upgradeFilename)
            return exitCode
        elif EWS_UPGRADE_REGEX_COMPILED.match(upgradeFilename) or EWS_UPGRADE_OLD_REGEX_COMPILED.match(upgradeFilename):
            logging.info("installUpgradeNonTerminating: EWS upgrade")
            if language == "spanish":
                message = "Actualización de la EWS\n\nCopiando actualización desde dispositivo fuente..."
            elif language == "french":
                message = "mise á niveau de EWS\n\nCopie de la mise á niveau á partir du dispositif source..."
            else:
                message = "EWS Upgrade\n\nCopying Upgrade from Source Device..."
            exitCode,  mbId = show_message(message, timer=5)
            exitCode = installPrePackDevice(upgradeFilename)
            # if told to remove the source upgrade
            cleanupPackageUpgradeSource(removeUpgrade, upgradeFilename)
            return exitCode
        elif XPE_UPGRADE_REGEX_COMPILED.match(upgradeFilename) or XPE_UPGRADE_OLD_REGEX_COMPILED.match(upgradeFilename):
            logging.info("installUpgradeNonTerminating: XPE upgrade")
            if language == "spanish":
                message = "Actualización de la EPP\n\nCopiando actualización desde dispositivo fuente..."
            elif language == "french":
                message = "mise á niveau de EPP\n\nCopie de la mise á niveau á partir du dispositif source..."
            else:
                message = "EPP Upgrade\n\nCopying Upgrade from Source Device..."
            exitCode,  mbId = show_message(message, timer=5)
            exitCode = installPrePackDevice(upgradeFilename)
            # if told to remove the source upgrade
            cleanupPackageUpgradeSource(removeUpgrade, upgradeFilename)
            return exitCode;
        elif WGH_UPGRADE_REGEX_COMPILED.match(upgradeFilename) or WGH_UPGRADE_OLD_REGEX_COMPILED.match(upgradeFilename):
            logging.info("installUpgradeNonTerminating: WGH upgrade")
            if language == "spanish":
                message = "Actualización de la AWS2\n\nCopiando actualización desde dispositivo fuente..."
            elif language == "french":
                message = "mise á niveau de AWS2\n\nCopie de la mise á niveau á partir du dispositif source..."
            else:
                message = "AWS2 Upgrade\n\nCopying Upgrade from Source Device..."
            exitCode,  mbId = show_message(message, timer=5)
            exitCode = installPrePackDevice(upgradeFilename)
            # if told to remove the source upgrade
            cleanupPackageUpgradeSource(removeUpgrade, upgradeFilename)
            return exitCode;
        elif RECOVERY_UPGRADE_REGEX_COMPILED.match(upgradeFilename) or RECOVERY_UPGRADE_OLD_REGEX_COMPILED.match(upgradeFilename):
            logging.info("installUpgradeNonTerminating: recovery upgrade")
            if language == 'spanish':
                message = "Actualización de recuperación\n\nCopiando actualización de recuperación desde dispositivo fuente..."
            elif language == 'french':
                message = "Mise á niveau de recouvrement\n\nCopie de la mise á niveau de\nrecouvrement du dispositif source..."
            else:
                message = "Recovery Upgrade\n\nCopying Recovery Upgrade from Source Device..."

            exitCode,  mbId = show_message(message)

            if os.name == 'posix':
                os.system('/bin/cp -f %s %s' % (upgradeFilename,  os.path.join("/", "tmp", "recovery.tar.gz")))

                # if told to remove the upgrade
                cleanupPackageUpgradeSource(removeUpgrade, upgradeFilename)

                os.system("/bin/sync")

            elif os.name == 'nt':
                shutil.copy(upgradeFilename, os.path.join(recoveryPartition, 'recovery.tar.gz'))

                # if told to remove the upgrade
                cleanupPackageUpgradeSource(removeUpgrade, upgradeFilename)

            messageBox.destroy(mbId)

            if language == 'spanish':
                message = "Actualización de recuperación (Etapa 1)\n\nInicializando Etapa 2..."
            elif language == 'french':
                message = "Mise á niveau de recouvrement (étape 1)\n\nInitialisation étape 2..."
            else:
                message = "Recovery Upgrade (Stage 1)\n\nInitiating Stage 2..."

            exitCode,  mbId = show_message(message=message)

            if os.name == 'posix':
                cmd = []
                cmd.append(os.path.join("/", "etc","init.d","stageRecovery.sh"))
                cmd.append("upgrade_recovery")
                exitCode, stdout, stderr = shellCommand.execute(cmd)
            elif os.name == 'nt':
                cmd = []
                cmd.append(os.path.join(configInfo.getRootFolder(), "bin", "stageRecovery.cmd"))
                cmd.append("upgrade_recovery")
                cmd.append(os.path.join(recoveryPartition, 'recovery.tar.gz'))
                osUtils.winRunElevated(cmd)

            messageBox.destroy(mbId)
            
            logging.info("installUpgradeNonTerminating: recovery upgrade done (%s)" % exitCode)
            return exitCode

        else:
            if language == 'spanish':
                message = "Archivo de actualización no reconocido, Cheque nombre de archivo."
            elif language == 'french':
                message = "Fichier de mise á jour non reconnu, Chèque nom de fichier."
            else:
                message = "Unrecognized upgrade file, check filename."

            logging.info("installUpgradeNonTerminating: unknown upgrade, ignoring")
            exitCode,  mbId = show_message(message=message, timer=5)

    else:
        #print "Where is the file???"
        logging.info("installUpgradeNonTerminating: huh???")
        return 1


if __name__ == '__main__':
    logging.basicConfig(filename=UPGRADE_LOG_FILENAME, level="DEBUG")
    r = 0
    if len(sys.argv) == 2:
        # this would be call from upgrade from USB thumb drive
        # and Qt land via a shell command
        r = installUpgrade(upgradeFilename=sys.argv[1])

    elif len(sys.argv) == 3:
        print((sys.argv[2]))
        r = installUpgrade(upgradeFilename=sys.argv[1], removeUpgrade=(sys.argv[2] == 'True'))

    if os.name == 'posix':
        os.system("/bin/sync")
    
    if r != 0:
        if (os.path.isfile('/etc/hobart/blackbox')):
            os.system("python /usr/local/hobart/python/blackboxBeeps.py error 5")
        
    sys.exit(r)
