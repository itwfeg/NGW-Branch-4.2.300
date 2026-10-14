@echo OFF
rem ***************************************************************
rem Parameters:  ALL INPUT PARAMETERS **MUST** BE IN QUOTES ON THE CMD LINE
rem
rem	%1 -> BUILDNBR_FILE == 'C:\Evolution\Application\source\app\BuildNumber.cpp
rem
rem	%2 -> RESET_FLAG == 'reset'
rem                          If not supplied, a new BuildNumber.cpp is created, and
rem                          the original is saved as BuildNumber.cpp.orig.
rem                          If 'reset' is passed, the BuildNumber.cpp.orig is restored.
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
set BLDNBR=%date:~13,1%%date:~4,2%%date:~7,2%.%time:~0,2%
set BLDNBR=%BLDNBR: =0%

if "%~1" == "" goto nada_to_do
if "%~2" == "" set SSFLAG=N
if NOT "%~2" == "" set SSFLAG=%~2

set BUILDNBR_FILE=%~1

if NOT EXIST %BUILDNBR_FILE% ( set RTN_ERRORLEVEL=1
                               echo %BUILDNBR_FILE% not found.
                               goto errors 
                             )


if NOT "%SSFLAG%"=="reset" (  set cmd="attrib -r %BUILDNBR_FILE%"
                              attrib -r %BUILDNBR_FILE%
                              if NOT %ERRORLEVEL%==0 ( set RTN_ERRORLEVEL=%ERRORLEVEL%
                                                       goto errors 
                                                     )

                              set CMD="copy /Y %BUILDNBR_FILE% %BUILDNBR_FILE%.orig"
                              copy /Y %BUILDNBR_FILE% %BUILDNBR_FILE%.orig
                              if NOT %ERRORLEVEL%==0  ( set RTN_ERRORLEVEL=%ERRORLEVEL%
                                                        goto errors 
                                                      )
        
                              set CMD="findstr /V /I /c:BuildNumber::buildNumber %BUILDNBR_FILE%.orig > %BUILDNBR_FILE%"
                              findstr /V /I /c:BuildNumber::buildNumber %BUILDNBR_FILE%.orig > %BUILDNBR_FILE%
                              if NOT %ERRORLEVEL%==0  ( set RTN_ERRORLEVEL=%ERRORLEVEL%
                                                        goto errors  
                                                      )

                              
                              if NOT "%time:~0,1%"==" " set BLDNBR=%date:~13,1%%date:~4,2%%date:~7,2%.%time:~0,2%
                              if "%time:~0,1%"==" " set BLDNBR=%date:~13,1%%date:~4,2%%date:~7,2%.0%time:~1,2%

                              echo const std::string BuildNumber::buildNumber = "%BLDNBR%";  // format: YMMDD.HH >> %BUILDNBR_FILE%
                              if NOT %ERRORLEVEL%==0  ( set RTN_ERRORLEVEL=%ERRORLEVEL%
                                                        goto errors  
                                                      )

			      echo Creating new BuildNumber.cpp
			      echo New Build Number is: %BLDNBR%
			      echo .

                              set cmd="attrib +r %BUILDNBR_FILE%"
                              attrib +r %BUILDNBR_FILE%
                              if NOT %ERRORLEVEL%==0 ( set RTN_ERRORLEVEL=%ERRORLEVEL%
                                                       goto errors 
                                                     )
                           )

if "%SSFLAG%"=="reset" (  if NOT EXIST %BUILDNBR_FILE%.orig ( set cmd="find %BUILDNBR_FILE%.orig - does not exist."
                                                            goto errors
                                                          )
                         
                          if EXIST %BUILDNBR_FILE% (  set cmd="attrib +r %BUILDNBR_FILE%"
                                                    attrib -r %BUILDNBR_FILE%
                                                    if NOT %ERRORLEVEL%==0 ( set RTN_ERRORLEVEL=%ERRORLEVEL%
                                                                             goto errors 
                                                                            )
                                                    del %BUILDNBR_FILE%
                                                    if NOT %ERRORLEVEL%==0 ( set RTN_ERRORLEVEL=%ERRORLEVEL%
                                                                             goto errors 
                                                                            )
                                                 )

                          set cmd="copy /Y %BUILDNBR_FILE%.orig %BUILDNBR_FILE%"
                          copy /Y %BUILDNBR_FILE%.orig %BUILDNBR_FILE% 
                          if NOT %ERRORLEVEL%==0  set RTN_ERRORLEVEL=%ERRORLEVEL%
                          set cmd="attrib +r %BUILDNBR_FILE%"
                          attrib +r %BUILDNBR_FILE%
                          if NOT %ERRORLEVEL%==0 ( set RTN_ERRORLEVEL=%ERRORLEVEL%
                                                   goto errors 
                                                 )

                           set cmd="attrib -r %BUILDNBR_FILE%.orig"
                           attrib -r %BUILDNBR_FILE%.orig
                           if NOT %ERRORLEVEL%==0 ( set RTN_ERRORLEVEL=%ERRORLEVEL%
                                                    goto errors 
                                                  )
                           del %BUILDNBR_FILE%.orig
                           if NOT %ERRORLEVEL%==0 ( set RTN_ERRORLEVEL=%ERRORLEVEL%
                                                    goto errors 
                                                  )
                       )


goto end

rem ***************************************************************
rem
rem  Usage Information
rem
rem ***************************************************************

:nada_to_do
echo * * * * * * * W A R N I N G * * * * * * * 
echo INVALID USAGE -- NOT ALL PARAMETERS ARE DEFINED.
echo USAGE: Update_BuildNumber p1 
echo        This program will create a copy of BuildNUmber.cpp.orig
echo        before creating a new BuildNumber.cpp with a new timestamp.
echo .
echo        p1 is the BuildNumber.cpp file to update.
echo        p2 is the string to set the selfserviceflag
echo           reset=restore BuildNumber.cpp.orig to BuildNumber.cpp
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