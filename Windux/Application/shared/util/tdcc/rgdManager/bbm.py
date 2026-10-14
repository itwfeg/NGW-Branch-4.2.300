import ctypes.wintypes
import datetime
import glob
import os
import shutil
import sys
from shutil import copyfile

import netaddr
import tzlocal
import wmi
from PyQt5 import QtCore
from PyQt5.QtCore import *
from PyQt5.QtCore import QStandardPaths, pyqtSignal
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
from fabric2 import Connection

import encryptDecryptBB
import network_command
import network_scan
from Ui_mainwindow import Ui_MainWindow

# TODO
# Need to figure out which directory we can write to probably APP_DATA directory
# Can not log to installation directory in C:\Program Files and why all logging is commented out
# import logging
# import logging.handlers

# NOTE:
# keep the same format (whitespace as well) for line below, file_match_and_replace.py sets the version number
# based on build_bbm.bat arguments
PRODUCT_VERSION = "3.1.2"
PRODUCT_NAME = "Black Box Device Setup Utility"


class SoftwareUpgradeWorker(QThread):

    alert_handler = pyqtSignal(str)
    progress_handler = pyqtSignal(int)

    def __init__(self, ip_address, software_upgrade_filename, parent=None):

        QThread.__init__(self, parent)
        self.ip_address = ip_address
        self.software_upgrade_filename = software_upgrade_filename.strip()
        # logging.debug("started software upgrade worker thread")

    def file_transmit_progress(self, transferred, total_to_transfer):
        # print("bytes=%d" % transferred)
        percent_complete = int((float(transferred) / float(total_to_transfer)) * 100.0)
        self.progress_handler.emit(percent_complete)

    def transmit_software_upgrade_file(self):
        # logging.debug("in transmit software upgrade file")
        message = "INFO: Transmitting the software upgrade file to the selected black box device. "

        file_size = os.path.getsize(self.software_upgrade_filename)

        if (file_size / (1024 * 1024)) > 10:
            message = message + " This file is %d MB in size.  Please be patient..." % (file_size / (1024 * 1024))

        self.alert_handler.emit(message)

        # logging.debug("calling remote file put to send file")
        status_code = network_command.remote_file_put(ip_address=self.ip_address,
                                                      local_filename=self.software_upgrade_filename,
                                                      remote_filename="/usr/local/hobart/upgrade/%s" %
                                                                      os.path.basename(self.software_upgrade_filename),
                                                      progress_callback=self.file_transmit_progress)

        # logging.debug("file transmitted")

        self.progress_handler.emit(0)

        return status_code

    def perform_software_upgrade(self):
        # logging.debug("in perform software upgrade")

        message = "INFO: Starting software upgrade on the selected black box device."
        monolithic_upgrade = False
        if "monolithic_core" in os.path.basename(self.software_upgrade_filename):
            monolithic_upgrade = True
            message = message + " Monolithic upgrades can take 30 minutes or longer. Please be patient..."

        self.alert_handler.emit(message)

        status_code, pid = network_command.remote_background_command(self.ip_address,
                                                                     "/usr/bin/python "
                                                                     "/usr/local/hobart/python/installUpgrade.py "
                                                                     "/usr/local/hobart/upgrade/%s True" %
                                                                     os.path.basename(
                                                                         self.software_upgrade_filename))

        if status_code == 0:

            message = "INFO: Black box device software upgrade initiated."

            if monolithic_upgrade:
                message = message + " Monolithic upgrades can take 30 minutes or longer. Please be patient..."

            self.progress_handler.emit(100)

        else:
            self.alert_handler.emit("ERROR: Selected black box device failed to initiate upgrade")

            self.progress_handler.emit(0)

        return status_code

    def __del__(self):
        self.wait()

    # run method gets called when we start the thread
    def run(self):

        # logging.debug("in software upgrade worker and before transmit of file to remote system")
        status_code = self.transmit_software_upgrade_file()

        if status_code == 0:

            self.alert_handler.emit("INFO: Software upgrade file transmitted to selected black box device")

            # logging.debug("in software upgrade worker and about to perform upgrade")

            status_code = self.perform_software_upgrade()

        else:
            self.alert_handler.emit(
                "ERROR: Software upgrade file could not be transmitted to selected black box device")


