@echo OFF
rem ***************************************************************
rem Parameters:
rem
rem   EXTRACT_VSS_FLAG "Y" = extract form VSS, "N" = dont extract.
rem   VSS_DATABASE the name of the vss database
rem   VSS_VERSION the label of the project to get.
rem   LXSCALE_SOURCE_BASE the local directory where the vss project is to be extracted to.
rem   VSS_INSTALL_DIR the directory where VSS is installed.
rem   DEV_STUDIO_INSTALL_DIR the directory where developer studio is installed.
rem   MAKE_CLEAN "Y" = perform a clean build. LXSCALE_SOURCE_BASE is deleted. "N" = perform incremental build
rem   SOLUTION_CONFIGURATION the configuration of the win32 app project. typically "Debug" or "Release"
rem   JDK_BASE the directory where the java development kit is installed.
rem   BUILD_MOMS "Y" build the moms using the JDK_BASE. "N" skip.
rem   LOCAL_TARGET_BASE where on the local machine the results of the build are packaged.
rem   WINDDK_BASE where the microsoft driver development kit is installed.
rem   UPDATE_REMOTE_SITE "Y" copy LOCAL_TARGET_BASE to this remote location. "N" skip.
rem   REMOTE_SITE_BASE where to copy the package results to.
rem   FTP_SERVER_DIR remote location to copy install packages to
rem   NEW_BUILDNBR "Y" update the buildnumber with the current date/time. "N" skip.
rem   UPDATE_VERSION_INI "Y" update the version number in version.ini. "N" skip.
rem   ZIP_INSTALL_DIR path to zip utility.
rem   INTERACTIVE "Y" prompts user to continue at various build stages.
rem   PAUSE_ON_CMPL "Y" pauses after completion of various build stages.
rem
rem
rem 	Modification		Reason			Who
rem ---------------------------------------------------------------
rem	OCT 25, 2006		Initial Release		JRB
rem
rem ***************************************************************
rem
rem
rem ***************************************************************
set CAPTURE_FILE=""
set RD=""
set RTN_ERRORLEVEL=0
set VSS_COMMENT=The Build_Evolution Script checked this module into VSS

if "%~1" == "" goto nada_to_do
if "%~2" == "" goto nada_to_do
if "%~3" == "" goto nada_to_do
if "%~4" == "" goto nada_to_do
if "%~5" == "" goto nada_to_do
if "%~6" == "" goto nada_to_do
if "%~7" == "" goto nada_to_do
if "%~8" == "" goto nada_to_do
if "%~9" == "" goto nada_to_do

set EXTRACT_VSS_FLAG=%~1
set VSS_DATABASE=%~2
set VSS_PROJECT=%~3
set VSS_VERSION=%~4
set LXSCALE_SOURCE_BASE=%~5
set VSS_INSTALL_DIR=%~6
set DEV_STUDIO_INSTALL_DIR=%~7
set MAKE_CLEAN=%~8
set SOLUTION_CONFIGURATION=%~9

shift /0

if "%~9" == "" goto nada_to_do
set JDK_BASE=%~9

shift /0

if "%~9" == "" goto nada_to_do
set BUILD_MOMS=%~9

shift /0

if "%~9" == "" goto nada_to_do
set LOCAL_TARGET_BASE=%~9

shift /0

if "%~9" == "" goto nada_to_do
set WINDDK_BASE=%~9

shift /0

if "%~9" == "" goto nada_to_do
set UPDATE_REMOTE_SITE=%~9

shift /0

if "%~9" == "" goto nada_to_do
set REMOTE_SITE_BASE=%~9

shift /0

if "%~9" == "" goto nada_to_do
set FTP_SERVER_DIR=%~9

shift /0

if "%~9" == "" goto nada_to_do
set NEW_BUILDNBR=%~9

shift /0

if "%~9" == "" goto nada_to_do
set UPDATE_VERSION_INI=%~9

shift /0

