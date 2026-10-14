#!/usr/bin/env python2
# -*- coding: utf-8 -*-

# not tested in python3

import os
import sys

# setup python path
# shouldn't need to add these, but it breaks otherwise
#sys.path.append( '/usr/lib/python2.7/dist-packages' )
#sys.path.append( '/usr/lib/python2.7/site-packages' )
os.environ['PYTHONPATH'] = "/usr/lib/python3.6/dist-packages:/usr/lib/python3.6/site-packages:/usr/local/hobart/python"
sys.path.append( '/usr/lib/python3.6/dist-packages' )
sys.path.append( '/usr/lib/python3.6/site-packages' )
sys.path.append( '/usr/local/hobart/python' )

try:
    # import PyQt5 modules
    from PyQt5.QtCore import Qt
    from PyQt5.QtWidgets import QApplication, QDialog, QPushButton, QLabel
    from PyQt5.QtGui import QFont

except:
    print('ERROR: Failed to import PyQt5!')
    pass

class chooseModelDialog(QDialog):

    modelType = None

    def __init__(self):
        QDialog.__init__(self)

        self.initialize()

    def initialize(self):
        self.setFixedSize( 800, 500 )
        self.setWindowTitle( "Choose Model" )
        
        f = self.font()
        f.setPointSize( 20 )
        self.setFont( f )

        l0 = QLabel(self)
        l0.setAlignment( Qt.AlignCenter )
        l0.setFixedSize( 700, 50 )
        l0.setText( "Please select this unit's Model Type and press OK:" )
        l0.move( 45, 50 )

        b0 = QPushButton(self)
        b0.setFixedSize( 250, 150 )
        b0.setText( "Service\nCounter" )
        b0.move( 100, 150 )
        b0.clicked.connect( self.setServiceCounter )

        b1 = QPushButton(self)
        b1.setFixedSize( 250, 150 )
        b1.setText( "Wrapper" )
        b1.move( 450, 150 )
        b1.clicked.connect( self.setWrapper )

        b2 = QPushButton(self)
        b2.setFixedSize( 200, 64 )
        b2.setText( "Ignore" )
        b2.move( 135, 400 )
        b2.clicked.connect( self.ignore )

        b3 = QPushButton(self)
        b3.setFixedSize( 200, 64 )
        b3.setText( "OK" )
        b3.move( 485, 400 )
        b3.clicked.connect( self.writeModel )

    def setServiceCounter(self):
        self.modelType = "service counter"

    def setWrapper(self):
        self.modelType = "pre-pack"

    def ignore(self):
        self.accept()

    def writeModel(self):

        print((self.modelType))

        if self.modelType != None:

            with open( "/etc/hobart/scale-type.dat", 'w' ) as f:
                f.write( self.modelType + '\n' )

            self.accept()


def chooseModel():
    os.environ['DISPLAY'] = ':0'

    # Create main app
    myApp = QApplication(sys.argv)

    # Create dialog and show
    d = chooseModelDialog()
    d.show()

    # Execute the Application and Exit
    myApp.exec_()

if __name__ == '__main__':
    chooseModel()
    sys.exit()
