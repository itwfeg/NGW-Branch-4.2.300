@echo off

::
:: This script will register the SSH service into Windows.
::
:: NOTE:
:: You need to run this script from admin command prompt.
::

set HOBARTDIR=%~dp0

:: install sshd activex control
regsvr32 /s "%HOBARTDIR%\wodSSHD.dll"
regsvr32 /s "%HOBARTDIR%\wodSSHD64.dll"

:: install hobart sshd service
c:\Windows\Microsoft.NET\Framework\v4.0.30319\installutil.exe "%HOBARTDIR%\HobartSSHD.exe"