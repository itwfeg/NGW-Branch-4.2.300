rem See README file for info on setting up build environment to run this script
rem change version number below to update version number on UI and Installer

rem change website link for corona psu, blackbox, or remote gateway device links

rem Black Box Device Installer
set product_name="Black Box Device Setup Utility"
set product_version=3.1.2
set product_installer_name="itw-win-black_box_device_setup_utility-%product_version%.exe"
set product_website="https://www.itw.com"
set product_publisher="ITW, Inc."

rem PillarSync Installer
rem set product_name="PillarSync Device Setup Utility"
rem set product_version="3.1.1"
rem set product_installer_name="itw-win-pillarsync_device_setup_utility-%product_version%.exe"
rem set product_website="https://www.itw.com"
rem set product_publisher="ITW Pillar Technologies, Inc."

rem verify that all of the paths are correct
set python_path=%ProgramFiles%\Python35
set python_scripts_path=%python_path%\Scripts
set windows_sdk_path=%ProgramFiles(x86)%\Windows Kits\10\bin\10.0.17763.0\x64
set nsis_path=%ProgramFiles(x86)%\NSIS
set qt_path=%python_path%\Lib\site-packages\PyQt5
set qt_tools_path=%python_path%\Lib\site-packages\pyqt5_tools\Qt\bin

set PATH=%python_path%;%python_scripts_path%;%windows_sdk_path%;%nsis_path%;%qt_path%;%qt_tools_path%;%PATH%

set PYTHONPATH=..\..\..\sessionHandler\python;..\..\licensing;%PYTHONPATH%;

ECHO Removing previous build output...
ECHO rm .\dist\bbm
ECHO rm .\build
rmdir /s /q .\dist\bbm
rmdir /s /q .\build
del /f /q %product_installer_name%

pyrcc5 resources.qrc -o resources_rc.py

python compile_forms.py
echo Building executable...
rem generate native Windows EXE from the python files
rem build windowed application

python file_match_and_replace.py bbm.py "^PRODUCT_VERSION = \"[0-9]+.[0-9]+.[0-9]+\"$" "PRODUCT_VERSION = ""%product_version%"""
python file_match_and_replace.py bbm.py "^PRODUCT_NAME = \".*\"$" "PRODUCT_NAME = """%product_name%""""

pyinstaller --noconsole --windowed --icon=bbIcon.ico --noconfirm bbm.py
rem pyinstaller --hidden-import PyQt5.sip --icon=bbIcon.ico bbm.py

copy bbIcon.ico .\dist\bbm
copy registerFirewallExceptions.bat .\dist\bbm
copy unregisterFirewallExceptions.bat .\dist\bbm

mkdir .\dist\bbm\doc
xcopy /e .\doc .\dist\bbm\doc

rem create Windows installer
echo Creating Windows Installer...

python file_match_and_replace.py installer.nsi "^!define PRODUCT_NAME .*$" "!define PRODUCT_NAME """%product_name%""""
python file_match_and_replace.py installer.nsi "^!define PRODUCT_VERSION .*$" "!define PRODUCT_VERSION """%product_version%""""
python file_match_and_replace.py installer.nsi "^!define PRODUCT_WEBSITE .*$" "!define PRODUCT_WEBSITE """%product_website%""""
python file_match_and_replace.py installer.nsi "^!define PRODUCT_PUBLISHER .*$" "!define PRODUCT_PUBLISHER """%product_publisher%""""
python file_match_and_replace.py installer.nsi "^!define PRODUCT_INSTALLER_NAME .*$" "!define PRODUCT_INSTALLER_NAME """%product_installer_name%""""

rem update version number

makensis.exe installer.nsi

rem sign the binary with ITW certificate
rem comodo site says http://timestamp.comodoca.com/authenticode may be used for timestamping also
rem old way to sign a file
rem signtool sign /f ../../../../../../Windux/itw_hobart_code_signing_certificate_2020.pfx /p Hobart123 /tr http://timestamp.comodoca.com/rfc3161 %product_installer_name%
signtool sign /f ../../../../../../Windux/itw_hobart_code_signing_certificate_2020.pfx /p Hobart123 /tr http://timestamp.comodoca.com/rfc3161 /fd sha256 /td sha256 /as /v %product_installer_name%
