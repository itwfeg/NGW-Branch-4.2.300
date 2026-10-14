from PyQt5.QtCore import *
from PyQt5.QtCore import pyqtSignal

import time
import serial
import sys
import glob


class Worker(QThread):

    alert_handler = pyqtSignal(str, str)
    test_complete_handler = pyqtSignal(str)

    def __init__(self,
                 part_number,
                 manufacture_date,
                 revision_number,
                 serial_device_port,
                 parent=None):

        QThread.__init__(self, parent)

        self.overall_test_status = 'FAIL'
        self.serial_device = None
        self.serial_device_port = serial_device_port
        self.execute_test = True
        self.part_number = '00-' + part_number + '\r\n'
        self.manufacture_date = manufacture_date.replace('/', '')
        self.manufacture_date = manufacture_date + '\r\n'
        self.revision_number = revision_number + '\r\n'
        self.configure_serial_port()

    def configure_serial_port(self):
        self.serial_device = serial.Serial()
        self.serial_device.port = str(self.serial_device_port)

        self.serial_device.baudrate = 115200
        self.serial_device.bytesize = serial.EIGHTBITS  # number of bits per bytes
        self.serial_device.parity = serial.PARITY_NONE  # set parity check: no parity
        self.serial_device.stopbits = serial.STOPBITS_ONE  # number of stop bits

        # self.serial_device.timeout = 1

        self.serial_device.xonxoff = False  # disable software flow control
        self.serial_device.rtscts = False  # disable hardware (RTS/CTS) flow control
        self.serial_device.dsrdtr = False  # disable hardware (DSR/DTR) flow control

    def poll_read_line(self):
        if self.serial_device.isOpen():
            while True:
                data = self.serial_device.readline()
                data = data.decode(encoding='ascii', errors='ignore')
                print(':' + data + ':')
                if data != '':
                    break
            return data[:-2]
        else:
            return ''

    def write_port(self, data):
        if self.serial_device.isOpen():
            print(':' + data + ':')
            self.serial_device.write(bytes(data, 'ascii'))

    def stop_test(self):
        self.execute_test = False

    def __del__(self):
        self.wait()

    def run(self):

        try:
            if not self.serial_device.isOpen():
                self.serial_device.open()

        except Exception as e:
            self.alert_handler.emit("error", "Can not open serial port " + str(e))
            self.overall_test_status = 'FAIL'

        else:

            self.overall_test_status = 'PASS'

            try:

                while self.execute_test:
                    line = self.poll_read_line()
                    if line.upper() == 'PRESS SEND EE DATA':
                        break

                if self.serial_device.isOpen():
                    self.write_port(self.part_number)
                    time.sleep(0.5)
                if self.serial_device.isOpen():
                    self.write_port(self.manufacture_date)
                    time.sleep(0.5)
                if self.serial_device.isOpen():
                    self.write_port(self.revision_number)
                    time.sleep(0.5)

                number_of_lines = 0

                response = ''
                while self.execute_test:
                    if self.serial_device.isOpen():
                        response = self.poll_read_line()
                    number_of_lines = number_of_lines + 1
                    print(':' + response + ':')

                    if len(response) > 0:
                        if response[-4:].upper() == 'FAIL':
                            self.alert_handler.emit('error', response)
                            self.overall_test_status = 'FAIL'
                        else:
                            self.alert_handler.emit("success", response)

                        # NOTE:
                        # If the node tester resident software changes as far as last test result
                        # emitted this check needs changed to that test name
                        if 'ANB0 LO' in response:
                            if self.overall_test_status == 'FAIL':
                                self.alert_handler.emit("error", "Test Summary FAIL")
                            else:
                                self.alert_handler.emit("success", "Test Summary PASS")
                            break

            except Exception as e1:
                self.alert_handler.emit("error", "Communication issue: %s" + str(e1))
                self.overall_test_status = 'FAIL'

            if self.serial_device.isOpen():
                self.serial_device.close()
            
        self.test_complete_handler.emit(self.overall_test_status)
