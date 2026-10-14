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
               echo ............. BuildTestCode ...................
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
echo ............. BuildTestCode ................... %rd:~1,-1%
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

if NOT "%INTERACTIVE%"=="Y" goto updateProjectFile
set /p ANS="Do you still want to continue? (Y/N) "
if not "%ANS%"=="Y" if not "%ANS%"=="y" ( echo Aborted.
                                          set RTN_ERRORLEVEL=1
                                          goto end )
:updateProjectFile


set cmd="%LXSCALE_SOURCE_BASE%\Application\modifyBuildFlags\Release\modifyBuildFlags.exe %LXSCALE_SOURCE_BASE%\Application\source\EvolutionRTC\EvolutionRTC.vcproj FAKE_SOCKET_WEIGHER"
"%LXSCALE_SOURCE_BASE%\Application\modifyBuildFlags\Release\modifyBuildFlags.exe" "%LXSCALE_SOURCE_BASE%\Application\source\EvolutionRTC\EvolutionRTC.vcproj" FAKE_SOCKET_WEIGHER 

set cmd="%LXSCALE_SOURCE_BASE%\Application\source\EvolutionRTC\EvolutionRTC.vcproj attrib -r %LXSCALE_SOURCE_BASE%\Application\source\EvolutionRTC\EvolutionRTC.vcproj"
if EXIST "%LXSCALE_SOURCE_BASE%\Application\source\EvolutionRTC\EvolutionRTC.vcproj" (attrib -r "%LXSCALE_SOURCE_BASE%\Application\source\EvolutionRTC\EvolutionRTC.vcproj"
			del /F /Q "%LXSCALE_SOURCE_BASE%\Application\source\EvolutionRTC\EvolutionRTC.vcproj"
			)

set cmd="mov %LXSCALE_SOURCE_BASE%\Application\source\EvolutionRTC\EvolutionRTC_New.vcproj %LXSCALE_SOURCE_BASE%\Application\source\EvolutionRTC\EvolutionRTC.vcproj"
move "%LXSCALE_SOURCE_BASE%\Application\source\EvolutionRTC\EvolutionRTC_New.vcproj" "%LXSCALE_SOURCE_BASE%\Application\source\EvolutionRTC\EvolutionRTC.vcproj"


rem *********************************************************************
rem * build solution
rem *********************************************************************

:buildSolution

set cmd="call %BATCHPATH%Build_DevStudio_Solution %DEV_STUDIO_INSTALL_DIR% %LXSCALE_SOURCE_BASE%\Application Y %SOLUTION_CONFIGURATION% %INTERACTIVE% %PAUSE_ON_CMPL% %CAPTURE_FILE%"
call %BATCHPATH%Build_DevStudio_Solution "%DEV_STUDIO_INSTALL_DIR%" "%LXSCALE_SOURCE_BASE%\Application" Y %SOLUTION_CONFIGURATION% %INTERACTIVE% %PAUSE_ON_CMPL% "%CAPTURE_FILE%"

rem *********************************************************************
rem * package
rem *********************************************************************

:package

echo copying %LXSCALE_SOURCE_BASE%\Application\source\app\%SOLUTION_CONFIGURATION%\ScaleService.exe...

set cmd="%LOCAL_TARGET_BASE% upgrade\Access Scale\ScaleService.exe attrib -r %LOCAL_TARGET_BASE% upgrade\Access Scale\ScaleService.exe"
if EXIST "%LOCAL_TARGET_BASE% upgrade\Access Scale\ScaleService.exe" (attrib -r "%LOCAL_TARGET_BASE% upgrade\Access Scale\ScaleService.exe"
		del /F /Q "%LOCAL_TARGET_BASE% upgrade\Access Scale\ScaleService.exe"
		)

set cmd="copy /Y %LXSCALE_SOURCE_BASE%\Application\source\app\%SOLUTION_CONFIGURATION%\ScaleService.exe %LOCAL_TARGET_BASE% upgrade\Access Scale\ScaleService.exe %rd:~1,-1%"
copy /Y "%LXSCALE_SOURCE_BASE%\Application\source\app\%SOLUTION_CONFIGURATION%\ScaleService.exe" "%LOCAL_TARGET_BASE% upgrade\Access Scale\ScaleService.exe" %rd:~1,-1%
if NOT %errorlevel%==0 goto error_handler

if EXIST "%LOCAL_TARGET_BASE% upgrade\hobart.zip" del /F /Q "%LOCAL_TARGET_BASE% upgrade\hobart.zip"

set cmd="%ZIP_INSTALL_DIR%\7z a -tzip %LOCAL_TARGET_BASE% upgrade\hobart.zip %LOCAL_TARGET_BASE% upgrade\Access Scale\* %rd:~1,-1%"
"%ZIP_INSTALL_DIR%\7z" a -tzip "%LOCAL_TARGET_BASE% upgrade\hobart.zip" "%LOCAL_TARGET_BASE% upgrade\Access Scale\*" %rd:~1,-1% 
if NOT %errorlevel%==0 goto error_handler


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



if NOT EXIST "%REMOTE_SITE_BASE%\upgradeTest" (mkdir "%REMOTE_SITE_BASE%\upgradeTest")

if EXIST "%REMOTE_SITE_BASE%\upgradeTest" (

set cmd="copy /Y %LOCAL_TARGET_BASE% upgrade\Hobart.zip %REMOTE_SITE_BASE%\upgradeTest\Hobart.zip %rd:~1,-1%"
copy /Y "%LOCAL_TARGET_BASE% upgrade\Hobart.zip" "%REMOTE_SITE_BASE%\upgradeTest\Hobart.zip"

set cmd="copy /Y %LOCAL_TARGET_BASE% upgrade\Hobart.ini %REMOTE_SITE_BASE%\upgradeTest\Hobart.ini %rd:~1,-1%"
copy /Y "%LOCAL_TARGET_BASE% upgrade\Hobart.ini" "%REMOTE_SITE_BASE%\upgradeTest\Hobart.ini"

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