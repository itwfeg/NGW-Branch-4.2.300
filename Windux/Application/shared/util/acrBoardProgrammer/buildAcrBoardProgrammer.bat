rem See README file for info on setting up build environment to run this script
rem change version number below to update version number on UI and Installer

rem Black Box Device Installer
set product_name="ACR Board Programmer"
set product_version=1.5.0
set product_installer_name="itw-win-acr_board_programmer_setup_utility-%product_version%.exe"
set product_website="https://www.hobartcorp.com/products/weigh-wrap"
set product_publisher="ITW Hobart, Inc."

rem verify that all of the paths are correct
set python_path=%ProgramFiles%\Python35
set python_scripts_path=%python_path%\Scripts
set windows_sdk_path=%ProgramFiles(x86)%\Windows Kits\10\bin\10.0.17763.0\x64
set nsis_path=%ProgramFiles(x86)%\NSIS
set qt_path=%python_path%\Lib\site-packages\PyQt5
set qt_tools_path=%python_path%\Lib\site-packages\pyqt5_tools\Qt\bin

set PATH=%python_path%;%python_scripts_path%;%windows_sdk_path%;%nsis_path%;%qt_path%;%qt_tools_path%;%PATH%

set PYTHONPATH=..\..\..\sessionHandler\python;..\boardTestServer;%PYTHONPATH%

ECHO Removing previous build output...
ECHO rm .\dist\acrBoardProgrammer
ECHO rm .\build
rmdir /s /q .\dist\acrBoardProgrammer
rmdir /s /q .\build
del /f /q %product_installer_name%

python ..\boardTestServer\compile_forms.py --resource acrBoardProgrammer

echo "Building executable..."

rem generate native Windows EXE from the python files
rem build windowed application

python ..\boardTestServer\file_match_and_replace.py acr_board_programmer.ini "^serial_port[ \t]*=[ \t]*.+$" "serial_port = COM3"
python ..\boardTestServer\file_match_and_replace.py acr_board_programmer.ini "^barcode_scanner_device[ \t]*=[ \t]*.+$" "barcode_scanner_device = COM4"

python ..\boardTestServer\file_match_and_replace.py acrBoardProgrammer.py "^PRODUCT_VERSION = \"[0-9]+.[0-9]+.[0-9]+\"$" "PRODUCT_VERSION = ""%product_version%"""
python ..\boardTestServer\file_match_and_replace.py acrBoardProgrammer.py "^PRODUCT_NAME = \".*\"$" "PRODUCT_NAME = """%product_name%""""

copy imx_usb* .\dist\acrBoardProgrammer
copy mx6_usb_work.conf .\dist\acrBoardProgrammer
copy u-boot-*.imx .\dist\acrBoardProgrammer
copy /y acrBoardProgrammer.ico .\dist\acrBoardProgrammer
copy /y .\icons\itw.ico .\dist\acrBoardProgrammer
copy /y acr_board_programmer.ini .\dist\acrBoardProgrammer
copy /y usage_guide.html .\dist\acrBoardProgrammer
copy /y acr_board_programmer_usage_diagram.png .\dist\acrBoardProgrammer
copy /y registerFirewallExceptions.bat .\dist\acrBoardProgrammer
copy /y unregisterFirewallExceptions.bat .\dist\acrBoardProgrammer

pyinstaller --noconsole --windowed --icon=acrBoardProgrammer.ico --noconfirm acrBoardProgrammer.py

rem create Windows installer
echo Creating Windows Installer...

python ..\boardTestServer\file_match_and_replace.py acrBoardProgrammer.nsi "^!define PRODUCT_NAME .*$" "!define PRODUCT_NAME """%product_name%""""
python ..\boardTestServer\file_match_and_replace.py acrBoardProgrammer.nsi "^!define PRODUCT_VERSION .*$" "!define PRODUCT_VERSION """%product_version%""""
python ..\boardTestServer\file_match_and_replace.py acrBoardProgrammer.nsi "^!define PRODUCT_WEBSITE .*$" "!define PRODUCT_WEBSITE """%product_website%""""
python ..\boardTestServer\file_match_and_replace.py acrBoardProgrammer.nsi "^!define PRODUCT_PUBLISHER .*$" "!define PRODUCT_PUBLISHER """%product_publisher%""""
python ..\boardTestServer\file_match_and_replace.py acrBoardProgrammer.nsi "^!define PRODUCT_INSTALLER_NAME .*$" "!define PRODUCT_INSTALLER_NAME """%product_installer_name%""""

rem create Windows installer
makensis.exe acrBoardProgrammer.nsi

signtool sign /f ../../../../../Windux/itw_hobart_code_signing_certificate_2020.pfx /p Hobart123 /tr http://timestamp.comodoca.com/rfc3161 /fd sha256 /td sha256 /as /v %product_installer_name%

rem net use w: \\engdev1\builds build1mgr /user:buildmgr

rem copy /y hte-win-wrapper_node_tester-*.exe w:\HTseries\test

rem net use w: /delete

