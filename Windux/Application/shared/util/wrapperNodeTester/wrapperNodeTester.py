import configparser
import datetime
import glob
import os
import sys
import time
import logging
import logging.handlers
import execute_test_worker
import serial

from PyQt5 import QtCore
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
from Ui_wrapperNodeTester import Ui_wrapperNodeTestDialog

if sys.platform == 'win32':
    application_directory = QStandardPaths.standardLocations(QStandardPaths.AppDataLocation)[0] + '/Node Controller Tester'
    if not os.path.exists(application_directory):
        os.makedirs(application_directory)
    print(application_directory)
    INSTALLATION_DIRECTORY = application_directory

else:
    if os.path.exists(os.path.join("/", "home", "pi", "test_station_server")):
        sys.path.append("/home/pi/test_station_server")
    else:
        sys.path.append("/home/pi/test_station_client")
    INSTALLATION_DIRECTORY = "/home/pi/wrapper_node_tester"

import host_ip_address
import barcodeScannerWorker
import boardTestServer
import ping
import gui

PRODUCT_VERSION = "1.6.0"

PYTHON_DEBUG = True

if PYTHON_DEBUG:
    pyqtRemoveInputHook()


def get_pid_file():
    pid = None
    try:
        with open('%s/wrapper_node_tester.pid' % INSTALLATION_DIRECTORY, 'r') as pid_file:
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


