@echo OFF
rem ***************************************************************
rem Parameters:  ALL INPUT PARAMETERS **MUST** BE IN QUOTES ON THE CMD LINE
rem
rem	%1 -> LXSCALE_SOURCE_BASE == root directory of project source
rem
rem	%2 -> JDK_BASE == installation dirrectory of the JDK
rem
rem     %3 -> BUILD_CONFIGURATION == where exe is location Debug 
rem                                                       Release
rem
rem     %4 -> ZIP_INSTALL_DIR == path to zip command line exe
rem    
rem     %5 -> INTERACTIVE == "Y" Bypass all user confirmation
rem                          "N", Interactive Mode.
rem
rem	%6 -> PAUSE_ON_CMPL == "Y"
rem                            "N"
rem
rem	%7 -> CAPTURE_FILE == Filename to capture output.
rem	                      If not supplied, and no env variable defined, output goes to the screen.
rem
rem 	Modification		Reason			Who
rem ---------------------------------------------------------------
rem	Nov 25, 2006		Initial Release		JRB
rem
rem ***************************************************************
rem
rem Initialize variables
rem
rem ***************************************************************
set INIT_DIR=%cd%
set SAVED_PATH=%path%
set RTN_ERRORLEVEL=0
set CMD=0
set CAPTURE_FILE=""

if "%~1" == "" goto nada_to_do
if "%~2" == "" goto nada_to_do
if "%~3" == "" goto nada_to_do
if "%~4" == "" goto nada_to_do
if "%~5" == "" goto nada_to_do
if "%~6" == "" goto nada_to_do
if "%~7" == "" set RD=""
if NOT "%~7" == "" set CAPTURE_FILE=%~7
if NOT "%~7" == "" set RD=">> %CAPTURE_FILE% 2>&1"

set TARGET_BASE=%~1
set LXSCALE_SOURCE_BASE=%~2
set BUILD_CONFIGURATION=%~3
set ZIP_INSTALL_DIR=%~4
set INTERACTIVE=%~5
set PAUSE_ON_CMPL=%~6


if NOT %CAPTURE_FILE%=="" (
	echo . 
        echo ...... Pakaging at %DATE% %TIME% ....... 
	echo . 
	echo TARGET_BASE = %TARGET_BASE%  
	echo LXSCALE_SOURCE_BASE = %LXSCALE_SOURCE_BASE%  
        echo BUILD_CONFIGURATION = %BUILD_CONFIGURATION%
        echo ZIP_INSTALL_DIR = %ZIP_INSTALL_DIR%
	echo INTERACTIVE = %INTERACTIVE%  
	echo PAUSE_ON_CMPL = %PAUSE_ON_CMPL%  
        if %CAPTURE_FILE%=="" echo CAPTURE_FILE = Screen
        if NOT %CAPTURE_FILE%=="" echo CAPTURE_FILE = %CAPTURE_FILE%  
	echo . )

echo .  %rd:~1,-1%
echo ...... Packaging at %DATE% %TIME% .......  %rd:~1,-1%
echo .  %rd:~1,-1%
echo TARGET_BASE = %TARGET_BASE% %rd:~1,-1% 
echo LXSCALE_SOURCE_BASE = %LXSCALE_SOURCE_BASE% %rd:~1,-1%
echo BUILD_CONFIGURATION = %BUILD_CONFIGURATION% %rd:~1,-1%
echo ZIP_INSTALL_DIR = %ZIP_INSTALL_DIR% %rd:~1,-1%
echo INTERACTIVE = %INTERACTIVE% %rd:~1,-1% 
echo PAUSE_ON_CMPL = %PAUSE_ON_CMPL% %rd:~1,-1% 
if %CAPTURE_FILE%=="" echo CAPTURE_FILE = Screen %rd:~1,-1%
if NOT %CAPTURE_FILE%=="" echo CAPTURE_FILE = %CAPTURE_FILE%   %rd:~1,-1%
echo . %rd:~1,-1%


if NOT "%INTERACTIVE%"=="Y" goto start_package
set /p ANS="Do you still want to continue? (Y/N) "
if not "%ANS%"=="Y" if not "%ANS%"=="y" ( echo Aborted.
                                          set RTN_ERRORLEVEL=1
                                          goto end )

