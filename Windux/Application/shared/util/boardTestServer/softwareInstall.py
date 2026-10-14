
from PyQt5.QtCore import *
from PyQt5.QtCore import pyqtSignal
import os
import boardTestServer
import installUtils


class Worker(QThread):

    alert_handler = pyqtSignal(str)
    install_complete_handler = pyqtSignal(str)

    def __init__(self, upgrade_filename, feature_name, parent=None):

        QThread.__init__(self, parent)

        self.upgrade_filename = upgrade_filename.strip()
        self.feature_name = feature_name + '_new_release'

    def perform_software_upgrade(self):

        self.alert_handler.emit("INFO: Stopping Board Test Server")
        boardTestServer.stop_board_test_server()

        installUtils.remove_feature(self.feature_name)
        os.mkdir(path=os.path.join(installUtils.get_feature_root(), self.feature_name), mode=0o755)

        self.alert_handler.emit("INFO: Extracting installation file")

        if installUtils.extract_installer_tarball(feature_name=self.feature_name,
                                                  installer_tarball_filename=self.upgrade_filename,
                                                  destination=os.path.join(installUtils.get_feature_root(),
                                                                           self.feature_name),
                                                  validate_checksum=True) == 0:

            installUtils.remove_installer_fragments(self.upgrade_filename)

            self.alert_handler.emit("INFO: Installing... Test Station may reboot")

            if installUtils.execute_install_script(feature_name=self.feature_name,
                                                   python_path_additions='/home/pi/test_station_server') == 0:

                self.alert_handler.emit("SUCCESS: Software installed successfully")

            else:
                self.alert_handler.emit("ERROR: Software could not be installed")

        else:
            self.alert_handler.emit("ERROR: Software installation file could not be extracted")

        self.install_complete_handler.emit("done")

    def __del__(self):
        self.wait()

    def run(self):

        self.perform_software_upgrade()