class WrapperNodeTester(QDialog):

    def __init__(self, parent=None):

        self.generate_pid_file()

        QDialog.__init__(self, parent)

        # create USK Key Creator Dialog
        self.ui = Ui_wrapperNodeTestDialog()
        self.execute_test_thread = None

        # manifest UI
        self.ui.setupUi(self)

        self.setWindowTitle("Node Controller Tester (Version: %s)" % PRODUCT_VERSION)

        QCoreApplication.setOrganizationName("Hobart")
        QCoreApplication.setOrganizationDomain("hobartcorp.com")
        QCoreApplication.setApplicationName("NODE_CONTROLLER_TESTER")

        if 'win' in sys.platform:
            self.serial_device_port = 'COM3'
            self.barcode_scanner_device = 'COM4'
        else:
            self.serial_device_port = '/dev/ttyUSB0'
            self.barcode_scanner_device = '/dev/ttyACM0'

        self.configuration = configparser.ConfigParser(inline_comment_prefixes='#')
        self.configuration.read("wrapper_node_tester.ini")

        self.log_level = self.configuration.get('GENERAL', "logging_level")

        self.severity_map = {'success': 1, 'info': 2, 'warning': 3, 'error': 4}
        self.overall_status = 'success'
        self.overall_status_color = 'green'

        log_filename = os.path.join(INSTALLATION_DIRECTORY, "wrapper_node_tester.log")

        self._setup_enhanced_logging(log_filename)

        self.scanner_support = self.configuration.get("GENERAL", "scanner_support").upper()

        self.serial_device = None
        self.serial_device_port = self.configuration.get("GENERAL", "serial_port")
        self.barcode_scanner_device = self.configuration.get("GENERAL", "barcode_scanner_device")

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

        local_ip_address = host_ip_address.get_local_addr(remote=None, ipv6=False)
        self.ui.localIpAddressLabel.setText(local_ip_address)

        self.board_test_server_ip_address = None

        if self.configuration.get("BOARD_TEST_SERVER", "ip_address") != '':
            self.board_test_server_ip_address = self.configuration.get("BOARD_TEST_SERVER", "ip_address")

        self.board_test_server = None

        if self.board_test_server_ip_address is None or self.board_test_server_ip_address == 'localhost':
            self.board_test_server_ip_address = 'localhost'

        self.board_test_server_port = 5081
        if str(self.configuration.getint("BOARD_TEST_SERVER", "port")) != '':
            self.board_test_server_port = self.configuration.getint("BOARD_TEST_SERVER", "port")

        self.board_test_server_timeout = 3.0
        if str(self.configuration.getfloat("BOARD_TEST_SERVER", "timeout")) != '':
            self.board_test_server_timeout = self.configuration.getfloat("BOARD_TEST_SERVER", "timeout")

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

        self.settings = QtCore.QSettings("Hobart", "Wrapper Node Tester")

        self.ui.cancelButton.clicked.connect(self.exit)
        self.ui.startButton.clicked.connect(self.start_stop_test)

        if sys.platform == 'linux':
            self.ui.helpPushButton.clicked.connect(
                lambda: QDesktopServices.openUrl(
                    QUrl('file://%s/usage_guide.html' % INSTALLATION_DIRECTORY)))
        else:
            self.ui.helpPushButton.clicked.connect(
                lambda: QDesktopServices.openUrl(
                    QUrl.fromLocalFile("./usage_guide.html")))

        self.barcode_scanner_thread = barcodeScannerWorker.SerialPortScanner(port_name=self.barcode_scanner_device)
        if not self.barcode_scanner_thread.device_is_open():
            self.barcode_scanner_thread.alert_handler.connect(self.set_status)
            self.barcode_scanner_thread.data_scanned_handler.connect(self.update_board_info)
            self.barcode_scanner_thread.start()
        else:
            self.set_status('error', 'Barcode scanner device is in use by another application that is running, '
                                     'close other application to use')

        board_part_numbers = list(self.configuration.options("BOARD_PART_NUMBERS"))
        self.ui.boardPartNumberComboBox.addItem('UNDEFINED')
        for part_number in board_part_numbers:
            self.ui.boardPartNumberComboBox.addItem(part_number)

        self.ui.revisionNumberLineEdit.setText("")

        self.ui.comPortComboBox.addItem("UNDEFINED")

        com_port_names = self.get_serial_ports()

        for com_port_name in com_port_names:
            self.ui.comPortComboBox.addItem(com_port_name)

        exit_code, output = ping.pingSystem(self.board_test_server_ip_address, str(2), 2)

        if exit_code != 0:
            message = """Board Test Server is not online at IP Address: %s
                    Make sure server is online, networked, and configured with above IP address.
                    board test results can not be recorded if server is not online.
                    Error:
                    %s""" % (self.board_test_server_ip_address, output)
            self.set_status('info', message)
            # self.ui.startButton.setEnabled(False)
            self.ui.serverIpAddressLabel.setText("Server Not Found")

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
                self.ui.serverIpAddressLabel.setText(self.board_test_server_ip_address)
                # self.ui.startButton.setEnabled(True)

            else:
                self.set_status('info', """Test Station Board Test Server is not running on specified host at %s
        Check wrapper_node_board_tester.ini file for errors""" %
                                self.board_test_server_ip_address)
                self.ui.serverIpAddressLabel.setText("Server Not Found")
                # self.ui.startButton.setEnabled(False)

    def generate_pid_file(self):
        with open('%s/wrapper_node_tester.pid' % INSTALLATION_DIRECTORY, 'w') as pid_file:
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

    def get_part_number(self):
        return self.ui.boardPartNumberComboBox.currentText()

    def set_part_number(self, part_number):
        index = self.ui.boardPartNumberComboBox.findText(part_number, Qt.MatchFixedString)
        if index >= 0:
            self.ui.boardPartNumberComboBox.setCurrentIndex(index)
        else:
            index = self.ui.boardPartNumberComboBox.findText('UNDEFINED', Qt.MatchFixedString)
            self.ui.boardPartNumberComboBox.setCurrentIndex(index)

    def get_serial_ports(self):
        """ Lists serial port names

            :raises EnvironmentError:
                On unsupported or unknown platforms
            :returns:
                A list of the serial ports available on the system
        """
        if sys.platform.startswith('win'):
            ports = ['COM%s' % (i + 1) for i in range(10)]
        elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
            # this excludes your current terminal "/dev/tty"
            ports = glob.glob('/dev/ttyUSB[0-9]*')
        elif sys.platform.startswith('darwin'):
            ports = glob.glob('/dev/tty.*')
        else:
            raise EnvironmentError('Unsupported platform')

        one_com_port_was_open = False

        result = []
        for port in ports:
            try:
                s = serial.Serial(port)
                s.close()
                one_com_port_was_open = True
                result.append(port)
            except (OSError, serial.SerialException):
                pass

        if not one_com_port_was_open:
            self.set_status('error', "No serial ports could be open to conduct test.")
        return result

    def clear_status(self):
        self.ui.statusLabel.clear()

    @pyqtSlot(str, str)
    def set_status(self, status, message, set_overall_status=False):
        color = 'green'
        if status.lower() == 'error':
            color = "red"
        elif status.lower() == 'success':
            color = "green"
        elif status.lower() == 'info':
            color = "black"
        elif status.lower() == 'warning':
            color = "yellow"

        if set_overall_status:
            self.overall_status = status.lower()
            self.overall_status_color = color
        else:
            if self.severity_map[status.lower()] > self.severity_map[self.overall_status]:
                self.overall_status = status.lower()
                self.overall_status_color = color

        self.ui.overallStatusLabel.setText(self.overall_status.upper())
        self.ui.overallStatusLabel.setStyleSheet("background-color: %s; color: white" % self.overall_status_color)

        self.ui.statusLabel.append('<br><font color="%s"><B>%s</B>: %s</font>' % (color, status.upper(), message))
        QtCore.QCoreApplication.processEvents()

    # exit dialog
    def exit(self):
        try:
            self.serial_device.close()
        except:
            pass
        QDialog.reject(self)

    def update_board_info_on_server(self,
                                    manufacture_date,
                                    part_number,
                                    serial_number,
                                    revision_number
                                    ):
        # Send message to store board data to board test server
        serial_number = serial_number
        wired_mac_address = ''
        wireless_mac_address = ''
        hostname = ''
        board_model = ''
        loader_version = ''
        peripheral_version = ''
        board_assembly_number = part_number
        board_rev_number = revision_number

        todays_date = datetime.datetime.today().strftime("%m/%d/%Y %H:%M:%S")

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
                  todays_date + '|'

        boardTestServer.send_message_with_response(
            board_test_server_ip_address=self.board_test_server_ip_address,
            board_test_server_port=self.board_test_server_port,
            board_test_server_timeout=self.board_test_server_timeout,
            message=command)

    def valid_revision_number(self, revision_number):
        valid = False
        revision_number_format_length = \
            (self.datamatrix_revision_number_end - self.datamatrix_revision_number_start) + 1
        if len(revision_number) >= 1:
            valid = True
        return valid

    def valid_serial_number(self, serial_number):
        valid = False
        serial_number_format_length = (self.datamatrix_serial_number_end - self.datamatrix_serial_number_start) + 1
        if len(serial_number) >= 1 and serial_number.isdigit():
            valid = True
        return valid

    def start_stop_test(self):

        if self.ui.startButton.text() == 'Start':

            self.part_number = self.get_part_number()
            self.overall_status = 'success'
            self.overall_status_color = 'green'

            if self.part_number != 'UNDEFINED':

                manufacture_date = self.get_manufacture_date()

                if self.valid_manufacture_date(manufacture_date):

                    self.serial_number = self.ui.serialNumberLineEdit.text()

                    if self.valid_serial_number(self.serial_number):

                        self.revision_number = self.ui.revisionNumberLineEdit.text()

                        if self.valid_revision_number(self.revision_number):

                            self.ui.startButton.setText("Stop")
                            self.clear_status()

                            self.set_status("success", "Board test started, power on or cycle power of board under test.", True)

                            self.serial_device_port = self.ui.comPortComboBox.currentText()
                            # NOTE:
                            # Serial Number is not stored in the board but there will
                            # be board info on sticker on board with QR code
                            self.execute_test_thread = execute_test_worker.Worker(self.part_number,
                                                                                  self.manufacture_date,
                                                                                  self.revision_number,
                                                                                  self.serial_device_port)

                            self.execute_test_thread.alert_handler.connect(self.set_status)

                            self.execute_test_thread.test_complete_handler.connect(self.test_complete)

                            self.execute_test_thread.start()

                        else:
                            self.set_status('error', 'Invalid revision number entered or scanned')
                    else:
                        self.set_status('error', 'Invalid serial number entered or scanned')
                else:
                    self.set_status('error', 'Invalid manufacture date entered or scanned')
            else:
                self.set_status('error', 'Part number has not been selected or scanned.')

        else:
            self.execute_test_thread.stop_test()
            self.set_status("success", "Board test aborted.", True)
            self.ui.startButton.setText("Start")

    @pyqtSlot(str)
    def test_complete(self, status):
        self.ui.startButton.setText('Start')
        if status == 'PASS':
            self.update_board_info_on_server(manufacture_date=self.manufacture_date,
                                             part_number=self.part_number,
                                             serial_number=self.serial_number,
                                             revision_number=self.revision_number)
            self.set_status('success', "Board test complete", True)

    def get_manufacture_date(self):
        manufacture_date = self.ui.manufactureDateMonthLabel.text() + '/' + \
                           self.ui.manufactureDateDayLabel.text() + '/' + \
                           self.ui.manufactureDateYearLabel.text()
        return manufacture_date

    def valid_manufacture_date(self, manufacture_date):
        valid_date = False
        month, day, year = manufacture_date.split("/")
        try:
            datetime.datetime(int(year), int(month), int(day))
            valid_date = True
        except ValueError:
            pass
        return valid_date

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


if __name__ == "__main__":

    # block programming from running more than once
    if is_program_running():
        sys.exit(0)

    app = QApplication(sys.argv)
    QApplication.setStyle(QStyleFactory.create("Cleanlooks"))
    QApplication.setPalette(QApplication.style().standardPalette())
    window = WrapperNodeTester(None)
    window.show()
    sys.exit(app.exec_())
