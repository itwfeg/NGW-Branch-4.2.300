#!/usr/bin/env python3

import os
import sys
import configparser
import datetime
import logging
import logging.handlers
import sys

from PyQt5 import QtCore
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from Ui_acrBoardProgrammer import Ui_acrBoardProgrammerDialog

if sys.platform == 'win32':
    application_directory = QStandardPaths.standardLocations(QStandardPaths.AppDataLocation)[0] + '/ACR Board Programmer'
    if not os.path.exists(application_directory):
        os.makedirs(application_directory)
    print(application_directory)
    INSTALLATION_DIRECTORY = application_directory
else:
    if os.path.exists(os.path.join("/", "home", "pi", "test_station_server")):
        sys.path.append("/home/pi/test_station_server")
    else:
        sys.path.append("/home/pi/test_station_client")
    INSTALLATION_DIRECTORY = "/home/pi/acr_board_programmer"

import barcodeScannerWorker
import boardTestServer
import host_ip_address
import ping
import gui

PRODUCT_VERSION = "1.6.0"

PYTHON_DEBUG = True


if PYTHON_DEBUG:
    pyqtRemoveInputHook()


def get_pid_file():
    pid = None
    try:
        with open('%s/acr_board_programmer.pid' % INSTALLATION_DIRECTORY, 'r') as pid_file:
            pid = int(pid_file.readline().strip())
    except:
        pass
    return pid


def is_program_running():
    is_running = False
    if get_pid_file() is not None:
        try:
            os.kill(get_pid_file(), 0)
        except:
            pass
        else:
            is_running = True

    return is_running