class BlackBoxSetupUtility(QMainWindow, Ui_MainWindow):
    file_transmission_complete_handler = pyqtSignal(str)

    version = PRODUCT_VERSION
    testing_ = False
    allowPrinterConfig = True
    allowScaleConfig = False
    IP_ = ""
    subnet_ = ""
    gateway_ = ""
    primary_ = ""
    secondary_ = ""
    tertiary_ = ""
    clientCertFilename = ""
    rootCertFilename = ""
    security_ = ""
    encryption_ = ""
    importSecurity_ = ""
    importEncryption_ = ""
    importRootCertFilename = ""
    importClientCertFilename = ""
    importDir = ""

    howMany = 0
    advChannelIndex = 0
    advLinkRateIndex = 0
    automaticLinkRateIntValue = 1
    advWirelessDriverIndex = 0
    Suffix1 = Suffix2 = Suffix3 = ""

    def __init__(self):

        self.log_level = 'ERROR'
        self.log_filename = 'bbm.log'
        # self._setup_enhanced_logging(self.log_filename)

        QMainWindow.__init__(self)
        self.setupUi(self)
        self.helpFrame.hide()
        self.wiredRadio.toggled.connect(self.wiredRadioClicked)
        self.WPAPassphrase.installEventFilter(self)
        self.tlsCLientPassword.installEventFilter(self)
        bbOpenedFile = ""

        # self.setupUi(this)
        self.setWindowTitle(PRODUCT_NAME)
        self.setFixedWidth(931)
        self.setFixedHeight(800)
        self.wirelessGroupBox.show()
        self.wirelessRadio.setChecked(True)
        self.DHCPRadio.setChecked(True)
        self.automaticDNSRadio.setChecked(True)
        self.createHDConfigurationFileBut.setEnabled(False)
        self.eapFrame.hide()
        self.wpaPassphraseFrame.hide()
        self.tlsFrame.hide()
        self.openWEPframe.show()
        self.securityCombo.setCurrentIndex(0)
        self.encryptionCombo.setCurrentIndex(0)
        self.SSID.setText("")
        self.openBBFileName.setText("")
        self.addPillarSyncCheckbox.stateChanged.connect(self.on_addPillarSyncCheckbox_state_changed)
        # self.startBlackBoxFileBut.hide()
        # self.openBlackBoxFileBut.hide()
        # self.createHDConfigurationFileBut.hide()
        # self.editingFileLabel.hide()
        self.openBBFileName.hide()
        # self.bigBBLabel.setGeometry(20,10,441,41)
        self.advancedWirelessFrame.hide()
        self.testingFrame.hide()
        self.advancedWirelessLinkRateCombo.setCurrentIndex(0)
        self.advancedWirelessChannelCombo.setCurrentIndex(0)
        self.advancedWirelessDriverCombo.setCurrentIndex(0)

        self.versionLabel.setText(self.version)
        self.configurationMethodComboBox.clear()
        self.configurationMethodComboBox.addItem("Network", "Network")
        self.configurationMethodComboBox.addItem("Disk", "Disk")
        self.configurationMethodComboBox.setCurrentIndex(0)

        # self.softwareUpgradeFilenameLineEdit.setText(self.settings.value('software_upgrade_filename'))
        self.selectSoftwareUpgradePushButton.clicked.connect(self.select_software_upgrade_file)
        self.softwareUpgradePushButton.clicked.connect(self.upgrade_device_software)
        self.infoPushButton.clicked.connect(self.get_device_info)
        self.scanPushButton.clicked.connect(self.scan_network_for_devices)
        self.setDateTimePushButton.clicked.connect(self.set_black_box_device_time_based_on_pc_time)
        self.rebootPushButton.clicked.connect(self.reboot_confirmation)
        self.loadPushButton.clicked.connect(self.on_openBlackBoxFileBut_clicked)

        # self.closePushButton.clicked.connect(self.close_application)
        self.progressBar.setTextVisible(True)
        self.progressBar.setFormat("%p% of network devices scanned")
        self.softwareUpgradeProgressBar.setFormat("%p% of upgrade file transferred")
        self.progressBar.setAlignment(Qt.AlignCenter)
        self.softwareUpgradeProgressBar.setAlignment(Qt.AlignCenter)
        self.network_scan = network_scan.RgdProbeNetwork(
            scan_handler=self.handle_network_scan_message)
        ip_address_list = self.network_scan.get_default_network_ip_address_range()
        self.startIpLineEdit.setText(str(ip_address_list[0]))
        self.endIpLineEdit.setText(str(ip_address_list[-1]))
        self.network_scan.progress.connect(self.update_scan_progress)
        self.network_scan.alerts_handler.connect(self.set_alert)

        #############################################################
        # C O N N E C T I O N S
        self.DHCPRadio.toggled.connect(self.dhcpRadioClicked)
        self.staticIPRadio.toggled.connect(self.staticIPRadioClicked)
        self.automaticDNSRadio.toggled.connect(self.automaticDNSRadioClicked)
        self.manualDNSRadio.toggled.connect(self.manualDNSRadioClicked)
        self.wiredRadio.toggled.connect(self.manageWirelessGroupBox)
        self.wirelessRadio.toggled.connect(self.manageWirelessGroupBox)
        self.securityCombo.currentIndexChanged.connect(self.securityChanged)
        self.WPAShowPassphrase.toggled.connect(self.WPAShowPasswordToggled)
        self.advancedWirelessApplyBut.clicked.connect(self.on_advancedWirelessApplyBut_clicked)
        self.advancedWirelessCancelBut.clicked.connect(self.on_advancedWirelessCancelBut_clicked)
        self.deleteRoot.clicked.connect(self.on_deleteRootBut_clicked)
        self.deleteClient.clicked.connect(self.on_deleteClientBut_clicked)
        self.ntpFrequency.currentIndexChanged.connect(self.ntpFrequencyChanged)
        self.closeHelpBut.clicked.connect(self.on_closeHelpBut_clicked)
        self.viewTrustedRootCertBut.clicked.connect(self.onViewTrustedRootCertButClicked)
        self.viewClientCertBut.clicked.connect(self.onViewClientCertButClicked)
        self.clearPushbutton.clicked.connect(self.clearEverything)
        #############################################################

        # Event Filters
        self.WPAPassphrase.installEventFilter(self)
        self.WEPKey1.installEventFilter(self)
        self.WEPKey2.installEventFilter(self)
        self.WEPKey3.installEventFilter(self)
        self.WEPKey4.installEventFilter(self)
        self.eapPassword.installEventFilter(self)
        self.tlsCLientPassword.installEventFilter(self)
        self.aboutBigFrame.installEventFilter(self)

        self.lePrimary.setEnabled(False)
        self.leSecondary.setEnabled(False)
        self.leTertiary.setEnabled(False)
        self.leIP.setEnabled(False)
        self.leSubnet.setEnabled(False)
        self.leGateway.setEnabled(False)
        self.DHCPRadio.setChecked(True)
        self.automaticDNSRadio.setChecked(True)
        self.loadAdvancedWirelessBBChoices()
        self.advancedWirelessChannelCombo.setCurrentIndex(0)
        self.advancedWirelessLinkRateCombo.setCurrentIndex(0)
        self.advancedWirelessDriverCombo.setCurrentIndex(0)

        # Load timezones
        self.ntpTimezone.clear()
        self.ntpTimezone.addItem("(UTC+00:00) Dublin, Edinburgh, Lisbon, London", "Europe/London")
        self.ntpTimezone.addItem("(UTC+01:00) Brussels, Copenhagen, Madrid, Paris", "Europe/Paris")
        self.ntpTimezone.addItem("(UTC+02:00) Cairo", "Africa/Cairo")
        self.ntpTimezone.addItem("(UTC+03:00) Moscow, St. Petersburg, Volgograd", "Europe/Moscow")
        self.ntpTimezone.addItem("(UTC+04:00) Abu Dhabi, Muscat", "Asia/Muscat")
        self.ntpTimezone.addItem("(UTC+05:00) Islamabad, Karachi", "Asia/Karachi")
        self.ntpTimezone.addItem("(UTC+06:00) Dhaka", "Asia/Dhaka")
        self.ntpTimezone.addItem("(UTC+07:00) Bangkok, Hanoi, Jakarta", "Asia/Jakarta")
        self.ntpTimezone.addItem("(UTC+08:00) Taipei", "Asia/Taipei")
        self.ntpTimezone.addItem("(UTC+09:00) Osaka, Sapporo, Tokyo", "Asia/Tokyo")
        self.ntpTimezone.addItem("(UTC+10:00) Brisbane", "Australia/Brisbane")
        self.ntpTimezone.addItem("(UTC+11:00) Solomon Is., New Caledonia", "Pacific/Noumea")
        self.ntpTimezone.addItem("(UTC+12:00) Auckland, Wellington", "Pacific/Auckland")
        self.ntpTimezone.addItem("(UTC-01:00) Cape Verde Is.", "Atlantic/Cape_Verde")
        self.ntpTimezone.addItem("(UTC-02:00) Mid-Atlantic", "Atlantic/Noronha")
        self.ntpTimezone.addItem("(UTC-02:00) Brazil DeNoronha", "Brazil/DeNoronha")
        self.ntpTimezone.addItem("(UTC-03:00) Greenland", "America/Godthab")
        self.ntpTimezone.addItem("(UTC-03:00) Buenos Aires", "America/Buenos_Aires")
        self.ntpTimezone.addItem("(UTC-03:00) Brazil East", "Brazil/East")
        self.ntpTimezone.addItem("(UTC-03:00) Santiago", "America/Santiago")
        self.ntpTimezone.addItem("(UTC-04:00) Atlantic Time (Canada)", "America/Halifax")
        self.ntpTimezone.addItem("(UTC-04:00) Brazil West", "Brazil/West")
        self.ntpTimezone.addItem("(UTC-04:30) Caracas", "America/Caracas")
        self.ntpTimezone.addItem("(UTC-05:00) Eastern Time (US & Canada)", "America/New_York")
        self.ntpTimezone.addItem("(UTC-05:00) Bogota", "America/Bogota")
        self.ntpTimezone.addItem("(UTC-05:00) Lima", "America/Lima")
        self.ntpTimezone.addItem("(UTC-05:00) Panama", "America/Panama")
        self.ntpTimezone.addItem("(UTC-05:00) Brazil Acre", "Brazil/Acre")
        self.ntpTimezone.addItem("(UTC-06:00) Central Time (US & Canada)", "America/Chicago")
        self.ntpTimezone.addItem("(UTC-06:00) Costa Rica", "America/Costa_Rica")
        self.ntpTimezone.addItem("(UTC-06:00) Guatemala", "America/Guatemala")
        self.ntpTimezone.addItem("(UTC-06:00) Mexico General", "Mexico/General")
        self.ntpTimezone.addItem("(UTC-07:00) Arizona", "America/Phoenix")
        self.ntpTimezone.addItem("(UTC-07:00) Mountain Time (US & Canada)", "America/Denver")
        self.ntpTimezone.addItem("(UTC-07:00) Mexico BajaSur", "Mexico/BajaSur")
        self.ntpTimezone.addItem("(UTC-08:00) Pacific Time (US & Canada)", "America/Los_Angeles")
        self.ntpTimezone.addItem("(UTC-08:00) Mexico BajaNorte", "Mexico/BajaNorte")
        self.ntpTimezone.addItem("(UTC-09:00) Alaska", "America/Anchorage")
        self.ntpTimezone.addItem("(UTC-10:00) Hawaii", "Pacific/Honolulu")
        self.ntpTimezone.addItem("(UTC-11:00) Coordinated Universal Time-11", "Pacific/Midway")
        self.ntpTimezone.addItem("(UTC-12:00) International Date Line West", "Pacific/Midway")

        self.ntpTimezone.setCurrentIndex(23)
        beginningTime = QTime(3, 3, 0, 0)
        self.ntpTimeOfDay.setTime(beginningTime)
        self.WPAPassphrase.setEchoMode(QLineEdit.Normal)
        self.BBtabWidget.setCurrentIndex(0)
        self.bbConfigTab.setTabText(0, "Black Box Configuration")

        # scene1 = QGraphicsScene()
        # pixmap1 = QPixmap(":/images/Step-1.jpg")
        # scene1.addPixmap(pixmap1)
        # self.step1GraphicsView.setScene(scene1)

        self.step1GraphicsView.show()
        # scene =  QGraphicsScene()
        # pixmap = QPixmap(":/images/Step-2.jpg")
        # scene.addPixmap(pixmap)
        # self.step2GraphicsView.setScene(scene)

        self.step2GraphicsView.hide()
        self.step2Label.hide()
        self.wiredRadio.setChecked(True)
        self.wirelessGroupBox.hide()
        # self.addNTPSetting.setEnabled(False)
        self.addNTPSetting.hide()
        # self.addMasterLinkCheckbox.hide()
        self.advancedWirelessAutoLinkRateCheckBox.setChecked(True)
        #self.BBtabWidget.setTabEnabled(4, False)
        self.BBtabWidget.setTabEnabled(4, False)
        self.BBtabWidget.removeTab(4)

        #if PRODUCT_NAME[:9] == "Black Box":
             # self.BBtabWidget.removeTab(6)
             # self.BBtabWidget.removeTab(5)
             # self.BBtabWidget.removeTab(4)
        #     self.BBtabWidget.removeTab(3)
        #     self.BBtabWidget.removeTab(2)
        #     self.addPillarSyncCheckbox.hide()
        # elif PRODUCT_NAME[:10] == 'PillarSync':
        #     self.BBtabWidget.removeTab(6)
        #     self.BBtabWidget.removeTab(5)
        #     self.BBtabWidget.removeTab(4)
        #     self.BBtabWidget.removeTab(3)

        # NTP Settings Tab removed below
        #self.BBtabWidget.removeTab(2)
        self.coverupgbChannel.hide()
        self.coverupgbLinkRate.hide()
        self.coverupgbWirelessDriver.hide()
        self.addHostnameCheckbox.setEnabled(False)
        # self.addMasterLinkCheckbox.setEnabled(False)
        # self.addNTPSetting.setEnabled(False)

        self.advancedWirelessDriverCombo.clear()
        self.advancedWirelessDriverCombo.addItem("Default Driver (nl80211)", "nl80211")
        self.advancedWirelessDriverCombo.addItem("Alternate Wireless Driver (wext)", "wext")
        self.advancedWirelessDriverCombo.setCurrentIndex(0)

        if self.allowPrinterConfig:
            self.printerTarget.setEnabled(True)
            self.printerTarget.setDisabled(False)
        else:
            self.printerTarget.setEnabled(False)
            self.printerTarget.setDisabled(True)

        if self.allowScaleConfig:
            self.scaleTarget.setEnabled(True)
            self.scaleTarget.setDisabled(False)
        else:
            self.scaleTarget.setEnabled(False)
            self.scaleTarget.setDisabled(True)

        self.setWindowIcon(QIcon("bbIcon.ico"))
        self.helpTextBrowser.append("ToDo - generate a help document to display.")

        # self.aboutAct = QAction("About",self)
        self.closeApp = QAction("Exit", self)
        self.helpInfo = QAction("Help", self)
        # self.aboutAct.setShortcut("Ctrl+A")
        # connect(About, SIGNAL(triggered()), this, SLOT(save()));
        self.closeApp.triggered.connect(self.close_application)

        install_directory = os.getcwd()
        install_directory = '%s\\doc' % install_directory
        print(install_directory)
        self.helpInfo.triggered.connect(lambda: QDesktopServices.openUrl(
            QUrl('file:///%s/blackbox_device_users_manual.html' % install_directory)))

        #self.helpAct = QAction("Help", self)
        #self.helpAct.setShortcut("Ctrl+H")
        # connect(About, SIGNAL(triggered()), this, SLOT(save()));
        #self.helpAct.triggered.connect(self.showHelp)

        self.masterLink.textChanged.connect(self.manageMainButton)
        self.nowServingDisplay1IP.textChanged.connect(self.manageMainButton)
        self.nowServingDisplay2IP.textChanged.connect(self.manageMainButton)
        self.pillarSyncBrokerPortLineEdit.textChanged.connect(self.manageMainButton)
        self.pillarSyncBrokerIpAddressLineEdit.textChanged.connect(self.manageMainButton)
        self.pillarSyncBrokerUsernameLineEdit.textChanged.connect(self.manageMainButton)
        self.pillarSyncBrokerPasswordLineEdit.textChanged.connect(self.manageMainButton)
        self.pillarSyncMqttEnabledCheckBox.stateChanged.connect(self.manageMainButton)

        self.mainToolBar.addAction(self.closeApp)
        self.mainToolBar.addAction(self.helpInfo)
        #self.mainToolBar.addAction(self.helpAct)
        # self.aboutVersion.setText(self.version)
        self.certFrame.hide()
        self.viewTrustedRootCertBut.hide()
        self.viewClientCertBut.hide()
        self.blackBoxTarget.setChecked(True)
        self.blackBoxTarget.hide()
        self.printerTarget.hide()
        self.scaleTarget.hide()
        self.chooseTargetLabel.hide()
        self.step1GraphicsView.hide()
        self.scan_in_progress = False
        self.software_upgrade_thread = None

    # def _setup_enhanced_logging(self,
    #                             filename,
    #                             maximum_file_size_bytes=5 * 1024 * 1024,
    #                             backup_logs=5):
    #     """
    #     Setup the python default logger to include additional elements in logged message
    #     as well as log backups which rotate when the maximum log size is reached
    #     :param filename: filename of the log file
    #     :param maximum_file_size_bytes: maximum size of log file before it is closed and put in rotation
    #     :param backup_logs: maximum number of backup logs maintained
    #     :return: None
    #     """
    #
    #     log_levels = {'DEBUG': logging.DEBUG,
    #                   'INFO': logging.INFO,
    #                   'WARNING': logging.WARNING,
    #                   'ERROR': logging.ERROR,
    #                   'CRITICAL': logging.CRITICAL}
    #
    #     try:
    #         logging_level = log_levels[self.log_level.upper()]
    #     except:
    #         logging_level = logging.DEBUG
    #
    #     logging.basicConfig(filename=filename,
    #                         filemode='w',
    #                         format='[%(asctime)s:%(levelname)s:%(module)s:%(funcName)s:%(lineno)d]-%(message)s',
    #                         level=logging_level,
    #                         datefmt='%m/%d/%Y %I:%M:%S %p')
    #
    #     logger = logging.getLogger('EnhancedLogger')
    #
    #     # Add the log message handler to the logger
    #     handler = logging.handlers.RotatingFileHandler(
    #         filename, maxBytes=maximum_file_size_bytes, backupCount=backup_logs)
    #
    #     logger.addHandler(handler)

    def help_info(self):
        pass

    def scan_network_for_devices(self):
        # self.scanPushButton.setDisabled(True)
        if not self.scan_in_progress:
            self.scanPushButton.setIcon(QIcon(":/resources/netscan_stop.png"))
            self.setCursor(QCursor(QtCore.Qt.WaitCursor))
            self.set_alert("Scanning network for black box devices...")
            self.rgdListWidget.clear()
            self.scan_in_progress = True
            valid_range = True
            start_ip = None
            end_ip = None
            try:
                start_ip = self.startIpLineEdit.text()
                ip = netaddr.IPAddress(start_ip.strip())
            except:
                valid_range &= False
                self.set_alert("Error: Invalid starting IP address range %s specified" % start_ip)

            if valid_range:
                try:
                    end_ip = self.endIpLineEdit.text()
                    ip = netaddr.IPAddress(end_ip.strip())
                except:
                    valid_range &= False
                    self.set_alert("Error: Invalid ending range IP address %s specified" % start_ip)

            if valid_range:
                ip_list = list(netaddr.iter_iprange(start_ip, end_ip))

                self.network_scan.scan(ip_address_list=ip_list)
                self.setCursor(QCursor(QtCore.Qt.ArrowCursor))
        else:
            self.scanPushButton.setIcon(QIcon(":/resources/netscan.png"))
            self.network_scan.stop()
            self.set_alert("Black box device discovery aborted.")
            self.scan_in_progress = False
            self.progressBar.setValue(0)

    @pyqtSlot(int)
    def software_upgrade_progress(self, percent_complete):
        self.softwareUpgradeProgressBar.setValue(percent_complete)
        if percent_complete == 100:
            self.selectSoftwareUpgradePushButton.setEnabled(True)
            self.softwareUpgradePushButton.setEnabled(True)
            self.softwareUpgradeProgressBar.setValue(0)

    @pyqtSlot(int)
    def update_scan_progress(self, percent_complete):
        self.progressBar.setValue(percent_complete)
        # print(percent_complete)
        if percent_complete >= 100:
            self.set_alert("Black box device discovery complete")
            self.progressBar.setValue(0)
            self.scan_in_progress = False
            self.scanPushButton.setIcon(QIcon(":/resources/netscan.png"))
            # self.scanPushButton.setDisabled(False)

    @pyqtSlot(str)
    def handle_network_scan_message(self, message):
        if message != '':
            self.rgdListWidget.addItem(message)
            item = self.rgdListWidget.findItems(message, QtCore.Qt.MatchRegExp)[0]
            self.rgdListWidget.scrollToItem(item, QAbstractItemView.PositionAtTop)

    def get_device_info(self):
        ip_address = self.get_selected_rgd()
        if ip_address:
            try:
                self.set_alert("INFO: Retrieving OS and application version info from selected black box device")
                with Connection(host=ip_address,
                                port=22,
                                user='root',
                                connect_timeout=2,
                                connect_kwargs={'password': 'ho1bart2'}) as c:
                    uboot_version = c.run("/usr/sbin/fw_printenv version | /usr/bin/cut -f2 -d=")
                    os_version = c.run("/bin/uname -r | /usr/bin/cut -f1 -d-")
                    application_version = c.run(
                        "/bin/cat /SOFTWARE_VERSION_INFO | /bin/grep Release | /usr/bin/awk '{print $3}'")
                    self.ubootVersionLineEdit.setText(uboot_version.stdout.strip())
                    self.osVersionLineEdit.setText(os_version.stdout.strip())
                    self.applicationVersionLineEdit.setText(application_version.stdout.strip())
                    self.set_alert("INFO: OS and Application Version Info updated")

            except:
                self.set_alert("ERROR: Can not get info of selected device") \

        else:
            self.set_alert("ERROR: No device selected")

    def get_selected_rgd(self):
        selected_rgd_items = self.rgdListWidget.selectedItems()
        if len(selected_rgd_items) == 0:
            return None
        else:
            return selected_rgd_items[0].text()

    def reboot_confirmation(self):

        reply = QMessageBox.question(self, 'Confirm Reboot',
                                     "Are you sure you want to reboot selected device?",
                                     QMessageBox.Yes, QMessageBox.No)

        if reply == QMessageBox.Yes:
            self.reboot_device()

    def reboot_device(self):
        ip_address = self.get_selected_rgd()
        if ip_address:
            status_code = network_command.remote_command(ip_address,
                                                         command="/sbin/reboot")
            if status_code == 0:
                self.set_alert("INFO: Selected device rebooted")
            else:
                self.set_alert("ERROR: Can not reboot selected device")
        else:
            self.set_alert("ERROR: No device selected")

    def close_application(self):
        self.close()

    def select_software_upgrade_file(self):

        if self.softwareUpgradePushButton.isEnabled():

            qfd = QFileDialog()
            qfd.setAcceptMode(QFileDialog.AcceptOpen)
            qfd.setDirectory(str(QStandardPaths.DocumentsLocation))
            qfd.setFileMode(QFileDialog.ExistingFile)
            qfd.setOption(QFileDialog.ReadOnly)
            qfd.setLabelText(QFileDialog.Accept, "Ok")
            qfd.setNameFilter(
                "Software Upgrade Filename (hti-linux-*.package.tgz)")
            qfd.setWindowTitle("Choose Software Upgrade Filename")
            qfd.setViewMode(QFileDialog.List)

            selected = []
            if QDialog.Accepted == qfd.exec_():
                selected = qfd.selectedFiles()

            if 1 == len(selected):
                self.softwareUpgradeFilenameLineEdit.setText(selected[0])
                # self.settings.setValue("software_upgrade_filename", selected[0])

        else:
            self.set_alert("INFO: Software upgrade already in progress")

    def set_black_box_device_time_based_on_pc_time(self):

        status_code = 1

        ip_address = self.get_selected_rgd()

        if ip_address:

            # logging.debug("about to update system time on remote system")
            self.set_alert("INFO: Updating system date time on selected black box device")
            todays_date = datetime.datetime.now().strftime('%m, %d, %Y, %H, %M, %S')
            date_tokens = todays_date.split(",")

            cmd = []
            cmd.append("/usr/bin/python")
            cmd.append("/usr/local/hobart/python/setTime.py")
            cmd.append("--day=%s" % date_tokens[1].strip())
            cmd.append("--month=%s" % date_tokens[0].strip())
            cmd.append("--year=%s" % date_tokens[2].strip())
            cmd.append("--hour=%s" % date_tokens[3].strip())
            cmd.append("--minute=%s" % date_tokens[4].strip())
            cmd.append("--second=%s" % date_tokens[5].strip())
            cmd.append('--timeZoneText="%s"' % tzlocal.get_localzone())
            cmd.append("--reboot=0")

            set_time_command = " ".join(cmd)

            status_code = network_command.remote_command(ip_address, set_time_command)

            # logging.debug("updated system time on remote system")

            if status_code == 0:
                self.set_alert("INFO: System date time updated on selected black box device")
            else:
                self.set_alert("ERROR: Could not update system date time on selected black box device")

        else:
            self.set_alert("ERROR: No black box device selected to update system date time")

        return status_code

    def upgrade_device_software(self):

        ip_address = self.get_selected_rgd()

        if ip_address:

            software_upgrade_filename = self.softwareUpgradeFilenameLineEdit.text()

            software_upgrade_filename = software_upgrade_filename.strip()

            self.set_alert("")

            if software_upgrade_filename != '':

                if os.path.exists(software_upgrade_filename):

                    status_code = self.set_black_box_device_time_based_on_pc_time()

                    if status_code == 0:
                        self.selectSoftwareUpgradePushButton.setEnabled(False)
                        self.softwareUpgradePushButton.setEnabled(False)

                        # logging.debug("about to start software upgrade worker thread")
                        self.software_upgrade_thread = SoftwareUpgradeWorker(ip_address,
                                                                             software_upgrade_filename)

                        # logging.debug("Connecting alert handler")
                        self.software_upgrade_thread.alert_handler.connect(self.set_alert)
                        # logging.debug("Connecting progress handler")
                        self.software_upgrade_thread.progress_handler.connect(self.software_upgrade_progress)
                        # logging.debug("starting software upgrade thread")
                        self.software_upgrade_thread.start()

                else:
                    self.set_alert("ERROR: Selected software upgrade file does not exist")
            else:
                self.set_alert("ERROR: No software upgrade file selected")
        else:
            self.set_alert("ERROR: no black box device selected to upgrade")

    @pyqtSlot(str)
    def set_alert(self, alert):
        self.statusBar.showMessage(alert)

    def onViewTrustedRootCertButClicked(self):
        # get cert contents text
        # load text browser
        self.certFrame.setGeometry(0, 0, 941, 631)
        self.certFrame.show()
        pass

    def onViewClientCertButClicked(self):
        pass

    def ntpFrequencyChanged(self):
        if self.ntpFrequency.currentIndex() == 0:
            self.ntpDayOfWeek.setEnabled(True)
            self.ntpDayOfWeek.setCurrentIndex(0)
        else:
            self.ntpDayOfWeek.setEnabled(False)
            self.ntpDayOfWeek.setCurrentIndex(-1)

    def showAbout(self):
        self.aboutBigFrame.setGeometry(0, 0, 961, 661)
        self.aboutBigFrame.show()
        # self.aboutFrame.setFocus()
        pass

    def showHelp(self):
        self.helpFrame.setGeometry(0, 0, 961, 631)
        self.helpFrame.show()

    def on_closeHelpBut_clicked(self):
        self.helpFrame.hide()

    @pyqtSlot()
    def on_openBlackBoxFileBut_clicked(self):
        # // let's clear everything
        self.clearEverything()
        importIsOK = False
        # filename1 = QFileDialog.getOpenFileName(self,"Hobart Device Configuration File", QDir.currentPath(),"All files (*.*)")

        c = wmi.WMI()
        removable_disks = c.Win32_LogicalDisk(DriveType=2)
        dirpath = "C:\\"
        if len(removable_disks):
            for disk in removable_disks:
                for dirpath, dirnames, filenames in os.walk(disk.Name + "\\"):
                    print(dirpath)
                    break
            filename1 = QFileDialog.getOpenFileName(self, "Hobart Device Configuration File", dirpath,
                                                    "All files (*.*)")[0]
        else:

            CSIDL_PERSONAL = 5  # My Documents
            SHGFP_TYPE_CURRENT = 0  # Get current, not default value

            buf = ctypes.create_unicode_buffer(ctypes.wintypes.MAX_PATH)
            ctypes.windll.shell32.SHGetFolderPathW(None, CSIDL_PERSONAL, None, SHGFP_TYPE_CURRENT, buf)

            # print(buf.value)
            filename1 = QFileDialog.getOpenFileName(self, "Hobart Device Configuration File", buf.value,
                                                    "All files (*.*)")[0]

        f = QFile()
        fileInfo = QFileInfo()

        self.importDir = os.path.dirname(os.path.abspath(filename1))
        if filename1 and filename1 != '':
            bbOpenedFile = filename1
            f = QFile(filename1)
            fileInfo = QFileInfo(f.fileName())

            filenameOnly = fileInfo.fileName()
            # self.bigBBLabel.setGeometry(20, -3, 441, 41)

            # self.openBBFileName.show()
            # self.openBBFileName.setText(filenameOnly)
            # self.editingFileLabel.show()
        else:
            self.clearAllButTargets()
            # self.blackBoxTarget.setChecked()
            self.step1GraphicsView.hide()
        self.alertsLineEdit.setText("Existing configuration has not been saved.")
        file = QFile(filename1)

        importIsOK = True
        if (importIsOK):
            # // redact all passwords
            self.WPAPassphrase.setEchoMode(QLineEdit.Password)
            self.WEPKey1.setEchoMode(QLineEdit.Password)
            self.WEPKey2.setEchoMode(QLineEdit.Password)
            self.WEPKey3.setEchoMode(QLineEdit.Password)
            self.WEPKey4.setEchoMode(QLineEdit.Password)
            self.tlsCLientPassword.setEchoMode(QLineEdit.Password)
            self.eapPassword.setEchoMode(QLineEdit.Password)

            decryptedContents = []

            networkConfigurations = encryptDecryptBB.importConfigs(str(filename1), "woozal")
            self.testingLabel.setPlainText(self.importDir + "\n" + networkConfigurations)
            self.testingFrame.setGeometry(10, 10, 871, 441)
            if self.testing_:
                self.testingFrame.show()
            else:
                self.testingFrame.hide()
            exitCode = 0
            if exitCode == 0:
                decryptedContents = networkConfigurations.splitlines()

            if len(decryptedContents) > 0:
                howMany = 0
                self.clearEverything()
                self.bbConfigTab.setTabText(0, "Black Box Configuration: %s" % filenameOnly)

                haveSuffix = False

                for i in range(0, len(decryptedContents)):

                    keyValuePairs = decryptedContents[i]
                    oneKeyValuePair = []
                    oneKeyValuePair = keyValuePairs.split(":")
                    key = oneKeyValuePair[0]
                    # print(str("KEY  :" + key))
                    if len(oneKeyValuePair) > 1:
                        value = oneKeyValuePair[1]
                        # print(str("VALUE: " + value))
                        key = key.strip()
                        value = value.strip()
                        if key == "TARGETTYPE":
                            if value == "BB":
                                self.blackBoxTarget.setChecked(True)
                                # self.createHDConfigurationFileBut.setText("Save Blackbox Configuration")
                            elif value == "SC":
                                self.scaleTarget.setChecked(True)
                                # self.createHDConfigurationFileBut.setText("Save Scale Configuration")
                            elif value == "PR":
                                self.printerTarget.setChecked(True)
                                # self.createHDConfigurationFileBut.setText("Save Printer Configuration")
                            else:
                                self.blackBoxTarget.setChecked(True)
                            self.step1GraphicsView.hide()
                            self.manageMainButton()
                        elif key == "SSID":
                            if value == "" or len(value) == 0:
                                # // it is wired
                                self.wiredRadio.setChecked(True)
                                self.SSID.setText("")
                            else:
                                # // it is wireless
                                self.wirelessRadio.setChecked(True)
                                self.SSID.setText(value)
                                self.step2GraphicsView.hide()
                                self.step2Label.hide()
                                self.manageMainButton()
                        elif key == "SECURITY" or key == "AUTHENTICATION":
                            self.securityCombo.setCurrentIndex(self.securityCombo.findText(value))
                            self.importSecurity_ = value
                            # // setup encryption boxes now
                            self.securityChanged(value)
                        elif key == "ENCRYPTION" or key == "CIPHER":
                            self.encryptionCombo.setCurrentIndex(self.encryptionCombo.findText(value))
                            self.importEncryption_ = value
                        elif key == "PASSPHRASE":
                            self.WPAPassphrase.setText(value)
                            self.WPAShowPassphrase.setChecked(False)
                            self.WPAShowPassphrase.setEnabled(False)
                            self.WPAPassphrase.setEchoMode(QLineEdit.Password)
                        elif key == "LINKRATE":
                            self.advancedWirelessLinkRateCombo.setCurrentIndex(
                                self.advancedWirelessLinkRateCombo.findData(value))
                            if self.advancedWirelessLinkRateCombo.currentIndex() != 0:
                                howMany += 1
                        elif key == "AUTOMATICLINKRATE":
                            if value == "1":
                                self.advancedWirelessAutoLinkRateCheckBox.setChecked(True)
                            elif value == "0":
                                howMany += 1
                                self.advancedWirelessAutoLinkRateCheckBox.setChecked(False)
                        elif key == "CHANNEL":
                            self.advancedWirelessChannelCombo.setCurrentIndex(
                                self.advancedWirelessChannelCombo.findData(value))
                            if self.advancedWirelessChannelCombo.currentIndex() != 0:
                                howMany += 1
                        elif key == "ISSTATIC":
                            if value == "1":
                                self.staticIPRadio.setEnabled(True)
                                self.manualDNSRadio.setEnabled(True)
                                self.leGateway.setEnabled(True)
                                self.leIP.setEnabled(True)
                                self.leSubnet.setEnabled(True)
                                self.leGateway.setEnabled(True)
                                self.lePrimary.setEnabled(True)
                                self.leSecondary.setEnabled(True)
                                self.leTertiary.setEnabled(True)
                                self.staticIPRadio.setChecked(True)
                                self.manualDNSRadio.setChecked(True)
                            elif value == "0":
                                self.DHCPRadio.setChecked(True)
                        elif key == "IPADDRESS":
                            if value != "64641964" and value != "" and len(value) >= 7:
                                self.leIP.setText(value)
                            else:
                                self.leIP.setText("")
                            if self.staticIPRadio.isChecked():
                                self.leIP.setEnabled(True)
                        elif key == "SUBNET":
                            if value != "64641964" and value != "" and len(value) >= 7:
                                self.leSubnet.setText(value)
                            else:
                                self.leSubnet.setText("")
                            if self.staticIPRadio.isChecked():
                                self.leSubnet.setEnabled(True)
                        elif key == "GATEWAY":
                            if value != "64641964" and value != "" and len(value) >= 7:
                                self.leGateway.setText(value)
                            else:
                                self.leGateway.setText("")
                            if self.staticIPRadio.isChecked():
                                self.leGateway.setEnabled(True)
                        elif key == "AUTODNS":
                            if value == "1":
                                self.automaticDNSRadio.setChecked(True)
                            elif value == "0":
                                self.manualDNSRadio.setChecked(True)
                        elif key == "PRIMARY":
                            if value != "64641964" and value != "" and len(value) >= 7:
                                self.lePrimary.setText(value)
                            else:
                                self.lePrimary.setText("")
                        elif key == "SECONDARY":
                            if value != "64641964" and value != "" and len(value) >= 7:
                                self.leSecondary.setText(value)
                            else:
                                self.leSecondary.setText("")
                        elif key == "TERTIARY":
                            if value != "64641964" and value != "" and len(value) >= 7:
                                self.leTertiary.setText(value)
                            else:
                                self.leTertiary.setText("")
                        elif key == "DNSSUFFIX1":
                            if value != "64641964" and value != "" and len(value) >= 7:
                                self.advancedWirelessDNSSuffix1.setText(value)
                                if not haveSuffix:
                                    howMany += 1
                                    haveSuffix = True
                            else:
                                self.advancedWirelessDNSSuffix1.setText("")
                        elif key == "DNSSUFFIX2":
                            if value != "64641964" and value != "" and len(value) >= 7:
                                self.advancedWirelessDNSSuffix2.setText(value)
                                if not haveSuffix:
                                    howMany += 1
                                    haveSuffix = True
                            else:
                                self.advancedWirelessDNSSuffix2.setText("")
                        elif key == "DNSSUFFIX3":
                            if value != "64641964" and value != "" and len(value) >= 7:
                                self.advancedWirelessDNSSuffix3.setText(value)
                                if not haveSuffix:
                                    howMany += 1
                                    haveSuffix = True
                            else:
                                self.advancedWirelessDNSSuffix3.setText("")
                        elif key == "DNSSUFFIX4":
                            if value != "64641964" and value != "" and len(value) >= 7:
                                self.advancedWirelessDNSSuffix3.setText(
                                    self.advancedWirelessDNSSuffix3.text() + " " + value)
                                if not haveSuffix:
                                    howMany += 1
                                    haveSuffix = True
                        elif key == "DNSSUFFIX5":
                            if value != "64641964" and value != "" and len(value) >= 7:
                                self.advancedWirelessDNSSuffix3.setText(
                                    self.advancedWirelessDNSSuffix3.text() + " " + value)
                                if not haveSuffix:
                                    howMany += 1
                                    haveSuffix = True
                        elif key == "DNSSUFFIX6":
                            if value != "64641964" and value != "" and len(value) >= 7:
                                self.advancedWirelessDNSSuffix3.setText(
                                    self.advancedWirelessDNSSuffix3.text() + " " + value)
                                if not haveSuffix:
                                    howMany += 1
                                    haveSuffix = True
                        elif key == "HOSTNAME":
                            if value != "64641964" and value != "" and value != "-1":
                                self.leHostname.setText(value)
                            else:
                                self.leHostname.setText("")
                        elif key == "TIMEKEEPER":
                            if value == "NTP":
                                self.addNTPSetting.setEnabled(True)
                                self.addNTPSetting.setChecked(True)
                            elif value == "REMOVE":
                                self.removeNTPSettings.setChecked(True)
                                self.addNTPSetting.setChecked(True)
                            else:
                                self.addNTPSetting.setChecked(False)
                                self.addNTPSetting.setEnabled(False)
                                self.ntpDayOfWeek.setCurrentIndex(0)
                                self.ntpFrequency.setCurrentIndex(0)
                                self.ntpTimezone.setCurrentIndex(23)
                                defaultTime = QTime(3, 30, 0, 0)
                                self.ntpTimeOfDay.setTime(defaultTime)
                                self.ntpServerIP.setText("")
                        elif key == "NTPSERVER":
                            self.ntpServerIP.setText(value)
                            if len(value):
                                self.addNTPSetting.setChecked(True)
                                self.addNTPSetting.setEnabled(True)
                            else:
                                self.addNTPSetting.setChecked(False)
                                self.addNTPSetting.setEnabled(False)
                        elif key == "NTPFREQUENCY":
                            if value != "64641964" and value != "":
                                newValue = 0
                                try:
                                    newValue = int(value)
                                except:
                                    pass
                                self.ntpFrequency.setCurrentIndex(newValue)
                            else:
                                self.ntpFrequency.setCurrentIndex(0)
                        elif key == "NTPDAYOFWEEK":
                            if value != "64641964" and value != "":
                                newValue = 0
                                try:
                                    newValue = int(value)
                                except:
                                    pass
                                self.ntpDayOfWeek.setCurrentIndex(newValue)
                            else:
                                self.ntpDayOfWeek.setCurrentIndex(0)
                        elif key == "NTPTIMEOFDAY":
                            importedTime = QTime()
                            defaultTime = QTime(3, 30, 0, 0)
                            if value != "64641964" and value != "":
                                # importingTimeList = []

                                # importingTimeList = value.split(":")
                                hour = ""
                                minute = ""
                                second = ""
                                if len(oneKeyValuePair) == 4:
                                    hour = oneKeyValuePair[1]
                                    minute = oneKeyValuePair[2]
                                    second = oneKeyValuePair[3]
                                    importedTime = QTime(int(hour), int(minute), int(second))
                                    self.ntpTimeOfDay.setTime(importedTime)
                                else:
                                    self.ntpTimeOfDay.setTime(defaultTime)
                        elif key == "NTPTIMEZONE":
                            if value != "":
                                self.ntpTimezone.setCurrentIndex(self.ntpTimezone.findData(value))
                        elif (key == "NTPANTIMERIDIAN"):
                            # // taken care of by NTPTIMEOFDAY key
                            pass
                        elif key == "DEFAULT":
                            if value == "1":
                                self.wepKey1Radio.setChecked(True)
                            elif value == "2":
                                self.wepKey2Radio.setChecked(True)
                            elif value == "3":
                                self.wepKey3Radio.setChecked(True)
                            elif value == "4":
                                self.wepKey4Radio.setChecked(True)
                            else:
                                self.wepKey1Radio.setChecked(True)
                        elif key == "KEY 1":
                            self.WEPKey1.setText(value)
                            self.WEPShowKeys.setChecked(False)
                            self.WEPShowKeys.setEnabled(False)
                            self.WEPKey1.setEchoMode(QLineEdit.Password)
                        elif key == "KEY 2":
                            self.WEPKey2.setText(value)
                            self.WEPShowKeys.setChecked(False)
                            self.WEPShowKeys.setEnabled(False)
                            self.WEPKey2.setEchoMode(QLineEdit.Password)
                        elif key == "KEY 3":
                            self.WEPKey3.setText(value)
                            self.WEPShowKeys.setChecked(False)
                            self.WEPShowKeys.setEnabled(False)
                            self.WEPKey3.setEchoMode(QLineEdit.Password)
                        elif key == "KEY 4":
                            self.WEPKey4.setText(value)
                            self.WEPShowKeys.setChecked(False)
                            self.WEPShowKeys.setEnabled(False)
                            self.WEPKey4.setEchoMode(QLineEdit.Password)
                        elif key == "USERNAME":
                            self.eapUsername.setText(value)
                        elif key == "PASSWORD":
                            self.eapPassword.setText(value)
                            self.eapShowPassword.setChecked(False)
                            self.eapShowPassword.setEnabled(False)
                            self.eapPassword.setEchoMode(QLineEdit.Password)
                        elif key == "TRUSTEDROOTFILENAME":
                            filename = bbOpenedFile
                            if filename and filename != '':
                                f = QFile(filename)
                                fileInfo = QFileInfo(f)
                                path = fileInfo.path()
                                # self.alertsLineEdit.setText(path)
                                certName = value
                                self.chooseRootCertBut.setText(value)
                                fi = QFileInfo(QDir(path), value)
                                self.rootCertFilename = fi.absoluteFilePath()
                            else:
                                self.rootCertFilename = ""
                            if self.testing_:
                                self.testingLabel.appendPlainText(self.rootCertFilename)
                                self.testingFrame.show()

                        elif key == "CLIENTCERTFILENAME":
                            # // use the same dir as the opened export file because that is
                            # // where the certs have been copied
                            filename = bbOpenedFile

                            if filename and filename != '':
                                f = QFile(filename)
                                fileInfo = QFileInfo(f)
                                path = fileInfo.path()
                                # self.alertsLineEdit.setText(path)
                                certName = value
                                self.chooseClientCertBut.setText(value)
                                fi = QFileInfo(QDir(path), value)
                                self.clientCertFilename = fi.absoluteFilePath()
                            else:
                                self.clientCertFilename = ""
                            if self.testing_:
                                self.testingLabel.appendPlainText(self.clientCertFilename)
                                self.testingFrame.show()

                        elif key == "CLIENTCERTPASSWORD":
                            if value != "" or value != "64641964":
                                self.tlsCLientPassword.setEchoMode(QLineEdit.Password)
                                self.tlsCLientPassword.setText(value)
                                self.tlsShowPassword.setChecked(False)
                                self.tlsShowPassword.setEnabled(False)
                            else:
                                self.tlsCLientPassword.setEchoMode(QLineEdit.Normal)
                                self.tlsCLientPassword.setText("")
                                self.tlsShowPassword.setChecked(True)
                                self.tlsShowPassword.setEnabled(True)
                        elif key == "MASTERLINK":
                            if value != "" and value != "64641964":
                                self.masterLink.setText(value)
                            else:
                                self.masterLink.setText("")
                        elif key == "WIRELESSDRIVER":
                            if value == "wext":
                                self.advancedWirelessDriverCombo.setCurrentIndex(1)
                                howMany += 0
                            else:
                                self.advancedWirelessDriverCombo.setCurrentIndex(0)
                        elif key == "ADDNETWORK":
                            if value == "1":
                                self.addNetworkSetting.setChecked(True)
                                self.addNetworkSetting.setEnabled(True)
                            else:
                                self.addNetworkSetting.setChecked(False)
                        elif key == "ADDNTP":
                            if value == "1":
                                self.addNTPSetting.setChecked(True)
                                self.addNTPSetting.setEnabled(True)
                            else:
                                self.addNTPSetting.setChecked(False)
                        elif key == "ADDHOSTNAME":
                            if value == "1":
                                self.addHostnameCheckbox.setChecked(True)
                                self.addHostnameCheckbox.setEnabled(True)
                            else:
                                self.addHostnameCheckbox.setChecked(False)
                        elif key == "ADDMASTERLINK":
                            if value == "1":
                                self.addMasterLinkCheckbox.setChecked(True)
                                self.addMasterLinkCheckbox.setChecked(True)
                            else:
                                pass
                                # self.addMasterLinkCheckbox.setChecked(False)
                        elif key == "DISPLAY1":
                            if len(value):
                                self.nowServingDisplay1IP.setText(str(value))
                            else:
                                self.nowServingDisplay1IP.setText("")
                        elif key == "DISPLAY2":
                            if len(value):
                                self.nowServingDisplay2IP.setText(str(value))
                            else:
                                self.nowServingDisplay2IP.setText("")
                    elif key.strip() != "":
                        # // corrupt file
                        self.alertsLineEdit.setText("The file does not appear to be HT Device compatible.")
                        self.editingFileLabel.setText("")
                        self.editingFileLabel.hide()
                        self.on_startBlackBoxFileBut_clicked()
                        self.alertsLineEdit.setText("The file does not appear to be HT Device compatible.")

                if self.ntpFrequency.currentIndex() != 0:
                    self.ntpDayOfWeek.setCurrentIndex(-1)
        self.manageMainButton()
        if self.wirelessRadio.isChecked():
            self.correctSecurityEncryption()
        self.BBtabWidget.setCurrentIndex(0)
        # self.blackBoxTarget.setChecked(True)

    def WPAShowPasswordToggled(self, state):
        if state:
            self.WPAPassphrase.setEchoMode(QLineEdit.Normal)
        else:
            self.WPAPassphrase.setEchoMode(QLineEdit.Password)

    def EAPShowPasswordToggled(self, state):
        if (state):
            self.eapPassword.setEchoMode(QLineEdit.Normal)
        else:
            self.eapPassword.setEchoMode(QLineEdit.Password)

    def TLSShowClientPasswordToggled(self, state):
        if (state):
            self.tlsCLientPassword.setEchoMode(QLineEdit.Normal)
        else:
            self.tlsCLientPassword.setEchoMode(QLineEdit.Password)

    def WEPShowKeysToggled(self, state):
        if (state):
            self.WEPKey1.setEchoMode(QLineEdit.Normal)
            self.WEPKey2.setEchoMode(QLineEdit.Normal)
            self.WEPKey3.setEchoMode(QLineEdit.Normal)
            self.WEPKey4.setEchoMode(QLineEdit.Normal)
        else:
            self.WEPKey1.setEchoMode(QLineEdit.Password)
            self.WEPKey2.setEchoMode(QLineEdit.Password)
            self.WEPKey3.setEchoMode(QLineEdit.Password)
            self.WEPKey4.setEchoMode(QLineEdit.Password)

    def correctSecurityEncryption(self):
        self.securityChanged(str(self.securityCombo.currentText()))
        self.encryptionCombo.setCurrentIndex(self.encryptionCombo.findText(self.importEncryption_))

    def securityChanged(self, value):
        value = str(self.securityCombo.currentText())
        if value == "OPEN" or value == "Open":
            self.encryptionCombo.clear()
            self.encryptionCombo.addItem("WEP", "WEP")
            self.encryptionCombo.addItem("None", "None")
        elif value == "WPA-Personal" or value == "WPA2-Personal" or value == "WPA EAP-TLS" or value == "WPA-Enterprise (PEAP)" or value == "WPA-Enterprise (LEAP)" or self.securityCombo.currentText() == "WPA2 EAP-TLS" or self.securityCombo.currentText() == "WPA2-Enterprise (PEAP)" or self.securityCombo.currentText() == "WPA2-Enterprise (LEAP)":
            self.encryptionCombo.clear()
            self.encryptionCombo.addItem("AES", "AES")
            self.encryptionCombo.addItem("TKIP", "TKIP")
        elif value == "802.1X (PEAP)" or value == "802.1X (LEAP)":
            self.encryptionCombo.clear()
            self.encryptionCombo.addItem("WEP", "WEP")
        # frames
        if value == "OPEN":
            self.wpaPassphraseFrame.hide()
            self.eapFrame.hide()
            self.tlsFrame.hide()
            self.openWEPframe.show()
        elif value == "WPA-Personal" or value == "WPA2-Personal":
            self.eapFrame.hide()
            self.openWEPframe.hide()
            self.tlsFrame.hide()
            self.wpaPassphraseFrame.show()
        elif value == "WPA-Enterprise (PEAP)" or value == "WPA-Enterprise (LEAP)" or value == "WPA2-Enterprise (PEAP)" or value == "WPA2-Enterprise (LEAP)" or value == "802.1X (PEAP)" or value == "802.1X (LEAP)":
            self.openWEPframe.hide()
            self.wpaPassphraseFrame.hide()
            self.tlsFrame.hide()
            self.eapFrame.show()
        elif value == "WPA EAP-TLS" or value == "WPA2 EAP-TLS":
            self.openWEPframe.hide()
            self.wpaPassphraseFrame.hide()
            self.eapFrame.hide()
            self.tlsFrame.show()
        clientCertFilename = ""
        rootCertFilename = ""

    def wiredRadioClicked(self, checked):
        if checked:
            self.wirelessGroupBox.hide()
        else:
            self.wirelessGroupBox.show()

    def on_deleteRootBut_clicked(self):
        self.rootCertFilename = ""
        self.chooseRootCertBut.setText("Choose Root Certificate File")

    def on_deleteClientBut_clicked(self):
        self.clientCertFilename = ""
        self.chooseClientCertBut.setText("Choose Client Certificate File")
        self.tlsCLientPassword.setText("")
        self.tlsShowPassword.setChecked(True)
        self.tlsShowPassword.setEnabled(True)

    def eventFilter(self, object, event):
        if event.type() == QEvent.FocusOut:
            if object == self.tlsCLientPassword:
                self.tlsShowPassword.setChecked(False)
                self.tlsShowPassword.setEnabled(False)
                self.tlsCLientPassword.setEchoMode(QLineEdit.Password)
            elif object == self.WPAPassphrase:
                self.WPAShowPassphrase.setChecked(False)
                self.WPAShowPassphrase.setEnabled(False)
                self.WPAPassphrase.setEchoMode(QLineEdit.Password)
            elif object == self.eapPassword:
                self.eapShowPassword.setChecked(False)
                self.eapShowPassword.setEnabled(False)
                self.eapPassword.setEchoMode(QLineEdit.Password)
            elif object == self.WEPKey1:
                self.WEPShowKeys.setChecked(False)
                self.WEPShowKeys.setEnabled(False)
                self.WEPKey1.setEchoMode(QLineEdit.Password)
            elif object == self.WEPKey2:
                self.WEPShowKeys.setChecked(False)
                self.WEPShowKeys.setEnabled(False)
                self.WEPKey2.setEchoMode(QLineEdit.Password)
            elif object == self.WEPKey3:
                self.WEPShowKeys.setChecked(False)
                self.WEPShowKeys.setEnabled(False)
                self.WEPKey3.setEchoMode(QLineEdit.Password)
            elif object == self.WEPKey4:
                self.WEPShowKeys.setChecked(False)
                self.WEPShowKeys.setEnabled(False)
                self.WEPKey4.setEchoMode(QLineEdit.Password)
        elif event.type() == QEvent.MouseButtonPress:
            if self.aboutBigFrame.isVisible():
                self.aboutBigFrame.hide()

        return False

    def isADeviceChosen(self):
        answer = False
        if self.blackBoxTarget.isChecked() or self.scaleTarget.isChecked() or self.printerTarget.isChecked():
            answer = True
        else:
            answer = False
        return answer

    def isASettingTypeChosen(self):
        answer = False
        if self.addNetworkSetting.isChecked() or self.addNTPSetting.isChecked() or \
                self.addPillarSyncCheckbox.isChecked() or self.addHostnameCheckbox.isChecked():
            answer = True
        else:
            answer = False
        return answer

    def validIpAddress(self, ip_address, required=False):
        entry_valid = False
        ip_address = ip_address.strip()
        if required:
            if ip_address != "":
                tokens = ip_address.split('.')
                if len(tokens) == 4:
                    four_numerics_found = True
                    for token in tokens:
                        if not token.isdigit():
                            four_numerics_found = False
                            break
                    if four_numerics_found:
                        try:
                            ip = netaddr.IPAddress(ip_address)
                            entry_valid = True
                        except:
                            pass
        else:
            entry_valid = True
        return entry_valid

    def manageMainButton(self):

        self.createHDConfigurationFileBut.setEnabled(False)

        if (self.DHCPRadio.isChecked() or self.staticIPRadio.isChecked()) and (
                self.automaticDNSRadio.isChecked() or self.manualDNSRadio.isChecked()) and (
                not self.wirelessRadio.isChecked()):
            if self.addHostnameCheckbox.isChecked() or self.addNetworkSetting.isChecked() or \
                    self.addPillarSyncCheckbox or self.addNTPSetting.isChecked():
                self.createHDConfigurationFileBut.setEnabled(True)

        elif self.wirelessRadio.isChecked() and self.SSID.text() == "":
            self.createHDConfigurationFileBut.setEnabled(False)
            self.addNetworkSetting.setEnabled(False)
            self.addNetworkSetting.setChecked(False)
            self.step2GraphicsView.show()
            self.step2Label.show()

        if not self.addHostnameCheckbox.isChecked() and not self.addNetworkSetting.isChecked() and \
                not self.addPillarSyncCheckbox.isChecked() and not self.addNTPSetting.isChecked():
            self.createHDConfigurationFileBut.setEnabled(False)
        else:
            if self.wirelessRadio.isChecked() and str(self.SSID.text()) != "" and self.addNetworkSetting.isChecked():
                self.createHDConfigurationFileBut.setEnabled(True)
            elif self.wiredRadio.isChecked():
                if self.addNetworkSetting.isChecked():
                    if self.staticIPRadio.isChecked():
                        if self.validIpAddress(self.leIP.text(), True) and \
                            self.validIpAddress(self.leGateway.text(), True) and \
                                self.validIpAddress(self.leSubnet.text(), True):
                            self.addNetworkSetting.setChecked(True)
                            self.createHDConfigurationFileBut.setEnabled(True)
                        else:
                            self.createHDConfigurationFileBut.setEnabled(False)
                            self.addNetworkSetting.setChecked(False)

        if self.isASettingTypeChosen():
            self.createHDConfigurationFileBut.setEnabled(True)
        else:
            self.createHDConfigurationFileBut.setEnabled(False)

        if self.wirelessRadio.isChecked() and self.SSID.text() == "":
            self.addNetworkSetting.setEnabled(False)

        elif self.wirelessRadio.isChecked() and self.SSID.text() != "":
            self.addNetworkSetting.setEnabled(True)

        if self.wiredRadio.isChecked() and self.DHCPRadio.isChecked() and self.automaticDNSRadio.isChecked():
            self.addNetworkSetting.setEnabled(True)

        if self.addMasterLinkCheckbox.isChecked():
            entry_valid = False
            if self.validIpAddress(self.masterLink.text(), True) and \
                self.validIpAddress(self.nowServingDisplay1IP.text(), True) and \
                    self.validIpAddress(self.nowServingDisplay2IP.text(), True):
                entry_valid = True
            self.addMasterLinkCheckbox.setEnabled(True)
            self.createHDConfigurationFileBut.setEnabled(entry_valid)

        if self.addPillarSyncCheckbox.isChecked():
            entry_valid = False
            self.set_alert("")
            if self.pillarSyncMqttEnabledCheckBox.isChecked():
                print(self.pillarSyncBrokerIpAddressLineEdit.text())
                if self.pillarSyncBrokerIpAddressLineEdit.text().strip() != '...' and \
                        self.pillarSyncBrokerPortLineEdit.text().strip() != '':
                    if self.validIpAddress(self.pillarSyncBrokerIpAddressLineEdit.text()):
                        entry_valid = True
                    else:
                        self.set_alert("Error: Invalid broker IP address specified")
            else:
                entry_valid = True

            self.addMasterLinkCheckbox.setEnabled(entry_valid)
            self.createHDConfigurationFileBut.setEnabled(entry_valid)

    def dhcpRadioClicked(self):
        self.manageMainButton()
        IP_ = self.leIP.text
        self.leIP.setText("")
        self.leIP.setEnabled(False)
        subnet_ = self.leSubnet.text
        self.leSubnet.setText("")
        self.leSubnet.setEnabled(False)
        gateway_ = self.leGateway.text
        self.leGateway.setText("")
        self.leGateway.setEnabled(False)

    def clearIPSubGate(self):
        IP_ = ""
        subnet_ = ""
        gateway_ = ""
        self.leIP.setText("")
        self.leGateway.setText("")
        self.leSubnet.setText("")

    def clearDNS(self):
        primary_ = ""
        secondary_ = ""
        tertiary_ = ""
        self.lePrimary.setText("")
        self.leSecondary.setText("")
        self.leTertiary.setText("")

    def dhcpRadioClicked(self):
        self.manageMainButton()
        IP_ = self.leIP.text()
        self.leIP.setText("")
        self.leIP.setEnabled(False)
        subnet_ = self.leSubnet.text()
        self.leSubnet.setText("")
        self.leSubnet.setEnabled(False)
        gateway_ = self.leGateway.text()
        self.leGateway.setText("")
        self.leGateway.setEnabled(False)

    def clearWirelessFields(self):
        self.SSID.setText("")
        self.securityCombo.setCurrentIndex(0)
        self.encryptionCombo.setCurrentIndex(0)
        self.WPAPassphrase.setText("")
        self.WPAPassphrase.setEchoMode(QLineEdit.Normal)
        self.WPAShowPassphrase.setEnabled(True)
        self.WPAShowPassphrase.setChecked(True)
        self.WEPKey1.setText("")
        self.WEPKey1.setEchoMode(QLineEdit.Normal)
        self.WEPKey2.setText("")
        self.WEPKey2.setEchoMode(QLineEdit.Normal)
        self.WEPKey3.setText("")
        self.WEPKey3.setEchoMode(QLineEdit.Normal)
        self.WEPKey4.setText("")
        self.WEPKey4.setEchoMode(QLineEdit.Normal)
        self.WEPShowKeys.setEnabled(True)
        self.WEPShowKeys.setChecked(True)
        self.wepKey1Radio.setChecked(True)
        self.eapPassword.setText("")
        self.eapPassword.setEchoMode(QLineEdit.Normal)
        self.eapShowPassword.setEnabled(True)
        self.eapShowPassword.setChecked(True)
        self.eapUsername.setText("")
        self.tlsCLientPassword.setText("")
        self.tlsCLientPassword.setEchoMode(QLineEdit.Normal)
        self.tlsShowPassword.setEnabled(True)
        self.tlsShowPassword.setChecked(True)
        clientCertFilename = ""
        rootCertFilename = ""

    def loadAdvancedWirelessBBChoices(self):
        self.advancedWirelessLinkRateCombo.clear()
        self.advancedWirelessLinkRateCombo.addItem(" Automatic", "auto")
        self.advancedWirelessLinkRateCombo.addItem(" 1 Mbps", "1M")
        self.advancedWirelessLinkRateCombo.addItem(" 11 Mbps  - Mode B", "11M")
        self.advancedWirelessLinkRateCombo.addItem(" 24 Mbps", "24M")
        self.advancedWirelessLinkRateCombo.addItem(" 36 Mbps", "36M")
        self.advancedWirelessLinkRateCombo.addItem(" 48 Mbps", "48M")
        self.advancedWirelessLinkRateCombo.addItem(" 54 Mbps  - Mode G ", "54M")
        self.advancedWirelessLinkRateCombo.addItem(" 150 Mbps - Mode N", "149M")
        self.advancedWirelessChannelCombo.clear()
        self.advancedWirelessChannelCombo.addItem("Auto : Roaming", "auto")
        self.advancedWirelessChannelCombo.addItem(" Channel 1 : 2.412 GHz", "2412")
        self.advancedWirelessChannelCombo.addItem(" Channel 2 : 2.417 GHz", "2417")
        self.advancedWirelessChannelCombo.addItem(" Channel 3 : 2.422 GHz", "2422")
        self.advancedWirelessChannelCombo.addItem(" Channel 4 : 2.427 GHz", "2427")
        self.advancedWirelessChannelCombo.addItem(" Channel 5 : 2.432 GHz", "2432")
        self.advancedWirelessChannelCombo.addItem(" Channel 6 : 2.437 GHz", "2437")
        self.advancedWirelessChannelCombo.addItem(" Channel 7 : 2.442 GHz", "2442")
        self.advancedWirelessChannelCombo.addItem(" Channel 8 : 2.447 GHz", "2447")
        self.advancedWirelessChannelCombo.addItem(" Channel 9 : 2.452 GHz", "2452")
        self.advancedWirelessChannelCombo.addItem(" Channel 10 : 2.457 GHz", "2457")
        self.advancedWirelessChannelCombo.addItem(" Channel 11 : 2.462 GHz", "2462")
        self.advancedWirelessChannelCombo.addItem(" Channel 149 : 5.745 GHz", "5745")
        self.advancedWirelessChannelCombo.addItem(" Channel 153 : 5.765 GHz", "5765")
        self.advancedWirelessChannelCombo.addItem(" Channel 157 : 5.785 GHz", "5785")
        self.advancedWirelessChannelCombo.addItem(" Channel 161 : 5.805 GHz", "5805")
        self.advancedWirelessChannelCombo.addItem(" Channel 165 : 5.825 GHz", "5825")

    def manageWirelessGroupBox(self):
        if self.wiredRadio.isChecked():
            self.wirelessGroupBox.hide()
            self.clearWirelessFields()

        elif self.wirelessRadio.isChecked():
            self.wirelessGroupBox.show()

    def staticIPRadioClicked(self):
        self.manageMainButton()

    def automaticDNSRadioClicked(self):
        self.manageMainButton()

    def manualDNSRadioClicked(self):
        self.manageMainButton()

    def clearAddSettingsCheckboxes(self):
        self.addHostnameCheckbox.setChecked(False)
        self.addMasterLinkCheckbox.setChecked(False)
        self.addNetworkSetting.setChecked(False)
        self.addPillarSyncCheckbox.setChecked(False)
        # self.addNTPSetting.setChecked(False)

    def clearTargetTypes(self):
        self.blackBoxTarget.setAutoExclusive(False)
        self.printerTarget.setAutoExclusive(False)
        self.scaleTarget.setAutoExclusive(False)
        self.blackBoxTarget.setChecked(False)
        self.printerTarget.setChecked(False)
        self.scaleTarget.setChecked(False)
        self.blackBoxTarget.setAutoExclusive(True)
        self.printerTarget.setAutoExclusive(True)
        self.scaleTarget.setAutoExclusive(True)

    def hideMonitors(self):
        self.nowServingDisplay1FormatHelp.hide()
        self.nowServingDisplay1IP.hide()
        self.nowServingDisplay1IP.setText("")
        self.nowServingDisplay1Label.hide()
        self.nowServingDisplay2FormatHelp.hide()
        self.nowServingDisplay2IP.hide()
        self.nowServingDisplay2IP.setText("")
        self.nowServingDisplay2Label.hide()

    def showMonitors(self):
        self.nowServingDisplay1FormatHelp.show()
        self.nowServingDisplay1IP.show()
        self.nowServingDisplay1IP.setText("")
        self.nowServingDisplay1Label.show()
        self.nowServingDisplay2FormatHelp.show()
        self.nowServingDisplay2IP.show()
        self.nowServingDisplay2IP.setText("")
        self.nowServingDisplay2Label.show()

    def clearHostname(self):
        self.leHostname.setText("")

    def clearMasterLink(self):
        self.masterLink.setText("")

    def clearAdvancedNetworking(self):
        self.advancedWirelessChannelCombo.setCurrentIndex(0)
        self.advancedWirelessAutoLinkRateCheckBox.setChecked(True)
        self.advancedWirelessDNSSuffix1.setText("")
        self.advancedWirelessDNSSuffix2.setText("")
        self.advancedWirelessDNSSuffix3.setText("")
        self.advancedWirelessDriverCombo.setCurrentIndex(0)
        self.advancedWirelessLinkRateCombo.setCurrentIndex(0)

    def clearAllButTargets(self):
        self.clearIPSubGate()
        self.clearMainIPs()
        self.clearHostname()
        self.clearMasterLink()
        self.clearNTPFields()
        self.clearWirelessFields()
        self.clearAddSettingsCheckboxes()
        self.clearDNS()

        rootCertFilename = ""
        clientCertFilename = ""
        bbOpenedFile = ""
        self.alertsLineEdit.setText("")
        self.chooseClientCertBut.setText("Choose Client Certificate File")
        self.chooseRootCertBut.setText("Choose Root Certificate File")

    def clearEverything(self):
        self.bbConfigTab.setTabText(0, "Black Box Configuration")
        self.clearIPSubGate()
        self.clearMainIPs()
        self.clearHostname()
        self.clearMasterLink()
        # self.clearNTPFields()
        self.clearTargetTypes()
        self.clearWirelessFields()
        self.clearAddSettingsCheckboxes()
        self.clearDNS()

        rootCertFilename = ""
        clientCertFilename = ""
        bbOpenedFile = ""
        self.alertsLineEdit.setText("")
        self.chooseClientCertBut.setText("Choose Client Certificate File")
        self.chooseRootCertBut.setText("Choose Root Certificate File")

        self.pillarSyncBrokerIpAddressLineEdit.setText("")
        self.pillarSyncMqttEnabledCheckBox.setChecked(False)
        self.pillarSyncBrokerUsernameLineEdit.setText("")
        self.pillarSyncBrokerPasswordLineEdit.setText("")
        self.pillarSyncSampleFrequencySpinBox.setValue(30)
        self.pillarSyncBrokerPortLineEdit.setText("1883")

    def on_startBlackBoxFileBut_clicked(self):
        IP_ = ""
        self.leIP.setText("")
        self.leIP.setEnabled(False)
        subnet_ = ""
        self.leSubnet.setText("")
        rootCertFilename = ""
        clientCertFilename = ""
        self.leSubnet.setEnabled(False)
        self.clearAddSettingsCheckboxes()
        gateway_ = ""
        self.leGateway.setText("")
        self.leGateway.setEnabled(False)
        self.lePrimary.setText("")
        self.leSecondary.setText("")
        self.leTertiary.setText("")
        self.lePrimary.setEnabled(False)
        self.leSecondary.setEnabled(False)
        self.leTertiary.setEnabled(False)
        self.DHCPRadio.setChecked(True)
        self.automaticDNSRadio.setChecked(True)
        self.automaticDNSRadio.setEnabled(True)
        bbOpenedFile = ""

        self.editingFileLabel.hide()
        self.openBBFileName.hide()
        self.openBBFileName.setText("")
        self.bigBBLabel.setGeometry(20, 10, 441, 41)
        self.clearIPSubGate()
        self.clearDNS()
        self.clearWirelessFields()
        self.leHostname.setText("")
        self.masterLink.setText("")
        self.addMasterLinkCheckbox.setChecked(False)
        # self.addMasterLinkCheckbox.setEnabled(False)
        self.clearNTPFields()
        self.clearAdvancedNetworking()
        self.BBtabWidget.setCurrentIndex(0)
        self.chooseTargetLabel.setStyleSheet("color: rgb(170, 0, 0)")
        self.chooseTargetLabel.setText("Choose a target Hobart Device for this configuration.")
        self.createHDConfigurationFileBut.setText("Choose a Hobart Device above")
        self.blackBoxTarget.setAutoExclusive(False)
        self.blackBoxTarget.setChecked(False)
        self.printerTarget.setAutoExclusive(False)
        self.printerTarget.setChecked(False)
        self.scaleTarget.setAutoExclusive(False)
        self.scaleTarget.setChecked(False)
        self.blackBoxTarget.setAutoExclusive(True)
        self.printerTarget.setAutoExclusive(True)
        self.scaleTarget.setAutoExclusive(True)
        # self.alertsLineEdit.setText("Choose a target device configuration.")
        self.wiredRadio.setChecked(True)
        # scene =  QGraphicsScene()
        # pixmap = QPixmap("://images/Step-1.jpg")
        # scene.addPixmap(pixmap)
        # self.step1GraphicsView.setScene(scene)
        # self.step1GraphicsView.show()
        self.step2GraphicsView.hide()
        self.step2Label.hide()
        howMany = 0
        self.advancedWirelessBut.setIcon(QIcon(""))
        self.manageMainButton()
        self.rootCertFilename = ""
        self.clientCertFilename = ""
        self.chooseRootCertBut.setText("Choose Root Certificate File")
        self.chooseClientCertBut.setText("Choose Client Certificate File")
        self.importDir = ""
        self.importRootCertFilename = ""
        self.importClientCertFilename = ""
        self.importEncryption_ = ""
        self.importSecurity_ = ""
        self.blackBoxTarget.setChecked()
        self.step1GraphicsView.hide()
        self.addNetworkSetting.setEnabled(True)

    def clearNTPFields(self):
        self.ntpDayOfWeek.setCurrentIndex(0)
        # self.BBtabWidget.setCurrentIndex(0)
        self.ntpFrequency.setCurrentIndex(0)
        self.ntpServerIP.setText("")
        beginningTime = QTime(3, 3, 0, 0)
        self.ntpTimeOfDay.setTime(beginningTime)
        self.ntpTimezone.setCurrentIndex(23)

    def clearMainIPs(self):
        self.lePrimary.setEnabled(False)
        self.leSecondary.setEnabled(False)
        self.leTertiary.setEnabled(False)
        self.leIP.setEnabled(False)
        self.leSubnet.setEnabled(False)
        self.leGateway.setEnabled(False)
        self.DHCPRadio.setChecked(True)
        self.automaticDNSRadio.setChecked(True)

    def on_staticIPRadio_clicked(self):
        self.leIP.setText(str(self.IP_))
        self.leSubnet.setText(str(self.subnet_))
        self.leGateway.setText(str(self.gateway_))
        self.leIP.setEnabled(True)
        self.leSubnet.setEnabled(True)
        self.leGateway.setEnabled(True)
        self.manualDNSRadio.setChecked(True)
        self.lePrimary.setEnabled(True)
        self.leSecondary.setEnabled(True)
        self.leTertiary.setEnabled(True)
        self.lePrimary.setText(self.primary_)
        self.leSecondary.setText(self.secondary_)
        self.leTertiary.setText(self.tertiary_)
        self.automaticDNSRadio.setEnabled(False)

    def on_automaticDNSRadio_clicked(self):

        primary_ = self.lePrimary.text()
        secondary_ = self.leSecondary.text()
        tertiary_ = self.leTertiary.text()
        self.lePrimary.setText("")
        self.leSecondary.setText("")
        self.leTertiary.setText("")
        self.lePrimary.setEnabled(False)
        self.leSecondary.setEnabled(False)
        self.leTertiary.setEnabled(False)
        self.manageMainButton()

    def clearAdvancedNetworkingFlags(self):
        pass

    def updateAdvancedNetworkingFlags(self):
        pass
        # self.networkAdvancedBut.setIcon( QIcon(":/resources/badge" + QString.number(howMany) + ".png"))

    def on_manualDNSRadio_clicked(self):
        self.lePrimary.setText(self.primary_)
        self.leSecondary.setText(self.secondary_)
        self.leTertiary.setText(self.tertiary_)
        self.lePrimary.setEnabled(True)
        self.leSecondary.setEnabled(True)
        self.leTertiary.setEnabled(True)

    def on_advancedWirelessBut_clicked(self):
        self.areYouSureFrame.setGeometry(0, 0, 931, 661)
        self.areYouSureFrame.show()
        # self.advancedWirelessFrame.setGeometry(10,10,871,411)
        # self.advancedWirelessFrame.show()

    def on_advancedWirelessApplyBut_clicked(self):
        howMany = 0
        if self.advancedWirelessChannelCombo.currentIndex() != 0:
            howMany += 1
        if not self.advancedWirelessAutoLinkRateCheckBox.isChecked():
            howMany += 1
        if self.advancedWirelessLinkRateCombo.currentIndex() != 0:
            howMany += 1
        if self.advancedWirelessDNSSuffix1.text() != "" or self.advancedWirelessDNSSuffix2.text() != "" or self.advancedWirelessDNSSuffix3.text() != "":
            howMany += 1
        if self.advancedWirelessDriverCombo.currentIndex() != 0:
            howMany += 1

        self.advancedWirelessBut.setIcon(QIcon(":/resources/badge" + str(howMany) + ".png"))

        self.advancedWirelessFrame.hide()

    def showAdvancedFlag(self):
        pass

    def on_advancedWirelessCancelBut_clicked(self):
        # set it all back to what it was then close
        self.advancedWirelessChannelCombo.setCurrentIndex(self.advChannelIndex)
        if (self.automaticLinkRateIntValue == 1):
            self.advancedWirelessAutoLinkRateCheckBox.setChecked(True)
        else:
            self.advancedWirelessAutoLinkRateCheckBox.setChecked(False)
        self.advancedWirelessDriverCombo.setCurrentIndex(self.advWirelessDriverIndex)
        self.advancedWirelessLinkRateCombo.setCurrentIndex(self.advLinkRateIndex)
        self.advancedWirelessDNSSuffix1.setText(self.Suffix1)
        self.advancedWirelessDNSSuffix2.setText(self.Suffix2)
        self.advancedWirelessDNSSuffix3.setText(self.Suffix3)
        self.advancedWirelessFrame.hide()

    def on_pushButton_clicked(self):
        self.testingFrame.hide()

    def on_SSID_textChanged(self, arg1):
        if self.SSID.text() == "" and self.wirelessRadio.isChecked():
            self.createHDConfigurationFileBut.setEnabled(False)
            self.addNetworkSetting.setChecked(False)
            self.addNetworkSetting.setEnabled(False)
        elif self.SSID.text() != "" and self.wirelessRadio.isChecked():
            self.createHDConfigurationFileBut.setEnabled(True)
            self.step2GraphicsView.hide()
            self.step2Label.hide()
            self.addNetworkSetting.setEnabled(True)
        self.manageMainButton()

    def on_wiredRadio_clicked(self):
        self.createHDConfigurationFileBut.setEnabled(True)
        self.addNetworkSetting.setEnabled(True)
        self.addNetworkSetting.setChecked(False)
        self.manageMainButton()

    def on_wirelessRadio_clicked(self):
        if self.wirelessRadio.isChecked() and self.SSID.text() == "":
            self.createHDConfigurationFileBut.setEnabled(False)
            self.step2GraphicsView.show()
            self.step2Label.show()
        elif self.wirelessRadio.isChecked() and self.SSID.text() != "":
            self.createHDConfigurationFileBut.setEnabled(True)
        self.manageMainButton()

    def on_DHCPRadio_clicked(self):
        self.automaticDNSRadio.setEnabled(True)

    def on_blackBoxTarget_clicked(self):
        self.clearAllButTargets()
        self.step1GraphicsView.hide()
        if self.blackBoxTarget.isChecked():
            self.bigBBLabel.setText("BlackBox Configuration")
            self.chooseTargetLabel.setStyleSheet("color: rgb(85, 170, 0)")
            self.createHDConfigurationFileBut.setText("Save BlackBox Configuration")
            self.alertsLineEdit.setText("Black Box configuration has not been saved")
            self.chooseTargetLabel.hide()
        self.manageMainButton()
        self.addNetworkSetting.setEnabled(True)
        self.hideMonitors()
        self.blackBoxTarget.setChecked(True)
        self.BBtabWidget.setCurrentIndex(0)
        if self.wirelessRadio.isChecked() and self.SSID.text() == "":
            self.addNetworkSetting.setEnabled(False)
        elif self.wirelessRadio.isChecked() and self.SSID.text() != "":
            self.addNetworkSetting.setEnabled(True)

    def on_printerTarget_clicked(self):
        self.step1GraphicsView.hide()
        if self.printerTarget.isChecked():
            self.bigBBLabel.setText("Printer Configuration")
            self.chooseTargetLabel.setStyleSheet("color: rgb(85, 170, 0)")
            self.createHDConfigurationFileBut.setText("Save Printer Configuration")
            # self.alertsLineEdit.setText("Printer configuration has not been saved")
            self.chooseTargetLabel.hide()
        self.clearAllButTargets()
        self.manageMainButton()
        self.hideMonitors()
        # self.printerTarget.setChecked(True)
        self.BBtabWidget.setCurrentIndex(0)
        if self.wirelessRadio.isChecked() and self.SSID.text() == "":
            self.addNetworkSetting.setEnabled(False)
        elif self.wirelessRadio.isChecked() and self.SSID.text() != "":
            self.addNetworkSetting.setEnabled(True)

    def on_scaleTarget_clicked(self):
        self.step1GraphicsView.hide()
        if self.scaleTarget.isChecked():
            self.bigBBLabel.setText("Scale Configuration")
            self.chooseTargetLabel.setStyleSheet("color: rgb(85, 170, 0)")
            self.createHDConfigurationFileBut.setText("Save Scale Configuration")
            self.alertsLineEdit.setText("Scale configuration has not been saved")
            self.chooseTargetLabel.hide()
        self.manageMainButton()
        self.clearAllButTargets()
        self.showMonitors()

    def on_removeNTPSettings_clicked(self):
        if self.removeNTPSettings.isChecked():
            self.addNTPSetting.setEnabled(True)
            self.addNTPSetting.setChecked(True)

        elif self.ntpServerIP.text() == "" and not self.removeNTPSettings.isChecked():
            self.addNTPSetting.setChecked(False)
            self.addNTPSetting.setEnabled(False)

    def on_ntpServerIP_textChanged(self, arg1):
        if self.ntpServerIP.text() == "" and not self.removeNTPSettings.isChecked():
            self.addNTPSetting.setEnabled(False)
            self.addNTPSetting.setChecked(False)
        elif self.ntpServerIP.text() != "":
            self.addNTPSetting.setEnabled(True)

    def on_addPillarSyncCheckbox_state_changed(self, state):
        self.manageMainButton()

    def on_addNetworkSetting_clicked(self):
        self.manageMainButton()

    def on_addHostnameCheckbox_clicked(self):
        self.manageMainButton()

    def on_addNTPSetting_clicked(self):
        self.manageMainButton()

    def on_addMasterLinkCheckbox_clicked(self):
        self.manageMainButton()

    def on_areYouSureNo_clicked(self):
        self.areYouSureFrame.hide()

    def on_areYouSureYes_clicked(self):
        # Record current incoming values
        advChannelIndex = self.advancedWirelessChannelCombo.currentIndex()
        advLinkRateIndex = self.advancedWirelessLinkRateCombo.currentIndex()
        advWirelessDriverIndex = self.advancedWirelessDriverCombo.currentIndex()
        Suffix1 = self.advancedWirelessDNSSuffix1.text()
        Suffix2 = self.advancedWirelessDNSSuffix2.text()
        Suffix3 = self.advancedWirelessDNSSuffix3.text()
        if self.automaticDNSRadio.isChecked():
            automaticLinkRateIntValue = 1
        else:
            automaticLinkRateIntValue = 0

        self.areYouSureFrame.hide()
        self.advancedWirelessFrame.setGeometry(0, 0, 890, 430)
        if self.wiredRadio.isChecked():
            self.gbWirelessChannel.setEnabled(False)
            self.gbWirelessDriver.setEnabled(False)
            self.gbLinkRate.setEnabled(False)
            self.advancedWirelessAutoLinkRateCheckBox.setEnabled(False)
            self.advancedWirelessChannelCombo.setEnabled(False)
            self.advancedWirelessDriverCombo.setEnabled(False)
            self.advancedWirelessLinkRateCombo.setEnabled(False)
            self.coverupgbChannel.show()
            self.coverupgbLinkRate.show()
            self.coverupgbWirelessDriver.show()

        elif self.wirelessRadio.isChecked():
            self.gbWirelessChannel.setEnabled(True)
            self.gbWirelessDriver.setEnabled(True)
            self.gbLinkRate.setEnabled(True)
            self.advancedWirelessAutoLinkRateCheckBox.setEnabled(True)
            self.advancedWirelessChannelCombo.setEnabled(True)
            self.advancedWirelessDriverCombo.setEnabled(True)
            self.advancedWirelessLinkRateCombo.setEnabled(True)
            self.coverupgbChannel.hide()
            self.coverupgbLinkRate.hide()
            self.coverupgbWirelessDriver.hide()

        self.advancedWirelessFrame.show()

    def on_securityCombo_currentIndexChanged(self, arg1):
        pass

    def on_securityCombo_currentIndexChanged(self, index):
        pass

    @pyqtSlot()
    def on_chooseRootCertBut_clicked(self):
        dialog = QFileDialog(self)

        dialog.setFileMode(QFileDialog.ExistingFile)
        filename = dialog.getOpenFileName(
            self,
            "Choose Trusted Root Certificate",
            QDir.currentPath(),
            "All files (*.*)")

        if not filename.isNull():
            self.rootCertFilename = filename
            f = QFile(filename)
            fileInfo = QFileInfo(f.fileName())
            filenameOnly = fileInfo.fileName()
            self.chooseRootCertBut.setText(filenameOnly)

    @pyqtSlot()
    def on_chooseClientCertBut_clicked(self):
        dialog = QFileDialog(self)
        dialog.setFileMode(QFileDialog.ExistingFile)
        filename = dialog.getOpenFileName(
            self,
            "Choose Client Certificate",
            QDir.currentPath(),
            "All files (*.*)")

        if not filename.isNull():
            f = QFile(filename)
            fileInfo = QFileInfo(f.fileName())
            filenameOnly = fileInfo.fileName()
            self.clientCertFilename = filename
            self.chooseClientCertBut.setText(filenameOnly)

    def on_advancedWirelessRevertDefaultsBut_clicked(self):
        self.clearAdvancedNetworking()

    def on_leHostname_textChanged(self, arg1):
        if self.leHostname.text() != "":
            self.addHostnameCheckbox.setEnabled(True)
        else:
            self.addHostnameCheckbox.setChecked(False)
            self.addHostnameCheckbox.setEnabled(False)

    def on_masterLink_textChanged(self, arg1):
        return
        # if self.masterLink.text() != "":
        #     self.addMasterLinkCheckbox.setEnabled(True)
        # else:
        #     self.addMasterLinkCheckbox.setChecked(False)
        #     self.addMasterLinkCheckbox.setEnabled(False)

    def remove_folder_and_files(self, folder):
        for filename in os.listdir(folder):
            file_path = os.path.join(folder, filename)
            try:
                if os.path.isfile(file_path) or os.path.islink(file_path):
                    os.unlink(file_path)
                elif os.path.isdir(file_path):
                    shutil.rmtree(file_path)
            except Exception as e:
                print('Failed to delete %s. Reason: %s' % (file_path, e))

    @pyqtSlot()
    def on_createHDConfigurationFileBut_clicked(self):
        device_ip_address = None

        if str(self.configurationMethodComboBox.currentText()) == "Network":
            device_ip_address = self.get_selected_rgd()
            if not device_ip_address:
                self.set_alert("ERROR: Select a black box device to update")
                return
            else:

                CSIDL_PERSONAL = 5  # My Documents
                SHGFP_TYPE_CURRENT = 0  # Get current, not default value

                buf = ctypes.create_unicode_buffer(ctypes.wintypes.MAX_PATH)
                ctypes.windll.shell32.SHGetFolderPathW(None, CSIDL_PERSONAL, None, SHGFP_TYPE_CURRENT, buf)
                filename1 = os.path.join(buf.value, "autoconfig", "hdconfig")
                if not os.path.exists(os.path.dirname(filename1)):
                    os.mkdir(os.path.dirname(filename1))
                else:
                    self.remove_folder_and_files(os.path.dirname(filename1))

        elif str(self.configurationMethodComboBox.currentText()) == 'Disk':
            c = wmi.WMI()
            removable_disks = c.Win32_LogicalDisk(DriveType=2)
            dirpath = "C:\\"
            if len(removable_disks):
                for disk in removable_disks:
                    for dirpath, dirnames, filenames in os.walk(disk.Name + "\\"):
                        print(dirpath)
                        break

                filename1 = QFileDialog.getSaveFileName(self, "Save Black Box Device Configuration File",
                                                        dirpath + "\\autoconfig\\hdconfig", "")[0]
            else:
                CSIDL_PERSONAL = 5  # My Documents
                SHGFP_TYPE_CURRENT = 0  # Get current, not default value

                buf = ctypes.create_unicode_buffer(ctypes.wintypes.MAX_PATH)
                ctypes.windll.shell32.SHGetFolderPathW(None, CSIDL_PERSONAL, None, SHGFP_TYPE_CURRENT, buf)

                # print(buf.value)
                filename1 = QFileDialog.getSaveFileName(self, "Save Black Box Device Configuration File",
                                                        buf.value + "\\hdconfig", "")[0]
        else:
            pass

        f = QFile()
        fileInfo = QFileInfo()
        bbOpenedFile = filename1
        f = QFile(filename1)
        fileInfo = QFileInfo(f.fileName())
        filenameOnly = fileInfo.fileName()

        if filename1 and filename1 != '':
            passphrase = "woozal"
            exportLocation = filename1
            options = {}
            options["ssid"] = str(self.SSID.text())
            options["securityMethod"] = str(self.securityCombo.currentText())
            options["encryption"] = str(self.encryptionCombo.currentText())
            if str(self.securityCombo.currentText()).__contains__("EAP)"):
                options["EAPUsername"] = str(self.eapUsername.text())
                options["EAPPassword"] = str(self.eapPassword.text())
            else:
                options["EAPUsername"] = str(self.eapUsername.text())
                options["EAPPassword"] = str(self.eapPassword.text())
            if str(self.securityCombo.currentText()) == "OPEN" and str(self.encryptionCombo.currentText()) == "WEP":
                defaultKey = "1"
                if self.wepKey1Radio.isChecked():
                    defaultKey = "1"
                elif self.wepKey2Radio.isChecked():
                    defaultKey = "2"
                elif self.wepKey3Radio.isChecked():
                    defaultKey = "3"
                elif self.wepKey4Radio.isChecked():
                    defaultKey = "4"
                options["defaultKey"] = str(defaultKey)
                options["WEPKey1"] = str(self.WEPKey1.text())
                options["WEPKey2"] = str(self.WEPKey2.text())
                options["WEPKey3"] = str(self.WEPKey3.text())
                options["WEPKey4"] = str(self.WEPKey4.text())
            else:
                options["defaultKey"] = "1"
                options["WEPKey1"] = ""
                options["WEPKey2"] = ""
                options["WEPKey3"] = ""
                options["WEPKey4"] = ""

            if str(self.securityCombo.currentText()) == "WPA-Personal" or str(
                    self.securityCombo.currentText()) == "WPA2-Personal":
                options["wpaPassphrase"] = str(self.WPAPassphrase.text())
            else:
                options["wpaPassphrase"] = ""

            if str(self.securityCombo.currentText()) == "WPA EAP-TLS" or str(
                    self.securityCombo.currentText()) == "WPA2 EAP-TLS":
                if str(self.chooseRootCertBut.text()) == "Choose Root Certificate File":
                    options["serverCertFilename"] = ""
                else:
                    options["serverCertFilename"] = str(self.chooseRootCertBut.text())

                    if self.wirelessRadio.isChecked() and self.addNetworkSetting.isChecked():
                        # save the chosen root cert to base dir of export file
                        destDir = os.path.dirname(str(filename1))
                        separator = ""
                        if destDir[-1] != "/":
                            separator = "/"
                        destCompleteFile = destDir + separator + str(self.chooseRootCertBut.text())
                        srcCompleteFile = str(self.rootCertFilename)
                        if self.rootCertFilename != destCompleteFile:
                            try:
                                copyfile(srcCompleteFile, destCompleteFile)
                            except:
                                try:
                                    os.remove(destCompleteFile)
                                    copyfile(srcCompleteFile, destCompleteFile)
                                except:
                                    pass

                options["clientCertSerialNum"] = ""
                if str(self.chooseClientCertBut.text()) == "Choose Client Certificate File":
                    options["clientCertFilename"] = ""
                else:
                    options["clientCertFilename"] = str(self.chooseClientCertBut.text())

                    if self.wirelessRadio.isChecked() and self.addNetworkSetting.isChecked():
                        # save the chosen root cert to base dir of export file
                        destDir = os.path.dirname(str(filename1))
                        separator = ""
                        if destDir[-1] != "/":
                            separator = "/"

                        destCompleteFile = destDir + separator + str(self.chooseClientCertBut.text())
                        srcCompleteFile = str(self.clientCertFilename)
                        if self.clientCertFilename != destCompleteFile:
                            try:
                                copyfile(srcCompleteFile, destCompleteFile)
                            except:
                                try:
                                    os.remove(destCompleteFile)
                                    copyfile(srcCompleteFile, destCompleteFile)
                                except:
                                    pass

                options["clientCertPassword"] = str(self.tlsCLientPassword.text())
            else:
                options["serverCertFilename"] = ""
                options["serverCertFilename"] = ""
                options["clientCertSerialNum"] = ""
                options["clientCertFilename"] = ""
                options["clientCertPassword"] = ""

            options["linkRate"] = self.advancedWirelessLinkRateCombo.currentText()

            autoLR = "1"
            if not self.advancedWirelessAutoLinkRateCheckBox.isChecked():
                autoLR = "0"
            else:
                autoLR = "1"
            options["automaticLinkRate"] = autoLR
            options["channel"] = self.advancedWirelessChannelCombo.currentText()
            isStatic = "1"

            if self.staticIPRadio.isChecked():
                isStatic = "1"
            else:
                isStatic = "0"
            options["isStatic"] = isStatic
            options["ipAddress"] = str(self.leIP.text())
            options["subnetMask"] = str(self.leSubnet.text())
            options["gateway"] = str(self.leGateway.text())

            isAutoDNS = "1"
            if self.automaticDNSRadio.isChecked():
                isAutoDNS = "1"
            else:
                isAutoDNS = "0"

            options["isAutoDNS"] = isAutoDNS
            options["primaryDNS"] = str(self.lePrimary.text())
            options["secondaryDNS"] = str(self.leSecondary.text())
            options["tertiaryDNS"] = str(self.leTertiary.text())
            options["DNSSuffix1"] = str(self.advancedWirelessDNSSuffix1.text())
            options["DNSSuffix2"] = str(self.advancedWirelessDNSSuffix2.text())
            options["DNSSuffix3"] = str(self.advancedWirelessDNSSuffix3.text())
            options["DNSSuffix4"] = ""
            options["DNSSuffix5"] = ""
            options["DNSSuffix6"] = ""

            if self.addHostnameCheckbox.isChecked():
                options["hostname"] = str(self.leHostname.text())
            else:
                options["hostname"] = "-1"

            if self.addNTPSetting.isChecked():
                options["timeKeeper"] = "NTP"
                options["ntpServer"] = str(self.ntpServerIP.text())
                options["ntpFrequency"] = str(self.ntpFrequency.currentIndex())

                if self.ntpFrequency.currentIndex() == 0:
                    options["ntpDayOfWeek"] = str(self.ntpDayOfWeek.currentIndex())
                else:
                    options["ntpDayOfWeek"] = "0"

                userTime = self.ntpTimeOfDay.time()

                options["ntpTimeOfDay"] = userTime.toString()
                options["ntpTimeZone"] = self.ntpTimezone.itemData(self.ntpTimezone.currentIndex()).toString()
                if int(userTime.toString()[:2]) > 12:
                    options["ntpAntiMeri"] = "AM"
                else:
                    options["ntpAntiMeri"] = "PM"

            elif self.removeNTPSettings.isChecked():
                options["timeKeeper"] = "REMOVE"
                options["ntpServer"] = ""
                options["ntpFrequency"] = "0"
                options["ntpDayOfWeek"] = "0"
                options["ntpTimeOfDay"] = "03:30:00"
                options["ntpTimeZone"] = "America/New_York"
                options["ntpAntiMeridian"] = "AM"
            else:
                options["timeKeeper"] = "NOTHING"
                options["ntpServer"] = ""
                options["ntpFrequency"] = "0"
                options["ntpDayOfWeek"] = "0"
                options["ntpTimeOfDay"] = "03:30:00"
                options["ntpTimeZone"] = "America/New_York"
                options["ntpAntiMeridian"] = "AM"

            options["wirelessDriver"] = self.advancedWirelessDriverCombo.currentText()

            options["targetType"] = "BB"

            if self.addNTPSetting.isChecked():
                options["addNTP"] = "1"
            else:
                options["addNTP"] = "0"

            if self.addNetworkSetting.isChecked():
                options["addNetwork"] = "1"
            else:
                options["addNetwork"] = "0"

            if self.addHostnameCheckbox.isChecked():
                options["addHostname"] = "1"
            else:
                options["addHostname"] = "0"

            options["addMasterLink"] = "0"

            options["display1"] = ""
            options["display2"] = ""

            options['addPillarSync'] = '0'
            if self.addPillarSyncCheckbox.isChecked():
                options['addPillarSync'] = '1'
                options['pillarSyncSampleFrequency'] = str(self.pillarSyncSampleFrequencySpinBox.value())
                options['pillarSyncMqttEnabled'] = '0'
                if self.pillarSyncMqttEnabledCheckBox.isChecked():
                    options['pillarSyncMqttEnabled'] = '1'
                    options['pillarSyncBrokerIpAddress'] = self.pillarSyncBrokerIpAddressLineEdit.text()
                    options['pillarSyncBrokerUsername'] = self.pillarSyncBrokerUsernameLineEdit.text()
                    options['pillarSyncBrokerPassword'] = self.pillarSyncBrokerPasswordLineEdit.text()
                    options['pillarSyncBrokerPort'] = self.pillarSyncBrokerPortLineEdit.text()

            # networkConfigs = encryptDecryptBB.addParseOptionsInOrder(options)
            plaintext = encryptDecryptBB.formatHTDeviceFile(options)
            # print "len is " + str(len(exportLocation))
            passphrase = "woozal"
            encryptDecryptBB.saveConfigsToFile(plaintext, passphrase, filename1)

        if str(self.configurationMethodComboBox.currentText()) == "Network":
            status_code = 1
            self.set_alert("INFO: Transmitting network configuration to selected black box device")
            try:
                network_command.remote_command(ip_address=device_ip_address,
                                               command="/bin/mkdir --parents /media/itw/autoconfig")
            except:
                pass

            for file in glob.glob(os.path.join(os.path.dirname(filename1), "*")):
                status_code = network_command.remote_file_put(ip_address=device_ip_address,
                                                              local_filename=file,
                                                              remote_filename="/media/itw/autoconfig/%s" %
                                                                              os.path.basename(file))
                if status_code != 0:
                    self.set_alert("ERROR: Network configuration files could not be transmitted")
                    break

            if status_code == 0:
                self.set_alert("INFO: Network configuration files transmitted to selected black box device")

        elif str(self.configurationMethodComboBox.currentText()) == 'Disk':
            if filename1 != '':
                self.set_alert("INFO: Saved network configuration to filename on selected disk")


if __name__ == "__main__":
    a = QApplication(sys.argv)
    window = BlackBoxSetupUtility()
    window.show()
    a.exec_()
