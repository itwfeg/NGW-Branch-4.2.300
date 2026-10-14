import sys
import os
sys.path.append( '/usr/lib/python2.7/dist-packages' )
sys.path.append( '/usr/lib/python2.7/site-packages' )
from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import QDialog, QPushButton, QLabel,  QScrollArea


#Creates a QDialog window with a "Yes" and "No" box. addText(message) Must be called in order to add a message box
class QuestionBox(QDialog):
    
    modelType = None
    
    def __init__(self):
        QDialog.__init__(self)
        self.initialize()
        self.setModal(True)

    def initialize(self):
        self.setWindowTitle( "SD Migration" )
        
        f = self.font()
        f.setPointSize( 20 )
        self.setFont( f )

        b2 = QPushButton(self)
        b2.setFixedSize( 200, 64 )
        b2.setText( "Yes" )
        b2.move( 235, 400 )
        b2.clicked.connect( self.yes )

        b3 = QPushButton(self)
        b3.setFixedSize( 200, 64 )
        b3.setText( "No" )
        b3.move( 585, 400 )
        b3.clicked.connect( self.ignore )
    
    
    # Adds a text box with the passed message to the Qdialog window
    def addText(self, message):
        l0 = QLabel(self)
        l0.setAlignment( Qt.AlignCenter )
        l0.setFixedHeight(100)
        l0.setText(message)
        l0.move( 245, 25 )
        l0.show()
        return l0

    #adds a scroll box with containg the items from the message_list in between the main text box and the buttons
    def addScrollBox(self,title, message_list):
        title_box = QLabel(self)
        title_box.setAlignment( Qt.AlignCenter )
        title_box.setFixedHeight(100)
        title_box.setText(title)
        title_box.move( 245, 125 )
        s0 = QScrollArea(self)
        s0.setWidgetResizable(True)
        s0.setFixedHeight(130)
        s0.setFixedWidth(500)
        s0.move( 245, 200 )
        message = os.linesep.join(message_list)
        #for x in message_list:
        m_box = QLabel(self)
        m_box.setAlignment( Qt.AlignLeft )
        m_box.setText(message)
        s0.setWidget(m_box)
        return title_box, m_box
        
    def yes(self):
        #returns 1
        self.accept()
    def ignore(self):
        #returns 0
        self.reject()
        
