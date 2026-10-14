@echo off
::
:: deploy.cmd
:: Build and deploy HobartSSHD to folder suitable for installation.
::
:: Copyright (c) 2016 Hobart Corporation. All Rights Reserved.
::
:: Prerequisites:
:: - Visual Studio 2010

set BUILDLOG=%TEMP%\build.log
set WORKINGDIRECTORY=%cd%

:: setup environment
call "c:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" x86  >> %BUILDLOG%

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: Show Splashscreen
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

echo Hobart SSHD Deploy Script
echo Copyright (c) 2016 Hobart Corporation. All Rights Reserved.
echo.

echo.  > %BUILDLOG%
echo ============================================================  >> %BUILDLOG%
echo Hobart SSHD Deploy Script  >> %BUILDLOG%
date /t  >> %BUILDLOG%
time /t  >> %BUILDLOG%
echo ============================================================  >> %BUILDLOG%
echo.  >> %BUILDLOG%

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: Build Visual Studio Projects
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:build

:: Remove any existing executables
if exist %WORKINGDIRECTORY%\bin\Release\HobartSSHD.exe  del %WORKINGDIRECTORY%\bin\Release\HobartSSHD.exe

echo -------------------
echo Building HobartSSHD
echo -------------------

devenv HobartSSHD.csproj /clean "Release|x86" /out %BUILDLOG%  >> %BUILDLOG%
devenv HobartSSHD.csproj /build "Release|x86" /out %BUILDLOG%  >> %BUILDLOG%

:: Make sure all of the executables were built
if not exist %WORKINGDIRECTORY%\bin\Release\HobartSSHD.exe (
    echo ERROR: HobartSSHD.exe NOT BUILT
    goto done
)

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: Deploy Binaries and Config
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:deploy

echo --------------------
echo Deploying HobartSSHD
echo --------------------

mkdir %WORKINGDIRECTORY%\deploy
mkdir %WORKINGDIRECTORY%\deploy\sshd
mkdir %WORKINGDIRECTORY%\deploy\sshd\public

copy %WORKINGDIRECTORY%\bin\Release\HobartSSHD.exe %WORKINGDIRECTORY%\deploy\sshd
copy %WORKINGDIRECTORY%\libeay32.dll %WORKINGDIRECTORY%\deploy\sshd
copy %WORKINGDIRECTORY%\libssl32.dll %WORKINGDIRECTORY%\deploy\sshd
copy %WORKINGDIRECTORY%\ssleay32.dll %WORKINGDIRECTORY%\deploy\sshd
copy %WORKINGDIRECTORY%\wodSSHD.dll %WORKINGDIRECTORY%\deploy\sshd

copy %WORKINGDIRECTORY%\_register_sshd.cmd %WORKINGDIRECTORY%\deploy\sshd
copy %WORKINGDIRECTORY%\_unregister_sshd.cmd %WORKINGDIRECTORY%\deploy\sshd

copy %WORKINGDIRECTORY%\*.txt %WORKINGDIRECTORY%\deploy\sshd
copy %WORKINGDIRECTORY%\root.pub %WORKINGDIRECTORY%\deploy\sshd
copy %WORKINGDIRECTORY%\smuser.pub %WORKINGDIRECTORY%\deploy\sshd
copy %WORKINGDIRECTORY%\README %WORKINGDIRECTORY%\deploy\sshd

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: Complete
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:done