if "%~9" == "" goto nada_to_do
set ZIP_INSTALL_DIR=%~9

shift /0

if "%~9" == "" goto nada_to_do
set INTERACTIVE=%~9

shift /0

if "%~9" == "" goto nada_to_do
set PAUSE_ON_CMPL=%~9

shift /0

if NOT "%~9" == "" set CAPTURE_FILE=%~9
if NOT "%~9" == "" set RD=">> %CAPTURE_FILE% 2>&1"

if NOT %CAPTURE_FILE%=="" (
               echo .
               echo .............main build ...................
               echo .
               echo EXTRACT_VSS_FLAG: %EXTRACT_VSS_FLAG%
               echo VSS_DATABASE: %VSS_DATABASE%
               echo VSS_PROJECT: %VSS_PROJECT%
               echo VSS_VERSION: %VSS_VERSION%
               echo LXSCALE_SOURCE_BASE: %LXSCALE_SOURCE_BASE%
               echo VSS_INSTALL_DIR: %VSS_INSTALL_DIR%
               echo DEV_STUDIO_INSTALL_DIR: %DEV_STUDIO_INSTALL_DIR%
               echo MAKE_CLEAN: %MAKE_CLEAN%
               echo SOLUTION_CONFIGURATION: %SOLUTION_CONFIGURATION%
               echo JDK_BASE: %JDK_BASE%
               echo BUILD_MOMS: %BUILD_MOMS%
               echo LOCAL_TARGET_BASE: %LOCAL_TARGET_BASE%
               echo WINDDK_BASE: %WINDDK_BASE%
               echo UPDATE_REMOTE_SITE: %UPDATE_REMOTE_SITE%
               echo REMOTE_SITE_BASE: %REMOTE_SITE_BASE%
               echo FTP_SERVER_DIR: %FTP_SERVER_DIR%
               echo NEW_BUILDNBR: %NEW_BUILDNBR%
               echo UPDATE_VERSION_INI: %UPDATE_VERSION_INI%
               echo ZIP_INSTALL_DIR: %ZIP_INSTALL_DIR%
               echo INTERACTIVE: %INTERACTIVE%
               echo PAUSE_ON_CMPL: %PAUSE_ON_CMPL%
               if %CAPTURE_FILE%=="" echo CAPTURE_FILE = Screen
               if NOT %CAPTURE_FILE%=="" echo CAPTURE_FILE = %CAPTURE_FILE%  
               echo . )

echo . %rd:~1,-1%
echo .............main build ................... %rd:~1,-1%
echo . %rd:~1,-1%
echo EXTRACT_VSS_FLAG: %EXTRACT_VSS_FLAG% %rd:~1,-1%
echo VSS_DATABASE: %VSS_DATABASE% %rd:~1,-1%
echo VSS_PROJECT: %VSS_PROJECT% %rd:~1,-1%
echo VSS_VERSION: %VSS_VERSION% %rd:~1,-1%
echo LXSCALE_SOURCE_BASE: %LXSCALE_SOURCE_BASE% %rd:~1,-1%
echo VSS_INSTALL_DIR: %VSS_INSTALL_DIR% %rd:~1,-1%
echo DEV_STUDIO_INSTALL_DIR: %DEV_STUDIO_INSTALL_DIR% %rd:~1,-1%
echo MAKE_CLEAN: %MAKE_CLEAN% %rd:~1,-1%
echo SOLUTION_CONFIGURATION: %SOLUTION_CONFIGURATION% %rd:~1,-1%
echo JDK_BASE: %JDK_BASE% %rd:~1,-1%
echo BUILD_MOMS: %BUILD_MOMS% %rd:~1,-1%
echo LOCAL_TARGET_BASE: %LOCAL_TARGET_BASE% %rd:~1,-1%
echo WINDDK_BASE: %WINDDK_BASE% %rd:~1,-1% 
echo UPDATE_REMOTE_SITE: %UPDATE_REMOTE_SITE% %rd:~1,-1%
echo REMOTE_SITE_BASE: %REMOTE_SITE_BASE% %rd:~1,-1%
echo FTP_SERVER_DIR: %FTP_SERVER_DIR% %rd:~1,-1%
echo NEW_BUILDNBR: %NEW_BUILDNBR% %rd:~1,-1%
echo UPDATE_VERSION_INI: %UPDATE_VERSION_INI% %rd:~1,-1%
echo ZIP_INSTALL_DIR: %ZIP_INSTALL_DIR% %rd:~1,-1%
echo INTERACTIVE: %INTERACTIVE% %rd:~1,-1%
echo PAUSE_ON_CMPL: %PAUSE_ON_CMPL% %rd:~1,-1%
if %CAPTURE_FILE%=="" echo CAPTURE_FILE = Screen %rd:~1,-1%
if NOT %CAPTURE_FILE%=="" echo CAPTURE_FILE = %CAPTURE_FILE% %rd:~1,-1%
echo . %rd:~1,-1%