rem ***************************************************************
rem
rem make sure the target location is there and empty
rem
rem ***************************************************************
:start_package


if EXIST "%TARGET_BASE%\Access Scale" (rd /S /Q "%TARGET_BASE%\Access Scale")
mkdir "%TARGET_BASE%\Access Scale"

echo copying %LXSCALE_SOURCE_BASE%\Application\scaleDB.empty...

set cmd="xcopy %LXSCALE_SOURCE_BASE%\Application\scaleDB.empty TARGET_BASE%\Access Scale\dbfiles /S /Y /Q /I"
xcopy "%LXSCALE_SOURCE_BASE%\Application\scaleDB.empty" "%TARGET_BASE%\Access Scale\dbfiles" /S /Y /Q /I %rd:~1,-1%

if NOT %errorlevel%==0 goto error_handler

set cmd="copy /Y %LXSCALE_SOURCE_BASE%\Application\source\db\defaults.ini %TARGET_BASE%\Access Scale\dbfiles\defaults.ini"
copy /Y "%LXSCALE_SOURCE_BASE%\Application\source\db\defaults.ini" "%TARGET_BASE%\Access Scale\dbfiles\defaults.ini" %rd:~1,-1%
if NOT %errorlevel%==0 goto error_handler

del /S "%TARGET_BASE%\Access Scale\dbfiles\vssver.scc" %rd:~1,-1%


set cmd="xcopy %TARGET_BASE%\Access Scale\dbfiles %TARGET_BASE%\Access Scale\dbempty /S /Y /Q /I"
xcopy "%TARGET_BASE%\Access Scale\dbfiles" "%TARGET_BASE%\Access Scale\dbempty" /S /Y /Q /I %rd:~1,-1%



if EXIST "%TARGET_BASE%\Access Scale\dbfiles\defaults.ini" attrib -r "%TARGET_BASE%\Access Scale\dbfiles\defaults.ini"

echo copying %LXSCALE_SOURCE_BASE%\Application\System...

set cmd="xcopy %LXSCALE_SOURCE_BASE%\Application\System TARGET_BASE%\Access Scale\System /E /Y /Q /I"
xcopy "%LXSCALE_SOURCE_BASE%\Application\System" "%TARGET_BASE%\Access Scale\System" /E /Y /Q /I %rd:~1,-1%

if NOT %errorlevel%==0 goto error_handler

del /S "%TARGET_BASE%\Access Scale\System\vssver.scc" %rd:~1,-1%


set cmd="xcopy %LXSCALE_SOURCE_BASE%\Application\User TARGET_BASE%\Access Scale\User /E /Y /Q /I"

echo copying %LXSCALE_SOURCE_BASE%\Application\User...
xcopy "%LXSCALE_SOURCE_BASE%\Application\User" "%TARGET_BASE%\Access Scale\User" /E /Y /Q /I  %rd:~1,-1%
if NOT %errorlevel%==0 goto error_handler

del /S "%TARGET_BASE%\Access Scale\User\vssver.scc" %rd:~1,-1%

echo copying %LXSCALE_SOURCE_BASE%\Application\scripts...
xcopy "%LXSCALE_SOURCE_BASE%\Application\scripts" "%TARGET_BASE%\Access Scale\scripts" /E /Y /Q /I  %rd:~1,-1%
if NOT %errorlevel%==0 goto error_handler

del /S "%TARGET_BASE%\Access Scale\scripts\vssver.scc" %rd:~1,-1%

echo copying %LXSCALE_SOURCE_BASE%\Application\WebPages...
set cmd="xcopy %LXSCALE_SOURCE_BASE%\Application\WebPages TARGET_BASE%\Access Scale\webPages /E /Y /Q /I"

xcopy "%LXSCALE_SOURCE_BASE%\Application\WebPages" "%TARGET_BASE%\Access Scale\webPages" /E /Y /Q /I %rd:~1,-1%
if NOT %errorlevel%==0 goto error_handler

del /S "%TARGET_BASE%\Access Scale\WebPages\vssver.scc" %rd:~1,-1%

set cmd="copy /Y %LXSCALE_SOURCE_BASE%\Application\source\mom\com\hobart\scale\mom\webPages\mom15.jar %TARGET_BASE%\Access Scale\WebPages\mom15.jar"
copy /Y "%LXSCALE_SOURCE_BASE%\Application\source\mom\com\hobart\scale\mom\webPages\mom15.jar" "%TARGET_BASE%\Access Scale\WebPages\mom15.jar" %rd:~1,-1%
if NOT %errorlevel%==0 goto error_handler

