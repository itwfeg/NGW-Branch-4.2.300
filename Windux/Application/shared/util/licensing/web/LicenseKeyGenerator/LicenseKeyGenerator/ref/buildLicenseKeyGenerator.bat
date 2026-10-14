rem Install PyQt4 Windows Installer and Python 2.7 first
set SOURCEDIR=%CD%
rem NSIS needs installed on your system
set QTDIR=C:\Qt\4.8.5
set QTCREATOR=C:\Qt\Qt5.2.1\Tools\QtCreator
set PATH=%QTDIR%\bin;%QTCREATOR%\bin;%ProgramFiles(x86)%\Launch4j;%ProgramFiles(x86)%\NSIS;%PATH%
set PYTHONPATH=..\..\sessionHandler\python;..\dongleKey;%PYTHONPATH%
rem PyInstaller needs installed
rem set PYTHONPATH=../../Windux/Application/shared/sessionHandler/python;%PYTHONPATH%
rem set PYTHONPATH=..\dongleKey;%PYTHONPATH%

svn update

rem create python gui file from qtdesigner form
rem you have to do this since pyuic4 is a bat file and
rem Windoze can not call a batch file from within a
rem batch file
call pyrcc4 lkg.qrc -o lkg_rc.py

call pyuic4 licenseKeyGenerator.ui -o Ui_licenseKeyGenerator.py

rem remove pyinstaller output build and dist directories
rmdir /s /q .\dist\licenseKeyGenerator
rmdir /s /q .\build
del /f /q hte-win-license_key_generator-*.exe

rem generate native Windows EXE from the python files
rem build windowed application
pyinstaller --windowed --icon=licenseKeyGenerator.ico --noconfirm licenseKeyGenerator.py

copy licenseKeyGenerator.ico .\dist\licenseKeyGenerator

python ../../sessionHandler/python/versionUpdater.py license_key_generator licenseKeyGenerator.nsi

rem create Windows installer
makensis.exe licenseKeyGenerator.nsi

rem net use w: \\engdev1\www-server build1mgr /user:buildmgr

rem copy /y hte-win-license_key_generator-*.exe w:\builds\HTseries\HTe\LicenseKeyGenerator

rem net use w: /delete