set BATCHPATH=%CD%\

if NOT "%INTERACTIVE%"=="Y" goto extractVSSProj
set /p ANS="Do you still want to continue? (Y/N) "
if not "%ANS%"=="Y" if not "%ANS%"=="y" ( echo Aborted.
                                          set RTN_ERRORLEVEL=1
                                          goto end )


rem ***************************************************************
rem
rem Extract most recent code from VSS
rem
rem ***************************************************************

echo BATCHPATH = %BATCHPATH%

:extractVSSProj

if NOT "%EXTRACT_VSS_FLAG%"=="Y" goto buildMOMs

if "%MAKE_CLEAN%"=="Y" (set DELETE_BEFORE_EXTRACT="Y") else (set DELETE_BEFORE_EXTRACT="N")

set cmd="call %BATCHPATH%EXTRACT_VSS %VSS_DATABASE% %VSS_PROJECT% %VSS_VERSION% %LXSCALE_SOURCE_BASE%  %VSS_INSTALL_DIR% %DELETE_BEFORE_EXTRACT% %INTERACTIVE% %PAUSE_ON_CMPL% %CAPTURE_FILE%"
call %BATCHPATH%EXTRACT_VSS "%VSS_DATABASE%" "%VSS_PROJECT%" "%VSS_VERSION%" "%LXSCALE_SOURCE_BASE%" "%VSS_INSTALL_DIR%" %DELETE_BEFORE_EXTRACT% %INTERACTIVE% %PAUSE_ON_CMPL% "%CAPTURE_FILE%"
echo extraction complete... RTN_ERRORLEVEL = %RTN_ERRORLEVEL% %rd:~1,-1%
if NOT %RTN_ERRORLEVEL%==0  goto error

:buildMOMs

if NOT "%BUILD_MOMS%"=="Y" goto updateBuildNumber

set cmd="call %BATCHPATH%BUILD_MOMS %LXSCALE_SOURCE_BASE% %JDK_BASE% %INTERACTIVE% %PAUSE_ON_CMPL% %CAPTURE_FILE%"
call "%BATCHPATH%"BUILD_MOMS "%LXSCALE_SOURCE_BASE%" "%JDK_BASE%" %INTERACTIVE% %PAUSE_ON_CMPL% "%CAPTURE_FILE%"
echo build moms complete... RTN_ERRORLEVEL = %RTN_ERRORLEVEL% %rd:~1,-1%
if NOT %RTN_ERRORLEVEL%==0  goto error

:updateBuildNumber
rem ***************************************************************
rem
rem Update the BuildNumber
rem This is a non-continuation error.  
rem 
rem ***************************************************************

if "%NEW_BUILDNBR%"=="N" goto updateVersionIni