set cmd="copy /Y %LXSCALE_SOURCE_BASE%\Application\source\config.dat %TARGET_BASE%\Access Scale\config.dat %rd:~1,-1%"
copy /Y "%LXSCALE_SOURCE_BASE%\Application\source\config.dat" "%TARGET_BASE%\Access Scale\config.dat" %rd:~1,-1%
if NOT %errorlevel%==0 goto error_handler

set cmd="copy /Y %LXSCALE_SOURCE_BASE%\Application\source\*.txt %TARGET_BASE%\Access Scale %rd:~1,-1%"
copy /Y "%LXSCALE_SOURCE_BASE%\Application\source\*.txt" "%TARGET_BASE%\Access Scale" %rd:~1,-1%
if NOT %errorlevel%==0 goto error_handler

echo copying %LXSCALE_SOURCE_BASE%\Application\source\app\%BUILD_CONFIGURATION%\ScaleService.exe...
set cmd="copy /Y %LXSCALE_SOURCE_BASE%\Application\source\app\%BUILD_CONFIGURATION%\ScaleService.exe %TARGET_BASE%\Access Scale\ScaleService.exe %rd:~1,-1%"
copy /Y "%LXSCALE_SOURCE_BASE%\Application\source\app\%BUILD_CONFIGURATION%\ScaleService.exe" "%TARGET_BASE%\Access Scale\ScaleService.exe" %rd:~1,-1%
if NOT %errorlevel%==0 goto error_handler

set cmd="copy /Y %LXSCALE_SOURCE_BASE%\Application\source\EvolutionRTC\CodeUpdater\atmelAT91\AT91Boot_DLL.dll %TARGET_BASE%\Access Scale\AT91Boot_DLL.unreg %rd:~1,-1%"
copy /Y "%LXSCALE_SOURCE_BASE%\Application\source\EvolutionRTC\CodeUpdater\atmelAT91\AT91Boot_DLL.dll" "%TARGET_BASE%\Access Scale\AT91Boot_DLL.unreg" %rd:~1,-1%
if NOT %errorlevel%==0 goto error_handler

echo copying %LXSCALE_SOURCE_BASE%\Application\appInstaller\%BUILD_CONFIGURATION%\appInstaller.exe...
set cmd="copy /Y %LXSCALE_SOURCE_BASE%\Application\appInstaller\%BUILD_CONFIGURATION%\appInstaller.exe %TARGET_BASE%\Access Scale\appInstaller.exe %rd:~1,-1%"
copy /Y "%LXSCALE_SOURCE_BASE%\Application\appInstaller\%BUILD_CONFIGURATION%\appInstaller.exe" "%TARGET_BASE%\Access Scale\appInstaller.exe" %rd:~1,-1%
if NOT %errorlevel%==0 goto error_handler

echo copying %LXSCALE_SOURCE_BASE%\Application\winAppLauncher\%BUILD_CONFIGURATION%\winAppLauncher.exe...
set cmd="copy /Y %LXSCALE_SOURCE_BASE%\Application\winAppLauncher\%BUILD_CONFIGURATION%\winAppLauncher.exe %TARGET_BASE%\Access Scale\appLauncher.exe %rd:~1,-1%"
copy /Y "%LXSCALE_SOURCE_BASE%\Application\winAppLauncher\%BUILD_CONFIGURATION%\winAppLauncher.exe" "%TARGET_BASE%\Access Scale\appLauncher.exe" %rd:~1,-1%
if NOT %errorlevel%==0 goto error_handler

echo copying %LXSCALE_SOURCE_BASE%\Application\WifiProfiles\%BUILD_CONFIGURATION%\WifiProfiles.exe...
set cmd="copy /Y %LXSCALE_SOURCE_BASE%\Application\WifiProfiles\%BUILD_CONFIGURATION%\WifiProfiles.exe %TARGET_BASE%\Access Scale\WifiProfiles.exe %rd:~1,-1%"
copy /Y "%LXSCALE_SOURCE_BASE%\Application\WifiProfiles\%BUILD_CONFIGURATION%\WifiProfiles.exe" "%TARGET_BASE%\Access Scale\WifiProfiles.exe" %rd:~1,-1%
if NOT %errorlevel%==0 goto error_handler

