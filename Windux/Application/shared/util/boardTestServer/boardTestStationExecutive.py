#!/usr/bin/env python3

import os
import re
import shutil
import sys
from datetime import datetime
import calendar

from PyQt5 import QtCore
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *

import boardTestServer
import softwareInstall
from Ui_boardTestStationExecutive import Ui_TestStationExecutiveDialog

VERSION = "1.6.0"

PYTHON_DEBUG = False

INSTALLATION_DIRECTORY = "/home/pi/test_station_server"

if PYTHON_DEBUG:
    pyqtRemoveInputHook()

def get_pid_file():
    pid = None
    try:
        with open('%s/board_test_station_executive.pid' % INSTALLATION_DIRECTORY, 'r') as pid_file:
            pid = int(pid_file.readline().strip())
    except:
        pass
    return pid


def is_program_running():
    is_running = False
    if get_pid_file() is not None:
        try:
            os.kill(get_pid_file(), 0)
        except OSError:
            pass
        else:
            is_running = True

    return is_running


def generate_pid_file():
    if not os.path.exists(INSTALLATION_DIRECTORY):
        os.mkdir(path=INSTALLATION_DIRECTORY, mode=0o755)
    with open('%s/board_test_station_executive.pid' % INSTALLATION_DIRECTORY, 'w') as pid_file:
        pid_file.write("%d\n" % os.getpid())