set cmd="call %BATCHPATH%Checkout_Vss %VSS_DATABASE% %VSS_PROJECT%\Application\source\app %VSS_VERSION% CHECKOUT %LXSCALE_SOURCE_BASE%\Application\source\app\buildnumber.cpp %VSS_INSTALL_DIR% %CAPTURE_FILE%"
call "%BATCHPATH%"Checkout_Vss "%VSS_DATABASE%" "%VSS_PROJECT%\Application\source\app" "%VSS_VERSION%" "CHECKOUT" "%LXSCALE_SOURCE_BASE%\Application\source\app\buildnumber.cpp" "%VSS_INSTALL_DIR%" "%CAPTURE_FILE%" 
if NOT %RTN_ERRORLEVEL%==0 goto error

set cmd="call %BATCHPATH%Update_Buildnumber %LXSCALE_SOURCE_BASE%\Application\source\app\buildnumber.cpp"
call %BATCHPATH%Update_Buildnumber %LXSCALE_SOURCE_BASE%\Application\source\app\buildnumber.cpp  %rd:~1,-1%
if NOT %RTN_ERRORLEVEL%==0 goto error

set cmd="call Checkin_Vss %VSS_DATABASE% %VSS_PROJECT%\Application\source\app %VSS_VERSION% %VSS_COMMENT% %LXSCALE_SOURCE_BASE%\Application\source\app\buildnumber.cpp %VSS_INSTALL_DIR% %CAPTURE_FILE%"
call %BATCHPATH%Checkin_Vss "%VSS_DATABASE%" "%VSS_PROJECT%\Application\source\app" "%VSS_VERSION%" "%VSS_COMMENT%" "%LXSCALE_SOURCE_BASE%\Application\source\app\buildnumber.cpp" "%VSS_INSTALL_DIR%" "%CAPTURE_FILE%"
if NOT %RTN_ERRORLEVEL%==0 goto error

:updateVersionIni
rem ***************************************************************
rem
rem Update the version.ini file with the new version number
rem The minor revision is incremented by 1.  
rem 
rem ***************************************************************

if "%UPDATE_VERSION_INI%"=="N" goto buildSolution

set cmd="call Checkout_Vss %VSS_DATABASE% %VSS_PROJECT%\Application\source\core\VersionManager %VSS_VERSION% CHECKOUT %LXSCALE_SOURCE_BASE%\Application\source\core\VersionManager\Version.ini "%VSS_INSTALL_DIR%" %CAPTURE_FILE%"
call %BATCHPATH%Checkout_Vss "%VSS_DATABASE%" "%VSS_PROJECT%\Application\source\core\VersionManager" "%VSS_VERSION%" "CHECKOUT" "%LXSCALE_SOURCE_BASE%\Application\source\core\VersionManager\Version.ini" "%VSS_INSTALL_DIR%" %CAPTURE_FILE%
if NOT %RTN_ERRORLEVEL%==0 echo Failed to checkout version.ini from source safe %rd:~1,-1%

set NEW_VERSION=0.0
set cmd="call %BATCHPATH%Create_VersionINI %LXSCALE_SOURCE_BASE%\Application\source\core\VersionManager\Version.ini"
call %BATCHPATH%Create_VersionINI "%LXSCALE_SOURCE_BASE%\Application\source\core\VersionManager\Version.ini" "%NEW_VERSION%"  %rd:~1,-1%
if NOT %RTN_ERRORLEVEL%==0 echo Failed to update version.ini from source safe %rd:~1,-1%

set cmd="call %BATCHPATH%Checkin_Vss %VSS_DATABASE% %VSS_PROJECT%\Application\source\core\VersionManager %VSS_VERSION% %VSS_COMMENT% %LXSCALE_SOURCE_BASE%\Application\source\core\VersionManager\Version.ini %VSS_INSTALL_DIR%"
call %BATCHPATH%Checkin_Vss "%VSS_DATABASE%" "%VSS_PROJECT%\Application\source\core\VersionManager" "%VSS_VERSION%" "%VSS_COMMENT%" "%LXSCALE_SOURCE_BASE%\Application\source\core\VersionManager\Version.ini" "%VSS_INSTALL_DIR%" %CAPTURE_FILE%
if NOT %RTN_ERRORLEVEL%==0 echo Failed to check in version.ini from source safe %rd:~1,-1%


