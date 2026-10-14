#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys

#--------------------------------------------------------------------
# Used for Service U-boot to determine if a Scale/Pre-Pack OR Slicer
#--------------------------------------------------------------------

# setup python path
sys.path.append( '/usr/lib/python2.7/dist-packages' )
sys.path.append( '/usr/lib/python2.7/site-packages' )
sys.path.append( '/usr/local/hobart/python' )

try:
    # import PyQt5 modules
    from PyQt5.QtCore import Qt
    from PyQt5.QtWidgets import QApplication, QDialog, QPushButton, QLabel
    from PyQt5.QtGui import QFont

except:
    print('ERROR: Failed to import PyQt5!')
    pass

class chooseProductDialog(QDialog):

    productType = None

    def __init__(self):
        QDialog.__init__(self)

        self.initialize()

    def initialize(self):
        self.setFixedSize( 800, 500 )
        self.setWindowTitle( "Choose Product" )
        
        f = self.font()
        f.setPointSize( 20 )
        self.setFont( f )

        l0 = QLabel(self)
        l0.setAlignment( Qt.AlignCenter )
        l0.setFixedSize( 700, 50 )
        l0.setText( "Please select Product Line and press OK:" )
        l0.move( 45, 50 )

        b0 = QPushButton(self)
        b0.setFixedSize( 250, 150 )
        b0.setText( "HT Scale\nor Pre-Pack" )
        b0.move( 100, 150 )
        b0.clicked.connect( self.setWeighWrap )

        b1 = QPushButton(self)
        b1.setFixedSize( 250, 150 )
        b1.setText( "Slicer" )
        b1.move( 450, 150 )
        b1.clicked.connect( self.setSlicer )

        #b2 = QPushButton(self)
        #b2.setFixedSize( 200, 64 )
        #b2.setText( "Ignore" )
        #b2.move( 135, 400 )
        #b2.clicked.connect( self.ignore )

        b3 = QPushButton(self)
        b3.setFixedSize( 200, 64 )
        b3.setText( "OK" )
        #b3.move( 485, 400 )
        b3.move( 300, 400 )
        b3.clicked.connect( self.writeProduct )

    def setWeighWrap(self):
        self.productType = "weighwrap"

    def setSlicer(self):
        self.productType = "slicer"

    def ignore(self):
        self.accept()

    def writeProduct(self):

        print((self.productType))

        if self.productType != None:

            if self.productType == "weighwrap":
                os.system("rm -f /etc/hobart/slicer")
            else:
                os.system("touch /etc/hobart/slicer")

            self.accept()


def chooseProduct():
    os.environ['DISPLAY'] = ':0'

    # Create main app
    myApp = QApplication(sys.argv)

    # Create dialog and show
    d = chooseProductDialog()
    d.show()

    # Execute the Application and Exit
    myApp.exec_()

if __name__ == '__main__':
    chooseProduct()
    sys.exit()
