@echo off

::
:: This script will unregister the SSH service into Windows.
::

set HOBARTDIR=%~dp0

:: stop any running service
net stop "Hobart SSHD"

:: install hobart sshd service
c:\Windows\Microsoft.NET\Framework\v4.0.30319\installutil.exe /u "%HOBARTDIR%\HobartSSHD.exe"

:: install sshd activex control
regsvr32 /u /s "%HOBARTDIR%\wodSSHD.dll"
