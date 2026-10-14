
import sys
sys.path.append('/usr/lib/python2.7/dist-packages')
sys.path.append('/usr/lib/python2.7/site-packages')
sys.path.append('/usr/local/hobart/python')

import os
from PyQt5 import QtCore
from PyQt5 import QtWidgets
from PyQt5.QtWidgets import QFileDialog
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *

#from PyQt5.QFileDialog import *
import md5checksum
import tarfile
import shutil
import time

VERSION_NUMBER="1.0"

LOAD_NORDIC=True

HFLB_ROOT = "/usr/local/hobart/hflbProgrammer"
sys.path.append(HFLB_ROOT)

DEBUG_LOG=True
if DEBUG_LOG:
    import logging
    LOGFILE = HFLB_ROOT + "/hflb_programmer.log"
    # setup log file
    logging.basicConfig(filename=LOGFILE, filemode='w', level=logging.DEBUG, format='%(asctime)s %(message)s',
                        datefmt='%m/%d/%Y %I:%M:%S %p')

NORDIC_LIBRARY_PATH = HFLB_ROOT + '/pc-nrfutil-0.5.3'

# you must execute the following commands on the scale for the Nordic Semi
# library below to load correctly
# pip install click
# pip install enum
# these commands will have to execute on the scale during first boot and
# the appropriate click and enum wheels must be local to the scale since

if LOAD_NORDIC:
    sys.path.append(NORDIC_LIBRARY_PATH)
    try:
        from nordicsemi.dfu.dfu import Dfu
        from nordicsemi.dfu.dfu_transport import DfuEvent
        from nordicsemi.dfu.dfu_transport_serial import DfuTransportSerial
        from nordicsemi.dfu.package import Package
        from nordicsemi import version as nrfutil_version
        from nordicsemi.dfu.signing import Signing
        from nordicsemi.dfu.util import query_func
    except Exception as e:
        print "ERROR: Noric Semi DFU library is missing"
        logging.error("Nordic Semi library did not load")

PYTHON_DEBUG = False

if PYTHON_DEBUG:
    # required to run a PyQt application from the command line python debugger via:
    # python -m pdb hflbProgrammer.py
    pyqtRemoveInputHook()



from Ui_hflbProgrammer import Ui_hflbProgrammerDialog


class HandsFreeLoginBeeper(QDialog):

    def __init__(self, version_number, hflb_root, parent=None):

        QDialog.__init__(self, parent)

        self.ui = Ui_hflbProgrammerDialog()

        self.ui.setupUi(self)

        QCoreApplication.setOrganizationName("Hobart")
        QCoreApplication.setOrganizationDomain("hobartcorp.com")
        QCoreApplication.setApplicationName("HFLB_PROGRAMMER")
        self.settings = QSettings(hflb_root + '/HFLB_programmer.ini', QSettings.IniFormat)

        self.setWindowFlags(QtCore.Qt.FramelessWindowHint)

        # self.setWindowTitle("i.MX6 Board Programmer (Version: %s)" % (VERSION))
        self.ui.versionLabel.setText(version_number)

        self.ui.installerFilenameLineEdit.setText(self.settings.value('installer_filename'))
        self.ui.selectProgramFileButton.clicked.connect(self.select_program_file)
        self.ui.programButton.clicked.connect(self.program_device)

    def select_program_file(self):
        selected = []

        default_device = "/lib/firmware/hfpd"

        qfd = QFileDialog()
        qfd.setAcceptMode(QFileDialog.AcceptOpen)
        qfd.setDirectory(default_device)
        qfd.setFileMode(QFileDialog.ExistingFile)
        qfd.setOption(QFileDialog.ReadOnly)
        qfd.setLabelText(QFileDialog.Accept, "Ok")
        qfd.setNameFilter(
            "Program File (*.zip)")
        qfd.setWindowTitle("Choose Programming File")
        qfd.setViewMode(QFileDialog.List)

        if QDialog.Accepted == qfd.exec_():
            selected = qfd.selectedFiles()

        if 1 == len(selected):
            self.ui.installerFilenameLineEdit.setText(selected[0])
            self.settings.setValue("installer_filename", selected[0])

    def program_device(self):

        installer_filename = self.ui.installerFilenameLineEdit.text()
        self.set_alert("")

        if installer_filename.strip() != '':

            if os.path.exists(installer_filename):

                self.set_alert("INFO: Programming the device")
                thread = DFUThread(filename=installer_filename)
                thread.progressSig.connect(self.set_progress_bar)
                thread.messageSig.connect(self.handle_message)
                thread.run()

            else:
                self.set_alert("ERROR: Installer package does not have firmware file")

    def set_alert(self, alert):
        self.ui.alertsLabel.setText(alert)
        #logging.info("alert is %s" % alert)
        QCoreApplication.processEvents(QEventLoop.AllEvents)
        time.sleep(0.5)

    @QtCore.pyqtSlot(int)
    def set_progress_bar(self, progress=10):
        if progress == 1:
            self.set_alert(
                "INFO: Programming the device has started.  Do not unplug programming cable.")

        self.ui.progressBar.setValue(progress)

        if progress == 100:
            self.set_alert(
                "INFO: Programming complete.  Wait for RED ring around button to turn off before removing programming cable.")

    @QtCore.pyqtSlot(str)
    def handle_message(self, message=""):
        self.set_alert(message)
        if "ERROR" in message:
            self.set_alert("ERROR: Programming did not complete. Unplug device and restart programming procedure listed above.")


class DFUThread(QtCore.QThread):
    progressSig = QtCore.pyqtSignal(object)
    messageSig = QtCore.pyqtSignal(object)

    def __init__(self, filename, port="/dev/ttymxc2", baud="115200"):
        QtCore.QThread.__init__(self)
        self.filename = filename
        self.port = port
        self.baud = baud

    def update_progress(self, progress=0, done=False, log_message=""):
        del done, log_message
        self.progressSig.emit(progress)

    def run(self):
        self.update_progress()
        # print(self.filename, self.port, self.baud)
        self.messageSig.emit("INFO: Connection to port {0}, baud rate {1}".format(self.port, self.baud))
        self.messageSig.emit("INFO: Initiating device programming.  This may take up to 30 seconds. Please wait.")
        serial_backend = DfuTransportSerial(self.port, self.baud, False)
        serial_backend.register_events_callback(DfuEvent.PROGRESS_EVENT, self.update_progress)
        dfu = Dfu(zip_file_path=self.filename, dfu_transport=serial_backend)
        try:
            dfu.dfu_send_images()
        except Exception as e:
            self.messageSig.emit("ERROR: {0}".format(e.message))


def program_hflb_device():
    os.environ['DISPLAY'] = ':0'

    logging.info("starting application GUI")
    app = QApplication(sys.argv)
    QApplication.setStyle(QStyleFactory.create("Cleanlooks"))
    QApplication.setPalette(QApplication.style().standardPalette())
    window = HandsFreeLoginBeeper(VERSION_NUMBER, HFLB_ROOT, None)
    window.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    program_hflb_device()
    sys.exit()
