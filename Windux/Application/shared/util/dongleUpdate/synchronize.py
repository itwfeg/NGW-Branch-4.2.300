import os
import requests
import shutil
import time
from PyQt5.QtCore import *
from PyQt5.QtCore import pyqtSignal

import directory_utils
import keyInfo
import md5checksum
import usbQuery


class Worker(QThread):
    progress_handler = pyqtSignal(int)
    show_dialog_handler = pyqtSignal(str, str, int)
    hide_dialog_handler = pyqtSignal(str)

    def __init__(self,
                 remote_download_url,
                 data_location,
                 monitor_frequency=20,
                 dialog_dismiss_timeout=10,
                 parent=None):

        QThread.__init__(self, parent)

        self.changed_file_data = None
        self.remote_download_url = remote_download_url
        self.monitor_frequency = float(monitor_frequency) * 1000.0
        self.data_location = data_location
        self.dialog_dismiss_timeout = float(dialog_dismiss_timeout) * 1000.0

    def check_for_usb_device_and_update(self):
        # self.usbDeviceTimer.stop()

        try:
            usb_device_letter = self.attached_usb_device()
        except ValueError as err:
            if self.changed_file_data:
                self.show_dialog_handler.emit('New feature files have been downloaded, but: ' + str(err),
                                              "show",
                                              self.dialog_dismiss_timeout)
                # self.show_dialog('New feature files have been downloaded, but: ' + str(err))

            # self.usbDeviceTimer.start()
            return

        usb_drive_feature_file_path = os.path.join(usb_device_letter, os.sep, 'features')

        if not os.path.exists(usb_drive_feature_file_path):
            os.makedirs(usb_drive_feature_file_path)

        copy_needed = False

        number_of_files_to_copy = 0

        for (path, dirs, files) in os.walk(self.data_location):

            relative_location = path.replace(self.data_location, '').replace('\\', '/').lstrip('/')

            for filename in files:
                relative_filepath = os.path.join(relative_location, filename).replace('\\', '/').lstrip('/')
                drive_filepath = os.path.join(usb_drive_feature_file_path, relative_filepath)

                if os.path.exists(drive_filepath):
                    if self.changed_file_data and relative_filepath in self.changed_file_data:
                        copy_needed = True
                else:
                    copy_needed = True

                if copy_needed:
                    number_of_files_to_copy += 1

        if copy_needed:
            self.show_dialog_handler.emit('Copying feature %d files to thumb drive, do not eject thumb drive' %
                                          number_of_files_to_copy,
                                          'show',
                                          0)
            # self.show_dialog(message='Copying feature files to thumb drive, do not eject thumb drive',
            #                  show_progress_info=True)

        current_file_number = 0

        for (path, dirs, files) in os.walk(self.data_location):
            relative_location = path.replace(self.data_location, '').replace('\\', '/').lstrip('/')
            drive_location = os.path.join(usb_drive_feature_file_path, relative_location)

            if not os.path.exists(drive_location):
                os.makedirs(drive_location)

            for filename in files:
                local_filepath = os.path.join(path, filename)
                relative_filepath = os.path.join(relative_location, filename).replace('\\', '/').lstrip('/')
                drive_filepath = os.path.join(usb_drive_feature_file_path, relative_filepath)

                if os.path.exists(drive_filepath):
                    if not self.changed_file_data or relative_filepath not in self.changed_file_data:
                        continue

                shutil.copyfile(local_filepath, drive_filepath)
                current_file_number += 1
                percent_complete = int((float(current_file_number) / float(number_of_files_to_copy)) * 100.00)
                self.progress_handler.emit(percent_complete)

        if copy_needed:
            self.show_dialog_handler.emit('Safely eject USB thumb drive from computer',
                                          'hide',
                                          self.dialog_dismiss_timeout)

            # self.show_dialog(message='Safely eject USB thumb drive from computer',
            #                 show_progress_info=False)

        self.changed_file_data = {}

        self.usbDeviceTimer.start()

    def attached_usb_device(self):

        usb_devices = usbQuery.UsbDeviceQuery()

        # Find all removable USB devices connected to computer
        devices = usb_devices.find_devices_in_class(usb_devices.DEVICE_TYPE_USB)
        # print len(devices)

        # do some error message processing based on USB devices we find/do not find
        if len(devices) < 1:
            self.show_dialog_handler.emit('No USB Thumb Drives found to update.<br>Insert Hobart approved thumb drive.',
                                          "hide",
                                          self.dialog_dismiss_timeout)
        elif len(devices) > 1:
            self.show_dialog_handler.emit(
                'More than one removable USB device found.<br><br>Remove all USB removable devices and try again.',
                "hide",
                self.dialog_dismiss_timeout)

        if type(devices) is dict:
            device = devices.keys()[0]  # Posix
        else:
            device = devices[0]  # Windows

        device_letter = usb_devices.get_device_letter(device)

        key, key_filename = keyInfo.get_key_info(keyInfo.service_key_name)

        if not os.path.exists(os.path.join(device_letter, key_filename)):
            self.show_dialog_handler.emit('The USB Thumb drive attached is not a valid service key.',
                                          'hide',
                                          self.dialog_dismiss_timeout)

        return device_letter

    def check_for_remote_server_file_changes(self):
        # self.toggle_timers(False)

        print('running update check')

        try:
            remote_file_data = directory_utils.list_remote_directory(self.remote_download_url)
        except requests.ConnectionError:
            self.show_dialog_handler.emit('No Internet Connection, please connect to the internet',
                                          "hide",
                                          self.dialog_dismiss_timeout)
            # self.toggle_timers(True)
            return

        except requests.HTTPError as err:
            self.show_dialog_handler.emit(str(err))
            self.show_dialog_handler.emit("ERROR: %s" % str(err),
                                          "hide",
                                          self.dialog_dismiss_timeout)
            # self.toggle_timers(True)
            return

        local_file_data = directory_utils.list_local_directory(self.data_location)
        changed_file_data = directory_utils.find_new_files(local_file_data, remote_file_data)

        if changed_file_data:
            self.show_dialog_handler.emit('Downloading new/updated features from product support server',
                                          "show",
                                          0)
            # self.show_dialog(message='Downloading new/updated features from product support server',
            #                 show_progress_info=True)

            self.changed_file_data = changed_file_data

        current_downloaded_file = 0
        number_files_to_download = len(changed_file_data)

        for relative_filepath in changed_file_data:
            url = self.remote_download_url + relative_filepath
            download_filepath = os.path.join(self.data_location, relative_filepath)

            if not os.path.exists(os.path.dirname(download_filepath)):
                os.makedirs(os.path.dirname(download_filepath))

            print('downloading {0} to {1}'.format(url, download_filepath))

            if directory_utils.download_remote_file(url, download_filepath) == 0:
                current_downloaded_file += 1
                percent_complete = int((float(current_downloaded_file) / float(number_files_to_download)) * 100.00)
                self.progress_handler.emit(percent_complete)
            else:
                self.show_dialog_handler.emit("Error downloading the file %s from product support server" %
                                              download_filepath,
                                              "hide",
                                              self.dialog_dismiss_timeout)

        invalid_checksums = []

        for filename in changed_file_data.iterkeys():
            if not filename.endswith('.tgz'):
                continue

            if filename.endswith('.package.tgz'):
                continue

            local_filepath = os.path.join(self.data_location, filename)

            local_checksum = md5checksum.md5_for_file(local_filepath)

            try:
                remote_md5_filepath = os.path.join(self.data_location,
                                                   os.path.splitext(filename)[0] + '.md5')
                with open(remote_md5_filepath, 'r') as remote_md5_file:
                    remote_checksum = remote_md5_file.read().rstrip()
            except:
                # no MD5 file for corresponding TGZ file keep going but alert service tech
                invalid_checksums.append(filename)
                continue

            if local_checksum != remote_checksum:
                invalid_checksums.append(filename)

        if invalid_checksums:
            self.show_dialog_handler.emit("Invalid checksums computed on downloaded files:\n" +
                                          '\n'.join(invalid_checksums),
                                          "hide",
                                          self.dialog_dismiss_timeout)
            # self.show_dialog('\n'.join(invalid_checksums))

        else:
            self.hide_dialog_handler.emit("done")

        # self.toggle_timers(True)

    def __del__(self):
        self.wait()

    def run(self):

        while True:
            self.check_for_remote_server_file_changes()

            self.check_for_usb_device_and_update()

            time.sleep(self.monitor_frequency * 1000)
