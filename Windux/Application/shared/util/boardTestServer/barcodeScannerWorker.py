from PyQt5.QtCore import *
from PyQt5.QtCore import pyqtSignal
import serial


class SerialPortScanner(QThread):

    data_scanned_handler = pyqtSignal(str)
    alert_handler = pyqtSignal(str, str)

    def __init__(self, port_name='/dev/ttyACM0', parent=None):

        QThread.__init__(self, parent)
        self.port_name = port_name
        self.serial_device = serial.Serial(baudrate=9600,
                                           bytesize=serial.EIGHTBITS,
                                           parity=serial.PARITY_NONE,
                                           stopbits=serial.STOPBITS_ONE,
                                           timeout=None,
                                           xonxoff=False,
                                           rtscts=False,
                                           dsrdtr=False)

        self.serial_device.port = port_name
        self.serial_port_open = False
        self.shutdown = False

    def open(self):
        try:
            print("device is open")
            self.serial_device.open()
            self.serial_device.flushInput()
            self.serial_device.flushOutput()
            self.serial_port_open = True
            return 'ok'
        except:
            self.alert_handler.emit('info', "Can not open barcode scanner device. Check if device is connected.")

    def device_is_open(self):
        return self.serial_device.isOpen()

    def read_message(self):
        status_code = 'error'
        message = ''

        if self.serial_device.isOpen():
            try:
                while True:
                    # print("here")
                    byte_value = self.serial_device.read(size=1)
                    # print(byte_value)
                    if byte_value.decode("ascii") == '\r':
                        # read barcode since barcode scanner has been setup to
                        # send a barcode when carriage return seen
                        break
                    else:
                        message += byte_value.decode("ascii")

                # print message
                status_code = 'ok'

            except Exception as e:
                status_code = 'error'
                self.alert_handler.emit('error', "Can not read barcode scanner")
        else:
            message = None
            status_code = 'error'

        return status_code, message

    def scan_barcode(self):

        status_code, data = self.read_message()

        self.data_scanned_handler.emit(data)

    def close(self):
        self.serial_device.close()

    def __del__(self):
        self.shutdown = True
        self.wait()

    # run method gets called when we start the thread
    def run(self):

        self.open()

        while not self.shutdown:

            if self.device_is_open():

                self.scan_barcode()

        self.close()
