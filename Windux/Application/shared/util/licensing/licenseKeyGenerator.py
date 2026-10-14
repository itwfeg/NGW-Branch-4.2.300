# -*- coding: utf-8 -*-
try:
    import environment_manager

    environment_manager.setup()
except ImportError:
    environment_manager = None

import hteVersionInfo

VERSION = hteVersionInfo.LICENSE_KEY_GENERATOR_VERSION

import csid
import os
import sys
import string
import license
import usbQuery
import re
import osUtils
from datetime import datetime
from datetime import timedelta
import win32clipboard

"""
Module implementing LicenseKeyGenerator.
"""
from PyQt4.QtCore import (QDir, Qt, SIGNAL, QStringList)
from PyQt4.QtGui import (QApplication, QFileDialog,
                         QInputDialog, QListWidgetItem, QMainWindow,
                         QStyleFactory)

from Ui_licenseKeyGenerator import Ui_MainWindow


class LicenseKeyGenerator(QMainWindow, Ui_MainWindow):
    """
    Class documentation goes here.
    """

    def __init__(self, parent=None):
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

        self.ui.getCSIDPushButton.hide()

        self.ui.targetSystemTypeComboBox.activated[str].connect(self.targetTypeChanged)
        self.setWindowTitle("HTe License Key Generator (Version: %s)" % VERSION)
        self.connect(self.ui.actionExit, SIGNAL("triggered()"), self.onExitClicked)
        self.connect(self.ui.actionCreate, SIGNAL("triggered()"), self.onCreateClicked)
        self.connect(self.ui.createLicensePushButton, SIGNAL("clicked()"), self.onCreateClicked)
        self.connect(self.ui.getCSIDPushButton, SIGNAL("clicked()"), self.onGetCSIDClicked)
        self.connect(self.ui.usbSerialPushButton, SIGNAL("clicked()"), self.onUsbSerialClicked)
        self.connect(self.ui.getLicenseDirPushButton, SIGNAL("clicked()"), self.onGetLicenseDirClicked)
        self.connect(self.ui.getIPPushButton, SIGNAL("clicked()"), self.onGetIPClicked)
        self.connect(self.ui.licenseFilePushButton, SIGNAL("clicked()"), self.onImportLicenseRequestClicked)
        self.connect(self.ui.importFromClipboardButton, SIGNAL("clicked()"), self.importFromClipboardButtonClicked)
        self.connect(self.ui.licenseTimeoutSpinBox, SIGNAL("valueChanged(int)"), self.onTimeoutValueChanged)
        self.connect(self.ui.ignoreCSIDPushButton, SIGNAL("clicked()"), self.onIgnoreCSIDClicked)

        self.reqLicenseeName = ""
        self.reqCompanyName = ""
        self.reqLicenseType = ""
        self.reqFileDir = ""

    def onGetIPClicked(self):
        # TBD:
        # popup dialog box to enter IP address of license server for the license
        ipAddressRegEx = re.compile("\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}")

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

    def onIgnoreCSIDClicked(self):
        self.ui.csidLineEdit.setText("IGNORE")
        self.licenseType = 'CSID'

    def onUsbSerialClicked(self):
        results = self.getUsbDriveSerialNumber()

        if not results:
            return

        serialNumber, driveLetter = results

        if serialNumber is not None:
            self.ui.csidLineEdit.setText(serialNumber)
            self.ui.licenseKeyFilenameLineEdit.setText(driveLetter + "\\")
            self.licenseType = "USB_SERIAL_NUMBER"
        else:
            self.licenseType = None

    def targetTypeChanged(self, text):
        self.ui.csidLineEdit.setText("")
        # self.ui.systemIdLabel.setText("USB Thumb Drive System ID:")
        self.ui.licensedFeaturesListWidget.clear()
        self.ui.getCSIDPushButton.setEnabled(False)
        self.ui.usbSerialPushButton.setEnabled(True)

        if str(text) == 'Scale':
            self.ui.csidLineEdit.setText("")
            self.setStatus('info', 'Please insert USB thumb drive for USB Serial or enter IP address of license server')
            self.ui.getCSIDPushButton.setEnabled(False)
            self.ui.usbSerialPushButton.setEnabled(True)
            self.ui.licenseFilePushButton.setEnabled(False)
            self.ui.cboRequestType.setEnabled(False)
            self.ui.getIPPushButton.setEnabled(True)
            self.ui.ignoreCSIDPushButton.setEnabled(False)
        else:
            self.ui.csidLineEdit.setText("")
            self.setStatus("info",
                           "Navigate to request file for import, and insert USB thumb drive for USB Serial number")
            self.ui.getCSIDPushButton.setEnabled(True)
            self.ui.usbSerialPushButton.setEnabled(True)
            self.ui.getIPPushButton.setEnabled(False)
            self.ui.licenseFilePushButton.setEnabled(True)
            self.ui.cboRequestType.setEnabled(True)
            self.ui.ignoreCSIDPushButton.setEnabled(True)

        for i in xrange(0, len(license.PRODUCTS)):
            if license.PRODUCTS[i][:6] == 'SCALE_':
                if str(text) == 'Scale':
                    item = QListWidgetItem(license.PRODUCTS[i])
                    self.ui.licensedFeaturesListWidget.addItem(item)
            elif str(text) == 'HTe':
                item = QListWidgetItem(license.PRODUCTS[i])
                self.ui.licensedFeaturesListWidget.addItem(item)

    def getUsbDriveSerialNumber(self):
        # notify user what is going on
        self.setStatus("info", "Scanning USB devices attached to this computing system.")

        # instantiate USB Query class
        usbDevices = usbQuery.UsbDeviceQuery()

        # Find all removable USB devices connected to computer
        devices = usbDevices.findDevicesInClass(usbDevices.DEVICE_TYPE_USB)
        # print len(devices)

        self.licenseType = None

        # do some error message processing based on USB devices we find/do not find
        if len(devices) < 1:
            self.setStatus("error", 'No USB thumb drives found. Insert Hobart approved thumb drive and try again.')
        else:
            if len(devices) > 1:
                listOfDevices = QStringList()
                for device in devices:
                    listOfDevices.append(device.Caption)

                selectedDevice, userClickedOK = QInputDialog.getItem(self,
                                                                     'Multiple USB Devices Detected',
                                                                     'Please select the USB that you would like to use:',
                                                                     listOfDevices, 0, False)

                if not userClickedOK:
                    self.setStatus("error", "Multiple USB drives found. Please unplug other USB devices")
                    return
                else:
                    for device in devices:
                        if device.Caption == selectedDevice:
                            devices[0] = device

            serialNumber = usbDevices.getSerialNumber(devices[0])
            driveLetter = usbDevices.getDeviceLetter(devices[0])

            if serialNumber is None or string.strip(serialNumber) == '':
                self.setStatus("error",
                               'Device not suitable for USB Key creation.<br><br>Insert a Hobart approved USB thumb drive.')
                return None
            else:
                self.setStatus('info',
                               "Eject USB thumb drive from Windows OS. DO NOT simply pull the USB thumb drive out of the USB port.")
                self.licenseType = "USB_SERIAL_NUMBER"
                return serialNumber, driveLetter

    def tr(self, text, disambiguation=None, n=-1):
        return self.trUtf8(text)

    def onGetLicenseDirClicked(self):
        currentPath = str(self.ui.licenseKeyFilenameLineEdit.text())

        if not os.path.exists(currentPath):
            currentPath = QDir.homePath()
            self.ui.licenseKeyFilenameLineEdit.setText("")

        filepath = str(QFileDialog.getSaveFileName(self,
                                                   self.tr("Select path to save license file to"),
                                                   currentPath,
                                                   self.tr("License Files (*.lic)")))
        if not filepath:
            return

        self.ui.licenseKeyFilenameLineEdit.setText(filepath)

    def onGetCSIDClicked(self):
        # popup dialog box to enter IP address of license server for the license
        csidRegEx = re.compile("[A-F,0-9]+")

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

    def onImportLicenseRequestClicked(self):
        self.licenseType = None
        fName = QFileDialog.getOpenFileName(self,
                                            self.tr("Select Target Computing System ID file to import"),
                                            QDir.homePath(),
                                            self.tr("License Request File (*.req)"),
                                            None)

        if not os.path.isfile(fName):
            self.setStatus('error', 'Cannot read selected request file, try again...')
            return

        with open(fName, 'rb') as request_file:
            request_text = request_file.read()

        resp_indv_regex = re.compile("Responsible Individual:(?P<resp_indv>.*)")
        company_name_regex = re.compile("Company Name:(?P<company_name>.*)")
        company_address_regex = re.compile("Company Address:(?P<company_address>.*)")
        email_address_regex = re.compile("Email Address:(?P<email_address>.*)")
        phone_number_regex = re.compile("Phone Number:(?P<phone_number>.*)")
        sales_rep_regex = re.compile("Sales Rep:(?P<sales_rep>.*)")
        request_type_regex = re.compile("Request Type:(?P<request_type>.*)")
        license_type_regex = re.compile("License Type Requested: \\[(?P<license_type>.*)\\]")
        license_feature_regex = re.compile("License Feature\\(s\\) Requested: \\[(?P<license_features>.*)\\]")
        num_licenses_regex = re.compile("Number of Licenses:(?P<num_licenses>.*)")
        csid_tag_regex = re.compile("(?P<csid_tag>.*)=(?P<csid>.*)")

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

        for group, regex in groups_to_regexes.items():
            match = regex.search(request_text)

            if not match:
                print("Could not find matching regex for: " + group)
                continue

            text = match.group(group)

            if group == 'license_features':
                control = self.ui.licensedFeaturesListWidget
                features = text.split(',')
                for feature in features:
                    matched_items = control.findItems(feature, Qt.MatchExactly)

                    if not matched_items:
                        continue

                    item = matched_items[-1]
                    control.setItemSelected(item, True)

                continue

            elif group == 'request_type':
                index = self.ui.cboRequestType.findText(text.strip())
                self.ui.cboRequestType.setCurrentIndex(index)

                # HTE_DEMO = index 0 and HTE_PURCHASE = index 1
                if index == 1:
                    self.ui.licenseTimeoutSpinBox.setValue(365)

            elif group == 'license_type':
                control = self.ui.licensedFeaturesListWidget
                feature = text.strip()
                matched_items = control.findItems(feature, Qt.MatchExactly)
                if not matched_items:
                    continue

                if feature == "HTE_SCALEMGMT_MULTISTORE":
                    self.reqLicenseType = "Multi"
                else:
                    self.reqLicenseType = "Pro"

                item = matched_items[-1]
                control.setItemSelected(item, True)

            elif group == "csid":
                # The request had a CSID so make that the license type.
                self.licenseType = "CSID"

            elif group == "resp_indv":
                try:
                    self.reqLicenseeName = text.split()[0].capitalize()
                except:
                    self.reqLicenseeName = ""

            elif group == "company_name":
                try:
                    self.reqCompanyName = text.split()[0].capitalize()[0:8]
                except:
                    self.reqCompanyName = ""

            if group not in groups_to_controls:
                continue

            control = groups_to_controls[group]
            control.setText(text)

        index = self.ui.targetSystemTypeComboBox.findText('HTe')
        self.ui.targetSystemTypeComboBox.setCurrentIndex(index)

        if not self.licenseType == "CSID":
            self.licenseType = "USB_SERIAL_NUMBER"

        if self.reqLicenseType and self.reqCompanyName and self.reqLicenseeName:
            try:
                newDate = datetime.now() + timedelta(days=self.ui.licenseTimeoutSpinBox.value())
                formattedDate = newDate.strftime("%Y%m%d")

                if self.ui.licenseTimeoutSpinBox.value() == 0:
                    expirationText = "_lifetime"
                else:
                    expirationText = "_expires"

                self.reqFileDir = os.path.dirname(str(fName))
                newPath = os.path.join(self.reqFileDir, "HTe" + self.reqLicenseType + "_" + self.reqLicenseeName +
                                       self.reqCompanyName + expirationText + formattedDate + ".lic")

                self.ui.licenseKeyFilenameLineEdit.setText(newPath)
            except:
                pass

    def importFromClipboardButtonClicked(self):
        try:
            # get clipboard data
            win32clipboard.OpenClipboard()
            request_text = win32clipboard.GetClipboardData()
            win32clipboard.CloseClipboard()

            resp_indv_regex = re.compile("Responsible Individual:(?P<resp_indv>.*)")
            company_name_regex = re.compile("Company Name:(?P<company_name>.*)")
            company_address_regex = re.compile("Company Address:(?P<company_address>.*)")
            email_address_regex = re.compile("Email Address:(?P<email_address>.*)")
            phone_number_regex = re.compile("Phone Number:(?P<phone_number>.*)")
            sales_rep_regex = re.compile("Sales Rep:(?P<sales_rep>.*)")
            request_type_regex = re.compile("Request Type:(?P<request_type>.*)")
            license_type_regex = re.compile("License Type Requested: \\[(?P<license_type>.*)\\]")
            license_feature_regex = re.compile("License Feature\\(s\\) Requested: \\[(?P<license_features>.*)\\]")
            num_licenses_regex = re.compile("Number of Licenses:(?P<num_licenses>.*)")
            csid_tag_regex = re.compile("(?P<csid_tag>.*)=(?P<csid>.*)")

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

            for group, regex in groups_to_regexes.items():
                match = regex.search(request_text)

                if not match:
                    print ("Could not find matching regex for: " + group)
                    continue

                text = match.group(group)

                if group == 'license_features':
                    control = self.ui.licensedFeaturesListWidget
                    features = text.split(',')
                    for feature in features:
                        matched_items = control.findItems(feature, Qt.MatchExactly)

                        if not matched_items:
                            continue

                        item = matched_items[-1]
                        control.setItemSelected(item, True)

                    continue

                elif group == 'request_type':
                    index = self.ui.cboRequestType.findText(text.strip())
                    self.ui.cboRequestType.setCurrentIndex(index)

                    # HTE_DEMO = index 0 and HTE_PURCHASE = index 1
                    if index == 1:
                        self.ui.licenseTimeoutSpinBox.setValue(365)

                elif group == 'license_type':
                    control = self.ui.licensedFeaturesListWidget
                    feature = text.strip()
                    matched_items = control.findItems(feature, Qt.MatchExactly)
                    if not matched_items:
                        continue

                    if feature == "HTE_SCALEMGMT_MULTISTORE":
                        self.reqLicenseType = "Multi"
                    else:
                        self.reqLicenseType = "Pro"

                    item = matched_items[-1]
                    control.setItemSelected(item, True)

                elif group == "csid":
                    # The request had a CSID so make that the license type.
                    self.licenseType = "CSID"

                elif group == "resp_indv":
                    try:
                        self.reqLicenseeName = text.split()[0].capitalize()
                    except:
                        self.reqLicenseeName = ""

                elif group == "company_name":
                    try:
                        self.reqCompanyName = text.split()[0].capitalize()[0:8]
                    except:
                        self.reqCompanyName = ""

                if group not in groups_to_controls:
                    continue

                control = groups_to_controls[group]
                control.setText(text)

            index = self.ui.targetSystemTypeComboBox.findText('HTe')
            self.ui.targetSystemTypeComboBox.setCurrentIndex(index)

            if not self.licenseType == "CSID":
                self.licenseType = "USB_SERIAL_NUMBER"

            if not self.ui.licenseKeyFilenameLineEdit.text():
                self.ui.licenseKeyFilenameLineEdit.setText(self.getFilePath())
        except:
            self.setStatus("warning",
                           "Unable to parse clipboard. Save clipboard data to .req file and import instead.")
            pass

    def getFilePath(self):
        if self.reqLicenseType and self.reqCompanyName and self.reqLicenseeName:
            try:
                newDate = datetime.now() + timedelta(days=self.ui.licenseTimeoutSpinBox.value())
                formattedDate = newDate.strftime("%Y%m%d")

                if self.ui.licenseTimeoutSpinBox.value() == 0:
                    expirationText = "_lifetime"
                else:
                    expirationText = "_expires"

                newPath = os.path.join(self.reqFileDir, "HTe" + self.reqLicenseType + "_" + self.reqLicenseeName +
                                       self.reqCompanyName + expirationText + formattedDate + ".lic")

                return newPath
            except:
                pass
        else:
            return ""

    def onTimeoutValueChanged(self, value):
        self.statusBar().showMessage("")
        newDate = datetime.now() + timedelta(days=value)

        if value == 0:
            self.setStatus("warning",
                           "This license will NEVER expire, since 0 days was entered")
            self.ui.expiryDateLineEdit.setText("Does not expire")
        else:
            if newDate.weekday() > 4:
                # Saturday or Sunday
                self.setStatus("warning",
                               "This license will expire on a weekend. HT Support may not be available on that day.")
            self.ui.expiryDateLineEdit.setText(newDate.strftime("%a, %b %d, %Y"))

        newPath = self.getFilePath()

        if newPath:
            self.ui.licenseKeyFilenameLineEdit.setText(newPath)


    def closeEvent(self, event):
        event.accept()

    def onExitClicked(self):
        QApplication.instance().quit()

    def onCreateClicked(self):
        individualName = string.upper(string.strip(str(self.ui.responsibleIndividualLineEdit.text())))
        if individualName == '':
            self.setStatus("error", "Responsible Individual can not be blank")
            return

        companyName = string.upper(string.strip(str(self.ui.companyNameLineEdit.text())))
        if companyName == '':
            self.setStatus("error", "Company Name can not be blank")
            return

        address = string.upper(string.strip(str(self.ui.companyAddressLineEdit.text())))
        if address == '':
            self.setStatus("error", "Company Address can not be blank")
            return

        if self.licenseType == 'USB_SERIAL_NUMBER' or self.licenseType == 'SERVER_IP_ADDRESS':
            serialNumber = string.strip(str(self.ui.csidLineEdit.text()))
            if serialNumber == '':
                self.setStatus("error", "Target Computing System ID can not be blank")
                return
            macAddress = license.NULL_MAC_ADDRESS
        elif self.licenseType == 'CSID':
            csidText = string.strip(str(self.ui.csidLineEdit.text()))
            if csidText == '':
                self.setStatus("error", "Target Computing System ID can not be blank")
                return
            macAddress, serialNumber = csid.extractCSInfo(csidText)
        else:
            self.setStatus('error', 'Target computing system id type not specified')
            return

        productNames = []

        for product in self.ui.licensedFeaturesListWidget.selectedItems():
            productNames.append(str(product.text()))

        if len(productNames) == 0:
            self.setStatus("error", "At least one license feature must be selected")
            return

        licenseTimeout = string.strip(str(self.ui.licenseTimeoutSpinBox.value()))

        if licenseTimeout == '':
            self.setStatus("error", "License Timeout can not be blank")
            return

        licenseAllotment = string.strip(str(self.ui.allotmentLineEdit.text()))

        if licenseAllotment == '':
            self.setStatus("error", "License Allotment can not be blank")
            return

        licenseKeyFilename = string.strip(str(self.ui.licenseKeyFilenameLineEdit.text()))

        if licenseKeyFilename == '':
            self.setStatus("error", "License Key Filename can not be blank")
            return

        if licenseKeyFilename[-4:] != '.lic':
            self.setStatus("error", "License Key Filename must end in .lic")
            return

        if os.path.dirname(licenseKeyFilename) == "":
            self.setStatus("error", "No destination directory specified for license key filename")
            return

        if os.path.basename(licenseKeyFilename) == "":
            self.setStatus("error", "No destination filename specified for license key")
            return

        licenseActivationDate = None

        status, reason, licenseData = license.createLicenseFile(licenseKeyFilename, individualName, companyName,
                                                                address, productNames, self.licenseType,
                                                                licenseActivationDate, licenseAllotment, licenseTimeout,
                                                                macAddress, serialNumber)

        if status == 'success':
            self.ui.licenseKeyPlainTextEdit.setPlainText(string.join(licenseData, '\n'))
            self.setStatus("success", "License key file created")
        else:
            self.setStatus(status, reason)

    def setStatus(self, status, message):
        self.statusBar().showMessage(self.tr("%s: %s" % (string.upper(status), message)))


if __name__ == "__main__":
    app = QApplication(sys.argv)

    # Look and feel changed to CleanLooks
    QApplication.setStyle(QStyleFactory.create("windowsvista"))
    QApplication.setPalette(QApplication.style().standardPalette())
    window = LicenseKeyGenerator()
    window.show()
    sys.exit(app.exec_())
