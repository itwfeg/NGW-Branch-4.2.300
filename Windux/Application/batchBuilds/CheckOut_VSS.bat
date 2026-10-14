@echo OFF
rem ***************************************************************
rem Parameters:
rem
rem	%1 -> Source Safe Database Name.
rem	%2 -> Source Safe Project Name (full path) containing item to checked out.
rem	%3 -> Source Save Version Label of Project Name to checkout
rem	%4 -> Checkout command to perform ('CHECKOUT' 'UNCHECKOUT')
rem	%5 -> Filename (full path) to checkout in Source Safe.
rem           The filename will be extracted to the full path location supplied
rem           in this parameter.
rem     %6 -> Path to the VSS installation directory
rem	%7 -> CAPTURE_FILE == Filename to capture output.
rem	                      If not supplied, and no env variable defined, output goes to the screen.
rem
rem 	Modification		Reason			Who
rem ---------------------------------------------------------------
rem	OCT, 24 		Initial Release		JRB
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
if "%7" == "" set RD=""
if NOT "%7" == "" set CAPTURE_FILE=%~7
if NOT "%7" == "" set RD=">> %CAPTURE_FILE% 2>&1"

set PROJ_DB=%~1
set PROJ_NAME=%~2
set PROJ_VER=%~3
set CHECKOUT_CMD=%~4
set FILE_DRIVE=%~d5
set FILE_DIR=%~p5
set FILE_NAME=%~nx5
set PATH_TO_VSS=%~6

rem ***************************************************************
rem
rem The path must have access to the VSS ss.exe
rem The ssdir must be defined as the VSS database to query.
rem \\Embeddedserver\embedded VSS\Evolution VSS Database
rem
rem ***************************************************************

PATH=%PATH_TO_VSS%;%PATH%
set ssdir=%PROJ_DB%

if NOT "%CHECKOUT_CMD%"=="CHECKOUT" if NOT "%CHECKOUT_CMD%"=="UNDOCHECKOUT" ( set cmd="Not a valid Option for CHECKOUT_VSS"
                                                                              goto errors )

rem ***************************************************************
rem
rem Echo control parameters
rem
rem ***************************************************************

if NOT %CAPTURE_FILE%=="" (
                     echo .
                     echo ..... %CHECKOUT_CMD% file: %FILE_NAME% .........
                     echo .
                     echo PROJ_DATABASE: %PROJ_DB%
                     echo PROJ_NAME: %PROJ_NAME%
                     echo PROJ_VERSION: %PROJ_VER%
                     echo FILE_DRIVE: %FILE_DRIVE%
                     echo FILE_DIR: %FILE_DIR%
                     echo FILE_NAME: %FILE_NAME%
                     echo PATH_TO_VSS: %PATH_TO_VSS%
                     echo . )


echo . %rd:~1,-1%
echo ..... %CHECKOUT_CMD% file: %FILE_NAME% ......... %rd:~1,-1%
echo . %rd:~1,-1%
echo PROJ_DATABASE: %PROJ_DB% %rd:~1,-1%
echo PROJ_NAME: %PROJ_NAME% %rd:~1,-1%
echo PROJ_VERSION: %PROJ_VER% %rd:~1,-1%
echo FILE_DRIVE: %FILE_DRIVE% %rd:~1,-1%
echo FILE_DIR: %FILE_DIR% %rd:~1,-1%
echo FILE_NAME: %FILE_NAME% %rd:~1,-1%
echo PATH_TO_VSS: %PATH_TO_VSS% %rd:~1,-1%
echo .

rem ***************************************************************
rem
rem  Move to the source directory and start extraction of all projects.
rem
rem ***************************************************************

if NOT EXIST %FILE_DRIVE%%FILE_DIR% ( set cmd="%FILE_DRIVE%%FILE_DIR% does not exist."
                                      set RTN_ERRORLEVEL=1
                                      goto errors )
cd %FILE_DRIVE%%FILE_DIR% 

rem ***************************************************************
rem
rem Processing.
rem
rem ***************************************************************

if NOT %CAPTURE_FILE%=="" ( echo .
                            echo Accessing VSS.... )
echo . %rd:~1,-1%
echo Accessing VSS.... %rd:~1,-1%

ss cp %PROJ_NAME% -i  %rd:~1,-1%
if NOT %ERRORLEVEL%==0 ( set cmd="Could set project %PROJ_NAME%"
                       set RTN_ERRORLEVEL=%ERRORLEVEL%
                       goto errors )


if NOT "%CHECKOUT_CMD%"=="CHECKOUT"  goto try_UNDOCHECKOUT
set cmd="ss Checkout -gwa %FILE_NAME%"
ss Checkout -gwa %FILE_NAME%  %rd:~1,-1%
if NOT %ERRORLEVEL%==0 ( set cmd="Could not checkout %FILE_NAME%"
                         set RTN_ERRORLEVEL=%ERRORLEVEL%
                         goto errors 
                       ) 
goto end

:try_UNDOCHECKOUT
if NOT "%CHECKOUT_CMD%"=="UNDOCHECKOUT" ( set cmd="Not a valid checkout command."
                                          set RTN_ERRORLEVEL=1
                                          goto errors ) 

set cmd="ss undoCheckout -I-Y %FILE_NAME%"
ss undoCheckout -I-Y %FILE_NAME% %rd:~1,-1%
if NOT %ERRORLEVEL%==0 ( echo ERROR %ERRORLEVEL%
                         set cmd="Could not undocheckout %FILE_NAME%"
                         set RTN_ERRORLEVEL=%ERRORLEVEL%
                         goto errors
                       )
goto end

rem ***************************************************************
rem
rem  Error Processing
rem
rem ***************************************************************
:errors

set RTN_ERRORLEVEL=1
if NOT %CAPTURE_FILE%=="" ( echo Checkout aborted. )
echo Checkout aborted. %rd:~1,-1%
goto end

rem ***************************************************************
rem
rem  The user did not pass all the parameters
rem
rem ***************************************************************
:nada_to_do
echo INVALID USAGE -- NOT ALL PARAMETERS ARE DEFINED.
echo USAGE: Checkout_Vss p1 p2 p3 p4 p5 p6 p7
echo                     p1 is the Source Safe Database
echo                     p2 is the Source Safe Project Name
echo                     p3 is the Source Safe Version Label
echo                     p4 is the Source Safe Command to perform.
echo                        "CHECKOUT" "UNDOCHECKOUT"
echo                     p5 is the filename to checkout w full path
echo                     p6 is the path to the vss installation dir
echo                     p7 is the file to capture output
echo .
set RTN_ERRORLEVEL=1

rem ***************************************************************
rem
rem  Restore and Exit
rem
rem ***************************************************************
:end

if %RTN_ERRORLEVEL%==0 ( if NOT %CAPTURE_FILE%=="" ( echo %CHECKOUT_CMD% successful. )
                         echo %CHECKOUT_CMD% successful. %rd:~1,-1% )

cd\
cd %INIT_DIR% 
%INIT_DIR:~0,2%
Path=%SAVED_PATH%
