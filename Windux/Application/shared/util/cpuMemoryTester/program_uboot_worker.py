import logging
import os
import shutil
import time
import datetime

from PyQt5.QtCore import *
from PyQt5.QtCore import pyqtSignal
from fabric2 import Connection

import cpu_memory_tests


class Worker(QThread):

    alert_handler = pyqtSignal(str)
    program_uboot_status_handler = pyqtSignal(str, str, str, int, float, float, str)

    def __init__(self,
                 filename,
                 devices_to_program,
                 installation_directory,
                 parent=None):

        QThread.__init__(self, parent)

        self.filename = filename
        self.devices_to_program = devices_to_program
        self.installation_directory = installation_directory
        self.connect_timeout = 10

    def get_board_info(self, ip_address):
        pass

    def get_tarball_uboot_version(self):

        temp_directory = os.path.join(self.installation_directory, "temp")
        if os.path.exists(temp_directory):
            shutil.rmtree(temp_directory)

        os.makedirs(name=temp_directory,
                    mode=0o777)

        shutil.unpack_archive(filename=os.path.join(self.installation_directory, self.filename),
                              extract_dir=temp_directory)

        shutil.unpack_archive(
            filename=os.path.join(self.installation_directory, "temp", self.filename.replace(".package.tgz", ".tgz")),
            extract_dir=temp_directory)

        with open(os.path.join(temp_directory, "uboot-version.txt")) as uboot_file:
            uboot_version = uboot_file.readline().strip()
            self.alert_handler.emit(
                "INFO: Uboot version in selected tarball to be used to program board is %s" % uboot_version)
            shutil.rmtree(temp_directory)
            return uboot_version

    def program_uboot(self):

        if self.filename.strip() == '':
            self.alert_handler.emit("ERROR: You must select a uboot file to program")
            return

        uboot_version_in_file = self.get_tarball_uboot_version()
        one_device_was_programmed = False

        for ip_address in self.devices_to_program:

            mac_address = 'Undefined'
            uboot_version = 'Undefined'
            cpu_cores = 'Undefined'
            cpu_temperature = 'Undefined'
            free_ram = 'Undefined'
            todays_date_time = datetime.datetime.today().strftime("%Y%m%d")
            
            with Connection(ip_address, port=22, user='root', connect_kwargs={'password': 'ho1bart2'},
                            connect_timeout=self.connect_timeout) as c:
                try:
                    uboot_version, mac_address, cpu_temperature, free_ram, cpu_cores = \
                        cpu_memory_tests.get_device_details(c)
                    if uboot_version != uboot_version_in_file:
                        one_device_was_programmed = True
                        logging.info("Setting date/time: %s on device: %s" % (todays_date_time, ip_address))
                        self.alert_handler.emit("Attempting to set date/time: %s on device: %s" % (todays_date_time, ip_address))
                        result = c.run('/bin/date +%Y%m%d -s "' + todays_date_time + '"', hide=True)
                        logging.info("Programming U-boot on device: %s" % ip_address)
                        result = c.put(local=self.filename, remote="/tmp", preserve_mode=True)
                        self.alert_handler.emit("INFO: attempting to program u-boot in device with ip address: %s" %
                                                ip_address)
                                                
                        logging.info("programming u-boot in device with ip address: %s" % ip_address)
                        status_code, pid = \
                            self.background_run(c,
                                                "/usr/bin/python /usr/local/hobart/python/installUpgrade.py /tmp/%s" %
                                                self.filename,
                                                log_file=None)
                    else:
                        self.alert_handler.emit(
                            "INFO: Uboot version currently programmed in device: %s is same as version in selected tar file" % ip_address)
                        self.program_uboot_status_handler.emit(ip_address,
                                                               mac_address,
                                                               uboot_version,
                                                               cpu_cores,
                                                               cpu_temperature,
                                                               free_ram,
                                                               'success')
    
                except:
                    c.close()
                    self.alert_handler.emit("ERROR: Attempting to program uboot on device %s failed." % ip_address)
                    logging.error("could not program uboot on device %s" % ip_address)
                    self.program_uboot_status_handler.emit(ip_address,
                                                           mac_address,
                                                           uboot_version,
                                                           cpu_cores,
                                                           cpu_temperature,
                                                           free_ram,
                                                           'error')

        if one_device_was_programmed:

            self.alert_handler.emit("INFO: Waiting one minute for selected devices to program and reboot")

            # sleep for 1 minutes then start checking if boards are up
            time.sleep(1.0 * 60.0)

            check_if_boards_booted = True
            number_attempts_for_all_boards = 0
            while check_if_boards_booted:

                all_boards_in_this_time_slice_booted = True

                for ip_address in self.devices_to_program:

                    with Connection(ip_address, port=22, user='root', connect_kwargs={'password': 'ho1bart2'},
                                    connect_timeout=self.connect_timeout) as c:
                        try:

                            uboot_version, mac_address, cpu_temperature, free_ram, cpu_cores = \
                                cpu_memory_tests.get_device_details(c)

                            if uboot_version == uboot_version_in_file:
                                self.alert_handler.emit("INFO: Successfully programmed device with ip address: %s" %
                                                        ip_address)
                                self.program_uboot_status_handler.emit(ip_address,
                                                                       mac_address,
                                                                       uboot_version,
                                                                       cpu_cores,
                                                                       cpu_temperature,
                                                                       free_ram,
                                                                       'success')

                            else:

                                # self.alert_handler.emit("ERROR: Device with IP address: %s did not program correctly" %
                                #                         ip_address)
                                # self.program_uboot_status_handler.emit(ip_address,
                                #                                        mac_address,
                                #                                        uboot_version,
                                #                                        cpu_cores,
                                #                                        cpu_temperature,
                                #                                        free_ram,
                                #                                        'error')
                                all_boards_in_this_time_slice_booted &= False
                                # break
                        except:
                            c.close()
                            pass

                if all_boards_in_this_time_slice_booted:

                    check_if_boards_booted = False
                    self.alert_handler.emit(
                        "INFO: All devices under test are back on-line to be tested")

                else:

                    self.alert_handler.emit("Waiting 30 seconds before checking if devices are on-line and booted")
                    time.sleep(30.0)
                    number_attempts_for_all_boards += 1
                    if number_attempts_for_all_boards >= 6:
                        check_if_boards_booted = False
                        self.alert_handler.emit("ERROR: All devices have not rebooted in a 4 minute period, "
                                                "check devices for errors.")

        else:
            self.alert_handler.emit("INFO: No devices needed programmed")

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

    def __del__(self):
        self.wait()

    def run(self):

        self.program_uboot()
