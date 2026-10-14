#!/usr/bin/env python3

import datetime
import logging
import logging.handlers
import os
import shutil
import sys
from functools import partial
import time

import netaddr
from PyQt5 import QtCore
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *

import cpu_memory_tester_worker
import network_scan
import program_uboot_worker
from Ui_cpu_memory_tester import Ui_cpuMemtesterMainWindow

import serial
# from pexpect_serial import SerialSpawn

VERSION = "1.6.0"
SUBNET = "192.168.0.0"

PYTHON_DEBUG = False

if sys.platform == 'darwin':
    INSTALLATION_DIRECTORY = "/Users/rsdavis/PycharmProjects"
else:
    INSTALLATION_DIRECTORY = "/home/pi/cpu_memory_tester"

if PYTHON_DEBUG:
    pyqtRemoveInputHook()


def get_pid_file():
    pid = None
    try:
        with open('%s/cpu_memory_tester.pid' % INSTALLATION_DIRECTORY, 'r') as pid_file:
            pid = int(pid_file.readline().strip())
    except:
        pass
    return pid


def is_programming_running():
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
    with open('%s/cpu_memory_tester.pid' % INSTALLATION_DIRECTORY, 'w') as pid_file:
        pid_file.write("%d\n" % os.getpid())


# Filter out the cpu_memory_tester folder for selection of Uboot file
# since this is the destination directory for copying the selected Uboot file
# to and if same file exists there it will crash program.
class FileFilterProxyModel(QtCore.QSortFilterProxyModel):

    def __init__(self, parent=None):
        super(QtCore.QSortFilterProxyModel, self).__init__(parent)

    def filterAcceptsRow(self, source_row: int, source_parent: QModelIndex) -> bool:
        source_model = self.sourceModel()
        index0 = source_model.index(source_row, 0, source_parent)
        # print(source_model.fileName(index0).lower())
        # print(source_model.filePath(index0).lower())
        if source_model.isDir(index0):
            if INSTALLATION_DIRECTORY in os.path.join(source_model.filePath(index0).lower(),
                                                      source_model.fileName(index0).lower()):
                return False
            else:
                return True
        return True


