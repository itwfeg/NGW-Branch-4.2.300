
from PyQt5.QtCore import *
from PyQt5.QtCore import pyqtSignal
import os
import socket
import select


class Worker(QThread):

    alert_handler = pyqtSignal(str)

    def __init__(self, parent=None):

        QThread.__init__(self, parent)

        self.stop_test = False

    def monitor_test(self):

        server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_address = ('localhost', 5079)
        server.bind(server_address)
        server.listen(5)
        inputs = [server]
        outputs = []

        timeout = 20.0 * 60.0

        while not self.stop_test:

            readable, writable, exceptional = select.select(inputs, outputs, inputs, timeout)

            # check if timeout reached
            if readable == [] and writable == [] and exceptional == []:
                break
            else:
                # received message from chobart_d
                for s in readable:

                    # socket message coming in to process
                    if s is server:

                        # accept network connection
                        connection, client_address = s.accept()
                        data = connection.recv(1024)
                        connection.close()

                        if data == 'lvdsComplete':
                            self.alert_handler.emit("Ready")

    def __del__(self):
        self.wait()

    def stop_test(self):
        self.stop_test = True

    def run(self):
        self.monitor_test()
