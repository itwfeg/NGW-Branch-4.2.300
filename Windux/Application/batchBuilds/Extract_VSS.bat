@echo OFF
rem ***************************************************************
rem Parameters:
rem
rem	%1 -> Source Safe Database Name to extract from.
rem	%2 -> Source Safe Project Name to extract.
rem	%3 -> Source Save Version Label of Project Name to extract.
rem	%4 -> Directory to put the source code of the projects
rem	%5 -> Path to VSS
rem     %6 -> DELETE_BEFORE_EXTRACTION == "Y" or "N"
rem	%7 -> INTERACTIVE == "Y" or "N" 
rem	%8 -> PAUSE_ON_CMPL == "Y" or "N" 
rem	%9 -> CAPTURE_FILE == Filename to capture output.
rem	                      If not supplied, and no env variable defined, output goes to the screen.
rem
rem
rem
rem 	Modification		Reason			Who
rem ---------------------------------------------------------------
rem	OCT 25, 2006		Initial Release		JRB
rem
rem ***************************************************************
rem
rem Initialize variables
rem
rem	PAth is changed to included VSS bin directory.
rem	ssdir is the location of the VSS database to access
rem     (VSS supports command-line commands by using the SS.exe program, 
rem      which is usually in the Win32 subfolder under the folder you 
rem      choose in the setup program.
rem
rem ***************************************************************
set INIT_DIR=%cd%
set SAVED_PATH=%path%
set CAPTURE_FILE=""

if "%~1" == "" goto nada_to_do
if "%~2" == "" goto nada_to_do
if "%~3" == "" goto nada_to_do
if "%~4" == "" goto nada_to_do
if "%~5" == "" goto nada_to_do
if "%~6" == "" goto nada_to_do
if "%~7" == "" goto nada_to_do
if "%~8" == "" goto nada_to_do
if "%9" == "" set RD=""
if NOT "%9" == "" set CAPTURE_FILE=%~9
if NOT "%9" == "" set RD=">> %CAPTURE_FILE% 2>&1"

set PROJ_DB=%~1
set PROJ_NAME=%~2
set PROJ_VER=%~3

set SRC_TRG=%~4
set SRC_TRG_DRV=%~d4
set SRC_TRG_DIR=%~p4
set PATH_TO_VSS=%~5
set DELETE_BEFORE_EXTRACTION=%~6
set INTERACTIVE=%~7
set PAUSE_ON_CMPL=%~8

rem ***************************************************************
rem
rem The path must have access to the VSS ss.exe
rem The ssdir must be defined as the VSS database to query.
rem \\Embeddedserver\embedded VSS\Evolution VSS Database
rem
rem ***************************************************************

PATH=%PATH_TO_VSS%;%PATH%
set ssdir=%PROJ_DB%

rem ***************************************************************
rem
rem Echo control parameters
rem
rem ***************************************************************

if NOT %CAPTURE_FILE%=="" (
               echo .
               echo .............Extracting from VSS ...................
               echo .
               echo PROJ_DATABASE: %PROJ_DB%
               echo PROJ_NAME: %PROJ_NAME%
               echo PROJ_VERSION: %PROJ_VER%
               echo SRC_TRG: %SRC_TRG%
               echo PATH_TO_VSS: %PATH_TO_VSS%
               echo DELETE_BEFORE_EXTRACTION: %DELETE_BEFORE_EXTRACTION%
               echo INTERACTIVE: %INTERACTIVE%
               echo PAUSE_ON_CMPL: %PAUSE_ON_CMPL%
               if %CAPTURE_FILE%=="" echo CAPTURE_FILE = Screen
               if NOT %CAPTURE_FILE%=="" echo CAPTURE_FILE = %CAPTURE_FILE%  
               echo . )

echo . %rd:~1,-1%
echo .............Extracting from VSS ................... %rd:~1,-1%
echo . %rd:~1,-1%
echo PROJ_DATABASE: %PROJ_DB% %rd:~1,-1%
echo PROJ_NAME: %PROJ_NAME% %rd:~1,-1%
echo PROJ_VERSION: %PROJ_VER% %rd:~1,-1%
echo SRC_TRG: %SRC_TRG% %rd:~1,-1%
echo PATH_TO_VSS: %PATH_TO_VSS% %rd:~1,-1%
echo DELETE_BEFORE_EXTRACTION: %DELETE_BEFORE_EXTRACTION% %rd:~1,-1%
echo INTERACTIVE: %INTERACTIVE% %rd:~1,-1%
echo PAUSE_ON_CMPL: %PAUSE_ON_CMPL% %rd:~1,-1%
if %CAPTURE_FILE%=="" echo CAPTURE_FILE = Screen %rd:~1,-1%
if NOT %CAPTURE_FILE%=="" echo CAPTURE_FILE = %CAPTURE_FILE% %rd:~1,-1%
echo . %rd:~1,-1%

if NOT "%INTERACTIVE%"=="Y" goto check
set /p ANS="Do you still want to continue? (Y/N) "
if not "%ANS%"=="Y" if not "%ANS%"=="y" ( echo Aborted.
                                          set RTN_ERRORLEVEL=1
                                          goto end )

rem ***************************************************************
rem
rem Delete first the old source directory before checking out new source
rem This is to make sure that deleted files in source safe dont linger
rem We will not delete object modules under workspaces. If we do we
rem force a make clean even if not selected.
rem
rem ***************************************************************

:Check
cd\

if "%DELETE_BEFORE_EXTRACTION%" == "Y" (if EXIST "%SRC_TRG%" (echo deleteing %SRC_TRG%....
                                                              rd /S /Q "%SRC_TRG%\"
                                                              )
                                        )else (if EXIST "%SRC_TRG%" (echo removing any non-compiled source...
                                                                 rd /S /Q "%SRC_TRG%\Application\source\mom"
                                                                 rd /S /Q "%SRC_TRG%\Application\webPages"
                                                                 rd /S /Q "%SRC_TRG%\Application\scaleDB.empty"
                                                                 )
                                               )

if not exist "%SRC_TRG%" (mkdir "%SRC_TRG%")


rem ***************************************************************
rem
rem Extraction Processing.
rem
rem ***************************************************************
 
if NOT %CAPTURE_FILE%=="" ( echo .  
                            echo Extracting from VSS.... 
                            echo You will be notified when completed. )

echo .  %rd:~1,-1%
echo Extracting from VSS....  %rd:~1,-1%

if "%INTERACTIVE%"=="Y" ( ss cp %PROJ_NAME% -I-Y  %rd:~1,-1% ) else ( ss cp %PROJ_NAME% -I-Y  %rd:~1,-1% )
ss Dir  %rd:~1,-1%
if "%INTERACTIVE%"=="Y" ( ss Project -I-Y %rd:~1,-1% ) else ( ss Project  %rd:~1,-1% )

rem ***************************************************************
rem
rem  Move to the source directory and start extraction of all projects.
rem
rem ***************************************************************

cd "%SRC_TRG%" 
%SRC_TRG_DRV% 

rem ss Workfold %PROJ_NAME% "%SRC_TRG%"
if %CAPTURE_FILE%=="" goto NoCapture1
if "%PROJ_VER%"=="*" if "%INTERACTIVE%"=="Y" ( ss Get * -GTU -r -gwa -O@%CAPTURE_FILE% )
if "%PROJ_VER%"=="*" if NOT "%INTERACTIVE%"=="Y" ( ss Get * -GTU -r -gwa -I-Y -O@%CAPTURE_FILE% )
if NOT "%PROJ_VER%"=="*" if "%INTERACTIVE%"=="Y" ( ss Get * -GTU -V%PROJ_VER% -r -gwa -O@%CAPTURE_FILE% )
if NOT "%PROJ_VER%"=="*" if NOT "%INTERACTIVE%"=="Y" ( ss Get * -GTU -V%PROJ_VER% -r -gwa -I-Y -O@%CAPTURE_FILE% )
goto next_Step1

:NoCapture1
if "%PROJ_VER%"=="*" if "%INTERACTIVE%"=="Y" ( ss Get * -GTU -r -gwa )
if "%PROJ_VER%"=="*" if NOT "%INTERACTIVE%"=="Y" ( ss Get * -GTU -r -gwa -I-Y )
if NOT "%PROJ_VER%"=="*" if "%INTERACTIVE%"=="Y" ( ss Get * -GTU -V%PROJ_VER% -r -gwa )
if NOT "%PROJ_VER%"=="*" if NOT "%INTERACTIVE%"=="Y" ( ss Get * -GTU -V%PROJ_VER% -r -gwa -I-Y )

:next_step1
if NOT %CAPTURE_FILE%=="" ( echo Project %PROJ_NAME% was extracted.
                            echo . )
echo Project %PROJ_NAME% was extracted. %rd:~1,-1%
echo . %rd:~1,-1%


goto end

rem ***************************************************************
rem
rem  The user did not pass all the parameters
rem
rem ***************************************************************
:nada_to_do
echo INVALID USAGE -- NOT ALL PARAMETERS ARE DEFINED.
echo USAGE: Extract_Vss p1 p2 p3 p4 p5 p6 p7 p8 p9
echo                    p1 is the Source Safe Database
echo                    p2 is the Source Safe Project Name
echo                    p3 is the Source Safe Version Label
echo                    p4 is the target (local machine) directory value.
echo                    p5 is the directory where visual source safe was installed (local machine) value.
echo                    p6 is the delete before extract Y=delete then extract N=extract over existing source
echo                    p7 is Interactive Flag. Y=User prompts  N=No Prompts
echo                    p8 is the pause on completion. Y=pause
echo                    p9 is the file to capture output (screen if not supplied)
echo .
set RTN_ERRORLEVEL=1

:end
cd\
cd %INIT_DIR%
Path=%SAVED_PATH%
if "%PAUSE_ON_CMPL%"=="Y" if "%INTERACTIVE%"=="Y" pause

