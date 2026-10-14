#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# @Filename : cpu_memory_tester
# @Creation Date : 2019-10-25-14-02
# @Project: python
# @Organization: ITW Hobart, Inc.
# @AUTHOR : Scott Davis
# Dependency Packages:
#   pip install fabric
#   pip install scapy

import argparse
import re
import socket
import sys
import time
from signal import signal, SIGINT

import invoke
from fabric2 import Connection

import network_scan


class CpuMemoryTestFacility:

    def __init__(self,
                 host_range=None,
                 minutes_to_run=10,
                 alert_handler_callback=None,
                 progress_handler_callback=None,
                 elapsed_time_handler_callback=None,
                 device_scan_handler_callback=None,
                 test_status_update_handler_callback=None):

        if alert_handler_callback is None:
            signal(SIGINT, self.ctrl_c_handler)

        self.abort_running_test = False

        # hosts = ['10.3.176.110']
        # host_range = '10.3.176.0'
        self.hosts = None
        self.connect_timeout = 10

        self.alert_handler_callback = alert_handler_callback
        self.progress_handler_callback = progress_handler_callback
        self.device_scan_handler_callback = device_scan_handler_callback
        self.test_status_update_handler_callback = test_status_update_handler_callback
        self.elapsed_time_handler_callback = elapsed_time_handler_callback

        interpret_host_range = False

        if self.alert_handler_callback is None:
            self.get_devices_online(host_range)
            interpret_host_range = True
            self.active_hosts = None

        self.minutes_to_run = minutes_to_run
        self.check_status_interval = 10
        self.test_start_time = time.time()
        self.overall_test_status = 'PASS'
        self.get_devices_online(host_range=host_range, interpret_host_range_and_scan=interpret_host_range)

    def get_devices_online(self, host_range, interpret_host_range_and_scan=True):

        valid_ip_address = True
        active_hosts = None

        if interpret_host_range_and_scan:

            host_range_tokens = host_range.split('.')

            if len(host_range_tokens) == 3:
                host_range = args.host_range + '.0'
                if not self.is_valid_ip_address(host_range):
                    valid_ip_address = False
            else:
                host_values = args.host_range.split(',')
                active_hosts = []
                for host_value in host_values:
                    host_value = host_value.strip()
                    host_tokens = host_value.split('.')
                    if len(host_tokens) != 4:
                        self.process_alert('Invalid host_range specified')
                        if self.alert_handler_callback is None:
                            sys.exit(1)
                    else:
                        if not self.is_valid_ip_address(host_value):
                            valid_ip_address = False
                            break
                        else:
                            active_hosts.append(host_value)

            if not valid_ip_address:
                self.process_alert("Invalid host_range argument")
                if self.alert_handler_callback is None:
                    sys.exit(1)

            host_range_tokens = host_range.split(".")
            router_gateway_ip_address = ".".join(host_range_tokens[:3]) + '.1'

            active_hosts, not_active_hosts = network_scan.PingSweep(host_range).ping_sweeper()

        else:

            active_hosts = host_range
            host_range_tokens = host_range[0].split('.')
            router_gateway_ip_address = ".".join(host_range_tokens[:3]) + '.1'

        self.hosts = {}
        host_name, host_ip_address = self.get_host_name_and_ip_address()

        for ip_address in active_hosts:
            if ip_address != host_ip_address and ip_address != router_gateway_ip_address:
                self.hosts[ip_address] = {'uboot_version': None,
                                          'mac_address': None,
                                          'cpu_temperature': 0.0,
                                          'cpu_cores': 0,
                                          'free_ram': 0,
                                          'pids': [],
                                          'status': 'PASS'}

    def get_host_details(self):
        return self.hosts

    def process_alert(self, message):
        if self.alert_handler_callback is None:
            self.process_alert(message)
        else:
            self.alert_handler_callback(message)

    def process_progress(self, percent_complete):
        if self.progress_handler_callback is None:
            self.process_alert("Progress is %d complete" % percent_complete)
        else:
            self.progress_handler_callback(percent_complete)

    def is_valid_ip_address(self, ip_address):
        pattern = r"\b(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\b"
        if re.match(pattern, ip_address):
            return True
        else:
            return False

    def ctrl_c_handler(self, signal_received, frame):
        # Handle any cleanup here
        self.process_alert('CTRL-C detected. Cleaning up test execution and capturing logs')
        self.stop_memtesters()
        # self.get_memtester_logs()
        self.cleanup()
        sys.exit(0)

    def get_host_name_and_ip_address(self):
        try:
            host_name = socket.gethostname()
            host_ip = socket.gethostbyname(host_name)
            return host_name, host_ip
        except:
            self.process_alert("Unable to get Hostname and IP Address")

    def background_run(self, connection, command, log_file):
        status_code = 'error'
        pid = None
        if log_file:
            remote_command = 'nohup %s > %s 2>&1 & echo $!' % (command, log_file)
        else:
            remote_command = 'nohup %s > /dev/null 2>&1 & echo $!' % command

        command_result = connection.run(remote_command, pty=False, hide=True)
        if command_result.ok:
            status_code = 'ok'
            pid = command_result.stdout.strip()
        return status_code, pid

    def get_number_cpu_cores(self, connection):
        try:
            result = connection.run("/usr/bin/grep -c ^processor /proc/cpuinfo", hide=True)
            number_cores = int(result.stdout.strip())
        except:
            self.alert_handler_callback("ERROR: could not get number cpu cores")
            number_cores = 0
        return number_cores

    def get_uboot_version(self, connection):
        try:
            # result = connection.run("/usr/bin/python /usr/local/hobart/python/getUbootUsdVersion.py", hide=True)
            result = connection.run("/usr/sbin/fw_printenv version", hide=True)
            output = result.stdout.strip()
            uboot_version = output.replace("version=", "")

        except:
            self.alert_handler_callback("ERROR: could not get uboot version")
            uboot_version = 'Undefined'
        return uboot_version

    def get_free_ram_mb_size(self, connection):
        try:
            cmd = "cat /proc/meminfo | grep MemFree | awk '{print $2}'"
            result = connection.run("/usr/bin/awk '/MemFree/ {print $2}' /proc/meminfo", hide=True)
            ram_size = int(result.stdout.strip()) / 1024
        except:
            self.alert_handler_callback("ERROR: could not get free ram")
            ram_size = 0
        return ram_size

    def get_cpu_core_temperature(self, connection):
        try:
            result = connection.run("/bin/cat /sys/class/thermal/thermal_zone0/temp", hide=True)
            core_temperature = float(result.stdout.strip()) / 1000.0
        except:
            self.alert_handler_callback("ERROR: could not get cpu core temperature")
            core_temperature = 0.0
        return core_temperature

    def get_mac_address(self, connection, device='eth0'):
        try:
            result = connection.run("/bin/cat /sys/class/net/%s/address" % device, hide=True)
            result = result.stdout.strip()
        except:
            self.alert_handler_callback("ERROR: could not get mac address")
            result = 'Undefined'
        return result

    def remote_command_check_exit_code(self, ip_address, command):

        try:

            c = Connection(ip_address, port=22, user='root', connect_kwargs={'password': 'ho1bart2'},
                           connect_timeout=self.connect_timeout)
            result = c.run(command, hide=True)
            c.close()

        # TODO
        # raises paramiko.ssh_exception.NoValidConnectionsError if run does not connect to remote system
        except invoke.exceptions.UnexpectedExit as e:
            return e.result.return_code

    def remote_run_command(self, ip_address, port, user, password, command, connect_timeout=10):

        with Connection(host=ip_address,
                        port=port,
                        user=user,
                        connect_timeout=connect_timeout,
                        connect_kwargs={'password': password}) as c:
            try:
                c.run(command)
            except invoke.exceptions.CommandTimedOut as e:
                self.alert_handler_callback("ERROR: could not run remote command: %s" % command)
                return e.result.return_code

    def get_elapsed_time(self, start_time):
        delta_time = time.time() - start_time
        elapsed_time = time.strftime("%H:%M:%S", time.gmtime(delta_time))
        return elapsed_time

    def start_memtesters(self):

        # verify all memtesters and subprocesses are not running on all test hosts
        self.stop_memtesters()
        self.alert_handler_callback("Stopping any tests running from previous run")
        self.cleanup(remove_logs=True)

        self.test_start_time = time.time()

        elapsed_time = self.get_elapsed_time(self.test_start_time)
        self.elapsed_time_handler_callback(elapsed_time)

        self.process_alert('Starting CPU and Memory Stress Test')
        self.process_alert("Number of Hosts found to test: %d" % len(self.hosts))

        for ip_address in self.hosts.keys():

            if self.alert_handler_callback is None:
                self.process_alert(40 * '=')
            self.process_alert("Starting test on host: %s" % ip_address)

            with Connection(ip_address, port=22, user='root', connect_kwargs={'password': 'ho1bart2'},
                            connect_timeout=self.connect_timeout) as c:

                try:
                    result = c.put(local="memtester", remote="/root", preserve_mode=True)
                    result = c.run("/bin/chmod ugo+x /root/memtester", hide=True)
                    result = c.put(local="run_command_trigger_event.py", remote="/root", preserve_mode=True)
                    result = c.put(local="imx6_sbc_leds.py", remote="/root", preserve_mode=True)

                    free_ram = self.get_free_ram_mb_size(c)
                    number_of_cores = self.get_number_cpu_cores(c)

                    memory_size_to_test = (free_ram / number_of_cores) - 50
                    # status_code, pid = self.background_run(connection=c,
                    #                                        command='/usr/bin/python /root/imx6_sbc_leds.py',
                    #                                        log_file=None)
                    # self.hosts[ip_address]['pids'].append(pid)

                    uboot_version = self.get_uboot_version(c)

                    cpu_temperature = self.get_cpu_core_temperature(c)

                    mac_address = self.get_mac_address(connection=c)

                    self.hosts[ip_address]['uboot_version'] = uboot_version
                    self.hosts[ip_address]['mac_address'] = mac_address
                    self.hosts[ip_address]['cpu_cores'] = number_of_cores
                    self.hosts[ip_address]['cpu_temperature'] = cpu_temperature
                    self.hosts[ip_address]['free_ram'] = free_ram

                    if self.alert_handler_callback is None:
                        self.process_alert("U-boot Version: %s" % uboot_version)
                        self.process_alert("MAC Address: %s" % mac_address)
                        self.process_alert("CPU Cores: %d" % number_of_cores)
                        self.process_alert("CPU Temperature: %.2f C" % cpu_temperature)
                        self.process_alert("RAM (free): %d MB" % free_ram)

                    status_text = 'PASS'
                    for i in range(0, number_of_cores):
                        status_code, pid = \
                            self.background_run(connection=c,
                                                command='/usr/bin/python /root/run_command_trigger_event.py --log memtest%d.log --blink "/root/memtester %dM" FAILURE' %
                                                        (i, memory_size_to_test),
                                                log_file=None)

                        self.hosts[ip_address]['pids'].append(pid)
                        if status_code != 'ok':
                            status_text = 'FAIL'
                            self.overall_test_status = 'FAIL'

                    self.hosts[ip_address]['status'] = status_text
                except:
                    self.alert_handler_callback("ERROR: could not start memtesters on host")

            elapsed_time = self.get_elapsed_time(self.test_start_time)
            self.elapsed_time_handler_callback(elapsed_time)

            self.test_status_update_handler_callback(self.hosts, self.overall_test_status)

    def stop_memtesters(self):

        for ip_address in self.hosts.keys():

            self.process_alert("Stopping test on host: %s" % ip_address)

            with Connection(ip_address, port=22, user='root', connect_kwargs={'password': 'ho1bart2'},
                            connect_timeout=self.connect_timeout) as c:

                # stop all run_command_trigger_event processes
                for pid in self.hosts[ip_address]['pids']:
                    try:
                        result = c.run("kill -9 %s" % pid, hide=True)
                    except:
                        pass

                # stop run_command_trigger_event memtester spawns
                try:
                    result = c.run("pkill memtester", hide=True)
                    result = c.run("/usr/bin/python /root/imx6_sbc_leds.py all off", hide=True)
                    # # stop the blink test leds
                    # result = c.run("pkill -9 -f imx6_sbc_leds.py", hide=True)
                except:
                    pass

            elapsed_time = self.get_elapsed_time(self.test_start_time)
            self.elapsed_time_handler_callback(elapsed_time)

            self.alert_handler_callback("All tests stopped on host: %s" % ip_address)

    def abort_running_tests(self):
        self.alert_handler_callback("Aborting running tests on all hosts")
        elapsed_time = self.get_elapsed_time(self.test_start_time)
        self.elapsed_time_handler_callback(elapsed_time)
        self.abort_running_test = True

    def check_status_of_memtesters(self):

        while time.time() < (self.test_start_time + self.minutes_to_run * 60) and not self.abort_running_test:

            time.sleep(self.check_status_interval)

            for ip_address in self.hosts.keys():

                if self.alert_handler_callback is None:
                    self.process_alert(40 * '=')
                self.process_alert("Checking test status of host: %s" % ip_address)

                with Connection(ip_address, port=22, user='root', connect_kwargs={'password': 'ho1bart2'},
                                connect_timeout=self.connect_timeout) as c:

                    try:
                        cpu_temperature = self.get_cpu_core_temperature(c)

                        if cpu_temperature > self.hosts[ip_address]['cpu_temperature']:
                            self.hosts[ip_address]['cpu_temperature'] = cpu_temperature

                        if self.alert_handler_callback is None:
                            self.process_alert("CPU Temperature: %.2f C" % cpu_temperature)

                        no_errors = True

                        for pid in self.hosts[ip_address]['pids']:

                            try:
                                result = c.run("kill -0 %s; echo $?" % pid, hide=True)

                                if result.stdout.strip() != '0':
                                    self.process_alert("Test failed on host: %s" % ip_address)
                                    no_errors = False
                                else:
                                    pass
                            except:
                                pass

                        if no_errors:
                            self.hosts[ip_address]['status'] = 'PASS'
                            self.process_alert("Test executed without errors on host: %s" % ip_address)
                        else:
                            self.hosts[ip_address]['status'] = 'FAIL'
                            self.process_alert("Test executed with errors on host: %s" % ip_address)
                    except:
                        self.alert_handler_callback("ERROR: filed to get status of host %s" % ip_address)


                elapsed_time = self.get_elapsed_time(self.test_start_time)
                self.elapsed_time_handler_callback(elapsed_time)

        if self.alert_handler_callback is None:
            self.process_alert(40 * '=')
            self.process_alert("CPU and Memory Test Results")
            self.process_alert("Number of hosts tested: %d" % len(self.hosts.keys()))
            self.process_alert("Host Test Info:")

        for ip_address in self.hosts.keys():
            if self.hosts[ip_address]['status'] == 'PASS':
                test_status = "PASS"
            else:
                self.overall_test_status = 'FAIL'
                test_status = 'FAIL'
            if self.alert_handler_callback is None:
                self.process_alert("""IP Address: %s
    U-boot Version: %s
    MAC Address: %s 
    CPU Cores: %d
    CPU Temperature (Max): %.2f C 
    RAM (free): %d MB
    Test Result: %s""" %
                      (ip_address,
                       self.hosts[ip_address]['uboot_version'],
                       self.hosts[ip_address]['mac_address'],
                       self.hosts[ip_address]['cpu_cores'],
                       self.hosts[ip_address]['cpu_temperature'],
                       self.hosts[ip_address]['free_ram'],
                       test_status))

        if self.test_status_update_handler_callback is not None:
            self.test_status_update_handler_callback(self.hosts, self.overall_test_status)

        elapsed_time = self.get_elapsed_time(self.test_start_time)
        self.elapsed_time_handler_callback(elapsed_time)

        if self.alert_handler_callback is None:
            self.process_alert("Execution Time: %d minutes" % self.minutes_to_run)

    def get_memtester_logs(self):
        for ip_address in self.hosts.keys():
            self.process_alert("Retrieving test log(s) from host: %s" % ip_address)
            with Connection(ip_address, port=22, user='root', connect_kwargs={'password': 'ho1bart2'},
                            connect_timeout=self.connect_timeout) as c:
                try:
                    for i in range(0, self.get_number_cpu_cores(c)):
                        result = c.get(remote="/root/memtest%d.log" % i, local="host_%s_memtest%d.log" % (ip_address, i))
                except:
                    self.alert_handler_callback("ERROR: retrieving memtester logs failed")

            elapsed_time = self.get_elapsed_time(self.test_start_time)
            self.elapsed_time_handler_callback(elapsed_time)

    def cleanup(self, remove_logs=False):

        for ip_address in self.hosts.keys():
            self.process_alert("Performing cleanup on host: %s" % ip_address)
            # check that eth0 is setup and not eth1-N
            with Connection(ip_address, port=22, user='root', connect_kwargs={'password': 'ho1bart2'},
                            connect_timeout=self.connect_timeout) as c:
                try:
                    # always remove eth0 persisten rules so the uSD card can be removed from board and put into
                    # another one and network can be brought up as eth0
                    result = c.run('/bin/rm -f /etc/udev/rules.d/70-persistent-net.rules', hide=True)
                    if remove_logs:
                        result = c.run("/bin/rm -f /root/memtest*.log")
                except:
                    self.alert_handler_callback("ERROR: cleanup failed to process correctly")

            elapsed_time = self.get_elapsed_time(self.test_start_time)
            self.elapsed_time_handler_callback(elapsed_time)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Run CPU & Memory Test on specified hosts")
    parser.add_argument("minutes_to_run", type=int, help='time in minutes to run test')
    parser.add_argument("host_range", type=str,
                        help="IP address wildcard range (EX: 192.168.1) or "
                             "quoted argument of IP addresses delimited by comma")
    args = parser.parse_args()

    tester = CpuMemoryTestFacility(args.host_range, args.minutes_to_run)
    tester.start_memtesters()
    tester.check_status_of_memtesters()
    tester.stop_memtesters()
    tester.get_memtester_logs()
    tester.cleanup()
