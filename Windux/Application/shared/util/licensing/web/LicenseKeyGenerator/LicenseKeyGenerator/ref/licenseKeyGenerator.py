# -*- coding: utf-8 -*-
import hteVersionInfo

VERSION = hteVersionInfo.LICENSE_KEY_GENERATOR_VERSION

import csid
import os
import sys
import string
import hashlib
import file_io
import volumeInfo
import argparse
import getMac
import license
import usbQuery
import keyInfo
import crypto
import re

"""
Module implementing LicenseKeyGenerator.
"""
from PyQt4.QtCore import (QByteArray, QDataStream, QDate, QDir,  QIODevice,
        QObject,  QRegExp, QSettings, QString, Qt, QVariant, SIGNAL,  SLOT)
from PyQt4.QtGui import (qApp, QApplication, QDateEdit, QFileDialog,  QFrame, QGridLayout,
        QHBoxLayout, QInputDialog, QLabel, QLineEdit, QListWidgetItem,  QMainWindow,  QMessageBox,  QPushButton, QRegExpValidator,
        QStyleFactory,  QWidget)

from PyQt4.QtCore import pyqtSignature

from Ui_licenseKeyGenerator import Ui_MainWindow

class LicenseKeyGenerator(QMainWindow, Ui_MainWindow):
    """
    Class documentation goes here.
    """
    def __init__(self, parent = None):
        """
        Constructor
        """
        QMainWindow.__init__(self, parent)

        self.ui = Ui_MainWindow()

        self.ui.setupUi(self)

        for i in xrange(0, len(license.PRODUCTS)):
            item = QListWidgetItem(license.PRODUCTS[i])
            self.ui.licensedFeaturesListWidget.addItem(item)

        self.ui.targetSystemTypeComboBox.addItem("HTe")
        self.ui.targetSystemTypeComboBox.addItem("Scale")

        self.ui.cboRequestType.addItem("HTE_DEMO")
        self.ui.cboRequestType.addItem("HTE_PURCHASE")

        self.targetTypeChanged("HTe")
        self.licenseType = None

        self.ui.targetSystemTypeComboBox.activated[str].connect(self.targetTypeChanged)
        self.setWindowTitle("HTe License Key Generator (Version: %s)" % (VERSION))
        self.connect(self.ui.actionExit, SIGNAL("triggered()"), self.onExitClicked)
        self.connect(self.ui.actionCreate,  SIGNAL("triggered()"), self.onCreateClicked)
        self.connect(self.ui.getCSIDPushButton,  SIGNAL("clicked()"),  self.onGetCSIDClicked)
        self.connect(self.ui.usbSerialPushButton,  SIGNAL("clicked()"),  self.onUsbSerialClicked)
        self.connect(self.ui.getLicenseDirPushButton,  SIGNAL("clicked()"),  self.onGetLicenseDirClicked)
        self.connect(self.ui.getIPPushButton,  SIGNAL("clicked()"),  self.onGetIPClicked)
        self.connect(self.ui.licenseFilePushButton, SIGNAL("clicked()"), self.onGetLicenseFileClicked)
#
#
    def onGetIPClicked(self):
        # TBD:
        # popup dialog box to enter IP address of license server for the license
        ipAddressRegEx= re.compile("\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}")

        inputInvalid = True
        while inputInvalid:
            text, ok = QInputDialog.getText(self, 'Input Dialog', 'Enter License Server IP Address or hostname:')
            if ok:
                if ipAddressRegEx.match(str(text)):
                    self.ui.csidLineEdit.setText(text)
                    inputInvalid = False
                    self.licenseType = "SERVER_IP_ADDRESS"
                else:
                    self.ui.csidLineEdit.setText(text)
                    inputInvalid = False
                    self.licenseType = 'SERVER_IP_ADDRESS'
            else:
                inputInvalid = False
                self.licenseType = None
#
#
    def onUsbSerialClicked(self):
        serialNumber = self.getUsbDriveSerialNumber()
        if serialNumber is not None:
            self.ui.csidLineEdit.setText(serialNumber)
            self.licenseType = "USB_SERIAL_NUMBER"
        else:
            self.licenseType = None
