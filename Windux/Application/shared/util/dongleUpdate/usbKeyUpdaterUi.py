# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'usbKeyUpdater.ui'
#
# Created: Fri Dec 01 15:30:16 2017
#      by: PyQt4 UI code generator 4.11.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    def _fromUtf8(s):
        return s

try:
    _encoding = QtGui.QApplication.UnicodeUTF8
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig, _encoding)
except AttributeError:
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig)

class Ui_usbKeyUpdaterDialog(object):
    def setupUi(self, usbKeyUpdaterDialog):
        usbKeyUpdaterDialog.setObjectName(_fromUtf8("usbKeyUpdaterDialog"))
        usbKeyUpdaterDialog.resize(430, 108)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(usbKeyUpdaterDialog.sizePolicy().hasHeightForWidth())
        usbKeyUpdaterDialog.setSizePolicy(sizePolicy)
        usbKeyUpdaterDialog.setMinimumSize(QtCore.QSize(430, 0))
        usbKeyUpdaterDialog.setMaximumSize(QtCore.QSize(430, 9999))
        font = QtGui.QFont()
        font.setPointSize(10)
        usbKeyUpdaterDialog.setFont(font)
        icon = QtGui.QIcon()
        icon.addPixmap(QtGui.QPixmap(_fromUtf8(":/icons/itw.ico")), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        usbKeyUpdaterDialog.setWindowIcon(icon)
        self.horizontalLayout = QtGui.QHBoxLayout(usbKeyUpdaterDialog)
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.label = QtGui.QLabel(usbKeyUpdaterDialog)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label.sizePolicy().hasHeightForWidth())
        self.label.setSizePolicy(sizePolicy)
        self.label.setMinimumSize(QtCore.QSize(0, 0))
        self.label.setMaximumSize(QtCore.QSize(64, 64))
        self.label.setText(_fromUtf8(""))
        self.label.setPixmap(QtGui.QPixmap(_fromUtf8(":/images/hobartUsbKeySmall.png")))
        self.label.setScaledContents(True)
        self.label.setObjectName(_fromUtf8("label"))
        self.horizontalLayout.addWidget(self.label)
        self.instructionsLabel = QtGui.QLabel(usbKeyUpdaterDialog)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.instructionsLabel.sizePolicy().hasHeightForWidth())
        self.instructionsLabel.setSizePolicy(sizePolicy)
        self.instructionsLabel.setMinimumSize(QtCore.QSize(0, 0))
        self.instructionsLabel.setMaximumSize(QtCore.QSize(16777215, 16777215))
        self.instructionsLabel.setAlignment(QtCore.Qt.AlignCenter)
        self.instructionsLabel.setWordWrap(True)
        self.instructionsLabel.setMargin(5)
        self.instructionsLabel.setObjectName(_fromUtf8("instructionsLabel"))
        self.horizontalLayout.addWidget(self.instructionsLabel)

        self.retranslateUi(usbKeyUpdaterDialog)
        QtCore.QMetaObject.connectSlotsByName(usbKeyUpdaterDialog)

    def retranslateUi(self, usbKeyUpdaterDialog):
        usbKeyUpdaterDialog.setWindowTitle(_translate("usbKeyUpdaterDialog", "Weigh/Wrap Technician Notice", None))
        self.instructionsLabel.setText(_translate("usbKeyUpdaterDialog", "<html><head/><body><p align=\"center\">Insert the Hobart Approved USB thumb drive for USB Key update into a USB 2.0 port of this computing system.</p></body></html>", None))

import usbKeyUpdater_rc
