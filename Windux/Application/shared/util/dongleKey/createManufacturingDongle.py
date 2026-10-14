from PyQt4 import QtGui, QtCore
import usbKeyCreator
import sys

if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    
    ## Look and feel changed to CleanLooks
    QtGui.QApplication.setStyle(QtGui.QStyleFactory.create("Cleanlooks"))
    QtGui.QApplication.setPalette(QtGui.QApplication.style().standardPalette())
    window = usbKeyCreator.UsbKeyCreator(None,'manufacturing')
    window.show()
    sys.exit(app.exec_())
