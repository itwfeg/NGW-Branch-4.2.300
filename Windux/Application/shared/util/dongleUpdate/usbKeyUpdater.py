try:
    import environment_manager

    environment_manager.setup()
except:
    pass

import filecmp
import os
import shutil
import sys

import psutil
from PyQt5 import QtCore
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *

import directory_utils
import file_io
import keyInfo
import md5checksum
import usbQuery
from Ui_usbKeyUpdater import Ui_usbKeyUpdaterDialog

VERSION = "1.4.0"

MD5_CHECKSUM_FILESIZE = 33

# try:
#     _fromUtf8 = QtCore.QString.fromUtf8
# except AttributeError:
#     def _fromUtf8(s):
#         return s


class UsbKeyUpdater(QDialog):

    def __init__(self, parent=None):

        QDialog.__init__(self, parent)

        # create USK Key Creator Dialog
        self.ui = Ui_usbKeyUpdaterDialog()
        self.ui.setupUi(self)

        self.show()

        desktop_bottom_right = QApplication.desktop().availableGeometry().bottomRight()
        self.move(desktop_bottom_right.x() - self.frameGeometry().width(), desktop_bottom_right.y() -
                  self.frameGeometry().height())

        self.hide()

        self.setWindowTitle("Weigh/Wrap Technician Notice: (HTe USB Key Updater Version: %s)" % (VERSION))

        self.settings = QtCore.QSettings("ITW Hobart", "USB Key Updater")
        self.licenseFilename = self.settings.value("licenseFilename").toString()
        self.lastThumbDriveUpdate = self.settings.value("lastThumbDriveUpdate",
                                                        QVariant(QDateTime.currentDateTime())).toDateTime()
        self.lastCacheUpdate = self.settings.value("lastCacheUpdate",
                                                   QVariant(QDateTime.currentDateTime())).toDateTime()

        self.changed_file_data = {}

        self.updateTimer = QtCore.QTimer()
        self.updateTimer.setInterval(1000 * 60 * 60 * 8)  # check PSS for new or changed files every 8 hours
        self.updateTimer.timeout.connect(self.check_for_updates)
        self.updateTimer.start()

        self.usbDeviceTimer = QtCore.QTimer()
        self.usbDeviceTimer.setInterval(10000)
        self.usbDeviceTimer.timeout.connect(self.check_for_usb)
        self.usbDeviceTimer.start()

        self.attachUSBTimer = QtCore.QTimer()
        self.attachUSBTimer.setInterval(8000)
        self.attachUSBTimer.timeout.connect(self.is_usb_attached)
        self.attachUSBTimer.start()

        self.newDayTimer = QtCore.QTimer()
        today = QDateTime.currentDateTime()
        tommorow = today.addDays(1)
        self.newDayTimer.setInterval(today.msecsTo(tommorow))
        self.newDayTimer.setInterval(today.msecsTo(tommorow))
        # self.newDayTimer.setInterval(30000)
        self.newDayTimer.timeout.connect(self.check_for_announce_files_changed)
        self.newDayTimer.start()

        self.fiveDayTimer = QtCore.QTimer()
        self.lastUpdate = self.lastThumbDriveUpdate
        self.fiveDays = self.lastUpdate.addDays(5)
        self.fiveDayTimer.setInterval(self.lastUpdate.msecsTo(self.fiveDays))
        self.fiveDayTimer.timeout.connect(self.update_reminder)
        self.fiveDayTimer.start()

        self.prompt_timer = QtCore.QTimer()
        self.current = QDateTime.currentDateTime()
        self.prompt_tomorrow = self.current.addDays(1)
        self.prompt_timer.setInterval(self.current.msecsTo(self.prompt_tomorrow))
        self.prompt_timer.timeout.connect(self.sync_reminder)

        self.dataLocation = str('C:\\ProgramData\\Hobart\\HTeUsbKeyUpdater')
        self.remoteDownloadUrl = 'https://wwengrel1.itwweighwrap.com/downloads/'

        if not os.path.exists(self.dataLocation):
            os.makedirs(self.dataLocation)

    def set_thumb_drive_last_update(self, device):
        now = QDateTime.currentDateTime()
        now_string = str(now.toString("MM/dd/yyyy HH:mm:ss"))
        status, data = file_io.write_to_file(os.path.join(device, os.sep, "lastUpdate.dat"), [now_string])
        return status

    def get_thumb_drive_last_update(self, device):
        status, lines = file_io.read_from_file(os.path.join(device, os.sep, "lastUpdate.dat"))
        if status == 'success':
            last_thumb_update = QDateTime.fromString(lines[0], "MM/dd/yyyy HH:mm:ss")
        else:
            last_thumb_update = None
        return last_thumb_update

    def update_reminder(self):
        self.fiveDayTimer.stop()
        QMessageBox.information(
            self,
            self.tr("Weigh/Wrap Technician Notice: (HTe USB Key Updater)"),
            self.tr("USB Key has not been synced in 5 days. Please insert USB key"),
            QMessageBox.StandardButtons(
                QMessageBox.Ok),
            QMessageBox.Ok)
        self.lastUpdate = QDateTime.currentDateTime()
        self.fiveDays = self.lastUpdate.addDays(5)
        self.fiveDayTimer.setInterval(self.lastUpdate.msecsTo(self.fiveDays))
        self.fiveDayTimer.start()

    def sync_reminder(self):
        if self.prompt_timer.isActive():
            self.prompt_timer.stop()
        QMessageBox.information(
            self,
            self.tr("Weigh/Wrap Technician Notice: (HTe USB Key Updater)"),
            self.tr("USB Key was not synced yesterday. Insert key to update"),
            QMessageBox.StandardButtons(
                QMessageBox.Ok),
            QMessageBox.Ok)
        self.current = QDateTime.currentDateTime()
        self.prompt_tomorrow = self.current.addDays(1)
        self.prompt_timer.setInterval(self.current.msecsTo(self.prompt_tomorrow))
        self.prompt_timer.start()

    def check_for_announce_files_changed(self):
        try:
            self.attached_usb_device()
        except ValueError:
            pass
        else:
            return

        cache_update = self.settings.value("lastCacheUpdate").toDateTime()
        thumb_update = self.settings.value('lastThumbDriveUpdate').toDateTime()

        if self.changed_file_data or (cache_update > thumb_update):
            QMessageBox.information(
                self,
                self.tr("Weigh/Wrap Technician Notice: (HTe USB Key Updater Version: %s)" % VERSION),
                self.tr("""Customer Feature files have been added, deleted, or modified.
Insert thumb drive to sync changes."""),
                QMessageBox.StandardButtons(
                    QMessageBox.Ok),
                QMessageBox.Ok)

    def file_will_fit_on_device(self, device, size_of_file):
        free_space = psutil.disk_usage(device)[2]
        if size_of_file <= free_space:
            return True
        else:
            return False

    def show_dialog(self, message, timeout=10):
        self.ui.instructionsLabel.setText(message)
        self.show()
        self.activateWindow()

        if timeout:
            QtCore.QTimer.singleShot(timeout * 10000, self.hide)

        QtCore.QCoreApplication.processEvents()

    def toggle_timers(self, enabled):
        if enabled:
            if not self.usbDeviceTimer.isActive():
                self.usbDeviceTimer.start()

            if not self.updateTimer.isActive():
                self.updateTimer.start()

        else:
            if self.usbDeviceTimer.isActive():
                self.usbDeviceTimer.stop()

            if self.updateTimer.isActive():
                self.updateTimer.stop()

    def check_for_usb(self):
        self.usbDeviceTimer.stop()

        try:
            usb_device_letter = self.attached_usb_device()
        except ValueError as error:
            self.usbDeviceTimer.start()
            return
        else:

            self.fiveDayTimer.stop()
            self.lastUpdate = self.lastThumbDriveUpdate
            self.fiveDays = self.lastUpdate.addDays(5)
            self.fiveDayTimer.setInterval(self.lastUpdate.msecsTo(self.fiveDays))
            self.fiveDayTimer.start()

            cache_update = self.settings.value("lastCacheUpdate").toDateTime()
            thumb_update = self.get_thumb_drive_last_update(usb_device_letter)

            # thumbUpdate = self.settings.value('lastThumbDriveUpdate').toDateTime()
            copy_needed = self.thumb_drive_needs_updated(usb_device_letter)

            if not copy_needed:
                if thumb_update is None:
                    copy_needed = True
                else:
                    if cache_update > thumb_update:
                        copy_needed = True

            if self.changed_file_data or copy_needed:
                res = QMessageBox.question(
                    self,
                    self.tr("Weigh/Wrap Technician Notice: (HTe USB Key Updater Version: %s)" % (VERSION)),
                    self.tr("""New or modified Customer Feature files need synced with Hobart Service Key.

Do you want to synchronize these files? """),
                    QMessageBox.StandardButtons(
                        QMessageBox.No |
                        QMessageBox.Yes),
                    QMessageBox.Yes)
                if res == QMessageBox.Yes:
                    self.copy_cache_to_thumb_drive(usb_device_letter)
                    self.changed_file_data = {}
                    self.set_thumb_drive_last_update(usb_device_letter)
                    self.settings.setValue("lastThumbDriveUpdate", QVariant(QDateTime.currentDateTime()))

                    if self.prompt_timer.isActive():
                        self.prompt_timer.stop()
                else:
                    if self.prompt_timer.isActive():
                        self.prompt_timer.stop()
                    self.current = QDateTime.currentDateTime()
                    self.prompt_tomorrow = self.current.addDays(1)
                    self.prompt_timer.setInterval(self.current.msecsTo(self.prompt_tomorrow))
                    self.prompt_timer.start()
                    return

        self.usbDeviceTimer.start()

    def thumb_drive_needs_updated(self, usbDeviceLetter):
        drive_feature_filepath = os.path.join(usbDeviceLetter, os.sep, 'HT_CUST_UPDATER')
        if not os.path.exists(drive_feature_filepath):
            os.makedirs(drive_feature_filepath)
        dir1 = self.dataLocation
        dir2 = drive_feature_filepath
        dif = filecmp.dircmp(dir1, dir2)
        copy_needed = self.file_differences(dif)
        print(copy_needed)
        # print(dir1)
        return copy_needed

        # copy_needed = False

        # for (path, dirs, files) in os.walk(self.dataLocation):
        #     relative_location = path.replace(self.dataLocation, '').replace('\\',  '/').lstrip('/')

        #     for filename in files:
        #         relative_filepath = os.path.join(relative_location, filename).replace('\\', '/').lstrip('/')
        #         drive_filepath = os.path.join(drive_feature_filepath, relative_filepath)

        #         if os.path.exists(drive_filepath):
        #             if self.changed_file_data and relative_filepath in self.changed_file_data:
        #                 copy_needed = True
        #         else:
        #             copy_needed = True
        # print(copy_needed)

        # return copy_needed

    def file_differences(self, dif):
        # if dif.right_only or dif.left_only or dif.funny_files:
        #     return True
        # for subdir in dif.subdirs.values():
        #      return self.fileDifferences(subdir)
        # return False
        if dif.right_only or dif.left_only or dif.funny_files:
            return True
        (_, mismatch, errors) = filecmp.cmpfiles(dif.right, dif.left, dif.common_files, shallow=False)
        if len(mismatch) > 0 or len(errors) > 0:
            print("mismatch")
            return True
        for subdir in dif.subdirs.values():
            copy_needed = self.file_differences(subdir)
            if copy_needed:
                return True
        return False

    def copy_cache_to_thumb_drive(self, usb_device_letter):
        drive_feature_filepath = os.path.join(usb_device_letter, os.sep, 'HT_CUST_UPDATER')

        copy_needed = self.thumb_drive_needs_updated(usb_device_letter)

        if copy_needed:
            self.show_dialog('Copying customer feature files to thumb drive, do not eject thumb drive', timeout=None)

        for (path, dirs, files) in os.walk(self.dataLocation):
            relative_location = path.replace(self.dataLocation, '').replace('\\', '/').lstrip('/')
            drive_location = os.path.join(drive_feature_filepath, relative_location)

            if not os.path.exists(drive_location):
                os.makedirs(drive_location)

            for filename in os.listdir(drive_location):
                QtCore.QCoreApplication.processEvents()

                drive_path = os.path.join(drive_location, filename)

                if filename not in files and filename not in dirs:
                    if os.path.isdir(drive_path):
                        shutil.rmtree(drive_path)
                    else:
                        os.remove(drive_path)

            for filename in files:
                QtCore.QCoreApplication.processEvents()

                local_filepath = os.path.join(path, filename)
                relative_filepath = os.path.join(relative_location, filename).replace('\\', '/').lstrip('/')
                drive_filepath = os.path.join(drive_feature_filepath, relative_filepath)

                if os.path.exists(drive_filepath):
                    drive_time = os.path.getmtime(drive_filepath)
                    local_time = os.path.getmtime(local_filepath)
                    # newer = driveTime - localTime
                    print('drive ' + str(drive_time) + ', local ' + str(local_time))
                    if drive_time < local_time:
                        print('removed file')
                        os.remove(drive_filepath)

                # if os.path.exists(drive_filepath):
                #     if not self.changed_file_data or relative_filepath not in self.changed_file_data:
                #         continue

                if self.file_will_fit_on_device(usb_device_letter, os.path.getsize(local_filepath)):
                    shutil.copyfile(local_filepath, drive_filepath)
                else:
                    self.hide()

                    QtCore.QCoreApplication.processEvents()

                    QMessageBox.information(
                        self,
                        self.tr("Weigh/Wrap Technician Notice: (HTe USB Key Updater Version: %s)" % VERSION),
                        self.tr(
                            "Out of disk space on %s, free some space for USB Key Updater to use." % usb_device_letter),
                        QMessageBox.StandardButtons(
                            QMessageBox.Ok),
                        QMessageBox.Ok)

                    return

        if copy_needed:
            self.hide()
            QtCore.QCoreApplication.processEvents()

            QMessageBox.information(
                self,
                self.tr("Weigh/Wrap Technician Notice: (HTe USB Key Updater Version: %s)" % VERSION),
                self.tr(
                    """USB Service Key is now up to date.  You can safely eject USB Service Key from the computer."""),
                QMessageBox.StandardButtons(
                    QMessageBox.Ok),
                QMessageBox.Ok)

        self.changed_file_data = {}

    def is_usb_attached(self):
        try:
            self.attached_usb_device()
        except ValueError:
            pass

    def attached_usb_device(self):
        usb_devices = usbQuery.UsbDeviceQuery()

        # Find all removable USB devices connected to computer
        devices = usb_devices.find_devices_in_class(usb_devices.DEVICE_TYPE_USB)
        # print len(devices)

        # do some error message processing based on USB devices we find/do not finddevice 
        if len(devices) < 1:
            #            if self.newDayTimer.isActive():
            #                self.newDayTimer.stop()
            if not self.usbDeviceTimer.isActive():
                self.usbDeviceTimer.start()
            raise ValueError(
                'USB Thumb Drive needs updated yet no USB Thumb Drive found to update.<br>'
                'Insert Hobart approved thumb drive.')
        elif len(devices) > 1:
            raise ValueError(
                'USB Thumb Drive needs updated yet more than one removable USB device found.<br><br>'
                'Remove all USB removable devices and try again.')

        if type(devices) is dict:
            device = devices.keys()[0]  # Posix
        else:
            device = devices[0]  # Windows

        deviceLetter = usb_devices.get_device_letter(device)

        if deviceLetter is None:
            raise ValueError('The USB Thumb drive attached is not a valid Hobart service key.')

        key, key_filename = keyInfo.get_key_info(keyInfo.service_key_name)

        if not os.path.exists(os.path.join(deviceLetter, key_filename)):
            raise ValueError('The USB Thumb drive attached is not a valid Hobart service key.')

        return deviceLetter

    def check_for_updates(self):
        self.updateTimer.stop()

        # print 'running update check'

        try:
            remote_file_data = directory_utils.list_remote_directory(self.remoteDownloadUrl)
        except:
            # self.showDialog('No Internet Connection, please connect to the internet')
            self.updateTimer.start()
            return

        local_file_data = directory_utils.list_local_directory(self.dataLocation)

        if directory_utils.delete_removed_files(local_file_data, remote_file_data, self.dataLocation):
            self.settings.setValue("lastCacheUpdate", QVariant(QDateTime.currentDateTime()))

        for (path, dirs, files) in os.walk(self.dataLocation):
            for directory in dirs:
                if os.path.exists(os.path.join(path, directory)):
                    if os.listdir(os.path.join(path, directory)) == []:
                        os.rmdir(os.path.join(path, directory))

        changed_file_data = directory_utils.find_new_files(local_file_data, remote_file_data)

        if changed_file_data:
            # self.showDialog('Downloading new/updated customer features from product support server')
            self.changed_file_data = changed_file_data

        for relative_filepath in changed_file_data:
            url = self.remoteDownloadUrl + relative_filepath
            download_filepath = os.path.join(self.dataLocation, relative_filepath)

            if not os.path.exists(os.path.dirname(download_filepath)):
                os.makedirs(os.path.dirname(download_filepath))

            if self.file_will_fit_on_device(os.path.splitdrive(download_filepath)[0],
                                            remote_file_data[relative_filepath]['size']):
                # print 'downloading {0} to {1}'.format(url, download_filepath)
                try:
                    directory_utils.download_remote_file(url, download_filepath)
                except:
                    # if we can not download the file now bail from this function and try later
                    self.updateTimer.start()
                    return
            else:
                QMessageBox.information(
                    self,
                    self.tr("Weigh/Wrap Technician Notice: (HTe USB Key Updater Version: %s)" % VERSION),
                    self.tr("Out of disk space on %s, free some space for USB Key Updater to use." % (
                        os.path.splitdrive(download_filepath)[0])),
                    QMessageBox.StandardButtons(
                        QMessageBox.Ok),
                    QMessageBox.Ok)
                self.updateTimer.start()
                return

        invalid_checksums = []

        for filename in changed_file_data.iterkeys():

            local_filepath = os.path.join(self.dataLocation, filename)

            # if the file is a tgz file then must check md5 to see if download successful
            if filename.endswith('.tgz') and not filename.endswith('packages.tgz'):
                local_checksum = md5checksum.md5_for_file(local_filepath)

                try:
                    remote_md5_filepath = os.path.join(self.dataLocation, os.path.splitext(filename)[0] + '.md5')
                    if self.file_will_fit_on_device(os.path.splitdrive(remote_md5_filepath)[0], MD5_CHECKSUM_FILESIZE):
                        with open(remote_md5_filepath, 'r') as remote_md5_file:
                            remote_checksum = remote_md5_file.read().rstrip()

                        if local_checksum != remote_checksum:
                            invalid_checksums.append(filename)

                    else:
                        QMessageBox.information(
                            self,
                            self.tr("Weigh/Wrap Technician Notice: (HTe USB Key Updater Version: %s)" % VERSION),
                            self.tr("Out of disk space on %s, free some space for USB Key Updater to use." % (
                            os.path.splitdrive(remote_md5_filepath)[0])),
                            QMessageBox.StandardButtons(
                                QMessageBox.Ok),
                            QMessageBox.Ok)
                        self.updateTimer.start()
                        return

                except:
                    # no MD5 file for corresponding TGZ file keep going but alert service tech
                    continue

        if invalid_checksums and not ''.join(invalid_checksums).strip():
            invalid_checksum_data = '\n'.join(invalid_checksums).strip()
            QMessageBox.information(
                self,
                self.tr("Weigh/Wrap Technician Notice: (HTe USB Key Updater Version: %s)" % VERSION),
                self.tr("Invalid Checksum computed on files:\n %s" % invalid_checksum_data),
                QMessageBox.StandardButtons(
                    QMessageBox.Ok),
                QMessageBox.Ok)

        elif changed_file_data:
            self.settings.setValue("lastCacheUpdate", QVariant(QDateTime.currentDateTime()))

        self.updateTimer.start()

    def closeEvent(self, args):
        self.hide()


def usb_key_updater_already_running():
    num_program_instances = osUtils.process_count('usbKeyUpdater.exe')

    if num_program_instances >= 2:
        return True
    else:
        return False


if __name__ == "__main__":
    app = QApplication(sys.argv)
    app.setQuitOnLastWindowClosed(False)

    QApplication.setStyle(QStyleFactory.create("Cleanlooks"))
    QApplication.setPalette(QApplication.style().standardPalette())
    # select one or the other to get super key creator or limitedKeyCreator
    # window = UsbKeyUpdater(None,[keyInfo.service_key_name, keyInfo.supervisor_key_name])
    window = UsbKeyUpdater(None)
    # window = UsbKeyUpdater(None,[keyInfo.supervisor_key_name])
    sys.exit(app.exec_())
