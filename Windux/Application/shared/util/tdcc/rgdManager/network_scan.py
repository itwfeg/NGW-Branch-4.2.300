#! /usr/bin/env python3

from PyQt5.QtCore import *
from PyQt5 import QtCore
import ipaddress

from logging import getLogger, ERROR

import socket
import time
import qt_worker
import subprocess
import sys
import host_ip_address
import select
import network_command
from fabric2 import Connection, exceptions
from patchwork.files import exists
import invoke


class RgdProbeNetwork(QtCore.QObject):

    progress = pyqtSignal(int)
    alerts_handler = pyqtSignal(str)

    def __init__(self,
                 scan_handler,
                 ping_count=1,
                 ping_timeout_in_milliseconds=300):

        super(RgdProbeNetwork, self).__init__()
        self.ip_address_list = self.get_default_network_ip_address_range()
        self.micro_sm_backend_port = 5076
        self.ping_timeout_milliseconds = ping_timeout_in_milliseconds
        self.max_message_size = 1024
        self.ping_count = ping_count
        self.connection_timeout = 10.0
        self.scan_handler = scan_handler
        self.thread_pool = QThreadPool()
        self.scan_in_progress = False
        self.nodes_scanned = 0

    def get_network_interface(self):
        address = host_ip_address.get_local_addr(remote=None, ipv6=False)
        return address

    def remote_device_is_black_box(self, ip_address):
        result = False
        self.nodes_scanned += 1
        try:
            with Connection(host=ip_address,
                            port=22,
                            user='root',
                            connect_timeout=1,
                            connect_kwargs={'password': 'ho1bart2'}) as c:

                # black box device if any of these files exists on remote system
                if exists(c, "/etc/hobart/coronaPsu") or exists(c, "/etc/hobart/blackbox") or \
                        exists(c, "/etc/hobart/testsrv") or exists(c, "/etc/hobart/boardtester"):
                    result = True
                else:
                    result = False
        except:
            pass
        return result

    def is_host_black_box(self, ip_address):
        if self.scan_in_progress:
            active_ip_address = None

            if self.remote_device_is_black_box(ip_address):
                print(ip_address)
                active_ip_address = ip_address

            progress = int((float(self.nodes_scanned) / float(len(self.ip_address_list))) * 100.0)
            self.progress.emit(progress)
            return active_ip_address
        else:
            self.progress.emit(0)

    def pinger(self, ip_address):
        if self.scan_in_progress:
            ping_command = "ping -n -c %d -W %d %s 2> /dev/null"
            online_response = "bytes from"
            if 'darwin' in sys.platform:
                pass
            elif 'linux' in sys.platform:
                self.ping_timeout_milliseconds = self.ping_timeout_milliseconds / 1000  # seconds in linux
            elif 'win' in sys.platform:
                ping_command = "ping -n %d -w %d %s"
                online_response = "Reply from"

            line = subprocess.getoutput(ping_command %
                                        (self.ping_count, self.ping_timeout_milliseconds, ip_address))

            active_ip_address = None
            if line.find(ip_address) and line.find(online_response) > -1:  # Host Active
                active_ip_address = ip_address

            tokens = ip_address.split('.')
            progress = (float(int(tokens[3])) / float(len(self.ip_address_list))) * 100.0
            self.progress.emit(progress)
            return active_ip_address
        else:
            self.progress.emit(100)

    def get_default_network_ip_address_range(self):
        tokens = self.get_network_interface().split('.')
        network_address_mask = '.'.join(tokens[:-1]) + '.0/24'
        ip_net = ipaddress.ip_network(network_address_mask)
        return list(ip_net.hosts())

    def scan(self, ip_address_list=None):
        start_time = time.time()
        self.nodes_scanned = 0
        if self.ip_address_list is not None:
            all_hosts = ip_address_list
            self.ip_address_list = ip_address_list
        else:
            all_hosts = self.ip_address_list
        self.scan_in_progress = True
        for device_ip_address in all_hosts:
            if self.scan_in_progress:
                worker = qt_worker.Worker(self.is_host_black_box, str(device_ip_address))
                worker.signals.result.connect(self.scan_handler)
                self.thread_pool.start(worker)

    def stop(self):
        self.scan_in_progress = False
        self.thread_pool.clear()
        self.thread_pool.waitForDone()
        self.progress.emit(100)


if __name__ == '__main__':
    pass
    # getInfo command requires a server side listening on 5076
    # since it closes the client connection right off the bat
    # ip_address = '10.3.177.9'
    # command = 'getInfo|\n'
    #
    # server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    # port = 5076
    # server_address = ('', port)
    # server.bind(server_address)
    # server.listen(20)
    #
    # client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # client.settimeout(5.0)
    # response = None
    #
    # # try:
    # print(ip_address)
    # client_address = (ip_address, port)
    # client.connect(client_address)
    # client.sendall(command.encode())
    #
    # # accept network connection
    # connection, client_address = server.accept()
    #
    # # process connection
    # response = connection.recv(4096)
    # print(response.decode())