rem *********************************************************************
rem * build solution
rem *********************************************************************

:buildSolution

set cmd="call %BATCHPATH%Build_DevStudio_Solution %DEV_STUDIO_INSTALL_DIR% %LXSCALE_SOURCE_BASE%\Application %MAKE_CLEAN% %SOLUTION_CONFIGURATION% %INTERACTIVE% %PAUSE_ON_CMPL% %CAPTURE_FILE%"
call %BATCHPATH%Build_DevStudio_Solution "%DEV_STUDIO_INSTALL_DIR%" "%LXSCALE_SOURCE_BASE%\Application" %MAKE_CLEAN% %SOLUTION_CONFIGURATION% %INTERACTIVE% %PAUSE_ON_CMPL% "%CAPTURE_FILE%"

set cmd="call %BATCHPATH%build_weighusb %WINDDK_BASE% %LXSCALE_SOURCE_BASE%\WeighUSB\WeighUSB %SOLUTION_CONFIGURATION% %INTERACTIVE% %PAUSE_ON_CMPL% %CAPTURE_FILE%"
call %BATCHPATH%build_weighusb "%WINDDK_BASE%" "%LXSCALE_SOURCE_BASE%\WeighUSB\WeighUSB" %SOLUTION_CONFIGURATION% %INTERACTIVE% %PAUSE_ON_CMPL% "%CAPTURE_FILE%"


rem *********************************************************************
rem * package
rem *********************************************************************

set cmd="call %BATCHPATH%package %LOCAL_TARGET_BASE% %LXSCALE_SOURCE_BASE% %SOLUTION_CONFIGURATION% %ZIP_INSTALL_DIR% %INTERACTIVE% %PAUSE_ON_CMPL% %CAPTURE_FILE%"
call %BATCHPATH%package "%LOCAL_TARGET_BASE%" "%LXSCALE_SOURCE_BASE%" %SOLUTION_CONFIGURATION% "%ZIP_INSTALL_DIR%" %INTERACTIVE% %PAUSE_ON_CMPL% "%CAPTURE_FILE%"

if not "%UPDATE_REMOTE_SITE%"=="Y" goto end

rem *********************************************************************
rem * update remote image
rem *********************************************************************

if NOT "%INTERACTIVE%"=="Y" goto updateRemote
set /p ANS="Updating remote location do you still want to continue? (Y/N) "
if not "%ANS%"=="Y" if not "%ANS%"=="y" ( echo Aborted.
                                          set RTN_ERRORLEVEL=1
                                          goto end )

:updateRemote

if NOT %CAPTURE_FILE%=="" (echo copying package from %LOCAL_TARGET_BASE%\Access Scale to %REMOTE_SITE_BASE%\resealed image\ProgramFiles\Hobart\Access Scale %rd:~1,-1%)
echo copying package from %LOCAL_TARGET_BASE%\Access Scale to %REMOTE_SITE_BASE%\resealed image\ProgramFiles\Hobart\Access Scale

set cmd="rd /S /Q %REMOTE_SITE_BASE%\resealed image\Program Files\Hobart\Access Scale %rd:~1,-1%"
rd /S /Q "%REMOTE_SITE_BASE%\resealed image\Program Files\Hobart\Access Scale" %rd:~1,-1%

set cmd="xcopy /E /I /Q /H /R /Y %LOCAL_TARGET_BASE%\Access Scale %REMOTE_SITE_BASE%\resealed image\Program Files\Hobart\Access Scale %rd:~1,-1%"
xcopy /E /I /Q /H /R /Y "%LOCAL_TARGET_BASE%\Access Scale" "%REMOTE_SITE_BASE%\resealed image\Program Files\Hobart\Access Scale" %rd:~1,-1%
if NOT %errorlevel%==0 goto error

