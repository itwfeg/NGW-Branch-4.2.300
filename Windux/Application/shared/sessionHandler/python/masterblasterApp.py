#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import sys, os, time, re
import pexpect
import threading
import shutil
from distutils.version import LooseVersion

# setup python path
sys.path.append('/usr/lib/python2.7/dist-packages')
sys.path.append('/usr/lib/python2.7/site-packages')
sys.path.append('/usr/local/hobart/python')

import messageBox
import base64
import md5checksum
import logging
import getIpAddress
import totp

# import PyQt5 modules
from PyQt5 import QtCore
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *

LOGFILE = "/usr/local/hobart/masterblaster.log"
USD_FILE_PATH = "/usr/local/hobart/releases"
MOUNTPOINT = "/media/thumb"
MAKE_SD_LOC = "/usr/local/hobart/python/masterblaster_make_SD.py"
STORE_LOGO_PATH = "/usr/local/hobart/images/system/selfServe/storelogo.png"
VERSION_NUMBER = "1.0.1"

DEBUG = False


class MasterBlasterStartScreen(QMainWindow):
    usdFilePath = ''
    devicePath = "/dev/mmcblk0"
    devicePart = "p"
    deviceSize = ''
    releaseNumber = None
    tarballs = []
    pageIndex = 0
    lastPage = 0  # gets set in __init__

    def __init__(self):

        QMainWindow.__init__(self)
        self.tarballs = {}
        self.StackedLayout = QStackedLayout()
        self.StackedLayout.addWidget(self.createLanding())
        self.StackedLayout.addWidget(self.createSelectUsd())
        self.StackedLayout.addWidget(self.createInfoPage())
        self.StackedLayout.currentChanged.connect(self.widgetChanged)
        self.MainWidget = QWidget()
        self.MainWidget.setLayout(self.StackedLayout)
        self.setCentralWidget(self.MainWidget)
        self.lastPage = self.StackedLayout.count() - 1
        self.ipThread.start()
        self.serviceThread.start()
        self.serviceTimer = QTimer(self)
        self.serviceTimer.timeout.connect(self.timedRefresh)
        # 1ms  * 60 = 1sec * 10min = timeout every 10 min
        self.serviceTimer.start(1000 * 60 * 10)
        self.clockTimer = QTimer(self)
        self.clockTimer.timeout.connect(self.updateTime)
        # 1ms  * 60 = 1sec * .5min = timeout every 30 sec
        self.clockTimer.start(1000 * 60 * .5)


    # Setup the main introduction window
    def createLanding(self):
        res = QDesktopWidget().screenGeometry()
        Widget = QWidget()
        self.landLayout = QGridLayout()

        self.ipLabel = QLabel(self)
        self.ipLabel.setAlignment(Qt.AlignLeft | Qt.AlignTop)
        self.ipLabel.setFont(QFont("Monospace", 20))
        self.ipLabel.setText("IP: N/A")
        self.ipThread = updateIPThread()
        self.ipThread.ip_ready.connect(self.updateIP)

        self.serviceRefreshButton = QPushButton()
        self.serviceRefreshButton.setFixedSize(res.height() / 10, res.height() / 20)
        self.serviceRefreshButton.setIcon(QIcon(os.path.join("/", "usr", "local", "hobart", "python", "Refresh_icon.png")))
        self.serviceRefreshButton.clicked.connect(self.refreshServiceCode)

        self.serviceCode = QLabel(self)
        self.serviceCode.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        self.serviceCode.setFont(QFont("Monospace", 20))
        self.serviceCode.setText("Service Key: N/A")
        self.serviceThread = updateServiceThread()
        self.serviceThread.service_ready.connect(self.updateService)

        self.serviceLayout = QBoxLayout(QBoxLayout.LeftToRight)
        self.serviceLayout.addWidget(self.serviceRefreshButton)
        self.serviceLayout.addWidget(self.serviceCode)

        image = QImage(STORE_LOGO_PATH)
        image = image.scaledToHeight(res.height() / 4, Qt.SmoothTransformation)
        self.landLogo = QLabel()
        self.landLogo.setPixmap(QPixmap.fromImage(image))

        self.landTitle = QLabel(self)
        self.landTitle.setAlignment(Qt.AlignHCenter)
        try:
            with open(os.path.join('/', 'SOFTWARE_VERSION_INFO')) as versionInfoFile:
                version = versionInfoFile.readline().strip().split(':')
                appVersion = version[1].strip()
        except:
            appVersion = ""
            pass

        self.landTitle.setText("MASTERBLASTER ({0})\nSD Card Creation Utility\n Application Version: {1}"
                               .format(VERSION_NUMBER, appVersion))
        self.landTitle.setFont(QFont("Monospace", 20, QFont.Bold))

        self.landNextButton = QPushButton("Begin")
        self.landNextButton.setFont(QFont("Monospace", 20))
        self.landNextButton.setFixedSize(res.width() / 5, res.height() / 3)
        self.landNextButton.clicked.connect(self.landNextClicked)

        self.powerButton = QPushButton()
        self.powerButton.setAutoFillBackground(True)
        self.powerButton.setIcon(QIcon(os.path.join("/", "usr", "local", "hobart", "python", "shutdownIconBlack.png")))
        self.powerButton.setIconSize(QSize(res.width() / 20, res.width() / 20))
        self.powerButton.setFixedSize(res.width() / 17, res.width() / 17)
        self.powerButton.clicked.connect(self.powerButtonClicked)

        self.timeLabel = QLabel(self)
        self.timeLabel.setAlignment(Qt.AlignLeft | Qt.AlignTop)
        self.timeLabel.setFont(QFont("Monospace", 15))
        self.timeLabel.setText(QDateTime().currentDateTime().toString("MMM dd yyyy hh:mm AP"))



        self.landLayout.addWidget(self.ipLabel, 0, 0, Qt.AlignLeft | Qt.AlignTop)
        self.landLayout.addLayout(self.serviceLayout, 0, 1, Qt.AlignRight | Qt.AlignTop)
        self.landLayout.addWidget(self.landLogo, 1, 0, 1, 2, Qt.AlignCenter | Qt.AlignTop)
        self.landLayout.addWidget(self.landTitle, 2, 0, 1, 2, Qt.AlignCenter | Qt.AlignTop)
        self.landLayout.addWidget(self.landNextButton, 3, 0, 1, 2, Qt.AlignCenter)
        self.landLayout.addWidget(self.powerButton, 4, 0, Qt.AlignLeft | Qt.AlignBottom)
        self.landLayout.addWidget(self.timeLabel, 4, 1, Qt.AlignRight | Qt.AlignBottom)

        # For developmental purposes
        #self.landQuit = QPushButton("Exit")
        #self.landQuit.clicked.connect(self.close)
        #self.landLayout.addWidget(self.landQuit)

        Widget.setLayout(self.landLayout)
        return Widget

    # Widget for getting user input
    def createSelectUsd(self):
        res = QDesktopWidget().screenGeometry()
        Widget = QWidget()
        self.usdLayout = QGridLayout()

        self.usdTitle = QLabel(self)
        self.usdTitle.setAlignment(Qt.AlignHCenter)
        self.usdTitle.setText("Please Select The Build Version Number.")
        self.usdTitle.setFont(QFont("Monospace", 20, QFont.Bold))

        self.usdVersionPicker = QComboBox()
        self.usdVersionPicker.setStyleSheet("combobox-popup: 0")
        self.usdVersionPicker.setFont(QFont("Monospace", 25, QFont.Bold))
        self.usdVersionPicker.setMinimumWidth((res.width() / 3) * 2)
        self.usdVersionPicker.setMaximumWidth(res.width() - (res.width() / 5)) # screen - size of next button
        self.usdVersionPicker.setMaxVisibleItems(15)
        self.usdVersionPicker.setSizeAdjustPolicy(QComboBox.AdjustToContents)
        self.refreshBuildVersions()
        self.usdVersionPicker.activated.connect(self.usdVersionPicked)

        self.infoGrid = QGridLayout()

        self.usdVersionLabel = QLabel(self)
        self.usdVersionLabel.setText("Version:")
        self.usdVersionLabel.setFont(QFont("Monospace", 20, QFont.Bold))
        self.usdPartNumberLabel = QLabel(self)
        self.usdPartNumberLabel.setText("Part Number:")
        self.usdPartNumberLabel.setFont(QFont("Monospace", 20, QFont.Bold))
        self.usdSizeLabel = QLabel(self)
        self.usdSizeLabel.setText("Size:")
        self.usdSizeLabel.setFont(QFont("Monospace", 20, QFont.Bold))
        self.usdFwVersionLabel = QLabel(self)
        self.usdFwVersionLabel.setText("Wrapper Firmware:")
        self.usdFwVersionLabel.setFont(QFont("Monospace", 20, QFont.Bold))

        self.usdVersion = QLabel(self)
        self.usdVersion.setText("")
        self.usdVersion.setFont(QFont("Monospace", 20))
        self.usdPartNumber = QLabel(self)
        self.usdPartNumber.setText("")
        self.usdPartNumber.setFont(QFont("Monospace", 20))
        self.usdSize = QLabel(self)
        self.usdSize.setText("")
        self.usdSize.setFont(QFont("Monospace", 20))
        self.usdFwVersion = QLabel(self)
        self.usdFwVersion.setText("")
        self.usdFwVersion.setFont(QFont("Monospace", 20))

        self.infoGrid.addWidget(self.usdVersionLabel, 0, 0)
        self.infoGrid.addWidget(self.usdPartNumberLabel, 1, 0)
        self.infoGrid.addWidget(self.usdSizeLabel, 2, 0)
        self.infoGrid.addWidget(self.usdFwVersionLabel, 3, 0, Qt.AlignLeft)
        self.infoGrid.addWidget(self.usdVersion, 0, 1, Qt.AlignLeft )
        self.infoGrid.addWidget(self.usdPartNumber, 1, 1, Qt.AlignLeft )
        self.infoGrid.addWidget(self.usdSize, 2, 1, Qt.AlignLeft )
        self.infoGrid.addWidget(self.usdFwVersion, 3, 1, Qt.AlignLeft)

        self.insertTitle = QLabel(self)
        self.insertTitle.setAlignment(Qt.AlignHCenter)
        self.insertTitle.setText("Please insert the SD card.\nThen press next to create a master...")
        self.insertTitle.setFont(QFont("Monospace", 20, QFont.Bold))

        self.usdVerifyButton = QPushButton("Verify")
        self.usdVerifyButton.setFont(QFont("Monospace", 20))
        self.usdVerifyButton.setFixedSize(res.width() / 6, res.height() / 6)
        self.usdVerifyButton.clicked.connect(self.checkMD5)
        self.usdVerifyButton.setEnabled(False)

        self.usdNextButton = QPushButton("Next")
        self.usdNextButton.setFont(QFont("Monospace", 20))
        self.usdNextButton.setFixedSize(res.width() / 6, res.height() / 6)
        self.usdNextButton.clicked.connect(self.usdNext)
        self.usdNextButton.setEnabled(False)

        self.usdBackButton = QPushButton("Back")
        self.usdBackButton.setFont(QFont("Monospace", 20))
        self.usdBackButton.setFixedSize(res.width() / 6, res.height() / 6)
        self.usdBackButton.clicked.connect(self.usdBack)
        self.usdBackButton.setEnabled(True)

        self.usdLayout.addWidget(self.usdTitle, 0, 0, Qt.AlignLeft | Qt.AlignBottom)
        self.usdLayout.addWidget(self.usdVersionPicker, 1, 0, Qt.AlignLeft)
        self.usdLayout.addWidget(self.insertTitle, 3, 0, Qt.AlignLeft | Qt.AlignCenter)
        self.usdLayout.addWidget(self.usdVerifyButton, 2, 1, Qt.AlignRight | Qt.AlignBottom)
        self.usdLayout.addWidget(self.usdBackButton, 4, 0, Qt.AlignLeft | Qt.AlignBottom)
        self.usdLayout.addWidget(self.usdNextButton, 4, 1, Qt.AlignRight | Qt.AlignBottom)
        self.usdLayout.addLayout(self.infoGrid,2,0)

        Widget.setLayout(self.usdLayout)
        return Widget

    # widget to display information to user while runing masterblaster_make_sd
    def createInfoPage(self):
        res = QDesktopWidget().screenGeometry()
        Widget = QWidget()
        self.infoLayout = QGridLayout()

        self.infoTitle = QLabel(self)
        self.infoTitle.setAlignment(Qt.AlignHCenter)
        self.infoTitle.setText("Please Wait While The Master Is Created...")
        self.infoTitle.setFont(QFont("Monospace", 20, QFont.Bold))

        self.infoBox = QPlainTextEdit()
        self.infoBox.setFont(QFont("Monospace", 16))
        self.infoBox.setReadOnly(True)
        self.infoBox.setWordWrapMode(0)

        self.infoNextButton = QPushButton("Running...")
        self.infoNextButton.setFont(QFont("Monospace", 20))
        self.infoNextButton.setFixedSize(res.width() / 6, 100)
        self.infoNextButton.clicked.connect(self.nextPage)
        self.infoNextButton.setEnabled(False)

        self.infoLayout.addWidget(self.infoTitle, 0, 0)
        self.infoLayout.addWidget(self.infoBox, 1, 0)
        self.infoLayout.addWidget(self.infoNextButton, 2, 0, Qt.AlignRight | Qt.AlignBottom)

        Widget.setLayout(self.infoLayout)
        return Widget

    # ------------------------------------------------------------------------------------------------------------------
    # Increments the current page index by 1 and changes the current layout
    # ------------------------------------------------------------------------------------------------------------------
    def nextPage(self):
        if self.pageIndex < self.lastPage:
            self.pageIndex += 1
        else:
            self.pageIndex = 0
        self.StackedLayout.setCurrentIndex(self.pageIndex)

    # ------------------------------------------------------------------------------------------------------------------
    # OnClicked handler for the TOTP Refresh button
    # Updates the TOTP label on the main screen
    # ------------------------------------------------------------------------------------------------------------------
    def refreshServiceCode(self):
        self.serviceRefreshButton.setEnabled(False)
        self.serviceCode.setText("Service Code: N/A")
        self.serviceThread.start()

    # ------------------------------------------------------------------------------------------------------------------
    # OnClicked handler for the usdNextButton
    # Checks for usb device and MD5 Verification Then starts the masterblaster process
    # ------------------------------------------------------------------------------------------------------------------
    def usdNext(self):
        self.usdNextButton.setEnabled(False)
        devs = os.listdir("/dev")
        slotFlag = False
        possibleDevs = []
        for dev in devs:
            # match parent drive
            match = re.match(r"^sd[a-z]$", dev)
            if match:
                possibleDevs.append(match.group(0))
                # print match.group(0)
            match = re.match(r"^mmcblk[0,2-9]$", dev)
            if match:
                # in linux partitions for mmcblk are p1, p2, p3, etc..
                # for sda they are 1, 2, 3 etc...
                slotFlag = True
                possibleDevs.append(match.group(0))
                # print match.group(0)

        if not os.path.isfile(os.path.join(os.path.dirname(self.usdFilePath), 'VERIFIED')):
            msgBox = QMessageBox()
            msgBox.setText("The version you selected has not been verified.")
            msgBox.setFont(QFont("Monospace", 40))
            msgBox.exec_()
            self.usdNextButton.setEnabled(True)

        elif len(possibleDevs) > 1:
            msgBox = QMessageBox()
            msgBox.setText("Mulitiple SD cards Found...\nPlease remove all other SD cards and USB devices,"
                           " then try again")
            msgBox.setFont(QFont("Monospace", 40))
            msgBox.exec_()
            self.usdNextButton.setEnabled(True)
        elif len(possibleDevs) < 1:
            msgBox = QMessageBox()
            # msgBox.setFixedSize(500,600)
            msgBox.setText("No SD Inserted")
            msgBox.setFont(QFont("Monospace", 40))
            msgBox.exec_()
            self.usdNextButton.setEnabled(True)
        else:
            if slotFlag:
                self.devicePart = "p"
            else:
                self.devicePart = ""
            self.devicePath = "/dev/{0}".format(possibleDevs[0])
            logging.debug("Device found on {0}".format(self.devicePath))
            self.nextPage()
            self.runMakeSD()
        self.usdNextButton.setEnabled(True)

    # ------------------------------------------------------------------------------------------------------------------
    # OnClicked handler for the usdBack button
    # Returns to the main layout
    # ------------------------------------------------------------------------------------------------------------------
    def usdBack(self):
        self.pageIndex = 0
        self.StackedLayout.setCurrentIndex(self.pageIndex)

    # ------------------------------------------------------------------------------------------------------------------
    # OnClicked handler for the main screen Begin button
    # advances to the usd selection screen
    # ------------------------------------------------------------------------------------------------------------------
    def landNextClicked(self):
        # prevent multiple clicks before the screen advances
        self.landNextButton.setEnabled(False)
        # advance the screeen
        self.nextPage()
        # reneable the next button
        self.landNextButton.setEnabled(True)

    # ------------------------------------------------------------------------------------------------------------------
    # OnClicked handler for the main screen power button
    # prompts the user for shutdown, reboot or cancel
    # ------------------------------------------------------------------------------------------------------------------
    def powerButtonClicked(self):
        Widget = QDialog()
        mBox = QGridLayout()
        pButton = QPushButton()
        pButton.setText("Shutdown")
        pButton.setFont(QFont("Monospace", 25))
        pButton.clicked.connect(self.shutdown)
        rButton = QPushButton()
        rButton.setText('Reboot')
        rButton.setFont(QFont("Monospace", 25))
        rButton.clicked.connect(self.reboot)
        cButton = QPushButton()
        cButton.setText('Cancel')
        cButton.setFont(QFont("Monospace", 25))
        cButton.clicked.connect(Widget.accept)
        mBox.addWidget(pButton, 0, 0)
        mBox.addWidget(rButton, 0, 1)
        mBox.addWidget(cButton, 0, 2)
        Widget.setLayout(mBox)
        Widget.setModal(True)
        Widget.exec_()

    # ------------------------------------------------------------------------------------------------------------------
    # OnClicked handler for the shutdown button
    # Powers off the massterblaster
    # ------------------------------------------------------------------------------------------------------------------
    def shutdown(self):
        os.system("sudo shutdown now")

    # ------------------------------------------------------------------------------------------------------------------
    # OnClicked handler for the reboot button
    # reboots masterblaster
    # ------------------------------------------------------------------------------------------------------------------
    def reboot(self):
        os.system("sudo reboot")

    # This function resets button enabled and refreshes page information
    def widgetChanged(self):
        logging.debug("Page Changed: {0}".format(self.pageIndex))
        if self.pageIndex == 1:
            self.refreshBuildVersions()
            self.usdVersionPicker.setCurrentIndex(0)

        elif self.pageIndex == 0:
            try:
                exit, ip = getIpAddress.getIpAddress()
                if exit:
                    raise Exception
                self.ipLabel.setText("IP: " + ip)
            except:
                logging.debug("Failed to get IP")
                self.ipLabel.setText("IP: NONE")
                pass
            self.usdVersion.setText("")
            self.usdSize.setText("")
            self.usdPartNumber.setText("")
            self.usdFwVersion.setText("")

            self.usdNextButton.setEnabled(False)
            self.infoNextButton.setText("Running...")
            self.infoNextButton.setStyleSheet("background-color: gray")
            self.infoNextButton.setEnabled(False)
            self.usdVerifyButton.setEnabled(False)
            self.infoBox.clear()

    # ------------------------------------------------------------------------------------------------------------------
    # Handler for the usdVersion Picker Box
    #   Called whenever a uSD version is selected by the user
    #
    #   @Result: Sets the uSD filepath, and Device Size based on the Part Number file. Also enables the Verify and Next
    #       Buttons
    # ------------------------------------------------------------------------------------------------------------------
    def usdVersionPicked(self):
        if self.usdVersionPicker.currentIndex() > 0:
            try:
                with open(os.path.join(USD_FILE_PATH, self.tarballs[self.usdVersionPicker.itemText(
                        self.usdVersionPicker.currentIndex())])) as relFile:
                    release = relFile.readline().strip()
                    size = relFile.readline().strip()
                    tarball = relFile.readline().strip()
                    model = relFile.readline().strip()
                    firmware = relFile.readline().strip()

                    self.usdVersion.setText(release)
                    self.usdSize.setText("{}GB".format(size))
                    self.usdPartNumber.setText(self.tarballs[self.usdVersionPicker.
                                               itemText(self.usdVersionPicker.currentIndex())].split(':')[1])
                    self.usdFwVersion.setText(firmware)

                    self.releaseNumber = release
                    self.deviceSize = size
                    self.usdFilePath = os.path.join(USD_FILE_PATH, "Rel_{0}".format(release), tarball)
                    self.usdVerifyButton.setEnabled(True)
                    self.usdNextButton.setEnabled(True)
                    # print(self.usdFilePath)
            except:
                msgBox = QMessageBox()
                msgBox.setText("Invalid Part File.")
                msgBox.setFont(QFont("Monospace", 40))
                msgBox.exec_()
                logging.error("Invalid Part File")
                pass
        else:
            self.usdVersion.setText("")
            self.usdSize.setText("")
            self.usdPartNumber.setText("")
            self.usdVerifyButton.setEnabled(False)
            self.usdNextButton.setEnabled(False)

    # ------------------------------------------------------------------------------------------------------------------
    # Updates usdVersionPicker text box and the tarballs list
    # ------------------------------------------------------------------------------------------------------------------
    def refreshBuildVersions(self):
        self.usdVersionPicker.clear()
        self.tarballs.clear()
        for root, directories, filename in os.walk(os.path.join(USD_FILE_PATH)):
            for file in filename:
                if 'Part_Number:' in file:
                    #print file
                    try:
                        with open(os.path.join(root, file)) as relFile:
                            release = relFile.readline().strip()
                            size = relFile.readline().strip()
                            tarball = relFile.readline().strip()
                            model = relFile.readline().strip()
                            if(size == '8' or size == '16' or size == '32'):
                                tag = "Version: {1:<8} {2:>2}GB Model: {0}".format(model, release, size)
                                filepath = os.path.join(USD_FILE_PATH, "Rel_{0}".format(release), tarball)
                                if(os.path.isfile(filepath)):
                                    self.tarballs[tag] = os.path.join(USD_FILE_PATH, "Rel_{0}".format(release), file)
                                    #print tag
                                    #print self.tarballs[tag]
                                else:
                                    if(DEBUG):
                                        logging.debug("Invalid Config: File Not Found. Config File:{0} Release Path:{1}"
                                                  .format(file, filepath))
                            else:
                                if (DEBUG):
                                    logging.debug("Invalid Config: Invalid SD Size. Config File:{0} Size:{1}".format(file, size))
                    except IOError:
                        msgBox = QMessageBox()
                        msgBox.setText("Invalid Part File.")
                        msgBox.setFont(QFont("Monospace", 40))
                        msgBox.exec_()
                        logging.debug("Could Not Open Config File {0}".format(file))
        sortedkeys = sorted(list(self.tarballs.keys()), cmp=self.verCompare)
        self.usdVersionPicker.addItem("--NONE--")
        for tag in sortedkeys:
            #print tag
            self.usdVersionPicker.addItem(tag)

    # ------------------------------------------------------------------------------------------------------------------
    # Helper for the refreshBuildVersions function. Compares two tags, (e.g.) "Part Number: 00-448959-13 Version: 3.2.0 Size: 8GB"
    # and sorts them by version number
    # ------------------------------------------------------------------------------------------------------------------

    def verCompare(self, x, y):
        #print x, y
        #print x.split(':')[2][:-5].strip()
        #print y.split(':')[2][:-5].strip()

        first = re.search(r"[\d]+[.][\d]+.[\d]+", x).group(0)
        second = re.search(r"[\d]+[.][\d]+.[\d]+", y).group(0)
        if (LooseVersion(first) > LooseVersion(second)):
            return 1
        elif (LooseVersion(first) == LooseVersion(second)):
            first = re.search(r"\s+([0-9]|[0-9][0-9])[G][B]", x).group(1)
            second = re.search(r"\s+([0-9]|[0-9][0-9])[G][B]", y).group(1)
            if first > second:
                return -1
            else:
                return 1
        else:
            return -1

    # ------------------------------------------------------------------------------------------------------------------
    # Checks the MD5 of the currently selected tarball
    # ------------------------------------------------------------------------------------------------------------------
    def checkMD5(self):
        # print "Checking MD5"
        exit, pid = messageBox.display("Checking MD5...")
        if (md5checksum.md5ValidBasedOnChecksumFile(self.usdFilePath)):
            # print "MD5 passed"
            messageBox.destroy(pid)
            msgBox = QMessageBox()
            # msgBox.setFixedSize(500,600)
            verified_file_path = os.path.join(os.path.dirname(self.usdFilePath), 'VERIFIED')
            with open(verified_file_path, 'a'):
                os.utime(verified_file_path, None)
            msgBox.setText("MD5 Check Passed.")
            msgBox.setFont(QFont("Monospace", 40))
            msgBox.exec_()

        else:
            # print "MD5 Failed"
            messageBox.destroy(pid)
            msgBox = QMessageBox()
            # msgBox.setFixedSize(500,600)
            msgBox.setText("Build Version Corrupted...")
            msgBox.setFont(QFont("Monospace", 40))
            msgBox.exec_()

    # ------------------------------------------------------------------------------------------------------------------
    # Helper function to start the masterblaster thread
    # ------------------------------------------------------------------------------------------------------------------
    def runMakeSD(self):
        self.thread = SDThread(self.usdFilePath, self.deviceSize, self.devicePath, self.devicePart, self.releaseNumber,
                               os.path.isfile(os.path.join(os.path.dirname(self.usdFilePath), 'NOOFFICE')))
        self.thread.update_ready.connect(self.updateInfoBox)
        self.thread.error.connect(self.handleError)
        self.thread.start()

    # ------------------------------------------------------------------------------------------------------------------
    # Masterblaster new message handler
    # Updates the message box whenever a message is emitted from the masterblaster thread
    # ------------------------------------------------------------------------------------------------------------------
    def updateInfoBox(self, info):
        if info == "OKIMFINALLYDONE":
            self.infoNextButton.setEnabled(True)
            msgBox = QMessageBox()
            msgBox.setText("Master Creation Success.\nPlease remove the SD card reader.")
            msgBox.setFont(QFont("Monospace", 40))
            msgBox.exec_()
            self.infoNextButton.setStyleSheet("background-color: green")
            self.infoNextButton.setText("Finished")
        else:
            self.infoBox.appendPlainText(info)
            self.infoBox.verticalScrollBar().setValue(self.infoBox.verticalScrollBar().maximum())
            self.infoBox.horizontalScrollBar().setValue(0)

    # ------------------------------------------------------------------------------------------------------------------
    # Masterblaster error message handler
    # Updates the message box whenever an error message is emitted from the masterblaster thread
    # ------------------------------------------------------------------------------------------------------------------
    def handleError(self, info):
        self.infoBox.appendPlainText(info)
        self.infoBox.verticalScrollBar().setValue(self.infoBox.verticalScrollBar().maximum())
        self.infoBox.horizontalScrollBar().setValue(0)
        msgBox = QMessageBox()
        msgBox.setText("Master Creation Failed")
        msgBox.setFont(QFont("Monospace", 40))
        msgBox.exec_()
        self.infoNextButton.setStyleSheet("background-color: red")
        self.infoNextButton.setText("Restart")
        self.infoNextButton.setEnabled(True)

    # ------------------------------------------------------------------------------------------------------------------
    # updateIPThread new message handler
    # Updates the IP label on the main screen
    # ------------------------------------------------------------------------------------------------------------------
    def updateIP(self, info):
        self.ipLabel.setText("IP: {0}".format(info))

    # ------------------------------------------------------------------------------------------------------------------
    # updateServiceThread new message handler
    # Updates the TOTP label on the main screen
    # ------------------------------------------------------------------------------------------------------------------
    def updateService(self, info):
        self.serviceCode.setText("Service Code: {0}".format(info))
        self.serviceRefreshButton.setEnabled(True)


    # ------------------------------------------------------------------------------------------------------------------
    # Timer interrupt handler
    # Updates the TOTP label and the ip label on the main screen
    # ------------------------------------------------------------------------------------------------------------------
    def timedRefresh(self):
        #logging.debug("Refresh Timer")
        self.serviceThread.start()
        self.ipThread.start()

    # ------------------------------------------------------------------------------------------------------------------
    # Timer interrupt handler
    # Updates the time on the main screen
    # ------------------------------------------------------------------------------------------------------------------
    def updateTime(self):
        self.timeLabel.setText(QDateTime().currentDateTime().toString("MMM dd yyyy hh:mm AP"))