class AcrBoardProgrammer(QDialog):

    # class constructor
    def __init__(self, parent=None):

        self.generate_pid_file()

        # instantiate QDialog class
        QDialog.__init__(self, parent)

        self.ui = Ui_acrBoardProgrammerDialog()

        # manifest UI
        self.ui.setupUi(self)

        self.setWindowTitle("ACR Board Programmer (Version: %s)" % PRODUCT_VERSION)

        QCoreApplication.setOrganizationName("Hobart")
        QCoreApplication.setOrganizationDomain("hobartcorp.com")
        QCoreApplication.setApplicationName("ACR_BOARD_PROGRAMMER")

        self.configuration = configparser.ConfigParser(inline_comment_prefixes='#')
        self.configuration.read("acr_board_programmer.ini")

        self.log_level = self.configuration.get('GENERAL', "logging_level")

        log_filename = os.path.join(INSTALLATION_DIRECTORY, "acr_board_programmer.log")

        self._setup_enhanced_logging(log_filename)
        self.overall_status = 'success'
        self.overall_status_color = 'green'

        # self.settings = QtCore.QSettings('./acrBoardProgrammer.ini', QtCore.QSettings.IniFormat)

        local_ip_address = host_ip_address.get_local_addr(remote=None, ipv6=False)
        self.ui.programmerIpAddressLabel.setText(local_ip_address)

        self.severity_map = {'success': 1, 'info': 2, 'warning': 3, 'error': 4}

        self.serial_number = None
        self.revision_number = None
        self.part_number = None
        self.manufacture_date = datetime.datetime.today().strftime("%m/%d/%y")

        gui.set_qlineedit_input_width(self.ui.manufactureDateDayLabel)
        gui.set_qlineedit_input_width(self.ui.manufactureDateMonthLabel)
        gui.set_qlineedit_input_width(self.ui.manufactureDateYearLabel)

        self.ui.manufactureDateMonthLabel.setText(self.manufacture_date[:2])
        self.ui.manufactureDateDayLabel.setText(self.manufacture_date[3:5])
        self.ui.manufactureDateYearLabel.setText(self.manufacture_date[6:])

        self.pool = None
        try:
            self.pool = self.configuration.getint("BOARD_TEST_SERVER", "mac_address_pool_number")
            self.ui.poolNumberLabel.setText(str(self.pool))
        except:
            self.ui.poolNumberLabel.setText("??")
            self.set_status("error", "MAC Address Pool number is not configured correctly. Programming not possible")

        self.board_test_server_ip_address = None

        if self.configuration.get("BOARD_TEST_SERVER", "ip_address") != '':
            self.board_test_server_ip_address = self.configuration.get("BOARD_TEST_SERVER", "ip_address")

        self.board_test_server = None

        if self.board_test_server_ip_address is None or self.board_test_server_ip_address == 'localhost':
            # stop any running board test server that may be running
            # if not self.is_board_test_server_running():
            #     self.start_board_test_server()
            self.board_test_server_ip_address = 'localhost'

        self.scanner_support = self.configuration.get("GENERAL", "scanner_support").upper()

        board_part_numbers = list(self.configuration.options("BOARD_PART_NUMBERS"))
        self.ui.boardPartNumberComboBox.addItem('UNDEFINED')
        for part_number in board_part_numbers:
            self.ui.boardPartNumberComboBox.addItem(part_number)

        self.board_test_server_port = 5081
        if str(self.configuration.getint("BOARD_TEST_SERVER", "port")) != '':
            self.board_test_server_port = self.configuration.getint("BOARD_TEST_SERVER", "port")

        self.board_test_server_timeout = 3.0
        if str(self.configuration.getfloat("BOARD_TEST_SERVER", "timeout")) != '':
            self.board_test_server_timeout = self.configuration.getfloat("BOARD_TEST_SERVER", "timeout")

        self.ui.serverIpAddressLabel.setText(self.board_test_server_ip_address)

        self.datamatrix_part_number_start = self.configuration.getint("DATAMATRIX_FORMAT", "part_number_start")
        self.datamatrix_part_number_end = self.configuration.getint("DATAMATRIX_FORMAT", "part_number_end")

        self.datamatrix_serial_number_start = self.configuration.getint("DATAMATRIX_FORMAT", "serial_number_start")
        self.datamatrix_serial_number_end = self.configuration.getint("DATAMATRIX_FORMAT", "serial_number_end")

        self.datamatrix_revision_number_start = self.configuration.getint("DATAMATRIX_FORMAT", "revision_number_start")
        self.datamatrix_revision_number_end = self.configuration.getint("DATAMATRIX_FORMAT", "revision_number_end")

        self.datamatrix_manufacture_date_start = self.configuration.getint("DATAMATRIX_FORMAT",
                                                                           "manufacture_date_start")
        self.datamatrix_manufacture_date_end = self.configuration.getint("DATAMATRIX_FORMAT", "manufacture_date_end")

        self.datamatrix_format_length = self.configuration.getint("DATAMATRIX_FORMAT", "required_format_length")

        self.datamatrix_serial_number_length = (self.datamatrix_serial_number_end -
                                                self.datamatrix_serial_number_start) + 1

        self.datamatrix_revision_number_length = \
            (self.datamatrix_revision_number_end - self.datamatrix_revision_number_start) + 1

        self.ui.serialNumberLineEdit.setMaxLength(self.datamatrix_serial_number_length)
        self.ui.revisionNumberLineEdit.setMaxLength(self.datamatrix_revision_number_length)

        gui.set_qlineedit_input_width(self.ui.serialNumberLineEdit)
        gui.set_qlineedit_input_width(self.ui.revisionNumberLineEdit)

        self.set_part_number('UNDEFINED')

        self.ui.exitPushButton.clicked.connect(self.exit)
        self.ui.programPushButton.clicked.connect(self.program)
        self.ui.helpPushButton.clicked.connect(
            lambda: QDesktopServices.openUrl(
                QUrl('file://%s/usage_guide.html' % INSTALLATION_DIRECTORY)))

        # self.ui.serialNumberLineEdit.setFocusPolicy(QtCore.Qt.StrongFocus)
        # self.ui.serialNumberLineEdit.setFocus()

        self.datamatrix_serial_number_length = (self.datamatrix_serial_number_end -
                                                self.datamatrix_serial_number_start) + 1

        self.datamatrix_revision_number_length = \
            (self.datamatrix_revision_number_end - self.datamatrix_revision_number_start) + 1

        self.ui.serialNumberLineEdit.setMaxLength(self.datamatrix_serial_number_length)
        self.ui.revisionNumberLineEdit.setMaxLength(self.datamatrix_revision_number_length)

        self.barcode_scanner_thread = barcodeScannerWorker.SerialPortScanner()
        if not self.barcode_scanner_thread.device_is_open():
            self.barcode_scanner_thread.alert_handler.connect(self.set_status)
            self.barcode_scanner_thread.data_scanned_handler.connect(self.update_board_info)
            self.barcode_scanner_thread.start()
        else:
            self.set_status('ERROR', 'Barcode scanner device is in use by another application that is running, '
                                     'close other application to use')

        self.currMacAddress = None

        exit_code, output = ping.pingSystem(self.board_test_server_ip_address, str(2), 2)

        if exit_code != 0:
            message = """Board Test Server is not online at IP Address: %s
            Make sure server is online, networked, and configured with above IP address.
            Board programming can not be performed if server is not online.
            Error:
            %s""" % (self.board_test_server_ip_address, output)
            self.set_status('error', message)
            self.ui.programPushButton.setEnabled(False)

        else:
            response = boardTestServer.send_message_with_response(
                board_test_server_ip_address=self.board_test_server_ip_address,
                board_test_server_port=self.board_test_server_port,
                board_test_server_timeout=self.board_test_server_timeout,
                message='ping' + '\n',
                status_in_response=False)
            if response.strip() == 'on-line':
                self.set_status('success', "Test Station Board Test Server is online and listening at IP Address: %s" %
                                self.board_test_server_ip_address)
            else:
                self.set_status('info', """Test Station Board Test Server is not running on specified host at %s
Check acr_board_programmer.ini file for errors""" %
                                self.board_test_server_ip_address)
                self.ui.programPushButton.setEnabled(False)
            # if self.pool is not None:
            #     QtCore.QTimer.singleShot(0, self.get_mac_address_from_server)

    @pyqtSlot(str)
    def update_board_info(self, barcode_data):
        try:
            print(len(barcode_data))
            print(':%s:' % barcode_data)
            logging.debug('barcode data is :%s:, length = %d' % (barcode_data, len(barcode_data)))
            self.parse_datamatrix_data(barcode_data)
            print(self.part_number)
            print(self.serial_number)
            print(self.revision_number)
            logging.debug("part number = %s, serial number = %s, revision number = %s" %
                          (self.part_number, self.serial_number, self.revision_number))
            self.set_part_number(self.part_number)
            self.ui.serialNumberLineEdit.setText(self.serial_number)
            self.ui.revisionNumberLineEdit.setText(self.revision_number)
            self.ui.manufactureDateMonthLabel.setText(self.manufacture_date[:2])
            self.ui.manufactureDateDayLabel.setText(self.manufacture_date[3:5])
            self.ui.manufactureDateYearLabel.setText(self.manufacture_date[6:])
        except:
            pass

    def valid_serial_number(self, serial_number):
        valid = False
        serial_number_format_length = (self.datamatrix_serial_number_end - self.datamatrix_serial_number_start) + 1
        if len(serial_number) == serial_number_format_length and serial_number.isdigit():
            valid = True
        return valid

    def valid_revision_number(self, revision_number):
        valid = False
        revision_number_format_length = \
            (self.datamatrix_revision_number_end - self.datamatrix_revision_number_start) + 1
        if len(revision_number) == revision_number_format_length:
            valid = True
        return valid

    def _setup_enhanced_logging(self,
                                filename,
                                maximum_file_size_bytes=5 * 1024 * 1024,
                                backup_logs=5):
        """
        Setup the python default logger to include additional elements in logged message
        as well as log backups which rotate when the maximum log size is reached
        :param filename: filename of the log file
        :param maximum_file_size_bytes: maximum size of log file before it is closed and put in rotation
        :param backup_logs: maximum number of backup logs maintained
        :return: None
        """

        log_levels = {'DEBUG': logging.DEBUG,
                      'INFO': logging.INFO,
                      'WARNING': logging.WARNING,
                      'ERROR': logging.ERROR,
                      'CRITICAL': logging.CRITICAL}

        try:
            logging_level = log_levels[self.log_level.upper()]
        except:
            logging_level = logging.DEBUG

        logging.basicConfig(filename=filename,
                            filemode='w',
                            format='[%(asctime)s:%(levelname)s:%(module)s:%(funcName)s:%(lineno)d]-%(message)s',
                            level=logging_level,
                            datefmt='%m/%d/%Y %I:%M:%S %p')

        logger = logging.getLogger('EnhancedLogger')

        # Add the log message handler to the logger
        handler = logging.handlers.RotatingFileHandler(
            filename, maxBytes=maximum_file_size_bytes, backupCount=backup_logs)

        logger.addHandler(handler)

    def generate_pid_file(self):
        with open('%s/acr_board_programmer.pid' % INSTALLATION_DIRECTORY, 'w') as pid_file:
            pid_file.write("%d\n" % os.getpid())

    def parse_datamatrix_data(self, datamatrix_encoded_data):
        self.revision_number = None
        self.part_number = None
        self.serial_number = None
        self.manufacture_date = None

        print(len(datamatrix_encoded_data))
        if len(datamatrix_encoded_data) == self.datamatrix_format_length:
            self.part_number = \
                datamatrix_encoded_data[self.datamatrix_part_number_start:self.datamatrix_part_number_end + 1]
            self.revision_number = \
                datamatrix_encoded_data[
                self.datamatrix_revision_number_start:self.datamatrix_revision_number_end + 1].strip()
            self.serial_number = \
                datamatrix_encoded_data[self.datamatrix_serial_number_start:self.datamatrix_serial_number_end + 1]
            self.manufacture_date = \
                datamatrix_encoded_data[self.datamatrix_manufacture_date_start:
                                        self.datamatrix_manufacture_date_start + 2] + '/' + \
                datamatrix_encoded_data[self.datamatrix_manufacture_date_start + 2:
                                        self.datamatrix_manufacture_date_start + 4] + '/' + \
                datamatrix_encoded_data[self.datamatrix_manufacture_date_start + 4:
                                        self.datamatrix_manufacture_date_start + 6]

    def clear_status(self):
        self.ui.statusLabel.clear()
        QtCore.QCoreApplication.processEvents()

    def append_output(self, output):
        self.ui.statusLabel.append(output)

    @pyqtSlot(str, str)
    def set_status(self, status, message, set_overall_status=False):
        color = None
        if status.lower() == 'error':
            color = "red"
            logging.error(message)
        elif status.lower() == 'success':
            color = "green"
            logging.info(message)
        elif status.lower() == 'info':
            color = "black"
            logging.info(message)
        elif status.lower() == 'warning':
            color = "yellow"
            logging.warning(message)

        if set_overall_status:
            self.overall_status = status.lower()
            self.overall_status_color = color
        else:
            if self.severity_map[status.lower()] > self.severity_map[self.overall_status]:
                self.overall_status = status.lower()
                self.overall_status_color = color

        self.ui.overallStatusLabel.setText(self.overall_status.upper())
        self.ui.overallStatusLabel.setStyleSheet("background-color: %s; color: white" % self.overall_status_color)

        self.ui.statusLabel.append('<br><font color="%s"><B>%s</B>: %s</font>' %
                                   (color, status.upper(), message))
        QtCore.QCoreApplication.processEvents()

    # exit dialog
    def exit(self):
        del self.barcode_scanner_thread
        QDialog.reject(self)

    def get_part_number(self):
        return self.ui.boardPartNumberComboBox.currentText()

    def set_part_number(self, part_number):
        index = self.ui.boardPartNumberComboBox.findText(part_number, Qt.MatchFixedString)
        if index >= 0:
            self.ui.boardPartNumberComboBox.setCurrentIndex(index)
        else:
            index = self.ui.boardPartNumberComboBox.findText('UNDEFINED', Qt.MatchFixedString)
            self.ui.boardPartNumberComboBox.setCurrentIndex(index)

    def get_next_mac_address(self):

        mac_address = None

        if self.pool is not None:

            try:

                self.append_output("Attempting to retrieve MAC Address from board test server")
                logging.info("Attempting to retrieve MAC Address from board test server")

                mac_address = boardTestServer.send_message_with_response(
                    board_test_server_ip_address=self.board_test_server_ip_address,
                    board_test_server_port=self.board_test_server_port,
                    board_test_server_timeout=self.board_test_server_timeout,
                    message='getNextMACAddress' + '|' + str(self.pool) + '\n')

                QApplication.processEvents()
                mac_address = mac_address.strip()
                if ((len(mac_address) < 17) or (':' != mac_address[2]) or (':' != mac_address[5])
                        or (':' != mac_address[8]) or (':' != mac_address[11]) or (':' != mac_address[14])):
                    self.append_output("Timeout from server trying to retrieve MAC Address.")
                    self.set_status('error',
                                    'Timeout from server trying to retrieve MAC Address.\n '
                                    'Verify Board Test Server running at IP address %s and/or port %d is not blocked'
                                    'in network.' %
                                    (self.board_test_server_ip_address, self.board_test_server_port))
                    return None
                else:
                    self.set_status('info', 'MAC Address %s retrieved from Board Test Server' % mac_address)

            except:
                self.set_status('error', 'Could not retrieve MAC Address from Board Test Server')
                pass

        return mac_address

    def get_mac_address_from_server(self):

        self.currMacAddress = None

        if self.pool is None:
            self.ui.poolNumberLabel.setText("??")
            self.set_status("error", "MAC Address Pool number is not configured correctly. Programming not possible")
            return

        attempt = 0
        next_mac_address = ''

        while not next_mac_address and attempt < 3:
            next_mac_address = self.get_next_mac_address()
            attempt = attempt + 1

        if ((len(next_mac_address) < 17) or (':' != next_mac_address[2]) or
                (':' != next_mac_address[5]) or (':' != next_mac_address[8]) or
                (':' != next_mac_address[11]) or (':' != next_mac_address[14])):
            self.set_status('error', 'bad MAC address retrieved')
            return

        else:
            self.currMacAddress = next_mac_address
            self.set_status('success', "MAC Address: %s retrieved from board test server." % next_mac_address)

        if self.currMacAddress is None:
            self.ui.macAddressLabel.setText('')
        else:
            self.ui.macAddressLabel.setText(self.currMacAddress)

    def get_programming_filename(self, part_number):
        return self.configuration.get('BOARD_PART_NUMBERS', part_number)

    def get_manufacture_date(self):
        manufacture_date = self.ui.manufactureDateMonthLabel.text() + '/' + self.ui.manufactureDateDayLabel.text() + '/' + \
                           self.ui.manufactureDateYearLabel.text()
        return manufacture_date

    def manufacture_date_is_valid(self, manufacture_date):
        valid_date = False
        month, day, year = manufacture_date.split("/")
        try:
            datetime.datetime(int(year), int(month), int(day))
            valid_date = True
        except ValueError:
            pass
        return valid_date

    def program(self):

        self.set_status("success", "Board programming started", True)

        if self.pool is None:
            self.set_status("ERROR", "MAC Address Pool has not been configured")
            return

        # if self.scanner_support != 'TRUE':
        serial_number = self.ui.serialNumberLineEdit.text()
        revision_number = self.ui.revisionNumberLineEdit.text()
        manufacture_date = self.get_manufacture_date()
        part_number = self.get_part_number()

        if not self.manufacture_date_is_valid(manufacture_date):
            self.set_status("ERROR", "Invalid manufacture date scanned or entered")
            return

        if not serial_number or serial_number.strip() == '':
            self.set_status("ERROR", "Board serial number has not been scanned or entered")
            return

        if len(serial_number) != self.datamatrix_serial_number_length:
            self.set_status("ERROR", "Board serial number is not at least %d characters long" %
                            self.datamatrix_serial_number_length)
            return

        if part_number == 'UNDEFINED':
            self.set_status('ERROR', "Board part selected or scanned is not defined")
            return

        if not revision_number or revision_number.strip() == '':
            self.set_status("ERROR", "Board revision number has not been scanned or entered")
            return

        self.get_mac_address_from_server()

        if self.currMacAddress is not None:

            # remove old image
            os.system('/bin/rm -f /tmp/u-boot.bin')
            os.system('/bin/sync')

            program_filename = self.get_programming_filename(part_number)

            # print(processor_type)
            # filename = 'u-boot-%sx.x.xx.imx'
            # if processor_type[:9] == '00-448410':
            #     filename = filename % 's-'
            # elif processor_type[:12] == '00-448420-1':
            #     filename = filename % ''
            # elif processor_type[:17] == '00-448420-2 ISSI':
            #     filename = filename % 'issi-'
            #     print(filename)
            # elif processor_type[:9] == '00-448430':
            #     filename = filename % 'q-'
            # elif processor_type[:17] == '00-449970-2':
            #     filename = filename % 'ark-dq-'
		
            self.set_status('info', 'File name =  %s ' % program_filename)

            try:
                with open(program_filename, 'rb') as program_file:
                    image = program_file.read()

            except:
                self.set_status('error', 'Failed to read programming file image')
                return

            mac_high = 'mac_addr_high=0x' + self.currMacAddress[0] + self.currMacAddress[1] + \
                       self.currMacAddress[3] + self.currMacAddress[4]
            mac_low = 'mac_addr_low=0x' + self.currMacAddress[6] + self.currMacAddress[7] + \
                      self.currMacAddress[9] + self.currMacAddress[10] + self.currMacAddress[12] + \
                      self.currMacAddress[13] + self.currMacAddress[15] + self.currMacAddress[16]

            # replace canned MAC address with ours
            image = image.replace(b'mac_addr_high=0x0123', bytes(mac_high, encoding='utf-8'))
            image = image.replace(b'mac_addr_low=0x456789ab', bytes(mac_low, encoding='utf-8'))

            # verify that uboot image has new mac address and not the original defaults
            if (image.find(b'mac_addr_high=0x0123') != -1 or image.find(b'mac_addr_low=0x456789ab') != -1):
                self.set_status('error', "Failed to write mac addr to u-boot.bin file")
                return
		
            # generate u-boot image
            try:
                with open('/tmp/u-boot.bin', 'wb') as dst:
                    dst.write(bytes(image))

            except:
                self.set_status('error', 'Failed to write u-boot.bin image to program board')
                return

            # sync disk
            os.system('/bin/sync')

            # cmd = ['/usr/bin/sudo',
            #        os.path.join(INSTALLATION_DIRECTORY, "imx_usb"),
            #        "/tmp/u-boot.bin"]

            try:
                os.chdir(INSTALLATION_DIRECTORY)
                exit_code = os.system("/usr/bin/sudo %s /tmp/u-boot.bin" %
                                      os.path.join(INSTALLATION_DIRECTORY, "imx_usb"))

                if exit_code == 0:
                    self.set_status('success', "Board has been programmed.")
                else:
                    self.set_status('error', "Programming board.")
                    return

                # Send message to store board data to board test server
                wired_mac_address = self.currMacAddress
                wireless_mac_address = ''
                hostname = ''
                board_model = ''
                loader_version = ''
                peripheral_version = ''
                board_assembly_number = part_number
                board_rev_number = revision_number

                date = datetime.datetime.today().strftime("%m/%d/%Y %H:%M:%S")

                command = 'storeBoardData|' + \
                          serial_number + '|' + \
                          wired_mac_address + '|' + \
                          wireless_mac_address + '|' + \
                          hostname + '|' + \
                          board_model + '|' + \
                          loader_version + '|' + \
                          peripheral_version + '|' + \
                          board_assembly_number + '|' + \
                          board_rev_number + '|' + \
                          manufacture_date + '|' + \
                          date + '|'

                boardTestServer.send_message_with_response(
                    board_test_server_ip_address=self.board_test_server_ip_address,
                    board_test_server_port=self.board_test_server_port,
                    board_test_server_timeout=self.board_test_server_timeout,
                    message=command)

                self.set_status(status="success",
                                message="Board info recorded in Test Station Server DB",
                                set_overall_status=True)

            except:
                self.set_status(status='error',
                                message='Failed to program board or record board info in Test Station Server')

        else:
            self.set_status('error',
                            'MAC Address not defined. Verify board test server is configured and online in the network.')

        self.ui.serialNumberLineEdit.setFocus()


if __name__ == "__main__":

    # block programming from running more than once
    if is_program_running():
        sys.exit(0)

    app = QApplication(sys.argv)
    QApplication.setStyle(QStyleFactory.create("Cleanlooks"))
    QApplication.setPalette(QApplication.style().standardPalette())
    board_programmer = AcrBoardProgrammer(None)
    board_programmer.show()
    sys.exit(app.exec_())