set cmd="del %REMOTE_SITE_BASE%\resealed image\windows\INF\wghusb.inf"
del "%REMOTE_SITE_BASE%\resealed image\windows\INF\wghUsb.inf" %rd:~1,-1%

set cmd="del %REMOTE_SITE_BASE%\resealed image\windows\system32\drivers\weighUsb.sys"
del "%REMOTE_SITE_BASE%\resealed image\windows\system32\drivers\weighUsb.sys" %rd:~1,-1%

set cmd="%REMOTE_SITE_BASE%\resealed image\windows\system32\drivers\smbus.sys"
del "%REMOTE_SITE_BASE%\resealed image\windows\system32\drivers\smbus.sys" %rd:~1,-1%

set cmd="copy /Y %LXSCALE_SOURCE_BASE%\SMBUS\driver\objfre_wxp_x86\i386\smbus.sys %REMOTE_SITE_BASE%\resealed image\windows\system32\drivers\smbus.sys"
copy /Y "%LXSCALE_SOURCE_BASE%\SMBUS\driver\objfre_wxp_x86\i386\smbus.sys" "%REMOTE_SITE_BASE%\resealed image\windows\system32\drivers\smbus.sys" %rd:~1,-1%

set cmd="copy /Y %LXSCALE_SOURCE_BASE%\WeighUsb\WeighUSB\objfre_wxp_x86\i386\weighUsb.sys %REMOTE_SITE_BASE%\resealed image\windows\system32\drivers\weighUsb.sys"
copy /Y "%LXSCALE_SOURCE_BASE%\WeighUsb\WeighUSB\objfre_wxp_x86\i386\weighUsb.sys" "%REMOTE_SITE_BASE%\resealed image\windows\system32\drivers\weighUsb.sys" %rd:~1,-1%

set cmd="copy /Y %LXSCALE_SOURCE_BASE%\WeighUsb\WeighUSB\wghusb.inf %REMOTE_SITE_BASE%\resealed image\windows\INF\wghusb.inf"
copy /Y "%LXSCALE_SOURCE_BASE%\WeighUsb\WeighUSB\wghusb.inf" "%REMOTE_SITE_BASE%\resealed image\windows\INF\wghusb.inf" %rd:~1,-1%

if NOT EXIST "%REMOTE_SITE_BASE%\upgrade" (mkdir "%REMOTE_SITE_BASE%\upgrade")

if EXIST "%REMOTE_SITE_BASE%\upgrade" (

set cmd="copy /Y %LOCAL_TARGET_BASE% upgrade\Hobart.zip %REMOTE_SITE_BASE%\upgrade\Hobart.zip %rd:~1,-1%"
copy /Y "%LOCAL_TARGET_BASE% upgrade\Hobart.zip" "%REMOTE_SITE_BASE%\upgrade\Hobart.zip"

set cmd="copy /Y %LOCAL_TARGET_BASE% upgrade\Hobart.ini %REMOTE_SITE_BASE%\upgrade\Hobart.ini %rd:~1,-1%"
copy /Y "%LOCAL_TARGET_BASE% upgrade\Hobart.ini" "%REMOTE_SITE_BASE%\upgrade\Hobart.ini"

								)


if NOT EXIST "%FTP_SERVER_DIR%\upgrade" (mkdir "%FTP_SERVER_DIR%\upgrade")

if EXIST "%FTP_SERVER_DIR%\upgrade" (

set cmd="copy /Y %LOCAL_TARGET_BASE% upgrade\Hobart.zip %FTP_SERVER_DIR%\upgrade\Hobart.zip %rd:~1,-1%"
copy /Y "%LOCAL_TARGET_BASE% upgrade\Hobart.zip" "%FTP_SERVER_DIR%\upgrade\Hobart.zip"

set cmd="copy /Y %LOCAL_TARGET_BASE% upgrade\Hobart.ini %FTP_SERVER_DIR%\upgrade\Hobart.ini %rd:~1,-1%"
copy /Y "%LOCAL_TARGET_BASE% upgrade\Hobart.ini" "%FTP_SERVER_DIR%\upgrade\Hobart.ini"

								)

