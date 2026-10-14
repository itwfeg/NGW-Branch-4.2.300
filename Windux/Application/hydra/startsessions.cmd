@echo off
::
:: startsessions.cmd
:: Copy all hydra build targets into the base build dir and execute them.
::
:: Copyright (c) 2021 Hobart Corporation. All Rights Reserved.
::
:: HOW TO USE THIS SCRIPT
:: 1) Drop script into the root of your hydra build folder
:: 2) Run this script
::

set WORKINGDIRECTORY=%cd%

:: Qt installation
set QT_DIR=C:\Qt\5.15.2\msvc2019_64\bin

:: DBUS installation
set DBUS_DIR=C:\dbus-msvc64\bin

:: build type (debug -or- release)
set BUILD_TYPE=debug

:: copy build to local folder
copy .\booda\%BUILD_TYPE%\booda.exe .\
copy .\eventadapters\%BUILD_TYPE%\eventadapters.dll .\
copy .\events\%BUILD_TYPE%\events.dll .\
copy .\hobartcomms\%BUILD_TYPE%\hobartcomms.exe .\
copy .\hobartsessionman\%BUILD_TYPE%\hobartsessionman.exe .\
copy .\hydramc\%BUILD_TYPE%\hydramc.exe .\

:: update path to include Qt and DBUS
set PATH=%PATH%;%QT_DIR%;%DBUS_DIR%

:: start hydra and managers
start "DBUS DAEMON" cmd.exe /c "%DBUS_DIR%\dbus-daemon --session"
start "Hydra MC" cmd.exe /c "hydramc"
start "Session Manager" cmd.exe /c "hobartsessionman"
start "Comms" cmd.exe /c "hobartcomms"

start "BOODA" cmd.exe /c "booda"
