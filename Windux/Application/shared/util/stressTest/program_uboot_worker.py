import logging
import time
import tarfile, os
import sys
import shutil

from PyQt5.QtCore import *
from PyQt5.QtCore import pyqtSignal
from fabric2 import Connection


class Worker(QThread):

    alert_handler = pyqtSignal(str)

    def __init__(self,
                 filename,
                 devices_to_program,
                 installation_directory,
                 parent=None):

        QThread.__init__(self, parent)

        self.filename = filename
        self.devices_to_program = devices_to_program
        self.installation_directory = installation_directory

    def get_tarball_uboot_version(self):

        temp_directory = os.path.join(self.installation_directory, "temp")
        if os.path.exists(temp_directory):
            shutil.rmtree(temp_directory)

        os.makedirs(name=temp_directory,
                    mode=0o777)

        shutil.unpack_archive(filename=os.path.join(self.installation_directory, self.filename),
                              extract_dir=temp_directory)

        shutil.unpack_archive(filename=os.path.join(self.installation_directory, "temp", self.filename.replace(".package.tgz", ".tgz")),
                              extract_dir=temp_directory)

        with open(os.path.join(temp_directory, "uboot-version.txt")) as uboot_file:
            uboot_version = uboot_file.readline().strip()
            self.alert_handler.emit("INFO: Uboot version in selected tarball to be used to program board is %s" % uboot_version)
            shutil.rmtree(temp_directory)
            return uboot_version

    def program_uboot(self):

        if self.filename.strip() == '':
            self.alert_handler.emit("ERROR: You must select a uboot file to program")
            return

        uboot_version_in_file = self.get_tarball_uboot_version()
        one_device_was_programmed = False

        for ip_address in self.devices_to_program:

            with Connection(ip_address, port=22, user='root', connect_kwargs={'password': 'ho1bart2'},
                            connect_timeout=10) as c:
                try:
                    installed_uboot_version_in_device = self.get_uboot_version_in_device(c)
                    if installed_uboot_version_in_device != uboot_version_in_file:
                        one_device_was_programmed = True
                        self.alert_handler.emit("Programming U-boot on device: %s" % ip_address)
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
                        self.alert_handler.emit("INFO: Uboot version currently programmed in device: %s is same as version in selected tar file" % ip_address)

                except:
                    self.alert_handler.emit("ERROR: could not program uboot on device %s" % ip_address)
                    logging.error("could not program uboot on device %s" % ip_address)

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
                                    connect_timeout=10) as c:
                        try:
                            installed_uboot_version_in_device = self.get_uboot_version_in_device(c)
                            if installed_uboot_version_in_device == uboot_version_in_file:
                                self.alert_handler.emit("INFO: Successfully programmed device with ip address: %s" %
                                                        ip_address)
                            else:
                                self.alert_handler.emit("ERROR: Device with IP address: %s did not program correctly" % 
                                ip_address)
                                all_boards_in_this_time_slice_booted &= False
                                break
                        except:
                            pass

                if all_boards_in_this_time_slice_booted:
                    check_if_boards_booted = False
                    self.alert_handler.emit(
                        "INFO: All devices under test are back on-line to be tested")
                else:
                    time.sleep(30.0)
                    self.alert_handler.emit("Waiting 30 seconds beforfe checking if devices are on-line and booted")
                    number_attempts_for_all_boards += 1
                    if number_attempts_for_all_boards >= 6:
                        check_if_boards_booted = False
                        self.alert_handler.emit("ERROR: Devices have not rebooted in a 4 minute period, "
                                                "check devices for errors.")
        else:
            self.alert_handler.emit("INFO: No devices needed programmed")

    def get_uboot_version_in_device(self, connection):
        try:
            # result = connection.run("/usr/bin/python /usr/local/hobart/python/getUbootUsdVersion.py", hide=True)
            result = connection.run("/usr/sbin/fw_printenv version", hide=True)
            output = result.stdout.strip()
            uboot_version = output.replace("version=", "")
        except:
            self.alert_handler.emit("ERROR: could not get uboot version")
            logging.error("ERROR: could not get uboot version")
            uboot_version = 'Undefined'
        return uboot_version

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
