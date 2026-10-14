#!/bin/sh -f
# You need to extract the Windux/Application/shared/sessionHandler/python folder so it is ../.. relative to this directory
# For linux runtime build of the package
# Install the following debian packages:
# sudo apt-get install pyqt4-dev-tools
# sudo pip install pyinstaller

# if you want to install all the pyqt4 development tools
# sudo apt-get install python-qt4 pyqt4-dev-tools qt4-designer

# must set path
PATH=/home/hobart/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin
PYTHONPATH=/usr/lib/python2.7:/usr/lib/python2.7/dist-packages:../python:../../util/acrBoardProgrammer:../../sessionHandler/python
CWD=$(pwd)

# make sure this gets changed with each release
version=1.3.0

rm -rf ./dist
mkdir -p ./dist

chmod +x ./imx_usb
chmod +x ./install.sh

pyuic4 acrBoardProgrammer.ui -o Ui_acrBoardProgrammer.py

pyinstaller --icon=acrBoardProgrammer.ico -s -p $PYTHONPATH acrBoardProgrammer.py

cp acrBoardProgrammer.ico ./dist/acrBoardProgrammer
cp imx_usb ./dist/acrBoardProgrammer
cp imx_usb.conf ./dist/acrBoardProgrammer
cp mx6_usb_work.conf ./dist/acrBoardProgrammer
cp u-boot-q-x.x.xx.imx ./dist/acrBoardProgrammer
cp u-boot-s-x.x.xx.imx ./dist/acrBoardProgrammer
cp u-boot-x.x.xx.imx ./dist/acrBoardProgrammer
cp u-boot-issi-x.x.xx.imx ./dist/acrBoardProgrammer
cp u-boot-ark-dq-issi-x.x.xx.imx ./dist/acrBoardProgrammer
cp install.sh ./dist/acrBoardProgrammer

tar cvzf acrBoardProgrammer-$version.tgz -C ./dist acrBoardProgrammer
