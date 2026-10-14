import hteVersionInfo

VERSION = hteVersionInfo.USB_KEY_CREATOR_VERSION

from PyQt4 import QtGui, QtCore
from usbKeyCreatorUi import Ui_usbKeyCreatorDialog
import sys
import crypto
import os
import keyInfo
import usbQuery
import file_io
import tarfile
import shellCommand
import osUtils

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    def _fromUtf8(s):
        return s

class UsbKeyCreator(QtGui.QDialog):

    # class constructor
    def __init__(self,parent=None,keyPermissions=[keyInfo.service_key_name, keyInfo.supervisor_key_name]):

    # instantiate QDialog class
        QtGui.QDialog.__init__(self,parent)

        # create USK Key Creator Dialog
        self.ui = Ui_usbKeyCreatorDialog()

        # manifest UI
        self.ui.setupUi(self)

        self.setWindowTitle("HTe USB Key Creator (Version: %s)" % (VERSION))

        self.settings = QtCore.QSettings("ITW Hobart", "USB Key Creator")
        self.licenseFilename = self.settings.value("licenseFilename").toString()

        if len(keyPermissions) == 2:
            if keyInfo.service_key_name in keyPermissions and keyInfo.supervisor_key_name in keyPermissions:
                self.ui.licenseButton.hide()
            else:
                self.ui.licenseButton.show()
                self.checkLicense(self.licenseFilename)

        # connect sockets for exit and create buttons
        QtCore.QObject.connect(self.ui.cancelButton, QtCore.SIGNAL(_fromUtf8("clicked()")), self.exit)
        QtCore.QObject.connect(self.ui.createButton, QtCore.SIGNAL(_fromUtf8("clicked()")), self.create)
        QtCore.QObject.connect(self.ui.eraseButton, QtCore.SIGNAL(_fromUtf8("clicked()")), self.erase)
        QtCore.QObject.connect(self.ui.licenseButton, QtCore.SIGNAL(_fromUtf8("clicked()")), self.loadLicense)

        # disable all checkboxes initially
        self.ui.serviceCheckBox.setEnabled(False)
        self.ui.supervisorCheckBox.setEnabled(False)
        self.ui.selfServiceCheckBox.setEnabled(False)
        self.ui.manufacturingCheckBox.setEnabled(False)
        self.ui.systemTestCheckBox.setEnabled(False)

        self.keyPermissions = keyPermissions

        # enable key type checkboxes based on permission of user
        for keyType in keyPermissions:
            if keyType == keyInfo.service_key_name:
                self.ui.serviceCheckBox.setEnabled(True)
            elif keyType == keyInfo.supervisor_key_name:
                self.ui.supervisorCheckBox.setEnabled(True)
            elif keyType == keyInfo.selfservice_key_name:
                self.ui.selfServiceCheckBox.setEnabled(True)
            elif keyType == keyInfo.manufacturing_key_name:
                self.ui.manufacturingCheckBox.setEnabled(True)
            elif keyType == keyInfo.systemtest_key_name:
                self.ui.systemTestCheckBox.setEnabled(True)

    def extractTarball(self,tarFilename,destination):
        tfile = tarfile.open(tarFilename)
        if tarfile.is_tarfile(tarFilename):
            tfile.extractall(destination)

    def clearStatus(self):
        self.ui.statusLabel.clear()

    def setStatus(self, status, message):
        if status.lower() == 'error':
            color = "red"
        elif status.lower() == 'success':
            color = "black"
        elif status.lower() == 'info':
            color = "green"
        elif status.lower() == 'warning':
            color = "yellow"

        self.ui.statusLabel.append(_fromUtf8('<br><font color="%s"><B>%s</B>: %s</font>' % (color, status.upper(), message)))
        QtCore.QCoreApplication.processEvents()

    # Get the usb key types that are current checked on UI
    def getKeyTypesChecked(self):
        keyList = []
        if self.ui.serviceCheckBox.isChecked():
            keyList.append(keyInfo.service_key_name)
        if self.ui.manufacturingCheckBox.isChecked():
            keyList.append(keyInfo.manufacturing_key_name)
        if self.ui.supervisorCheckBox.isChecked():
            keyList.append(keyInfo.supervisor_key_name)
        if self.ui.selfServiceCheckBox.isChecked():
            keyList.append(keyInfo.selfservice_key_name)
        if self.ui.systemTestCheckBox.isChecked():
            keyList.append(keyInfo.systemtest_key_name)
        return keyList

    # exit dialog
    def exit(self):

        usbDevices = usbQuery.UsbDeviceQuery()
        devices = usbDevices.findDevicesInClass(usbDevices.DEVICE_TYPE_USB)
        if len(devices) == 1:
            if type(devices) is dict:
                device = devices.keys()[0] #Posix
            else:
                device = devices[0] # Windows

            serialNumber = usbDevices.getSerialNumber(device)
            if serialNumber != None and serialNumber.strip() != '':
                deviceLetter = usbDevices.getDeviceLetter(device)
                if deviceLetter is not None:
                    if os.name == 'posix':
                        cmd = []
                        cmd.append("/bin/sync")
                        shellCommand.execute(cmd)
                        cmd = []
                        cmd.append("/usr/bin/eject")
                        cmd.append(deviceLetter)
                    else:
                        # need sync command for windows
                        cmd = []
                        cmd.append("USB_Disk_Eject")
                        cmd.append("/REMOVELETTER")
                        cmd.append(deviceLetter[:-1])
                    shellCommand.execute(cmd)

        QtGui.QDialog.reject(self)

    def erase(self):
        status_code = 1

        self.clearStatus()

        # notify user what is going on
        self.setStatus("info","Scanning USB devices attached to this computing system.")

        # instantiate USB Query class
        usbDevices = usbQuery.UsbDeviceQuery()

        # Find all removable USB devices connected to computer
        devices = usbDevices.findDevicesInClass(usbDevices.DEVICE_TYPE_USB)
        #print len(devices)

        # do some error message processing based on USB devices we find/do not find
        if len(devices) < 1:
            self.setStatus("error",'No USB thumb drives found.<br>Insert Hobart approved thumb drive and try again.')
        elif len(devices) > 1:
            self.setStatus("error",'More than one removable USB device found.<br><br>Remove all USB removable devices and try again.')
        elif len(devices) == 1:
            if type(devices) is dict:
                device = devices.keys()[0] #Posix
            else:
                device = devices[0] # Windows

            serialNumber = usbDevices.getSerialNumber(device)
            if serialNumber == None or serialNumber.strip() == '':
                self.setStatus("error",'No serial number found on thumb drive.<br><br>Insert a Hobart approved USB thumb drive.')
            else:
                print '  Serial Number: ', serialNumber
                deviceLetter = usbDevices.getDeviceLetter(device)
                if deviceLetter is None:
                    self.setStatus('error','Thumb drive can not be mounted.<br><br>Insert a Hobart approved USB thumb drive')
                    return

                vendor = usbDevices.getVendorName(device)
                if vendor == None or vendor.strip() == '':
                    vendor = "Unspecified"
                product = usbDevices.getProductDescription(device)
                if product == None or product.strip() == '':
                    product = "Unspecified"

                #print '  Device Letter: ', deviceLetter
                if os.path.exists(os.path.join(deviceLetter)):
                    self.setStatus('info','Found removable USB Device:<br>Vendor: %s<br>Description: %s<br>Drive Letter: %s<br>Serial Number: %s' % (vendor, product, deviceLetter, serialNumber))

                    keyList = self.getKeyTypesChecked()

                    for keyType in keyList:

                        key, key_filename = keyInfo.getKeyInfo(keyType)

                        try:
                            if os.path.exists(os.path.join(deviceLetter,key_filename)):
                                os.remove(os.path.join(deviceLetter,key_filename))
                                self.setStatus('success',"%s Key erased." % (keyType.capwords()))
                                status_code = 0
                            else:
                                self.setStatus("info",'The %s key to erase was not found on the USB thumb drive.' % (keyType.capwords()))
                                status_code = 1
                                break
                        except:
                            self.setStatus('info','The %s key to erase was not found on the USB thumb drive.' % (keyType.capwords()))
                            status_code = 1

                    if len(keyList) == 0:
                        self.setStatus('error','You must select at least one key type to erase.')
                    else:
                        osUtils.rmDestPath(os.path.join(deviceLetter,"HTi Defaults"))
                        osUtils.rmDestPath(os.path.join(deviceLetter,"Service"))

                        if os.name == 'posix':
                            cmd = []
                            cmd.append("/bin/sync")
                            shellCommand.execute(cmd)
                            cmd = []
                            cmd.append("/usr/bin/eject")
                            cmd.append(deviceLetter)
                        else:
                            # need sync command for windows
                            cmd = []
                            cmd.append("USB_Disk_Eject")
                            cmd.append("/REMOVELETTER")
                            cmd.append(deviceLetter[:-1])
                        shellCommand.execute(cmd)

                        self.setStatus('success','Remove USB thumb drive from computer')

    # create USB key on thumb drive if possible
    def create(self):
        status_code = 1

        self.clearStatus()

        # notify user what is going on
        self.setStatus("info","Scanning USB devices attached to this computing system.")

        # instantiate USB Query class
        usbDevices = usbQuery.UsbDeviceQuery()

        # Find all removable USB devices connected to computer
        devices = usbDevices.findDevicesInClass(usbDevices.DEVICE_TYPE_USB)
        #print len(devices)

        # do some error message processing based on USB devices we find/do not find
        if len(devices) < 1:
            self.setStatus("error",'No USB thumb drives found.<br>Insert Hobart approved thumb drive and try again.')
        elif len(devices) > 1:
            self.setStatus("error",'More than one removable USB device found.<br><br>Remove all USB removable devices and try again.')
        elif len(devices) == 1:
            device = None
            if type(devices) is dict:
                device = devices.keys()[0] #Posix
            else:
                device = devices[0] # Windows

            serialNumber = usbDevices.getSerialNumber(device)
            if serialNumber == None or serialNumber.strip() == '':
                self.setStatus("error",'Thumb drive is not an approved Hobart Thumb Drive, serial number missing.<br><br>Insert a Hobart approved USB thumb drive.')
            else:
                print '  Serial Number: ', serialNumber
                deviceLetter = usbDevices.getDeviceLetter(device)
                if deviceLetter is None:
                    self.setStatus('error','Thumb drive can not be mounted.<br><br>Insert a Hobart approved USB thumb drive.')
                    return

                vendor = usbDevices.getVendorName(device)
                if vendor == None or vendor.strip() == '':
                    vendor = "Unspecified"
                product = usbDevices.getProductDescription(device)
                if product == None or product.strip() == '':
                    product = "Unspecified"

                #print '  Device Letter: ', deviceLetter
                if os.path.exists(os.path.join(deviceLetter)):
                    self.setStatus('info','Found removable USB Device:<br>Vendor: %s<br>Description: %s<br>Drive Letter: %s<br>Serial Number: %s' % (vendor, product, deviceLetter, serialNumber))

                    keyList = self.getKeyTypesChecked()

                    for keyType in keyList:

                        key, key_filename = keyInfo.getKeyInfo(keyType)

                        try:
                            data = crypto.encrypt(key, serialNumber)

                            status, result = file_io.writeBufferToFile(os.path.join(deviceLetter,key_filename),data,'wb')

                            if status == 'success':
                                self.setStatus('success',"%s Key created." % (keyType.capwords()))
                                status_code = 0
                            else:
                                self.setStatus('error','Can not write to USB thumb drive<br>Is it formatted?<br>Is it write-protected?<br>Does it have free space?')
                                status_code = 1
                                break
                        except:
                            self.setStatus('error','Can not write to USB thumb drive<br>Is it formatted?<br>Is it write-protected?<br>Does it have free space?')
                            status_code = 1
                            break

                    if status_code == 0:
                        self.extractTarball("hobart.tgz",deviceLetter)
                        if os.name == 'posix':
                            cmd = []
                            cmd.append("/bin/sync")
                            shellCommand.execute(cmd)
                            cmd = []
                            cmd.append("/usr/bin/eject")
                            cmd.append(deviceLetter)
                        else:
                            # need sync command for windows
                            cmd = []
                            cmd.append("USB_Disk_Eject")
                            cmd.append("/REMOVELETTER")
                            cmd.append(deviceLetter[:-1])
                        shellCommand.execute(cmd)

                        self.setStatus('success','Remove USB thumb drive from computer')

                    if len(keyList) == 0:
                        self.setStatus('error','You must select at least one key type to create.')
                        status_code = 1

    def checkLicense(self, licenseFilename):
        validLicense = False
        try:
            import license
            if license.validLicenseKey(licenseFilename, 'HTE_USBKEYGEN'):
                self.ui.createButton.setEnabled(True)
                self.ui.eraseButton.setEnabled(True)
                self.settings.setValue("licenseFilename", QtCore.QVariant(licenseFilename))
                validLicense = True
            else:
                self.ui.createButton.setEnabled(False)
                self.ui.eraseButton.setEnabled(False)
                self.setStatus('error', 'Application not licensed on this computing system, contact support at HT.support@itwfeg.com for a license file.')
                validLicense = False
        except:
            self.ui.createButton.setEnabled(False)
            self.ui.eraseButton.setEnabled(False)
            self.setStatus('error', 'Application not licensed on this computing system, contact support at HT.support@itwfeg.com for a license file.')
            validLicense = False
        return validLicense

    def loadLicense(self):
        fName = QtGui.QFileDialog.getOpenFileName(self, 'Select ITW Hobart License File', QtCore.QDir.homePath(),"License Files (*.lic)")
        if fName != "":
            self.licenseFilename = fName
            validLicense = self.checkLicense(self.licenseFilename)
            if validLicense:
                self.setStatus('success', 'License file imported successfully, application features enabled.')

if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)

    ## Look and feel changed to CleanLooks
    QtGui.QApplication.setStyle(QtGui.QStyleFactory.create("Cleanlooks"))
    QtGui.QApplication.setPalette(QtGui.QApplication.style().standardPalette())
    # select one or the other to get super key creator or limitedKeyCreator
    #window = UsbKeyCreator(None,[keyInfo.service_key_name, keyInfo.supervisor_key_name])
    window = UsbKeyCreator(None,keyInfo.all_keys)
    #window = UsbKeyCreator(None,[keyInfo.supervisor_key_name])
    window.show()
    sys.exit(app.exec_())