#
#
    def targetTypeChanged(self, text):
        # update feature list based on target type
        if str(text) == 'Scale':
            self.ui.csidLineEdit.setText("")
            #self.ui.systemIdLabel.setText("USB Thumb Drive System ID:")
            self.setStatus('info', 'Please insert USB thumb drive for USB Serial or enter IP address of license server')
            self.ui.getCSIDPushButton.setEnabled(False)
            self.ui.usbSerialPushButton.setEnabled(True)
            self.ui.licenseFilePushButton.setEnabled(False)
            self.ui.getIPPushButton.setEnabled(True)
        else:
            self.ui.csidLineEdit.setText("")
            #self.ui.systemIdLabel.setText("Target Computing System ID:")
            self.setStatus("info", "Navigate to CSID file for import")
            self.ui.getCSIDPushButton.setEnabled(True)
            self.ui.usbSerialPushButton.setEnabled(False)
            self.ui.getIPPushButton.setEnabled(False)
            self.ui.licenseFilePushButton.setEnabled(True)

        self.ui.licensedFeaturesListWidget.clear()
        for i in xrange(0, len(license.PRODUCTS)):
            if license.PRODUCTS[i][:6] == 'SCALE_':
                if str(text) == 'Scale':
                    item = QListWidgetItem(license.PRODUCTS[i])
                    self.ui.licensedFeaturesListWidget.addItem(item)
            else:
                if str(text)  == 'HTe':
                    item = QListWidgetItem(license.PRODUCTS[i])
                    self.ui.licensedFeaturesListWidget.addItem(item)
#
#
    def getUsbDriveSerialNumber(self):
        # notify user what is going on
        self.setStatus("info","Scanning USB devices attached to this computing system.")

        # instantiate USB Query class
        usbDevices = usbQuery.UsbDeviceQuery()

        # Find all removable USB devices connected to computer
        devices = usbDevices.findDevicesInClass(usbDevices.DEVICE_TYPE_USB)
        #print len(devices)

        self.licenseType = None

        # do some error message processing based on USB devices we find/do not find
        if len(devices) < 1:
            self.setStatus("error",'No USB thumb drives found. Insert Hobart approved thumb drive and try again.')
        elif len(devices) > 1:
            self.setStatus("error",'More than one removable USB device found.<br><br>Remove all USB removable devices and try again.')
        elif len(devices) == 1:
            serialNumber = usbDevices.getSerialNumber(devices[0])
            if serialNumber == None or string.strip(serialNumber) == '':
                self.setStatus("error",'Device not suitable for USB Key creation.<br><br>Insert a Hobart approved USB thumb drive.')
                return None
            else:
                self.setStatus('info',"Eject USB thumb drive from Windows OS. DO NOT simply pull the USB thumb drive out of the USB port.")
                self.licenseType = "USB_SERIAL_NUMBER"
                return serialNumber
#
#
    def tr(self, text):
        return self.trUtf8(text)
#
#
    def onGetLicenseDirClicked(self):
        directory = str(QFileDialog.getExistingDirectory(self, self.tr("Select Destination Directory for License File"),
                   QDir.homePath()))
        if directory != "":
            if string.strip(str(self.ui.licenseKeyFilenameLineEdit.text())) != "":
                licenseKeyFilename = os.path.join(directory, str(self.ui.licenseKeyFilenameLineEdit.text()))
                self.ui.licenseKeyFilenameLineEdit.setText(licenseKeyFilename)
            else:
                self.ui.licenseKeyFilenameLineEdit.setText(directory)

        else:
            self.setStatus('error', 'No destination directory selected')
#
#
    def onGetCSIDClicked(self):
        # popup dialog box to enter IP address of license server for the license
        csidRegEx= re.compile("[A-F,0-9]+")

        inputInvalid = True
        while inputInvalid:
            text, ok = QInputDialog.getText(self, 'Input Dialog', 'Enter CSID Manually:')
            if ok:
                if csidRegEx.match(str(text)):
                    self.ui.csidLineEdit.setText(text)
                    inputInvalid = False
                    self.licenseType = "CSID"
            else:
                inputInvalid = False
                self.licenseType = None
