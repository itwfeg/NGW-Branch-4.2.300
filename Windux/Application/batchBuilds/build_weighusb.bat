@echo OFF
rem ***************************************************************
rem Parameters:  ALL INPUT PARAMETERS **MUST** BE IN QUOTES ON THE CMD LINE
rem
rem	%1 -> WINDDK_BASE == 'C:\WINDDK\3790.1830'
rem
rem	%2 -> WEIGH_USB_BASE == 'C:\HOBART\WeighUSB\WeighUSB'
rem
rem	%3 -> BUILD_TYPE== "Debug" - debug version
rem                        "Release" - release version
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
set INIT_DIR=%cd%

if "%~1" == "" goto nada_to_do
if "%~2" == "" goto nada_to_do
if "%~3" == "" goto nada_to_do
if "%~4" == "" goto nada_to_do
if "%~5" == "" goto nada_to_do
if "%~6" == "" set RD=""
if NOT "%~6" == "" set CAPTURE_FILE=%~6
if NOT "%~6" == "" set RD=">> %CAPTURE_FILE% 2>&1"


set WINDDK_BASE=%~1
set WEIGH_USB_BASE=%~2
set BUILD_TYPE=%~3
set INTERACTIVE=%~4
set PAUSE_ON_CMPL=%~5



if NOT %CAPTURE_FILE%=="" (
	echo . 
        echo ...... Building Solution at %DATE% %TIME% ....... 
	echo . 
	echo WINDDK_BASE = %WINDDK_BASE% 
	echo WEIGH_USB_BASE = %WEIGH_USB_BASE% 
        echo BUILD_TYPE = %BUILD_TYPE%
        echo INTERACTIVE = %INTERACTIVE%
        echo PAUSE_ON_CMPL = %PAUSE_ON_CMPL%
        if %CAPTURE_FILE%=="" echo CAPTURE_FILE = Screen
        if NOT %CAPTURE_FILE%=="" echo CAPTURE_FILE = %CAPTURE_FILE%  
	echo . )


echo .  %rd:~1,-1%
echo ...... Building Solution at %DATE% %TIME% ....... %rd:~1,-1%
echo .  %rd:~1,-1%
echo WINDDK_BASE = %WINDDK_BASE%  %rd:~1,-1%
echo WEIGH_USB_BASE = %WEIGH_USB_BASE%  %rd:~1,-1%
echo BUILD_TYPE = %BUILD_TYPE% %rd:~1,-1%
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

if "%BUILD_TYPE%"=="Debug" (echo debug...
  if %CAPTURE_FILE%=="" (
    call "%WINDDK_BASE%\bin\setenv.bat" %WINDDK_BASE% chk) else (
    call "%WINDDK_BASE%\bin\setenv.bat" %WINDDK_BASE% chk %rd:~1,-1%)) else (echo release...
  if %CAPTURE_FILE%=="" (
    call "%WINDDK_BASE%\bin\setenv.bat" %WINDDK_BASE% fre) else (
    call "%WINDDK_BASE%\bin\setenv.bat" %WINDDK_BASE% fre %rd:~1,-1%)) 

cd %WEIGH_USB_BASE%
if %CAPTURE_FILE%=="" (build -c) else (build -c)


goto end

:nada_to_do
rem ***************************************************************
rem
rem  The user did not pass in the correct arguments.
rem
rem ***************************************************************
echo * * * * * * * W A R N I N G * * * * * * * 
echo USAGE: Build_DevStudio_Solution p1 p2 p3 p4 p5 p6 
echo .
echo p1 = WINDDK_BASE
echo p2 = WEIGH_USB_BASE
echo p3 = BUILD_TYPE
echo p4 = INTERACTIV
echo p5 = PAUSE_ON_CMPL
echo p6 = CAPTURE_FILE

set RTN_ERRORLEVEL=1

goto :eof

:end
cd\
cd %INIT_DIR%
if "%PAUSE_ON_CMPL%"=="Y" if "%INTERACTIVE%"=="Y" pause
goto :eof
