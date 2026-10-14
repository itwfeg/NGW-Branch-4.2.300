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

call "%ProgramFiles(x86)%\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" x86

rem create python resource file from qtdesigner resource file
pyrcc4 usbKeyCreator.qrc -o usbKeyCreator_rc.py

rem create python gui file from qtdesigner form
rem you have to do this since pyuic4 is a bat file and
rem Windoze can not call a batch file from within a
rem batch file
call pyuic4 usbKeyCreator.ui -o usbKeyCreatorUi.py

rem remove pyinstaller output build and dist directories
rmdir /s /q .\dist
rmdir /s /q .\build
del /f /q hte-win-usb_key_creator-*.exe

rem generate native Windows EXE from the python files
rem build windowed application
pyinstaller --windowed --icon=usbKeyCreator.ico usbKeyCreator.py

copy USB_Disk_Eject.exe .\dist\usbKeyCreator
copy hobart.tgz .\dist\usbKeyCreator
copy usbKeyCreator.ico .\dist\usbKeyCreator

python ../../sessionHandler/python/versionUpdater.py usb_key_creator usbKeyCreator.nsi

rem create Windows installer
makensis.exe usbKeyCreator.nsi

signtool sign /f ../../../../../Windux/itw_hobart_code_signing_certificate_2020.pfx /p Hobart123 /tr http://timestamp.comodoca.com/rfc3161 /fd sha256 /td sha256 /v hte-win-usb_key_creator-*.exe

net use w: \\engdev1\builds build1mgr /user:buildmgr

copy /y hte-win-usb_key_creator-*.exe w:\HTseries\HTe\UsbKeyCreator

net use w: /delete