#
#
    def onGetLicenseFileClicked(self):
        self.licenseType = None
        fName = QFileDialog.getOpenFileName(self,
                                            self.tr("Select Target Computing System ID file to import"),
                                            QDir.homePath(),
                                            self.tr("License Request File (*.req)"),
                                            None)
#
        if not os.path.isfile(fName):
            self.setStatus('error', 'Cannot read selected request file, try again...')
            return
#
        with open(fName, 'rb') as request_file:
            request_text = request_file.read()
#
        resp_indv_regex       = re.compile("Responsible Individual:(?P<resp_indv>.*)")
        company_name_regex    = re.compile("Company Name:(?P<company_name>.*)")
        company_address_regex = re.compile("Company Address:(?P<company_address>.*)")
        email_address_regex   = re.compile("Email Address:(?P<email_address>.*)")
        phone_number_regex    = re.compile("Phone Number:(?P<phone_number>.*)")
        sales_rep_regex       = re.compile("Sales Rep:(?P<sales_rep>.*)")
        request_type_regex    = re.compile("Request Type:(?P<request_type>.*)")
        license_type_regex    = re.compile("License Type Requested: \\[(?P<license_type>.*)\\]")
        license_feature_regex = re.compile("License Feature\\(s\\) Requested: \\[(?P<license_features>.*)\\]")
        num_licenses_regex    = re.compile("Number of Licenses:(?P<num_licenses>.*)")
        csid_tag_regex        = re.compile("(?P<csid_tag>.*)=(?P<csid>.*)")

        groups_to_regexes = {'resp_indv': resp_indv_regex,
                             'company_name': company_name_regex,
                             'company_address': company_address_regex,
                             'email_address': email_address_regex,
                             'phone_number': phone_number_regex,
                             'sales_rep': sales_rep_regex,
                             'request_type': request_type_regex,
                             'license_type': license_type_regex,
                             'license_features': license_feature_regex,
                             'num_licenses': num_licenses_regex,
                             'csid': csid_tag_regex,
                             }

        groups_to_controls = {'resp_indv': self.ui.responsibleIndividualLineEdit,
                              'company_name': self.ui.companyNameLineEdit,
                              'company_address': self.ui.companyAddressLineEdit,
                              'email_address': self.ui.txtEmailAddress,
                              'phone_number': self.ui.txtPhoneNumber,
                              'sales_rep': self.ui.txtSalesRep,
                              'num_licenses': self.ui.allotmentLineEdit,
                              'csid': self.ui.csidLineEdit,
                              }
#
        for group, regex in groups_to_regexes.items():
            match = regex.search(request_text)
            text  = match.group(group)

            if group == 'license_features':
                control = self.ui.licensedFeaturesListWidget
                features = text.split(',')
                for feature in features:
                    matched_items = control.findItems(feature, Qt.MatchExactly)
                    if not matched_items:
                        continue
#
                    item = matched_items[-1]
                    control.setItemSelected(item, True)
#
                continue
#
            elif group == 'request_type':
                index = self.ui.cboRequestType.findText(text.strip())
                self.ui.cboRequestType.setCurrentIndex(index)
#
            elif group == 'license_type':
                control = self.ui.licensedFeaturesListWidget
                feature = text.strip()
                matched_items = control.findItems(feature, Qt.MatchExactly)
                if not matched_items:
                    continue
#
                item = matched_items[-1]
                control.setItemSelected(item, True)
#
            if group not in groups_to_controls:
                continue
#
            control = groups_to_controls[group]
            control.setText(text)
#
        index = self.ui.targetSystemTypeComboBox.findText('HTe')
        self.ui.targetSystemTypeComboBox.setCurrentIndex(index)
#
        self.licenseType = "CSID"
