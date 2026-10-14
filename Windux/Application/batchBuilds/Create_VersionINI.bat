@echo OFF
rem ***************************************************************
rem Parameters:  ALL INPUT PARAMETERS **MUST** BE IN QUOTES ON THE CMD LINE
rem
rem	%1 -> THEFILE == 'version.ini'
rem	%2 -> THEVERSION == 'Current incoming version'
rem
rem
rem 	Modification		Reason			Who
rem ---------------------------------------------------------------
rem	Oct 23, 2003		Initial Release		Beach, P
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
set THE_TMPFILE=v.ini
 
if "%~1" == "" goto nada_to_do
if "%~2" == "" goto nada_to_do

set THE_FILE=%~1

set OLD_VERSION=%~2

if NOT EXIST %THE_FILE% ( set RTN_ERRORLEVEL=1
                          echo %THE_FILE% not found.
                          goto errors 
                         )

set cmd="attrib -r %THE_FILE%"
attrib -r %THE_FILE%
if NOT %ERRORLEVEL%==0 ( set RTN_ERRORLEVEL=%ERRORLEVEL%
                         goto errors 
                       )

copy /Y %THE_FILE% %THE_FILE%.orig
if NOT %ERRORLEVEL%==0 ( set RTN_ERRORLEVEL=%ERRORLEVEL%
                         goto errors 
                       )

del /q %THE_FILE%
if NOT %ERRORLEVEL%==0 ( set RTN_ERRORLEVEL=%ERRORLEVEL%
                         goto errors 
                       )

rem **********************************************************
rem
rem Steps
rem	1 - Extract only the line containing "component.2.version"
rem     2 - Extract all other lines into version.ini
rem     3 - Parse the line into pieces (value1, valuw2, value3)
rem     4 - Trim the blanks and set core, major, minor
rem     5 - Update Minor and/or major
rem
rem
rem **********************************************************

findstr /I /c:"component.2.version" %THE_FILE%.orig > %THE_TMPFILE%
if NOT %ERRORLEVEL%==0 ( set RTN_ERRORLEVEL=%ERRORLEVEL%
                         goto errors 
                       )

findstr /V /I /c:"component.2.version" %THE_FILE%.orig > %THE_FILE%
if NOT %ERRORLEVEL%==0 ( set RTN_ERRORLEVEL=%ERRORLEVEL%
                         goto errors 
                       )

FOR /F " tokens=1,2,3,4* " %%i in (%THE_TMPFILE%) do ( set value1=%%i 
                                                       set value2=%%j 
                                                       set value3=%%k
                                                     )
if NOT %ERRORLEVEL%==0 ( set RTN_ERRORLEVEL=%ERRORLEVEL%
                         goto errors 
                       )
set value1=%value1: =%
set value2=%value2: =%
set value3=%value3: =%


FOR /F " tokens=1,2,3,4* delims=. " %%i in ("%value3%") do ( set core=%%i 
                                                           set major=%%j 
                                                           set minor=%%k
                                                           set build=%%l 
                                              )
if NOT %ERRORLEVEL%==0 ( set RTN_ERRORLEVEL=%ERRORLEVEL%
                         goto errors 
                       )
set core=%core: =%
set major=%major: =%
set minor=%minor: =%
set build=%build: =%


set /A nwbuild = %build% + 1


echo %value1% %value2% %core%.%major%.%minor%.%nwbuild% >> %THE_FILE%


set cmd="attrib +r %THE_FILE%"
attrib +r %THE_FILE%
if NOT %ERRORLEVEL%==0 ( set RTN_ERRORLEVEL=%ERRORLEVEL%
                         goto errors 
                       )

set OLD_VERSION=%major%.%minor%.%build%
set NEW_VERSION=%major%.%minor%.%nwbuild%

echo Creating new %THE_FILE%
echo .
echo Existing Version: %OLD_VERSION%
echo New Version: %NEW_VERSION%
echo .


if EXIST %THE_TMPFILE% ( DEL /Q %THE_TMPFILE% )

goto end

rem ***************************************************************
rem
rem  Usage Information
rem
rem ***************************************************************

:nada_to_do
echo * * * * * * * W A R N I N G * * * * * * * 
echo INVALID USAGE -- NOT ALL PARAMETERS ARE DEFINED.
echo USAGE: Create_Config p1 p2 
echo        This program will create a p1.orig
echo        before creating a new config.
echo .
echo        p1 is the config.dat file to utilize.
echo        p2 is the string to set the selfserviceflag
echo           reset=restore config.dat.orig to config.dat
echo .
set RTN_ERRORLEVEL=1
goto end

rem ***************************************************************
rem
rem Error Handling
rem
rem ***************************************************************
:errors

echo .
if NOT %CMD%==0 ( echo Error trying to: %CMD% )
set RTN_ERRORLEVEL=1

rem ***************************************************************
rem
rem Exiting
rem
rem ***************************************************************

:end
cd\
cd %INIT_DIR% 
%INIT_DIR:~0,2%
Path=%SAVED_PATH%