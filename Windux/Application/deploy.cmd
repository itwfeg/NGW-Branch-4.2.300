@echo off
::
:: deploy.cmd
:: Copy all HTe binaries and required files into one location suitable for deployment.
::
:: Copyright (c) 2013 Hobart Corporation. All Rights Reserved.
::
:: Note:
:: This script is intended to be run from the Windux\Application folder.
::
:: Prerequisites:
:: - Qt Libraries 4.8.5 for Visual Studio 2010
::


::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: ENGDEV3 has a different version of Qt installed, so the
:: QTDIR environment variable may need to be set differently
:: than your own machine.
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
set WORKING_DIR=%cd%

if %COMPUTERNAME% == ENGDEV3 (
    :: Different directory path for ENGDEV3
    set QT_BASE_DIR=C:\Qt\Qt5.4.1
) else (
    set QT_BASE_DIR=C:\Qt\Qt5.4.0
)

if EXIST %QT_BASE_DIR% (
    echo.
    echo Qt Directory "%QT_BASE_DIR%" is OK.
    echo.
    goto continue
) ELSE (
    echo.
    echo Qt Directory "%QT_BASE_DIR%" does not exist.
    echo.
    goto done
)

:continue

set QTDIR=%QT_BASE_DIR%\5.4\msvc2010_opengl

set DEPLOYDIR=deploy

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

echo HTe Deploy Script
echo Copyright (c) 2013 Hobart Corporation. All Rights Reserved.
echo.

:: remove existing
rmdir /s /q %DEPLOYDIR%

:: create again
mkdir %DEPLOYDIR%
cd %DEPLOYDIR%
mkdir Hobart
cd Hobart

::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: Copy SOFTWARE_VERSION_INFO file to Hobart directory
::::::::::::::::::::::::::::::::::::::::::::::::::::::
copy ..\..\SOFTWARE_VERSION_INFO .

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: Audio
mkdir Audio
cd Audio
:: HT-45 Need to keep audio files here for Channel Stereo Testing
copy ..\..\..\shared\sessionHandler\audio\*.* .
copy ..\..\..\shared\settingsWizardCore\python\qmSettings\sound\beep.wav .
cd ..

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: Database
mkdir Db
cd Db

mkdir Diagnostics

mkdir Scale
cd Scale

mkdir DbEmpty
mkdir DbFiles
mkdir DbMirror

cd DbEmpty
copy ..\..\..\..\..\scaleDB.empty\dbftn.db .
cd ..

cd DbFiles
copy ..\..\..\..\..\scaleDB.empty\dbftn.db .
copy ..\..\..\..\..\source\db\defaults.ini .
cd ..

cd ..

mkdir SettingsWizard
cd SettingsWizard
copy ..\..\..\..\shared\sessionHandler\db\*.* .
cd ..

cd ..

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: Documents
mkdir Docs

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: Html
mkdir Html
cd Html
:: HT-15 Need to copy both Service Counter and Pre-Pack operator manuals
xcopy ..\..\..\shared\sessionHandler\html\*.* . /s /i
cd ..

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: Images
mkdir Images
cd Images

mkdir Categories
mkdir Planograms
mkdir Products
mkdir SelfServe

mkdir Softkeys
cd Softkeys
copy "..\..\..\..\System\softkeys\*.*" .
cd ..

mkdir Specials
cd Specials
copy ..\..\..\..\shared\sessionHandler\images\specials\*.* .
cd ..

mkdir System
cd System
copy ..\..\..\..\..\shared\sessionHandler\images\diagnostics\*.* .

mkdir Categories
cd Categories
copy ..\..\..\..\..\System\categories\*.* .
cd ..
mkdir Products
cd Products
copy ..\..\..\..\..\System\products\*.* .
cd ..
mkdir SelfServe
cd SelfServe
copy ..\..\..\..\..\shared\sessionHandlerHooks\images\selfServe\*.* .
cd ..
cd ..

cd ..

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: Labels
mkdir Labels

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: Layouts
mkdir Layouts
cd Layouts
copy ..\..\..\customerInterface\gcd\images\*.* .
copy ..\..\..\customerInterface\gcd\layouts\*.* .
copy ..\..\..\..\..\shared\sessionHandler\images\background_pick5.png .
cd ..

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: Playlists
mkdir Playlists
cd Playlists
copy ..\..\..\customerInterface\gcd\playlists\*.* .
cd ..

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: Empty temporary downloads folder
mkdir downloadHolding
cd ..

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: System
mkdir Python
cd Python
copy ..\..\..\shared\sessionHandler\python\*.* .
:: get backend scripts and config files too
:: working directory = Windux/
:: svn_branch_path = ../
:: build_path = ./Application/deploy
cd %WORKING_DIR%\..

python installSmBackend.py ../ ./Application/deploy/ ./Application/deploy/

cd %WORKING_DIR%\%DEPLOYDIR%\Hobart\Python

:: donglekey utils
copy ..\..\..\shared\util\dongleKey\crypto.py .
copy ..\..\..\shared\util\dongleKey\keyInfo.py .
copy ..\..\..\shared\util\dongleKey\usbQuery.py .
copy ..\..\..\shared\util\dongleKey\verifyDongleKey.py .

:: licensing utils
copy ..\..\..\shared\util\licensing\license.py .
copy ..\..\..\shared\util\licensing\csid.py .
copy ..\..\..\shared\util\licensing\volumeinfo.py .

