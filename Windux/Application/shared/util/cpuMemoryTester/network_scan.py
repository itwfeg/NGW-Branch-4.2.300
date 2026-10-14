# from __future__ import print_function
import ipaddress
import shlex
import struct
import subprocess
import sys
import time
from ipaddress import ip_address
from logging import getLogger, ERROR
from multiprocessing import Process, Queue
from socket import inet_aton
from sys import exit

from PyQt5 import QtCore
from PyQt5.QtCore import *
from fabric2 import Connection
from patchwork.files import exists
from scapy.all import get_if_addr, conf

import cpu_memory_tests
import host_ip_address
import network_scan_worker

getLogger("scapy.runtime").setLevel(ERROR)


# from commands import getoutput

def generate_ips_in_range(start, end):
    start = ip_address(start)
    end = ip_address(end)
    result = []
    while start <= end:
        result.append(str(start))
        start += 1
    return result


class ProbeNetwork(QtCore.QObject):
    progress = pyqtSignal(int)
    alerts_handler = pyqtSignal(str)

    def __init__(self,
                 scan_handler,
                 ping_count=1,
                 ping_timeout_in_milliseconds=300,
                 subnet='192.168.1.0'):

        super(ProbeNetwork, self).__init__()
        self.ip_address_list = self.get_default_network_ip_address_range(subnet=subnet)
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

    def remote_device_is_hobart_device(self, ip_address):

        self.nodes_scanned += 1
        mac_address = uboot_version = 'Undefined'
        cpu_cores = 0
        cpu_temperature = free_ram = 0.0

        is_hobart_device = False
        status = 'PASS'
        try:
            with Connection(host=ip_address,
                            port=22,
                            user='root',
                            connect_timeout=1,
                            connect_kwargs={'password': 'ho1bart2'}) as c:

                # black box device if any of these files exists on remote system
                if exists(c, "/usr/local/hobart"):
                    is_hobart_device = True

                    uboot_version, mac_address, cpu_temperature, free_ram, cpu_cores = \
                        cpu_memory_tests.get_device_details(c)

                    status = 'PASS'

        except:
            c.close()

        return is_hobart_device, ip_address, mac_address, uboot_version, cpu_cores, cpu_temperature, free_ram, status

    def is_host_hobart_device(self, ip_address):
        if self.scan_in_progress:
            active_ip_address = None

            (is_hobart_device,
             ip_address,
             mac_address,
             uboot_version,
             cpu_cores,
             cpu_temperature,
             free_ram,
             status) = self.remote_device_is_hobart_device(ip_address)

            if is_hobart_device:
                print(ip_address)
                active_ip_address = ip_address

            progress = int((float(self.nodes_scanned) / float(len(self.ip_address_list))) * 100.0)
            self.progress.emit(progress)
            return is_hobart_device, ip_address, mac_address, uboot_version, cpu_cores, cpu_temperature, free_ram, status

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

    def get_default_network_ip_address_range(self, subnet='192.168.1.0'):
        network_address_mask = subnet + '/24'
        try:
            tokens = self.get_network_interface().split('.')
            if tokens and len(tokens) == 4:
                network_address_mask = '.'.join(tokens[:-1]) + '.0/24'
        except:
            pass

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
                worker = network_scan_worker.Worker(self.is_host_hobart_device, str(device_ip_address))
                worker.signals.result.connect(self.scan_handler)
                self.thread_pool.start(worker)

    def stop(self):
        self.scan_in_progress = False
        self.thread_pool.clear()
        self.thread_pool.waitForDone()
        self.progress.emit(100)


class PingSweep(object):

    def __init__(self, host_range=None, ping_count=1, ping_timeout_in_seconds=5):
        self.ping_count = ping_count
        self.ping_timeout_in_seconds = ping_timeout_in_seconds
        self.host_range = host_range
        self.is_active = Queue(255)
        self.not_active = Queue(255)

    def pinger(self, host_num):
        """thread pinger function"""
        if self.host_range is None:
            hostadrr = get_if_addr(conf.iface).split('.')[:-1]
        else:
            hostadrr = self.host_range.split('.')[:-1]

        hostadrr = '.'.join(hostadrr) + '.' + repr(host_num)
        command = "/bin/ping -n -c %d -W %d %s 2> /dev/null" % \
                  (self.ping_count, self.ping_timeout_in_seconds, hostadrr)
        process = subprocess.Popen(shlex.split(command), stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        output_lines = []
        while True:
            output = process.stdout.readline()
            if output == '' and process.poll() is not None:
                break
            if output:
                output_lines.append(output.strip())

        rc = process.poll()

        # line = getoutput("ping -n -c %d -W %d %s 2> /dev/null" %
        #                 (self.ping_count, self.ping_timeout_in_seconds, hostadrr))

        for line in output_lines:
            if line.find(hostadrr) and line.find("bytes from") > -1:  # Host Active
                self.is_active.put(hostadrr)
                # alive_host = self.is_active.pop()
                print("Host %s is \033[92m Active \033[0m" % hostadrr)
                break
            elif line.find(hostadrr) and \
                    (line.find("Unreachable") > -1 or
                     line.find("100% packet loss") > -1 or
                     line.find("100.0% packet loss") > -1):  # No response from host
                self.not_active.put(hostadrr)
                print("\033[91m No response from \033[0m", hostadrr)
                break
            else:
                exit(0)

    def ping_sweeper(self):
        ping_processes = []
        is_active = []
        not_active = []
        for host_num in range(1, 255):
            p = Process(target=self.pinger, args=(host_num,))
            ping_processes.append(p)
            p.start()

        for ping_process in ping_processes:
            ping_process.join()

        while not self.is_active.empty():
            is_active.append(self.is_active.get())

        while not self.not_active.empty():
            not_active.append(self.not_active.get())

        is_active = sorted(is_active, key=lambda ip: struct.unpack("!L", inet_aton(ip))[0])
        not_active = sorted(not_active, key=lambda ip: struct.unpack("!L", inet_aton(ip))[0])

        return is_active, not_active


if __name__ == '__main__':
    try:
        active_hosts, not_active_hosts = PingSweep('192.168.1.0').ping_sweeper()
        print(active_hosts)
        print(not_active_hosts)
    except KeyboardInterrupt:
        pass