set cmd="copy /Y %LXSCALE_SOURCE_BASE%\3RDPartySoftware\microsoft\srvany.exe %TARGET_BASE%\Access Scale\srvany.exe %rd:~1,-1%"
copy /Y "%LXSCALE_SOURCE_BASE%\3RDPartySoftware\microsoft\srvany.exe" "%TARGET_BASE%\Access Scale\srvany.exe" %rd:~1,-1%
if NOT %errorlevel%==0 goto error_handler

mkdir "%TARGET_BASE%\Access Scale\win"
set cmd="copy /Y %LXSCALE_SOURCE_BASE%\3RDPartySoftware\goAheadWebServer\WIN\webs.exe %TARGET_BASE%\Access Scale\win\webs.exe %rd:~1,-1%"
copy /Y "%LXSCALE_SOURCE_BASE%\3RDPartySoftware\goAheadWebServer\WIN\webs.exe" "%TARGET_BASE%\Access Scale\win\webs.exe" %rd:~1,-1%
if NOT %errorlevel%==0 goto error_handler

set cmd="copy /Y %LXSCALE_SOURCE_BASE%\Application\bootParams.cfg %TARGET_BASE%\Access Scale\bootParams.cfg %rd:~1,-1%"
copy /Y "%LXSCALE_SOURCE_BASE%\Application\bootParams.cfg" "%TARGET_BASE%\Access Scale\bootParams.cfg" %rd:~1,-1%
if NOT %errorlevel%==0 goto error_handler

set cmd="copy /Y %LXSCALE_SOURCE_BASE%\Application\source\core\versionManager\version.ini %TARGET_BASE%\Access Scale\version.ini %rd:~1,-1%"
copy /Y "%LXSCALE_SOURCE_BASE%\Application\source\core\versionManager\version.ini" "%TARGET_BASE%\Access Scale\version.ini" %rd:~1,-1%
if NOT %errorlevel%==0 goto error_handler


if EXIST "%TARGET_BASE% upgrade" (del "%TARGET_BASE% upgrade" /S /Q /F %rd:~1,-1%)

xcopy "%TARGET_BASE%" "%TARGET_BASE% upgrade" /E /C /I /Q /R /Y %rd:~1,-1%
rd /S /Q "%TARGET_BASE% upgrade\Access Scale\dbfiles"
rd /S /Q "%TARGET_BASE% upgrade\Access Scale\user"

set cmd="copy /Y %TARGET_BASE%\Access Scale\version.ini %TARGET_BASE% upgrade\Hobart.ini %rd:~1,-1%"
copy /Y "%TARGET_BASE%\Access Scale\version.ini" "%TARGET_BASE% upgrade\Hobart.ini" %rd:~1,-1%
if NOT %errorlevel%==0 goto error_handler

mkdir "%TARGET_BASE% upgrade\Access Scale\drivers"

set cmd="copy /Y %LXSCALE_SOURCE_BASE%\SMBUS\driver\objfre_wxp_x86\i386\smbus.sys %TARGET_BASE% upgrade\Access Scale\drivers\smbus.sys %rd:~1,-1%"
copy /Y "%LXSCALE_SOURCE_BASE%\SMBUS\driver\objfre_wxp_x86\i386\smbus.sys" "%TARGET_BASE% upgrade\Access Scale\drivers\smbus.sys" %rd:~1,-1%
if NOT %errorlevel%==0 goto error_handler

set cmd="copy /Y %LXSCALE_SOURCE_BASE%\WeighUsb\WeighUSB\objfre_wxp_x86\i386\weighusb.sys %TARGET_BASE% upgrade\Access Scale\drivers\weighusb.sys %rd:~1,-1%"
copy /Y "%LXSCALE_SOURCE_BASE%\WeighUsb\WeighUSB\objfre_wxp_x86\i386\weighusb.sys" "%TARGET_BASE% upgrade\Access Scale\drivers\weighusb.sys" %rd:~1,-1%
if NOT %errorlevel%==0 goto error_handler

