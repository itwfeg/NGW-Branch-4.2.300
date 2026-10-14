export PYTHONPATH="$PYTHONPATH:../../sessionHandler/python:../../util/licensing"
pyinstaller --icon=usbKeyCreator.ico -s -p $PYTHONPATH usbKeyCreator.py
cp -f hobart.tgz ./dist/usbKeyCreator