class CpuMemoryTester(QMainWindow, Ui_cpuMemtesterMainWindow):

    def __init__(self):

        generate_pid_file()

        QMainWindow.__init__(self)

        self.setupUi(self)
        self.log_level = 'info'
        self.test_start_date_time = datetime.datetime.today().strftime("%m_%d_%Y_%H_%M_%S")
        self._setup_enhanced_logging(filename='cpu_memory_tester_%s.log' % self.test_start_date_time)

        self.setWindowTitle("CPU Memory Tester version (%s)" % VERSION)
        self.actionExit.triggered.connect(self.exit_application)
        self.actionExit.setShortcut('Ctrl+Q')
        self.actionStartScan.triggered.connect(self.scan_network)
        self.actionStartScan.setShortcut('Ctrl+B')
        self.actionStopScan.triggered.connect(self.stop_scanning_network)
        self.actionStopScan.setShortcut('Ctrl+E')
        self.actionStartTest.triggered.connect(self.start_testing)
        self.actionStartTest.setShortcut('Ctrl+T')
        self.actionStopTest.triggered.connect(self.stop_testing)
        self.actionStopTest.setShortcut('Ctrl+X')
        self.actionSelectUboot.triggered.connect(self.select_uboot_file)
        self.actionSelectUboot.setShortcut('Ctrl+U')
        self.actionNetwork.triggered.connect(self.program_uboot_via_network)
        self.actionSerial.setEnabled(False)
        self.actionSerial.triggered.connect(self.program_uboot_via_serial)
        self.actionStopScan.setEnabled(False)
        self.actionStopTest.setDisabled(True)
        self.actionUsage.triggered.connect(
            lambda: QDesktopServices.openUrl(
                QUrl('file://%s/usage_guide.html' % INSTALLATION_DIRECTORY)))
        self.actionUsage.setShortcut('Ctrl+H')
        self.network_scan = network_scan.ProbeNetwork(
            scan_handler=self.handle_network_scan_message,
            subnet=SUBNET)
        ip_address_list = self.network_scan.get_default_network_ip_address_range()
        self.startIpLineEdit.setText(str(ip_address_list[0]))
        self.endIpLineEdit.setText(str(ip_address_list[-1]))
        self.network_scan.progress.connect(self.update_scan_progress)
        self.network_scan.alerts_handler.connect(self.set_alert)
        self.scan_in_progress = False
        self.number_of_devices_to_test = 0
        # self.test_start_date_time = None
        self.uboot_filename_qt_regex = "hti-linux-uboot-*-*.package.tgz"
        self.program_uboot_worker = None

        self.details_buttons = [self.detailsPushButton1, self.detailsPushButton2, self.detailsPushButton3,
                                self.detailsPushButton4, self.detailsPushButton5, self.detailsPushButton6,
                                self.detailsPushButton7, self.detailsPushButton8, self.detailsPushButton9,
                                self.detailsPushButton10, self.detailsPushButton11, self.detailsPushButton12,
                                self.detailsPushButton13, self.detailsPushButton14, self.detailsPushButton15,
                                self.detailsPushButton16, self.detailsPushButton17, self.detailsPushButton18,
                                self.detailsPushButton19, self.detailsPushButton20, self.detailsPushButton21,
                                self.detailsPushButton22, self.detailsPushButton23, self.detailsPushButton24,
                                self.detailsPushButton25, self.detailsPushButton26, self.detailsPushButton27,
                                self.detailsPushButton28, self.detailsPushButton29, self.detailsPushButton30,
                                self.detailsPushButton31, self.detailsPushButton32, self.detailsPushButton33,
                                self.detailsPushButton34, self.detailsPushButton35, self.detailsPushButton36,
                                self.detailsPushButton37, self.detailsPushButton38, self.detailsPushButton39,
                                self.detailsPushButton40, self.detailsPushButton41, self.detailsPushButton42,
                                self.detailsPushButton43, self.detailsPushButton44, self.detailsPushButton45,
                                self.detailsPushButton46, self.detailsPushButton47, self.detailsPushButton48,
                                self.detailsPushButton49, self.detailsPushButton50]

        self.elapsed_time = 0
        button_number = 0
        for details_button in self.details_buttons:
            button_number_call = partial(self.details_button_handler, button_number)
            details_button.clicked.connect(button_number_call)
            details_button.hide()
            details_button.setDisabled(True)
            button_number = button_number + 1

        self.deviceFrames = [self.frame1, self.frame2, self.frame3, self.frame4, self.frame5]
        self.device_ip_addresses = [
            self.ipAddressLabel1, self.ipAddressLabel2, self.ipAddressLabel3, self.ipAddressLabel4,
            self.ipAddressLabel5,
            self.ipAddressLabel6, self.ipAddressLabel7, self.ipAddressLabel8, self.ipAddressLabel9,
            self.ipAddressLabel10,
            self.ipAddressLabel11, self.ipAddressLabel12, self.ipAddressLabel13, self.ipAddressLabel14,
            self.ipAddressLabel15,
            self.ipAddressLabel16, self.ipAddressLabel17, self.ipAddressLabel18, self.ipAddressLabel19,
            self.ipAddressLabel20,
            self.ipAddressLabel21, self.ipAddressLabel22, self.ipAddressLabel23, self.ipAddressLabel24,
            self.ipAddressLabel25,
            self.ipAddressLabel26, self.ipAddressLabel27, self.ipAddressLabel28, self.ipAddressLabel29,
            self.ipAddressLabel30,
            self.ipAddressLabel31, self.ipAddressLabel32, self.ipAddressLabel33, self.ipAddressLabel34,
            self.ipAddressLabel35,
            self.ipAddressLabel36, self.ipAddressLabel37, self.ipAddressLabel38, self.ipAddressLabel39,
            self.ipAddressLabel40,
            self.ipAddressLabel41, self.ipAddressLabel42, self.ipAddressLabel43, self.ipAddressLabel44,
            self.ipAddressLabel45,
            self.ipAddressLabel46, self.ipAddressLabel47, self.ipAddressLabel48, self.ipAddressLabel49,
            self.ipAddressLabel50]

        for device_ip_address in self.device_ip_addresses:
            device_ip_address.hide()

        # for deviceFrame in self.deviceFrames:
        #      deviceFrame.hide()
        self.cpu_memory_tester_worker = None
        self.hosts = {}
        self.overall_test_status = 'PASS'

        #self.dhcp_server_restart()

    def dhcp_server_restart(self):
        exit_code = os.system("/usr/bin/sudo /usr/sbin/service isc-dhcp-server status > /dev/null")
        if exit_code != 0:
            self.set_alert("Attempting to start DCHP server")
            exit_code = os.system("/usr/bin/sudo /usr/sbin/service isc-dhcp-server restart > /dev/null")
            if exit_code != 0:
                self.set_alert("ERROR: can not start dhcp server")
            else:
                self.set_alert("INFO: DCHP server started")
        else:
            self.set_alert("INFO: DHCP server is running")

    def clear_devices_under_test_area(self):
        for device_ip_address in self.device_ip_addresses:
            device_ip_address.setText("")
            device_ip_address.hide()
        for details_button in self.details_buttons:
            details_button.hide()

    def initialize_devices_status(self):
        for device_ip_address in self.device_ip_addresses:
            device_ip_address.setStyleSheet("color: black;")

    def program_uboot_via_serial(self):

        uboot_comnands = ['sf probe',
                          'sf erase 0x0 0xc0000',
                          'sf erase 0xc0000 0x2000',
                          'mmc dev 1',  # could be dev 0
                          'ext2load mmc 1 10800000 u-boot-itw.imx',
                          'sf write 10800000 0x400 $filesize',
                          'reset']

        command_prompt = '.*>'

        command_response_delay = 20.0
        wait_for_operator_and_uboot_delay = 20.0

        programming_info_popup = QMessageBox(self)
        programming_info_popup.setWindowTitle("Program U-Boot (Serial Method)")
        programming_info_popup.setIcon(QMessageBox.Information)
        programming_info_popup.setFixedWidth(500)
        programming_info_popup.setText("""1. Insert Service U-boot Update uSD card into board to program,

2. Connect the board's serial port to the Test Station so it can be programmed.

3. Press Ok button and within %d seconds power on the board""" % (wait_for_operator_and_uboot_delay/2))
        programming_info_popup.setStandardButtons(QMessageBox.Ok | QMessageBox.Cancel)
        programming_info_popup.setDefaultButton(QMessageBox.Ok)
        response = programming_info_popup.exec_()
        if response == QMessageBox.Ok:
            try:
                with serial.Serial('/dev/tty0', 115200, timeout=wait_for_operator_and_uboot_delay) as ser:
                    ss = SerialSpawn(ser)
                    ss.expect("Hit any key to stop autoboot:", timeout=command_response_delay)
                    ss.sendline('\n')
                    ss.expect(command_prompt, timeout=command_response_delay)
                    for command in uboot_comnands:
                        ss.sendline(command)
                        ss.expect(command_prompt, timeout=command_response_delay)
                        time.sleep(command_response_delay)
                    self.set_alert("INFO: Rescan devices and check Details to make sure board updated")

            except:
                self.set_alert("ERROR: Attempt to program board failed. "
                               "Power cycle the board to attempt another programming session "
                               "and Select Program U-boot Serial")

    def program_uboot_via_network(self):
        self.initialize_devices_status()
        self.program_uboot_worker = program_uboot_worker.Worker(filename=self.ubootFileLabel.text(),
                                                                installation_directory=INSTALLATION_DIRECTORY,
                                                                devices_to_program=self.get_devices_to_test())
        self.program_uboot_worker.alert_handler.connect(self.set_alert)
        self.program_uboot_worker.program_uboot_status_handler.connect(self.display_board_info)
        self.program_uboot_worker.start()

    @pyqtSlot(str, str, str, int, float, float, str)
    def display_board_info(self,
                           ip_address,
                           mac_address,
                           uboot_version,
                           cpu_cores,
                           cpu_temperature,
                           free_ram,
                           status):

        button_index = 0

        for device_ip_address in self.device_ip_addresses:

            if ip_address in device_ip_address.text():

                self.hosts[ip_address]['mac_address'] = mac_address
                self.hosts[ip_address]['uboot_version'] = uboot_version
                self.hosts[ip_address]['cpu_cores'] = cpu_cores
                self.hosts[ip_address]['free_ram'] = free_ram
                self.hosts[ip_address]['cpu_temperature'] = cpu_temperature

                if status == 'success':
                    status_color = 'green'
                elif status == 'error':
                    status_color = 'red'
                else:
                    status_color = 'black'

                device_ip_address.setStyleSheet("color: %s;" % status_color)

                self.details_buttons[button_index].show()
                self.details_buttons[button_index].setEnabled(True)
                break

            button_index += 1

    def select_uboot_file(self):

        qfd = QFileDialog()
        qfd.setAcceptMode(QFileDialog.AcceptOpen)
        qfd.setDirectory(os.path.join("/", "media", "pi"))
        qfd.setFileMode(QFileDialog.ExistingFile)
        qfd.setOption(QFileDialog.ReadOnly)
        qfd.setLabelText(QFileDialog.Accept, "Ok")
        qfd.setNameFilter(
            "U-boot Filename (%s)" % self.uboot_filename_qt_regex)
        qfd.setWindowTitle("Choose U-boot Filename")
        qfd.setOption(QFileDialog.DontUseNativeDialog)
        qfd.setProxyModel(FileFilterProxyModel())
        qfd.setViewMode(QFileDialog.List)

        selected = []
        if QDialog.Accepted == qfd.exec_():
            selected = qfd.selectedFiles()

        if 1 == len(selected):
            self.ubootFileLabel.setText(os.path.basename(selected[0]))
            shutil.copy(selected[0], INSTALLATION_DIRECTORY)

    @pyqtSlot(str, str, str, int, float, float, str)
    def handle_network_scan_message(self,
                                    ip_address,
                                    mac_address,
                                    uboot_version,
                                    cpu_cores,
                                    cpu_temperature,
                                    free_ram,
                                    status):

        if ip_address != '':
            self.deviceListWidget.addItem(ip_address)
            item = self.deviceListWidget.findItems(ip_address, QtCore.Qt.MatchRegExp)[0]
            self.deviceListWidget.scrollToItem(item, QAbstractItemView.PositionAtTop)
            self.numberDevicesFoundLabel.setText(str(self.number_of_devices_to_test+1))
            self.device_ip_addresses[self.number_of_devices_to_test].setText(ip_address)
            self.device_ip_addresses[self.number_of_devices_to_test].show()
            self.details_buttons[self.number_of_devices_to_test].show()
            self.details_buttons[self.number_of_devices_to_test].setEnabled(True)
            self.hosts[ip_address] = {'uboot_version': uboot_version,
                                      'mac_address': mac_address,
                                      'cpu_cores': cpu_cores,
                                      'cpu_temperature': cpu_temperature,
                                      'free_ram': free_ram,
                                      'status': status}
            self.number_of_devices_to_test += 1

    def details_button_handler(self, button_number):

        device_ip_address = self.device_ip_addresses[button_number].text()
        self.ubootVersionLabel.setText(self.hosts[device_ip_address]['uboot_version'])
        self.macAddressLabel.setText(self.hosts[device_ip_address]['mac_address'])
        self.cpuCoresLabel.setText(str(self.hosts[device_ip_address]['cpu_cores']))
        self.cpuTemperatureLabel.setText('%.2f' % self.hosts[device_ip_address]['cpu_temperature'])
        self.ramFreeLabel.setText('%.2f' % self.hosts[device_ip_address]['free_ram'])

    def scan_network(self):
        if not self.scan_in_progress:
            self.clear_devices_under_test_area()
            self.number_of_devices_to_test = 0
            self.numberDevicesFoundLabel.setText('0')
            self.deviceScanProgressBar.setValue(0)
            self.actionStartScan.setEnabled(False)
            self.actionStopScan.setEnabled(True)
            self.actionStopTest.setDisabled(True)
            self.setCursor(QCursor(QtCore.Qt.WaitCursor))
            self.set_alert("Scanning network for devices to test...")
            self.deviceListWidget.clear()
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
                self.initialize_devices_status()
                self.network_scan.scan(ip_address_list=ip_list)
                self.setCursor(QCursor(QtCore.Qt.ArrowCursor))
        else:
            self.actionStartScan.setEnabled(True)
            self.actionStopScan.setEnabled(False)
            self.actionStartTest.setEnabled(True)
            self.actionStopTest.setEnabled(False)
            self.network_scan.stop()
            self.set_alert("Search for devices to test aborted.")
            self.scan_in_progress = False
            self.progressBar.setValue(0)

    def get_devices_to_test(self):
        devices_to_test = []
        for i in range(self.deviceListWidget.count()):
            devices_to_test.append(self.deviceListWidget.item(i).text())
        return devices_to_test

    def start_testing(self):

        try:
            execution_time_for_test = self.executionTimeLineEdit.text()
            execution_time_for_test = int(execution_time_for_test.strip())
        except:
            self.set_alert("Execution time to conduct test not entered or not an integer in minutes")
            return

        try:
            start_ip = self.startIpLineEdit.text()
            ip = netaddr.IPAddress(start_ip.strip())
        except:
            self.set_alert("Starting IP Address not entered or invalid starting IP address specified.")
            return

        try:
            end_ip = self.endIpLineEdit.text()
            ip = netaddr.IPAddress(end_ip.strip())
        except:
            self.set_alert("Ending IP Address not entered or invalid ending IP address specified.")
            return

        hosts_to_test = self.get_devices_to_test()
        if len(hosts_to_test) > 0:

            self.test_start_date_time = datetime.datetime.today().strftime("%m/%d/%Y %H:%M:%S")
            self.testStartTimeLabel.setText(self.test_start_date_time)
            self.actionStartTest.setDisabled(True)
            self.actionStopTest.setEnabled(True)
            self.actionStartScan.setDisabled(True)
            self.actionStopScan.setDisabled(True)

            # for details_button in self.details_buttons:
            #     details_button.setEnabled(True)

            self.elapsed_time = 0
            self.overall_test_status = 'PASS'
            logging.info("Test started")
            logging.info("Execution Runtime: %d" % execution_time_for_test)
            logging.info("U-boot File used: %s" % self.ubootFileLabel.text())
            logging.info("Starting IP Address: %s" % self.startIpLineEdit.text())
            logging.info("Ending IP Address: %s" % self.endIpLineEdit.text())
            logging.info("IP Address of devices to test: %s" % hosts_to_test)

            self.actionNetwork.setDisabled(True)
            self.actionSerial.setDisabled(True)
            self.initialize_devices_status()
            self.cpu_memory_tester_worker = cpu_memory_tester_worker.Worker(host_range=hosts_to_test,
                                                                            minutes_to_run_test=execution_time_for_test,
                                                                            parent=None)

            self.cpu_memory_tester_worker.test_status_update_handler.connect(self.test_status_update)
            self.cpu_memory_tester_worker.alert_handler.connect(self.set_alert)
            self.cpu_memory_tester_worker.test_complete_handler.connect(self.test_complete)
            self.cpu_memory_tester_worker.elapsed_time_handler.connect(self.elapsed_time_update)
            self.cpu_memory_tester_worker.start()

        else:
            self.set_alert("No devices found to test")

    @pyqtSlot(str)
    def elapsed_time_update(self, elapsed_time):
        self.elapsed_time = elapsed_time
        self.testExecutionTimeLabel.setText(elapsed_time)

    @pyqtSlot(dict, str)
    def test_status_update(self, hosts_status, overall_test_status):
        self.hosts = hosts_status
        self.overall_test_status = overall_test_status
        for device_ip_address in self.device_ip_addresses:
            if device_ip_address.text() in self.hosts:
                if self.hosts[device_ip_address.text()]['status'] == 'PASS':
                    color = 'green'
                else:
                    color = 'red'
                    logging.error("Device IP Address: %s, Test Status: FAIL" % device_ip_address.text())

                device_ip_address.setStyleSheet("background-color: %s; color: white" % color)

        self.overallTestStatusLabel.setText(overall_test_status)

        if self.overall_test_status == 'PASS':
            self.overallTestStatusLabel.setStyleSheet("background-color: green; color: white")
        else:
            self.overallTestStatusLabel.setStyleSheet("background-color: red; color: white")
            logging.error("Overall Test Status: FAIL")

    @pyqtSlot(str)
    def test_complete(self, status):

        self.actionStartScan.setEnabled(True)
        self.actionStopScan.setEnabled(False)
        self.actionStartTest.setEnabled(True)
        self.actionStopTest.setDisabled(True)
        self.actionNetwork.setEnabled(True)
        self.actionSerial.setEnabled(True)

        self.overall_test_status = status
        for ip_address in self.hosts.keys():
            if self.hosts[ip_address]['status'] == 'PASS':
                logging.info("IP Address: %s, U-boot Version: %s, MAC Address: %s, CPU Cores: %d, "
                             "CPU Temperature (Max): %.2f C, RAM (free): %d MB, Status: %s" %
                                       (ip_address,
                                        self.hosts[ip_address]['uboot_version'],
                                        self.hosts[ip_address]['mac_address'],
                                        self.hosts[ip_address]['cpu_cores'],
                                        self.hosts[ip_address]['cpu_temperature'],
                                        self.hosts[ip_address]['free_ram'],
                                        self.hosts[ip_address]['status']))
            else:
                logging.error("IP Address: %s, U-boot Version: %s, MAC Address: %s, CPU Cores: %d, "
                              "CPU Temperature (Max): %.2f C, RAM (free): %d MB, Status: %s" %
                                       (ip_address,
                                        self.hosts[ip_address]['uboot_version'],
                                        self.hosts[ip_address]['mac_address'],
                                        self.hosts[ip_address]['cpu_cores'],
                                        self.hosts[ip_address]['cpu_temperature'],
                                        self.hosts[ip_address]['free_ram'],
                                        self.hosts[ip_address]['status']))

        if self.overall_test_status == "PASS":
            logging.info("Overall Test Status: PASS")
        else:
            logging.error("Overall Test Status: FAIL")
        self.set_alert("Test complete.")

    @pyqtSlot(str)
    def set_alert(self, message):
        logging.info(message)
        self.statusbar.showMessage(message)

    @pyqtSlot(int)
    def update_scan_progress(self, percent_complete):
        self.deviceScanProgressBar.setValue(percent_complete)
        # print(percent_complete)
        if percent_complete >= 100:
            self.set_alert("Test device discovery complete")
            # self.deviceScanProgressBar.setValue(0)
            self.scan_in_progress = False
            self.actionStartScan.setEnabled(True)
            self.actionStopScan.setEnabled(False)

    def stop_testing(self):
        self.cpu_memory_tester_worker.stop_test()
        self.set_alert("Test has been aborted")
        self.actionStartScan.setEnabled(True)
        self.actionStopScan.setEnabled(False)
        self.actionStartTest.setEnabled(True)
        self.actionStopTest.setDisabled(True)
        self.actionNetwork.setEnabled(True)
        self.actionSerial.setEnabled(True)

    def stop_scanning_network(self):
        self.network_scan.stop()

    def exit_application(self):
        try:
            self.cpu_memory_tester_worker.stop_test()
        except:
            pass
        self.close()

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
    if is_programming_running():
        sys.exit(0)

    app = QApplication(sys.argv)
    QApplication.setStyle(QStyleFactory.create("Cleanlooks"))
    cpu_memory_tester = CpuMemoryTester()
    cpu_memory_tester.show()
    sys.exit(app.exec_())