#
#
    def closeEvent(self, event):
        event.accept()

    def onExitClicked(self):
        qApp.quit()

    def onCreateClicked(self):
        individualName = string.upper(string.strip(str(self.ui.responsibleIndividualLineEdit.text())))
        if individualName == '':
            self.setStatus("error", "Responsible Individual can not be blank")
            return

        companyName = string.upper(string.strip(str(self.ui.companyNameLineEdit.text())))
        if companyName == '':
            self.setStatus("error",  "Company Name can not be blank")
            return

        address = string.upper(string.strip(str(self.ui.companyAddressLineEdit.text())))
        if address == '':
            self.setStatus("error",  "Company Address can not be blank")
            return

        if self.licenseType == 'USB_SERIAL_NUMBER' or self.licenseType == 'SERVER_IP_ADDRESS':
            serialNumber = string.strip(str(self.ui.csidLineEdit.text()))
            if serialNumber == '':
                self.setStatus("error",  "Target Computing System ID can not be blank")
                return
            macAddress = license.NULL_MAC_ADDRESS
        elif self.licenseType == 'CSID':
            csidText = string.strip(str(self.ui.csidLineEdit.text()))
            if csidText == '':
                self.setStatus("error",  "Target Computing System ID can not be blank")
                return
            macAddress,  serialNumber = csid.extractCSInfo(csidText)
        else:
            self.setStatus('error', 'Target computing system id type not specified')
            return

        productNames = []
        for product in self.ui.licensedFeaturesListWidget.selectedItems():
            productNames.append(str(product.text()))

        if len(productNames) == 0:
            self.setStatus("error",  "At least one license feature must be selected")
            return

        licenseTimeout = string.strip(str(self.ui.licenseTimeoutSpinBox.value()))
        if licenseTimeout == '':
            self.setStatus("error",  "License Timeout can not be blank")
            return

        licenseAllotment = string.strip(str(self.ui.allotmentLineEdit.text()))
        if licenseAllotment == '':
            self.setStatus("error",  "License Allotment can not be blank")
            return

        licenseKeyFilename = string.strip(str(self.ui.licenseKeyFilenameLineEdit.text()))
        if licenseKeyFilename == '':
            self.setStatus("error",  "License Key Filename can not be blank")
            return

        if licenseKeyFilename[-4:] != '.lic':
            self.setStatus("error",  "License Key Filename must end in .lic")
            return

        if os.path.dirname(licenseKeyFilename) == "":
            self.setStatus("error",  "No destination directory specified for license key filename")
            return

        if os.path.basename(licenseKeyFilename) == "":
            self.setStatus("error",  "No destination filename specified for license key")
            return

        licenseActivationDate = None

        status,  reason,  licenseData = license.createLicenseFile(licenseKeyFilename, individualName, companyName, address, productNames, self.licenseType, licenseActivationDate, licenseAllotment, licenseTimeout, macAddress, serialNumber)

        if status == 'success':
            self.ui.licenseKeyPlainTextEdit.setPlainText(string.join(licenseData, '\n'))
            self.setStatus("success","License key file created")
        else:
            self.setStatus(status,  reason)

        #self.licenseType = None
        # test if valid license key produced for machine license key was generated on
        #if self.validLicenseKey(licenseKeyFilename,'SCALEMGMT'):
        #    print 'Valid license key'
        #else:
        #    print 'Invalid license key'

    def setStatus(self, status, message):
        if string.lower(status) == 'error':
            color = "red"
        elif string.lower(status) == 'success':
            color = "black"
        elif string.lower(status) == 'info':
            color = "green"
        elif string.lower(status) == 'warning':
            color = "yellow"

        self.statusBar().showMessage(self.tr("%s: %s" % (string.upper(status), message)))

if __name__ == "__main__":
    app = QApplication(sys.argv)

    ## Look and feel changed to CleanLooks
    QApplication.setStyle(QStyleFactory.create("Cleanlooks"))
    QApplication.setPalette(QApplication.style().standardPalette())
    window = LicenseKeyGenerator()
    window.show()
    sys.exit(app.exec_())