# ----------------------------------------------------------------------------------------------------------------------
# Thread for updating the ip address of the scale
# ----------------------------------------------------------------------------------------------------------------------
class updateIPThread(QtCore.QThread):
    ip_ready = QtCore.pyqtSignal(object)

    def __init__(self):
        QtCore.QThread.__init__(self)
    def run(self):
        startTime = time.time()
        while True:
            try:
                exit, ip = getIpAddress.getIpAddress()
                if exit:
                    raise Exception
                if (DEBUG):
                    logging.debug("IP: {0}".format(ip))
                self.ip_ready.emit(ip)
                break
            except:
                time.sleep(.5)
                pass
            if time.time() >= startTime + 1 * 60:
                self.ip_ready.emit("N/A")
                logging.debug("Timeout Fetching IP")
                break

# ----------------------------------------------------------------------------------------------------------------------
# Thread for updating the TOTP code
# ----------------------------------------------------------------------------------------------------------------------
class updateServiceThread(QtCore.QThread):
    service_ready = QtCore.pyqtSignal(object)

    def __init__(self):
        QtCore.QThread.__init__(self)
    def run(self):
        self.service_ready.emit(totp.TOTP(base64.b32decode(totp.getPassKey()), digits=6, window=14400))

# ----------------------------------------------------------------------------------------------------------------------
# Thread for running masterblaster_make_SD.py
# ----------------------------------------------------------------------------------------------------------------------
class SDThread(QtCore.QThread):
    update_ready = QtCore.pyqtSignal(object)
    error = QtCore.pyqtSignal(object)

    def __init__(self, usdPath, size, devicePath, devicePart , releaseNumber, noOfficeViewer):
        QtCore.QThread.__init__(self)
        self.usdFilePath = usdPath
        self.deviceSize = size
        self.devicePath = devicePath
        self.noOfficeViewer = noOfficeViewer
        self.devicePart = devicePart
        self.releaseNumber = releaseNumber

    def run(self):
        try:
            if(os.path.isfile(LOGFILE)):
                shutil.copy(LOGFILE, "/usr/local/hobart/old_masterblaster.log" )
                os.system("rm -rf " + LOGFILE)
            logging.debug("Running makeSD...")
            self.update_ready.emit('Running makeSD...')
            logging.debug("Spawning masterblaster proceess")
            self.update_ready.emit("Spawning masterblaster proceess")
            if self.noOfficeViewer:
                makeSd = pexpect.spawn("python " + MAKE_SD_LOC + " -n")
            else:
                makeSd = pexpect.spawn("python " + MAKE_SD_LOC)
            logging.debug("Waiting for SD location prompt")
            self.update_ready.emit("Waiting for SD location prompt")
            makeSd.expect_exact("Enter SD card device location: ")
            logging.debug("Sending Location: {0}".format(self.devicePath))
            self.update_ready.emit("Sending Location: {0}".format(self.devicePath))
            makeSd.sendline(self.devicePath)
            logging.debug( "Waiting for SD size prompt")
            self.update_ready.emit("Waiting for SD size prompt")
            makeSd.expect_exact("Enter SD size (8GB, 16GB or 32GB: ")
            logging.debug("Sending: {0}".format(self.deviceSize))
            self.update_ready.emit("Sending: {0}".format(self.deviceSize))
            makeSd.sendline(self.deviceSize)
            logging.debug( "Waiting for Release path prompt")
            self.update_ready.emit("Waiting for Release path prompt")
            index = makeSd.expect_exact(["Enter release path: ", "Invalid size given. Expected: 8,16,32"])
            if index == 0:
                logging.debug("Sending {0}".format(self.usdFilePath))
                self.update_ready.emit("Sending {0}".format(self.usdFilePath))
                makeSd.sendline(self.usdFilePath)
            elif index == 1:
                makeSd.close()
                self.error.emit("Invalid size given. Expected: 8,16,32")
                self.terminate()
                self.wait()

            index = makeSd.expect_exact(
                ["Release Could not be found", "SD Card Not Found", "SD Card Found Unmounting..."], timeout=None)
            if index == 0:
                makeSd.close()
                self.error.emit("Release Could Not Be Found")
                self.terminate()
                self.wait()
            elif index == 1:
                makeSd.close()
                self.error.emit("SD Card Not Found")
                self.terminate()
                self.wait()
            elif index == 2:
                logging.debug("SD Card Found Unmounting...")
                self.update_ready.emit("SD Card Found Unmounting...")

            makeSd.expect_exact("Creating Partitons...", timeout=None)
            try:
                index = makeSd.expect_exact(["Creation of Partitions Failed...", "Formatting Partiton 1..."],
                                            timeout=None)
                if index == 0:
                    makeSd.close()
                    self.error.emit("Creation of Partitions Failed...")
                    self.terminate()
                    self.wait()
                elif index == 1:
                    logging.debug("Formatting Partiton 1...")
                    self.update_ready.emit("Formatting Partiton 1...")
            except:
                makeSd.close(force=True)
                self.error.emit("Creation of Partitions Failed...")
                self.terminate()
                self.wait()

            index = makeSd.expect_exact(["Failed To Format Partition 1", "Formatting Partiton 2..."], timeout=None)
            if index == 0:
                makeSd.close()
                self.error.emit("Failed To Format Partition 1")
                self.terminate()
                self.wait()
            elif index == 1:
                logging.debug("Formatting Partiton 2...")
                self.update_ready.emit("Formatting Partiton 2...")

            index = makeSd.expect_exact(["Failed To Format Partition 2",
                                         "recovery present from previous format. umount and remove directory...",
                                         "Eject SD card from device and reinsert into reader/writer device"],
                                        timeout=None)
            if index == 0:
                makeSd.close()
                self.error.emit("Failed To Format Partition 2")
                self.terminate()
                self.wait()
            elif index == 1:
                makeSd.expect_exact("Eject SD card from device and reinsert into reader/writer device", timeout=None)
                self.update_ready.emit("recovery present from previous format. umount and remove directory...")
            elif index == 2:
                # self.update_ready.emit("Eject SD card from device and reinsert into reader/writer device")
                pass
            counter = 0
            while 1:

                makeSd.expect("Press <Enter> after card re-inserted and it mounts in your VM", timeout=None)
                self.update_ready.emit("Ejecting and remounting SD please wait...")

                if os.system("partprobe" + " 2>&1 | tee -a /usr/local/hobart/masterblaster.log"):
                    self.error.emit('Failed refresh usb')
                    self.terminate()
                    self.wait()
                timeout = time.time() + 60 * 1

                if self.devicePart == "":
                    self.update_ready.emit("Waiting on automount...")
                    # wait for automount after calling partprobe
                    while not os.path.ismount("/media/thumb/hobart") or not os.path.ismount("/media/thumb") or not \
                            os.path.ismount("/media/thumb/recovery"):
                        # while not os.path.ismount("/media/thumb/") and not os.path.ismount("/media/thumb/hobart/"):

                        if time.time() > timeout:
                            self.update_ready.emit("Timeout Waiting for automount")
                            logging.debug("TIMEOUT: Waiting for automount")
                            break
                        else:
                            time.sleep(1)

                os.system("umount {0}".format(self.devicePath) + " 2>&1 | tee -a " + LOGFILE)
                os.system("umount {0}{1}1".format(self.devicePath, self.devicePart) + " 2>&1 | tee -a " + LOGFILE)
                os.system("umount {0}{1}2".format(self.devicePath,self.devicePart) + " 2>&1 | tee -a " + LOGFILE)
                os.system("rm -rf /media/thumb/hobart" + " 2>&1 | tee -a " + LOGFILE)
                os.system("rm -rf /media/thumb/recovery" + " 2>&1 | tee -a " + LOGFILE)
                # os.system("partprobe {0}".format(self.devicePath) + " 2>&1 | tee -a /usr/local/hobart/masterblaster.log")
                os.system("mkdir /media/thumb/hobart" + " 2>&1 | tee -a " + LOGFILE)
                os.system("mkdir /media/thumb/recovery" + " 2>&1 | tee -a " + LOGFILE)
                if os.system("mount {0}{1}1 /media/thumb/hobart".format(self.devicePath, self.devicePart) + " 2>&1 | tee -a " + LOGFILE):
                    print("hobart mount failed")
                if os.system("mount {0}{1}2 /media/thumb/recovery".format(self.devicePath, self.devicePart) + " 2>&1 | tee -a " + LOGFILE):
                    print("recovery mount failed")

                # added wait 10 for mounting due to failure to mount. os.system does not block for long enough on mount
                # command 5/24/2018 EDL
                timeout = time.time() + 10
                while not os.path.ismount("/media/thumb/hobart") and not os.path.ismount("/media/thumb/recovery"):
                    time.sleep(1)
                    if time.time() >= timeout:
                        break


                makeSd.sendline('\n')
                index = makeSd.expect_exact(["Card Found", "SD card not mounted yet, re-insert"])
                if index == 0:
                    break
                counter += 1
                self.update_ready.emit('Remount failed. Trial {0} of 5'.format(counter))
                logging.debug('Number of Attempts ' + str(counter))
                if counter >= 5:
                    makeSd.close()
                    self.error.emit("Could Not Mount SD")
                    self.terminate()
                    self.wait()

            self.update_ready.emit("Formatting SD")
            logging.debug("Formatting SD")

            makeSd.expect_exact("SD card formatted and mounted...", timeout=None)
            logging.debug("SD formatted...")
            self.update_ready.emit("SD Formatted")

            makeSd.expect_exact("Applying root file system to SD card.", timeout=None)
            logging.debug("Applying root file system to SD card")
            self.update_ready.emit("Applying root file system to SD card")
            self.readAndEmitUntil(makeSd, "Root file system applied...")
            if not self.noOfficeViewer:
                index = makeSd.expect_exact(["ERROR", "Installing Office Viewer and PDF Viewer features ..."], timeout=None)
                if index == 0:
                    while True:
                        makeSd.expect("\r\n", timeout=None)
                        out = makeSd.before.strip()
                        if out == "Exiting":
                            self.error.emit("Root File System Extraction Unsuccessful")

                            self.terminate()
                            self.wait()
                            break
                        else:
                            self.update_ready.emit(out)
                    time.sleep(.005)

                logging.debug("Extracting Office Viewer...")
                self.update_ready.emit("Extracting Office Viewer...")
                self.readAndEmitUntil(makeSd, "Office Viewer Extracted...")

                index = makeSd.expect_exact(["ERROR", "Extracting PDF viewer..."], timeout=None)
                if index == 0:
                    while True:
                        makeSd.expect("\r\n", timeout=None)
                        out = makeSd.before.strip()
                        if out == "Exiting":
                            self.error.emit("Office Viewer Extraction Unsuccessful")
                            self.terminate()
                            self.wait()
                            break
                        else:
                            self.update_ready.emit(out)
                    time.sleep(.005)

                logging.debug("Extracting PDF Viewer...")
                self.update_ready.emit("Extracting PDF Viewer...")
                self.readAndEmitUntil(makeSd, "PDF Viewer Extracted...")

                index = makeSd.expect_exact(["ERROR", "Could not set up dpkg", "Dpkg setup success"], timeout=None)
                if index == 0:
                    while True:
                        makeSd.expect("\r\n", timeout=None)
                        out = makeSd.before.strip()
                        if out == "Exiting":
                            self.error.emit("PDF Viewer Extraction Unsuccessful")
                            self.terminate()
                            self.wait()
                            break
                        else:
                            self.update_ready.emit(out)
                    time.sleep(.005)
                elif index == 1:
                    self.error.emit("Could not set up dpkg")
                    self.terminate()
                    self.wait()

                makeSd.expect_exact("Installing Office Viewer...")
                self.readAndEmitUntil(makeSd, "Office Viewer Installation Complete")
            index = makeSd.expect_exact(["Error Installing Office Viewer", "Verifying software version information..."],
                                timeout=None)
            if index == 0:
                self.error.emit("Verification of Office Viewer Failed.")
                self.terminate()
                self.wait()
            elif index == 1:
                self.update_ready.emit("Verifying Software Version")

            index = makeSd.expect_exact(["Verification of Software Version Failed.", "Cleaning up temp files..."],
                                        timeout=None)
            if index == 0:
                self.error.emit("Verification of Software Version Failed.")
                self.terminate()
                self.wait()
            elif index == 1:
                self.update_ready.emit("Cleaning up temp files...")

            makeSd.expect("SD card is ready for target...")
            makeSd.close()
            logging.debug("SD card is ready for target...")
            self.update_ready.emit("SD card is ready for target...")
            self.update_ready.emit("OKIMFINALLYDONE")
            self.exit(0)
        except pexpect.EOF as e:
            logging.debug(e)
            self.error.emit("Make SD closed unexpectedly. Exiting...")
        self.terminate()
        self.wait()

    # ------------------------------------------------------------------------------------------------------------------
    # Helper for masterblaster run.
    # Reads input from thread and emits the received messages until end is received
    # Throttled down to one message ever .005 sec to not overload the application
    # ------------------------------------------------------------------------------------------------------------------
    def readAndEmitUntil(self, thread, end):
        while True:
            thread.expect("\r\n", timeout=None)
            out = thread.before.strip()
            # print out
            if out == end:
                self.update_ready.emit(out)
                break
            else:
                self.update_ready.emit(out)
            time.sleep(.005)


def masterBlaster():
    os.environ['DISPLAY'] = ':0'

    # setup log file
    logging.basicConfig(filename=LOGFILE, filemode='a', level=logging.DEBUG, format='%(asctime)s %(message)s',
                        datefmt='%m/%d/%Y %I:%M:%S %p')
    # Create main app
    my_app = QApplication(sys.argv)
    # Create dialog and show
    d = MasterBlasterStartScreen()
    d.showFullScreen()
    d.show()

    # Execute the Application and Exit
    my_app.exec_()


if __name__ == '__main__':
    masterBlaster()
    sys.exit()
