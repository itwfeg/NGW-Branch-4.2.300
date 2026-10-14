rem Install PyQt4 Windows Installer and Python 2.7 first
set SOURCEDIR=%CD%
rem NSIS needs installed on your system
rem PyInstaller needs installed
rem set PYTHONPATH=../../Windux/Application/shared/sessionHandler/python;%PYTHONPATH%

set QTDIR=C:\Qt\4.8.5
set QTCREATOR=C:\Qt\Qt5.2.1\Tools\QtCreator
set PATH=%QTDIR%\bin;%QTCREATOR%\bin;%ProgramFiles(x86)%\Launch4j;%ProgramFiles(x86)%\NSIS;%PATH%
set PYTHONPATH=..\..\sessionHandler\python;%PYTHONPATH%
svn update

rem create python resource file from qtdesigner resource file
pyrcc4 usbKeyUpdater.qrc -o usbKeyUpdater_rc.py

rem create python gui file from qtdesigner form
rem you have to do this since pyuic4 is a bat file and
rem Windoze can not call a batch file from within a
rem batch file
call pyuic4 usbKeyUpdater.ui -o usbKeyUpdaterUi.py

rem remove pyinstaller output build and dist directories
rmdir /s /q .\dist
rmdir /s /q .\build
del /f /q hte-*-usb_key_updater-*.exe

rem generate native Windows EXE from the python files
rem build windowed application
pyinstaller --windowed --icon=usbKeyUpdater.ico usbKeyUpdater.py

rem copy USB_Disk_Eject.exe .\dist\usbKeyUpdater
rem copy hobart.tgz .\dist\usbKeyUpdater
copy usbKeyUpdater.ico .\dist\usbKeyUpdater

python copySSLCertificate.py

python ../../sessionHandler/python/versionUpdater.py usb_key_updater usbKeyUpdater.nsi

python osTypeUpdater.py usbKeyUpdater.nsi

rem create Windows installer
makensis.exe usbKeyUpdater.nsi

net use w: \\engdev1\builds build1mgr /user:buildmgr

copy /y hte-*-usb_key_updater-*.exe w:\HTseries\HTe\UsbKeyUpdater

net use w: /delete
