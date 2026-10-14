#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import shutil
import sys
import installUtils
import installUpgrade
import messageBox
PYTHON_PATH = '/usr/local/hobart/python'
sys.path.append(PYTHON_PATH)
import license
import getModelName
import time

FEATURE_NAME = 'wm_remote_printing'
FEATURE_FILENAME = FEATURE_NAME + '.tgz'
FEATURE_INSTALLER_NAME = FEATURE_NAME + '_install.py'
FEATURE_INSTALL_PATH = os.path.join(installUtils.getFeatureRoot(), FEATURE_NAME)
DOWNLOAD_PATH = os.path.join('/','usr','local','hobart','download')
PENDING_PATH = os.path.join('/','usr','local','hobart','pending')
DEVICE = 'hti'
SUPPORTED_SYSTEM = 'linux'

# feature file stuff
LICENSE_FILE = 'remote_printing.lic'
#BUSINESS_LOGIC_BINARY = 'chobart_d'
#SCALEUI_BINARY = 'scaleui'
#VERSION_INFO = 'version.ini'
#SOFTWARE_VERSION_INFO = 'SOFTWARE_VERSION_INFO'
#CONFIGL = 'config_linux.dat'
#SMBACKEND_PY = 'smBackend.py'
#DEFAULTS_LINUX='defaults_linux.ini'
#DB_FILE='dbftm.db'

CONFIG_FILE = 'menu_ordering.conf'
ICON_FILE = 'MOA.PNG'
#MOA_FK_FILE = 'moa_fk.ht'
WEBAPP_BIN= 'menu_ordering/webapp'
WEBAPP_CONFIG_FILE='menu_ordering/webapp.config'
LOGO_PNG_FILE='menu_ordering/logo.png'
PROXY_SETTINGS_FILES='menu_ordering/proxySettings'

ICON1 = os.path.join('menu_ordering','res','backspaceIcon.png')
ICON2 = os.path.join('menu_ordering','res','capsLockIcon.png')
ICON3 = os.path.join('menu_ordering','res','clearIcon.png')
ICON4 = os.path.join('menu_ordering','res','dismissIcon.png')
ICON5 = os.path.join('menu_ordering','res','exitIcon.png')
ICON6 = os.path.join('menu_ordering','res','minimizeIcon.png')
ICON7 = os.path.join('menu_ordering','res','refreshIcon.png')
ICON8 = os.path.join('menu_ordering','res','returnIcon.png')
ICON9 = os.path.join('menu_ordering','res','showIcon.png')

def installLicense(licenseFile):
    # read the flash key buffer
    xmlLicenseBuffer = license.readFlashKeyBuffer()

    if xmlLicenseBuffer == None:
         xmlLicenseBuffer = license.xmlCreateInitialLicenseBuffer()
     # get elements in license file
    status, individualName, companyName, companyAddress, productNames, licenseType, licenseActivationDate, licenseAllotment, licenseTimeout, licenseKey = license.getLicenseElements(licenseFile)

    addLicense = False
    if productNames == None: 
         addLicense = True
    else: 
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

exitCode = 0

exitCode |= installUtils.setPythonRoot()

# ----------------------
exit_code, process_id = messageBox.display('Installing application license')
exitCode = installLicense(os.path.join("/", "usr", "local", "hobart", "features", LICENSE_FILE))
messageBox.destroy(process_id)

#exitCode = installUtils.removeInstallerFiles(FEATURE_NAME)
exitCode = installUtils.removeInstallerFiles(FEATURE_NAME)

exit_code, process_id = messageBox.display('Installation is complete...', timer=5)
messageBox.destroy(process_id)
# sync filesystem and reboot

os.system("/bin/sync")
#os.system("/sbin/reboot")

sys.exit(0)

