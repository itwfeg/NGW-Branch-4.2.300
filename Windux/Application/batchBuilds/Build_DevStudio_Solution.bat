@echo OFF
rem ***************************************************************
rem Parameters:  ALL INPUT PARAMETERS **MUST** BE IN QUOTES ON THE CMD LINE
rem
rem	%1 -> DEVSTUDIO_BASE == 'C:\Program Files\Microsoft Visual Studio 8'
rem
rem	%2 -> APP_SOURCE_BASE == 'C:\HOBART\Application'
rem
rem	%3 -> MAKE_CLEAN== "Y" - recompile all source
rem                        "N" - recompile only source that is out of date
rem
rem	%4 -> SOLUTION_CONFIGURATION == Release,Debug
rem
rem	%5 -> INTERACTIVE == "Y" prompt before starting
rem                          "N" dont prompt
rem
rem	%6 -> PAUSE_ON_CMPL == "Y" pause before ending
rem                          "N" dont pause
rem
rem	%7 -> CAPTURE_FILE == Filename to capture output.
rem	                      If not supplied, and no env variable defined, output goes to the screen.
rem
rem 	Modification		Reason			Who
rem ---------------------------------------------------------------
rem	Oct 25, 2006		Initial Release		JRB
rem
rem ***************************************************************
rem
rem Initialize variables
rem
rem ***************************************************************
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

set DEVSTUDIO_BASE=%~1
set APP_SOURCE_BASE=%~2
set MAKE_CLEAN=%~3
set SOLUTION_CONFIGURATION=%~4
set INTERACTIVE=%~5
set PAUSE_ON_CMPL=%~6


if "%MAKE_CLEAN%"=="Y" (set BUILD_TYPE=/Rebuild) else (set BUILD_TYPE=/Build)
		
if NOT %CAPTURE_FILE%=="" (
	echo . 
        echo ...... Building Solution at %DATE% %TIME% ....... 
	echo . 
	echo DEVSTUDIO_BASE = %DEVSTUDIO_BASE% 
	echo APP_SOURCE_BASE = %APP_SOURCE_BASE% 
	echo BUILD_TYPE = %BUILD_TYPE%  
	echo SOLUTION_CONFIGURATION = %SOLUTION_CONFIGURATION% 
        echo INTERACTIVE = %INTERACTIVE%
        echo PAUSE_ON_CMPL = %PAUSE_ON_CMPL%
        if %CAPTURE_FILE%=="" echo CAPTURE_FILE = Screen
        if NOT %CAPTURE_FILE%=="" echo CAPTURE_FILE = %CAPTURE_FILE%  
	echo . )


echo .  %rd:~1,-1%
echo ...... Building Solution at %DATE% %TIME% ....... %rd:~1,-1%
echo .  %rd:~1,-1%
echo DEVSTUDIO_BASE = %DEVSTUDIO_BASE%  %rd:~1,-1%
echo APP_SOURCE_BASE = %APP_SOURCE_BASE%  %rd:~1,-1%
echo BUILD_TYPE = %BUILD_TYPE%  %rd:~1,-1%
echo SOLUTION_CONFIGURATION = %SOLUTION_CONFIGURATION%  %rd:~1,-1%
echo INTERACTIVE = %INTERACTIVE% %rd:~1,-1%
echo PAUSE_ON_CMPL = %PAUSE_ON_CMPL% %rd:~1,-1%
if %CAPTURE_FILE%=="" echo CAPTURE_FILE = Screen  %rd:~1,-1%
if NOT %CAPTURE_FILE%=="" echo CAPTURE_FILE = %CAPTURE_FILE% %rd:~1,-1%
echo .  %rd:~1,-1%

if NOT "%INTERACTIVE%"=="Y" goto start
set /p ANS="Do you still want to continue? (Y/N) "
if not "%ANS%"=="Y" if not "%ANS%"=="y" ( echo Aborted.
                                          set RTN_ERRORLEVEL=1
                                          goto end )

:start

if not exist "%DEVSTUDIO_BASE%\VC\bin\vcvars32.bat" goto missing
call "%DEVSTUDIO_BASE%\VC\bin\vcvars32.bat" %rd:~1,-1%

if NOT %CAPTURE_FILE%=="" ( echo .  
                            echo building solution.... 
                            echo You will be notified when completed. )
 
set HobartApp=%APP_SOURCE_BASE%

DevEnv "%APP_SOURCE_BASE%\AccessCounter.sln" %BUILD_TYPE% %SOLUTION_CONFIGURATION% %rd:~1,-1%
if NOT %errorlevel%==0 goto error_handler

goto end

:missing
echo The specified configuration type is missing.  The tools for the
echo configuration might not be installed.
goto end

:nada_to_do
rem ***************************************************************
rem
rem  The user did not pass in the correct arguments.
rem
rem ***************************************************************
echo * * * * * * * W A R N I N G * * * * * * * 
echo USAGE: Build_DevStudio_Solution p1 p2 p3 p4 p5 p6 p7
echo .
echo p1 = DEVSTUDIO_BASE
echo p2 = APP_SOURCE_BASE
echo p3 = MAKE_CLEAN
echo p4 = SOLUTION_CONFIGURATION
echo p5 = INTERACTIV
echo p6 = PAUSE_ON_CMPL
echo p7 = CAPTURE_FILE

set RTN_ERRORLEVEL=1

goto :eof

:end
if "%PAUSE_ON_CMPL%"=="Y" if "%INTERACTIVE%"=="Y" pause
goto :eof