:: Clean-up compiled junk
erase *.pyc

cd ..

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: System
mkdir System
cd System

mkdir "Flashkey Graphics"
cd "Flashkey Graphics"
copy "..\..\..\..\System\softkeys\*.*" .
cd ..

mkdir Fonts
cd Fonts
copy ..\..\..\..\System\Fonts\*.* .
cd ..

mkdir Graphics
cd Graphics
copy ..\..\..\..\System\Graphics\*.* .
cd ..

:: Now Serving Audio
mkdir Audio
cd Audio
copy ..\..\..\..\shared\sessionHandler\audio\system\*.* .

cd ..\..


:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: User
mkdir User
cd User

mkdir Graphics

cd ..

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: Videos
mkdir Videos
cd Videos
copy ..\..\..\..\..\Multimedia\videos\en\*.mp4 .

cd ..

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: HTA-85
:: Prepack firmware
mkdir Firmware
cd Firmware

copy ..\..\..\prepack_firmware_bins\EPP2Prn.zip .
copy ..\..\..\prepack_firmware_bins\EPPFlashUpdate.zip .
copy ..\..\..\prepack_firmware_bins\EPPPrn.zip .

:: HT-46 (signed upgrade files)
:: HT-48 Replace 10.20 AWS upgrade with 10.50
copy ..\..\..\..\CustomInstallers\feature_AWS_10_50\hte-aws-10.50.0-release.package.tgz .
copy ..\..\..\..\CustomInstallers\feature_EWS_5_50\hte-ews-5.50.0-release.package.tgz .
copy ..\..\..\..\CustomInstallers\feature_CLA_1_33\hte-cla-1.33.0-release.package.tgz .

cd ..

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: Scale Service
copy ..\..\Win32\Release\ScaleService.exe .
copy ..\..\source\Text*.txt .
copy ..\..\source\config.dat .
copy ..\..\source\configc.dat .
copy ..\..\source\configdebug.dat .
copy ..\..\source\version.ini .
copy ..\..\source\software_revision.ini .

:: SSHD
copy ..\..\HobartSSHD\bin\Release\HobartSSHD.exe .
copy ..\..\HobartSSHD\wodSSHD.dll .
copy ..\..\HobartSSHD\root.pub .
copy ..\..\HobartSSHD\hobart.pub .
:: these three are required for scaleui also; qtwebkit uses them for https:// sites
copy ..\..\HobartSSHD\libeay32.dll .
copy ..\..\HobartSSHD\libssl32.dll .
copy ..\..\HobartSSHD\ssleay32.dll .

:: Scale UI
copy ..\..\operatorInterface\scaleui\release\scaleui.exe .
copy ..\..\..\scaleuiWin32.config scaleui.config
:: qt runtime is required by scaleui, gcd, and mb
copy %QTDIR%\bin\*.dll .
xcopy %QTDIR%\plugins\*.* qt_plugins /s /i
:: Needed for Qt 5
xcopy %QTDIR%\plugins\platforms\*.* platforms /s /i
:: remove qt debug runtime
del /s /q *d4.dll
del /s /q q*d.dll
del /s /q *.lib
del /s /q *.pdb

:: GCD
copy ..\..\customerInterface\gcd\release\gcd.exe .
copy ..\..\..\gcdWin32.config gcd.config

:: MB
copy ..\..\operatorInterface\mb\release\mb.exe .

:: Scale Model Choice
copy ..\..\operatorInterface\scaleModelChoice\release\scaleModelChoice.exe .

:: VIDEOPLAYER
copy ..\..\operatorInterface\videoPlayer\release\videoPlayer.exe .

:: Signature Certification Tool
copy ..\..\HobartSigCertTool\release\sigcerttool.exe .

:: HOBARTHD
copy ..\..\Win32\release\HobartHD.exe .

:: WLANUTIL and BLUETOOTHUTIL
mkdir bin
cd bin
copy ..\..\..\Win32\release\wlanutil.exe .
copy ..\..\..\Win32\release\bluetoothutil.exe .

:: EAP Utility
copy ..\..\..\shared\hobartWifi\hobartEAPUtility\Release\Hobart-EAPUtility.exe .
cd ..

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: Compile all of the Python scripts to make sure there are no syntax errors, etc.
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

python -m compileall python

if %ERRORLEVEL% NEQ 0 goto pythonerrormessage
goto pythoncompileok

:pythonerrormessage
echo.
echo Python compile FAILED!!
echo.

goto pythoncleanup

:pythoncompileok
echo.
echo All Python scripts compiled successfully.
echo.

:pythoncleanup
erase python\*.pyc

python -m compileall backend

if %ERRORLEVEL% NEQ 0 goto pythonerrormessage_backend
goto pythoncompileok_backend

:pythonerrormessage_backend
echo.
echo Python backend compile FAILED!!
echo.

goto pythoncleanup_backend

:pythoncompileok_backend
echo.
echo All Python backend scripts compiled successfully.
echo.

:pythoncleanup_backend
erase backend\*.pyc

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:::::::::::
:: engdebug
:::::::::::
::
:: TEMPORARY: Create C:\Program Files\Hobart\engdebug file so that Windows key will be enabled
::            DO NOT INCLUDE ON PRODUCTION RELEASE
::
echo "test" > engdebug

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:done
cd ..\..