set cmd="copy /Y %LXSCALE_SOURCE_BASE%\WeighUsb\WeighUSB\wghusb.inf %TARGET_BASE% upgrade\Access Scale\drivers\wghusb.inf %rd:~1,-1%"
copy /Y "%LXSCALE_SOURCE_BASE%\WeighUsb\WeighUSB\wghusb.inf" "%TARGET_BASE% upgrade\Access Scale\drivers\wghusb.inf" %rd:~1,-1%
if NOT %errorlevel%==0 goto error_handler


set cmd="%ZIP_INSTALL_DIR%\7z a -tzip %TARGET_BASE% upgrade\hobart.zip %TARGET_BASE% upgrade\Access Scale\* %rd:~1,-1%"
"%ZIP_INSTALL_DIR%\7z" a -tzip "%TARGET_BASE% upgrade\hobart.zip" "%TARGET_BASE% upgrade\Access Scale\*" %rd:~1,-1% 
if NOT %errorlevel%==0 goto error_handler

rem ****************************************************************
rem 
rem stuff you dont want in the firmware upgrade zips should appear
rem here!
rem
rem ****************************************************************

mkdir "%TARGET_BASE%\Access Scale\Firmware"

set cmd="copy /Y %LXSCALE_SOURCE_BASE%\Application\source\EvolutionRTC\CodeUpdater\atmelAT91\APCFirmware.zip %TARGET_BASE%\Access Scale\Firmware\APC.zip %rd:~1,-1%"
copy /Y "%LXSCALE_SOURCE_BASE%\Application\source\EvolutionRTC\CodeUpdater\atmelAT91\APCFirmware.zip" "%TARGET_BASE%\Access Scale\Firmware\APC.zip" %rd:~1,-1%
if NOT %errorlevel%==0 goto error_handler

del "%TARGET_BASE%\Access Scale\version.ini"

set cmd="copy /y %LXSCALE_SOURCE_BASE%\Application\source\core\versionManager\version.ini + %LXSCALE_SOURCE_BASE%\Application\source\EvolutionRTC\CodeUpdater\atmelAT91\APCFirmware.ini /B %TARGET_BASE%\Access Scale\version.ini"
copy /y "%LXSCALE_SOURCE_BASE%\Application\source\core\versionManager\version.ini" + "%LXSCALE_SOURCE_BASE%\Application\source\EvolutionRTC\CodeUpdater\atmelAT91\APCFirmware.ini" /B "%TARGET_BASE%\Access Scale\version.ini"
if NOT %errorlevel%==0 ( set RTN_ERRORLEVEL=%errorlevel%
                         goto errors_occurred )

goto end

:nada_to_do
rem ***************************************************************
rem
rem  The user did not pass in the project directory and or the output file.
rem
rem ***************************************************************
echo * * * * * * * W A R N I N G * * * * * * * 
echo INVALID USAGE -- NOT ALL PARAMETERS ARE DEFINED.
echo USAGE: Build_Moms p1 p2 p3 p4 p5 p6
echo .
echo        p1 is the "TARGET_BASE" value.
echo        p2 is the "LXSCALE_SOURCE_BASE" value.
echo        p3 is the "BUILD_CONFIGURATION" value.
echo        p4 is the "INTERACTIVE" value.
echo        p5 is the "PAUSE_ON_CMPL" value.
echo        p6 is the output capture file (optional)
echo .
set RTN_ERRORLEVEL=1
goto end

:error_handler
set RTN_ERRORLEVEL=1

:end

if NOT %CAPTURE_FILE%=="" (
	if %RTN_ERRORLEVEL%==0 (  echo .
                                  echo package COMPLETED at %DATE% %TIME%. )
	if NOT %RTN_ERRORLEVEL%==0  ( echo .  
        	                      echo package had errors.  
                	              echo .  
                        	      echo Error trying to execute: %cmd% )
                            )


if %RTN_ERRORLEVEL%==0 (  echo .
                          echo package COMPLETED at %DATE% %TIME%.  %rd:~1,-1% )
if NOT %RTN_ERRORLEVEL%==0  ( echo .  %rd:~1,-1%
                              echo package had errors.  %rd:~1,-1%
                              echo .  %rd:~1,-1%
                              echo Error trying to execute: %cmd%  %rd:~1,-1%)
echo .
cd\
cd %INIT_DIR% 
%INIT_DIR:~0,2%
Path=%SAVED_PATH%
if "%PAUSE_ON_CMPL%"=="Y" pause