goto end

:error


if NOT "%CAPTURE_FILE%"=="" ( echo . %rd:~1,-1%
                             echo Processing aborted. %rd:~1,-1%
                             if NOT %CMD%==0 echo ERROR on CMD: %cmd% %rd:~1,-1%
                             echo .  %rd:~1,-1%)
 
echo .
echo Processing aborted.
if NOT %CMD%==0 echo ERROR on CMD: %cmd%

echo .
goto :eof

rem ***************************************************************
rem
rem  The user did not pass all the parameters
rem
rem ***************************************************************
:nada_to_do
echo INVALID USAGE -- NOT ALL PARAMETERS ARE DEFINED.
echo USAGE: Main p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 p11 p12 p13 p14 p15 p16 p17 p18
echo                    p1 is EXTRACT_VSS_FLAG "Y" = extract form VSS, "N" = dont extract.
echo                    p2 is VSS_DATABASE the name of the vss database
echo                    p3 is VSS_VERSION the label of the project to get.
echo                    p4 is LXSCALE_SOURCE_BASE the local directory where the vss project is to be extracted to.
echo                    p5 is VSS_INSTALL_DIR the directory where VSS is installed.
echo                    p6 is DEV_STUDIO_INSTALL_DIR the directory where developer studio is installed.
echo                    p7 is MAKE_CLEAN "Y" = perform a clean build. LXSCALE_SOURCE_BASE is deleted. "N" = perform incremental build
echo                    p8 is SOLUTION_CONFIGURATION the configuration of the win32 app project. typically "Debug" or "Release"
echo                    p9 is JDK_BASE the directory where the java development kit is installed.
echo                    p10 is BUILD_MOMS "Y" build the moms using the JDK_BASE. "N" skip.
echo                    p11 is LOCAL_TARGET_BASE where on the local machine the results of the build are packaged.
echo                    p12 is WINDDK_BASE where the microsoft driver development kit is installed.
echo                    p13 is UPDATE_REMOTE_SITE "Y" copy LOCAL_TARGET_BASE to this remote location. "N" skip.
echo                    p14 is REMOTE_SITE_BASE where to copy the package results to.
echo                    p15 is NEW_BUILDNBR "Y" update the buildnumber with the current date/time. "N" skip.
echo                    p16 is UPDATE_VERSION_INI "Y" update the Version.ini with new version. "N" skip.
echo                    p17 is INTERACTIVE "Y" prompts user to continue at various build stages.
echo                    p18 is PAUSE_ON_CMPL "Y" pauses after completion of various build stages.
echo .

set RTN_ERRORLEVEL=1

:end

if NOT %CAPTURE_FILE%=="" (
	if %RTN_ERRORLEVEL%==0 (  echo .
                                  echo universal build COMPLETED at %DATE% %TIME%. )
	if NOT %RTN_ERRORLEVEL%==0  ( echo .  
        	                      echo universal build had errors.  
                	              echo .  
                        	      echo Error trying to execute: %cmd% )
                            )


if %RTN_ERRORLEVEL%==0 (  echo .
                          echo universal build COMPLETED at %DATE% %TIME%.  %rd:~1,-1% )
if NOT %RTN_ERRORLEVEL%==0  ( echo .  %rd:~1,-1%
                              echo universal build had errors.  %rd:~1,-1%
                              echo .  %rd:~1,-1%
                              echo Error trying to execute: %cmd%  %rd:~1,-1%)
echo .
cd\
cd %INIT_DIR% 
%INIT_DIR:~0,2%
Path=%SAVED_PATH%
if "%PAUSE_ON_CMPL%"=="Y" pause