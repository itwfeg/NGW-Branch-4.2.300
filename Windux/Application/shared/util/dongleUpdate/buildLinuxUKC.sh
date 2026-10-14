export PYTHONPATH="$PYTHONPATH:../../sessionHandler/python:../../util/licensing"
pyinstaller --icon=usbKeyUpdater.ico -s -p $PYTHONPATH usbKeyUpdater.py
cp -f hobart.tgz ./dist/usbKeyUpdater

