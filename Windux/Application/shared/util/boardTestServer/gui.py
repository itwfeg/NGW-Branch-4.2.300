from PyQt5 import QtCore
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *


def set_qlineedit_input_width(input_widget, length=None):
    fm = input_widget.fontMetrics()
    # fm = QFontMetrics(font)
    m = input_widget.textMargins()
    c = input_widget.contentsMargins()
    # input_width = ' ' * (input_widget.maxLength() + 2)
    # pixels_wide = fm.width(input_width)
    # pixels_high = input_widget.height()
    if length:
        max_length = length
    else:
        max_length = input_widget.maxLength()

    w = max_length * fm.width('x') + m.left() + m.right() + c.left() + c.right()
    input_widget.setMaximumWidth(w + 8)
