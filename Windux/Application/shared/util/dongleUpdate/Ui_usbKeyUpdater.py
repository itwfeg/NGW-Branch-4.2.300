# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'Z:\Data\software development\trunk\Windux\Application\shared\util\dongleUpdate/usbKeyUpdater.ui'
#
# Created by: PyQt5 UI code generator 5.13.0
#
# WARNING! All changes made in this file will be lost!


from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_usbKeyUpdaterDialog(object):
    def setupUi(self, usbKeyUpdaterDialog):
        usbKeyUpdaterDialog.setObjectName("usbKeyUpdaterDialog")
        usbKeyUpdaterDialog.resize(430, 165)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(usbKeyUpdaterDialog.sizePolicy().hasHeightForWidth())
        usbKeyUpdaterDialog.setSizePolicy(sizePolicy)
        usbKeyUpdaterDialog.setMinimumSize(QtCore.QSize(430, 165))
        usbKeyUpdaterDialog.setMaximumSize(QtCore.QSize(430, 165))
        font = QtGui.QFont()
        font.setPointSize(10)
        usbKeyUpdaterDialog.setFont(font)
        icon = QtGui.QIcon()
        icon.addPixmap(QtGui.QPixmap(":/icons/itw.ico"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        usbKeyUpdaterDialog.setWindowIcon(icon)
        self.verticalLayoutWidget = QtWidgets.QWidget(usbKeyUpdaterDialog)
        self.verticalLayoutWidget.setGeometry(QtCore.QRect(10, 20, 411, 126))
        self.verticalLayoutWidget.setObjectName("verticalLayoutWidget")
        self.verticalLayout = QtWidgets.QVBoxLayout(self.verticalLayoutWidget)
        self.verticalLayout.setContentsMargins(0, 0, 0, 0)
        self.verticalLayout.setObjectName("verticalLayout")
        self.horizontalLayout = QtWidgets.QHBoxLayout()
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.label = QtWidgets.QLabel(self.verticalLayoutWidget)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label.sizePolicy().hasHeightForWidth())
        self.label.setSizePolicy(sizePolicy)
        self.label.setMinimumSize(QtCore.QSize(0, 0))
        self.label.setMaximumSize(QtCore.QSize(64, 64))
        self.label.setText("")
        self.label.setPixmap(QtGui.QPixmap(":/images/hobartUsbKeySmall.png"))
        self.label.setScaledContents(True)
        self.label.setObjectName("label")
        self.horizontalLayout.addWidget(self.label)
        self.instructionsLabel = QtWidgets.QLabel(self.verticalLayoutWidget)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.instructionsLabel.sizePolicy().hasHeightForWidth())
        self.instructionsLabel.setSizePolicy(sizePolicy)
        self.instructionsLabel.setMinimumSize(QtCore.QSize(0, 0))
        self.instructionsLabel.setMaximumSize(QtCore.QSize(16777215, 16777215))
        self.instructionsLabel.setAlignment(QtCore.Qt.AlignCenter)
        self.instructionsLabel.setWordWrap(True)
        self.instructionsLabel.setObjectName("instructionsLabel")
        self.horizontalLayout.addWidget(self.instructionsLabel)
        self.verticalLayout.addLayout(self.horizontalLayout)
        self.progressHorizontalLayout = QtWidgets.QHBoxLayout()
        self.progressHorizontalLayout.setObjectName("progressHorizontalLayout")
        self.progressLabel = QtWidgets.QLabel(self.verticalLayoutWidget)
        self.progressLabel.setObjectName("progressLabel")
        self.progressHorizontalLayout.addWidget(self.progressLabel)
        self.progressBar = QtWidgets.QProgressBar(self.verticalLayoutWidget)
        self.progressBar.setMaximum(112)
        self.progressBar.setProperty("value", 0)
        self.progressBar.setObjectName("progressBar")
        self.progressHorizontalLayout.addWidget(self.progressBar)
        self.verticalLayout.addLayout(self.progressHorizontalLayout)

        self.retranslateUi(usbKeyUpdaterDialog)
        QtCore.QMetaObject.connectSlotsByName(usbKeyUpdaterDialog)

    def retranslateUi(self, usbKeyUpdaterDialog):
        _translate = QtCore.QCoreApplication.translate
        usbKeyUpdaterDialog.setWindowTitle(_translate("usbKeyUpdaterDialog", "Weigh/Wrap Technician Notice"))
        self.instructionsLabel.setText(_translate("usbKeyUpdaterDialog", "<html><head/><body><p align=\"center\">Insert the Hobart Approved USB thumb drive for USB Key update into a USB 2.0 port of this computing system.</p></body></html>"))
        self.progressLabel.setText(_translate("usbKeyUpdaterDialog", "Download Progress:"))
import usbKeyUpdater_rc