class BoardTestStationExecutive(QDialog):

    # class constructor
    def __init__(self, parent=None):

        generate_pid_file()

        QDialog.__init__(self, parent)

        self.calendar = None
        self.current_month = datetime.now().month
        self.current_year = datetime.now().year
        self.current_day = datetime.now().day

        self.software_install_thread = None
        self.ui = Ui_TestStationExecutiveDialog()
        self.software_install_filename_qt_regex = "itw-test_station-*-*.tgz"
        self.software_install_filename_regex = "itw-test_station-[a-z,_]+-[0-9]+\.[0-9]+\.[0-9]+\.tgz"

        self.ui.setupUi(self)

        self.ui.exitPushbutton.clicked.connect(self.exit)
        self.ui.restartServerPushButton.clicked.connect(self.restart_server)
        self.ui.dateTimePushButton.clicked.connect(self.display_calendar)
        self.ui.helpPushButton.clicked.connect(
            lambda: QDesktopServices.openUrl(
                QUrl('file://%s/usage_guide.html' % INSTALLATION_DIRECTORY)))
        self.ui.installSoftwarePushButton.clicked.connect(self.install_software)
        self.ui.folderPushButton.clicked.connect(self.select_install_filename)

        if os.path.exists(os.path.join(INSTALLATION_DIRECTORY, "server_install")):
            self.setWindowTitle("Test Station Server (Version: %s)" % VERSION)
            self.ui.testStationInfoLabel.setText("Test Station Server")
            self.ui.testStationStatusLabel.show()
            self.ui.exportDbPushButton.show()
            self.ui.exportDbPushButton.clicked.connect(self.export_database)
            self.ui.stopServerPushbutton.show()
            self.ui.stopServerPushbutton.clicked.connect(self.stop_server)
            self.ui.startServerPushButton.show()
            self.ui.startServerPushButton.clicked.connect(self.start_server)
            # self.ui.dhcpServerPushButton.hide()
            # self.ui.dhcpServerPushButton.show()
            # self.ui.dhcpServerPushButton.clicked.connect(self.dhcp_server_restart)

        elif os.path.exists(os.path.join(INSTALLATION_DIRECTORY, "client_install")):
            self.setWindowTitle("Test Station Client (Version: %s)" % VERSION)
            self.ui.testStationInfoLabel.setText("Test Station Client")
            self.ui.stopServerPushbutton.hide()
            self.ui.startServerPushButton.hide()
            self.ui.exportDbPushButton.hide()
            self.ui.testStationStatusLabel.hide()
            # self.ui.dhcpServerPushButton.hide()
            self.ui.statusTextLabel.hide()

        # self.dhcp_server_restart()

        QtCore.QTimer.singleShot(0, self.set_status)
        # QtCore.QTimer.singleShot(0, self.dhcp_server_restart)

    def set_status(self):
        status = boardTestServer.status_board_test_server()
        if status == 'stopped':
            pixmap = QPixmap(':/images/red_status.png')
            self.set_alert("INFO: Test Station Server Stopped")
        else:
            pixmap = QPixmap(":/images/green_status.png")
            self.set_alert("INFO: Test Station Server Started")

        self.ui.testStationStatusLabel.setPixmap(pixmap)

    def dhcp_server_restart(self):
        exit_code = os.system("/usr/bin/sudo /usr/sbin/service isc-dhcp-server status > /dev/null")
        if exit_code != 0:
            self.set_alert("Attempting to start DCHP server")
            exit_code = os.system("/usr/bin/sudo /usr/sbin/service isc-dhcp-server stop > /dev/null")
            exit_code = os.system("/usr/bin/sudo /bin/rm -f /var/lib/dhcp/dhcpd.leases*")
            exit_code = os.system("/usr/bin/sudo /usr/sbin/service isc-dhcp-server start > /dev/null")
            if exit_code != 0:
                self.set_alert("ERROR: can not start dhcp server")
            else:
                self.set_alert("INFO: DCHP server started")
        else:
            self.set_alert("INFO: DHCP server is running")

    def restart_server(self):
        self.set_alert("INFO: Rebooting Test Station")
        os.system("/usr/bin/sudo /sbin/reboot")

    def stop_server(self):
        boardTestServer.stop_board_test_server()
        self.set_status()

    def start_server(self):
        boardTestServer.start_board_test_server()
        self.set_status()

    def install_software(self):

        try:

            if self.ui.softwareUpgradeFilenameLineEdit.text().strip() == '':
                self.set_alert("ERROR: you must select a software file to install")
                return

            self.ui.installSoftwarePushButton.setDisabled(True)

            regex_pattern = '^.*%s$'  # HTA-91: Accept anything up to our filename string

            feature_install_regex_compiled = re.compile(regex_pattern %
                                                        self.software_install_filename_regex)

            self.set_alert("INFO: Software installs may cause the Test Station to reboot")

            upgrade_filename = self.ui.softwareUpgradeFilenameLineEdit.text().strip()

            # pick hte-feature_{name} out of .tgz tarball name by splitting it at dash
            upgrade_file_elements = os.path.basename(upgrade_filename).split('-')

            feature_name = upgrade_file_elements[2]

            if feature_install_regex_compiled.match(upgrade_filename):

                self.software_install_thread = softwareInstall.Worker(upgrade_filename, feature_name)

                self.software_install_thread.alert_handler.connect(self.set_alert)

                self.software_install_thread.install_complete_handler.connect(self.install_complete)

                self.software_install_thread.start()

        except:
            self.set_alert("ERROR: Software installation failed")

    @pyqtSlot(str)
    def install_complete(self, message):
        self.ui.installSoftwarePushButton.setEnabled(True)

    @pyqtSlot(str)
    def set_alert(self, message):
        self.ui.statusLabel.setText(message)

    def select_install_filename(self):

        qfd = QFileDialog()
        qfd.setAcceptMode(QFileDialog.AcceptOpen)
        qfd.setDirectory(os.path.join("/", "media", "pi"))
        qfd.setFileMode(QFileDialog.ExistingFile)
        qfd.setOption(QFileDialog.ReadOnly)
        qfd.setLabelText(QFileDialog.Accept, "Ok")
        qfd.setNameFilter(
            "Software Upgrade Filename (%s)" % self.software_install_filename_qt_regex)
        qfd.setWindowTitle("Choose Software Install Filename")
        qfd.setViewMode(QFileDialog.List)

        selected = []
        if QDialog.Accepted == qfd.exec_():
            selected = qfd.selectedFiles()

        if 1 == len(selected):
            self.ui.softwareUpgradeFilenameLineEdit.setText(selected[0])

    def display_calendar(self):
        self.calendar = QCalendarWidget(self)
        self.calendar.move(20, 20)
        self.calendar.setGridVisible(True)
        todays_date = QDate(self.current_year, self.current_month, self.current_day)
        self.calendar.setSelectedDate(todays_date)
        self.calendar.setMinimumDate(QDate(self.current_year, self.current_month-1, 1))
        self.calendar.setMaximumDate(QDate(self.current_year, self.current_month + 1,
                                           calendar.monthrange(
                                               self.current_year, self.current_month)[1]))
        self.calendar.clicked.connect(self.set_date_time)
        self.calendar.setGeometry(100, 100, 300, 300)
        self.calendar.setWindowTitle('Select Date/Time')
        self.calendar.show()

    def set_date_time(self, qDate):
        print(qDate.toString())
        self.calendar.hide()
        set_date_time_command = '/usr/bin/sudo date -s ' + '"%04d%02d%02d %02d%02d"' % (
            qDate.year(),
            qDate.month(),
            qDate.day(),
            datetime.now().hour,
            datetime.now().minute)
        print(set_date_time_command)
        # set os system time
        os.system(set_date_time_command)
        # Set hardware clock from system time
        os.system("/usr/bin/sudo /sbin/hwclock -w")

    def export_database(self):

        insert_usb_drive_message = """Insert Thumb Drive into USB port on back of this Test Station,
Press Ok after thumb drive inserted or 
Press Cancel to abort."""

        response = QMessageBox.information(self,
                                           "Message",
                                           insert_usb_drive_message,
                                           QMessageBox.Ok |
                                           QMessageBox.Cancel,
                                           QMessageBox.Ok)

        if response == QMessageBox.Ok:
            mounted_drives = os.listdir(os.path.join('/', "media", "pi"))
            try:
                if len(mounted_drives) == 1:
                    if os.path.ismount('/media/pi/%s' % (mounted_drives[0])):
                        exit_code = os.system(
                            '/usr/bin/sqlite3 -header -csv %s/board_test_server.sqlite '
                            '"select * from BoardInformation;" > %s/board_test_server_data.csv' %
                            (INSTALLATION_DIRECTORY, '/media/pi/%s' % mounted_drives[0]))

                        if exit_code == 0:
                            message = """Filename: %s exported to USB Thumb Drive.
Remove USB thumb drive from USB port on back of this Test Station""" % 'board_test_server_data.csv'
                            response = QMessageBox.information(self,
                                                               "Message",
                                                               message,
                                                               QMessageBox.Ok,
                                                               QMessageBox.Ok)

                            os.system("/usr/bin/eject /media/pi/%s" % mounted_drives[0])

                        else:
                            message = "Database could not be exported to USB Thumb Drive."
                            response = QMessageBox.critical(self,
                                                            "Message",
                                                            message,
                                                            QMessageBox.Ok,
                                                            QMessageBox.Ok)

                elif len(mounted_drives) == 0:
                    message = "No USB thumb drive found for export. Insert USB Thumb Drive"
                    response = QMessageBox.critical(self,
                                                    "Message",
                                                    message,
                                                    QMessageBox.Ok,
                                                    QMessageBox.Ok)

                else:
                    message = "Only one USB thumb drive can be inserted at a time"
                    response = QMessageBox.critical(self,
                                                    "Message",
                                                    message,
                                                    QMessageBox.Ok,
                                                    QMessageBox.Ok)

            except Exception as e:
                response = QMessageBox.critical(self,
                                                "Message",
                                                str(e.args),
                                                QMessageBox.Ok,
                                                QMessageBox.Ok)

    # exit dialog
    def exit(self):
        QDialog.reject(self)


if __name__ == "__main__":

    # block programming from running more than once
    if is_program_running():
        sys.exit(0)

    app = QApplication(sys.argv)
    QApplication.setStyle(QStyleFactory.create("Cleanlooks"))
    board_programmer = BoardTestStationExecutive(None)
    board_programmer.show()
    sys.exit(app.exec_())
