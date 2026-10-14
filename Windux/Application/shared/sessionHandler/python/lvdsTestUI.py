import os
import sys
import sendDeviceEventMessage
import socket
import select
import lvdsTestUiWorker


# setup python path
sys.path.append( '/usr/lib/python2.7/dist-packages' )
sys.path.append( '/usr/lib/python2.7/site-packages' )
sys.path.append( '/usr/local/hobart/python' )

try:
    # import PyQt5 modules
    from PyQt5.QtCore import Qt
    from PyQt5.QtWidgets import QApplication, QDialog, QPushButton, QLabel
    from PyQt5.QtCore import pyqtSlot
except:
    print('ERROR: Failed to import PyQt5!')
    pass


class LvdsDialog(QDialog):

    def __init__(self):

        QDialog.__init__(self)

        self.setFixedSize(800, 500)
        self.setWindowTitle("LVDS1 Test")
        
        f = self.font()
        f.setPointSize(20)
        self.setFont(f)

        l0 = QLabel(self)
        l0.setAlignment(Qt.AlignCenter)
        l0.setFixedSize(700, 50)
        l0.setText("Press start button to begin LVDS test.")
        l0.move(45, 50)

        self.label = QLabel("Ready", self)
        self.label.setAlignment(Qt.AlignCenter)
        self.label.setFixedSize(250, 200)
        self.label.move(280,  250)
        
        self.b0 = QPushButton(self)
        self.b0.setFixedSize(250, 150)
        self.b0.setText("Start Test")
        self.b0.move(280, 150)
        self.b0.clicked.connect(self.start_test)

        self.monitor_test_worker = lvdsTestUiWorker.Worker()
        self.monitor_test_worker.alert_handler.connect(self.set_alert)
        self.monitor_test_worker.start()

    @pyqtSlot(str)
    def set_alert(self, message):

        self.label.clear()
        self.label.setText(message)
        self.label.adjustSize()
        if message == 'Ready':
            self.b0.setDisabled(False)
        else:
            self.b0.setDisabled(True)

    def start_test(self):
        self.label.clear()
        self.label.setText("Running")
        self.label.adjustSize()
        self.b0.setDisabled(True)
        sendDeviceEventMessage.sendMessage(server='localhost', port=6000,
                                           message="RTDO" + chr(0x1F) + "TNStartLvds1Test" + chr(0x1E))


def start_lvds_test():
    os.environ['DISPLAY'] = ':0'

    # Create main app
    app = QApplication(sys.argv)

    # Create dialog and show
    d = LvdsDialog()
    d.show()

    # Execute the Application and Exit
    app.exec_()


if __name__ == '__main__':
    start_lvds_test()
    sys.exit()
