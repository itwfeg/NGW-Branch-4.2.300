export PYTHONPATH="$PYTHONPATH:../../sessionHandler/python:../../util/acrBoardProgrammer"
pyinstaller --icon=usbKeyCreator.ico -s -p $PYTHONPATH usbKeyCreator.py
