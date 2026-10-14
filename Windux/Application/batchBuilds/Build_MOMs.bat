@echo OFF
rem ***************************************************************
rem Parameters:  ALL INPUT PARAMETERS **MUST** BE IN QUOTES ON THE CMD LINE
rem
rem	%1 -> LXSCALE_SOURCE_BASE == root directory of project source
rem
rem	%2 -> JDK_BASE == installation dirrectory of the JDK
rem
rem     %3 -> INTERACTIVE == "Y" Bypass all user confirmation
rem                          "N", Interactive Mode.
rem
rem	%4 -> PAUSE_ON_CMPL == "Y"
rem                            "N"
rem
rem	%5 -> CAPTURE_FILE == Filename to capture output.
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
if "%~5" == "" set RD=""
if NOT "%~5" == "" set CAPTURE_FILE=%~5
if NOT "%~5" == "" set RD=">> %CAPTURE_FILE% 2>&1"

set LXSCALE_SOURCE_BASE=%~1
set JDK_BASE=%~2
set INTERACTIVE=%~3
set PAUSE_ON_CMPL=%~4


if NOT %CAPTURE_FILE%=="" (
	echo . 
        echo ...... Building MOMs Jar Files at %DATE% %TIME% ....... 
	echo . 
	echo LXSCALE_SOURCE_BASE = %LXSCALE_SOURCE_BASE%  
	echo JDK_BASE = %JDK_BASE%  
	echo INTERACTIVE = %INTERACTIVE%  
	echo PAUSE_ON_CMPL = %PAUSE_ON_CMPL%  
        if %CAPTURE_FILE%=="" echo CAPTURE_FILE = Screen
        if NOT %CAPTURE_FILE%=="" echo CAPTURE_FILE = %CAPTURE_FILE%  
	echo . )

echo .  %rd:~1,-1%
echo ...... Building MOMs Jar Files at %DATE% %TIME% .......  %rd:~1,-1%
echo .  %rd:~1,-1%
echo LXSCALE_SOURCE_BASE = %LXSCALE_SOURCE_BASE% %rd:~1,-1% 
echo JDK_BASE = %JDK_BASE% %rd:~1,-1%
echo INTERACTIVE = %INTERACTIVE% %rd:~1,-1% 
echo PAUSE_ON_CMPL = %PAUSE_ON_CMPL% %rd:~1,-1% 
if %CAPTURE_FILE%=="" echo CAPTURE_FILE = Screen %rd:~1,-1%
if NOT %CAPTURE_FILE%=="" echo CAPTURE_FILE = %CAPTURE_FILE%   %rd:~1,-1%
echo . %rd:~1,-1%


if NOT "%INTERACTIVE%"=="Y" goto start_build
set /p ANS="Do you still want to continue? (Y/N) "
if not "%ANS%"=="Y" if not "%ANS%"=="y" ( echo Aborted.
                                          set RTN_ERRORLEVEL=1
                                          goto end )

rem ***************************************************************
rem
rem Update the path to find all the executables.
rem
rem ***************************************************************
:start_build

path=%JDKBASE%\bin;%path%

rem
rem  start in %LXSCALE_SOURCE_BASE%\Application\source\mom
rem 

cd\
set cmd="cd %LXSCALE_SOURCE_BASE%\Application\source\mom"
cd %LXSCALE_SOURCE_BASE%\Application\source\mom
if NOT %errorlevel%==0 goto error_handler

rem ***************************************************************
rem
rem Create mom118.jar
rem
rem ***************************************************************

echo Cleaning up existing mom15.jar and *.class %rd:~1,-1%

if EXIST "com\hobart\scale\mom\webPages\mom15.jar" ( set cmd="attrib -r com\hobart\scale\mom\webPages\mom15.jar"
                                                 attrib -r com\hobart\scale\mom\webPages\mom15.jar
                                                 set cmd="del /q com\hobart\scale\mom\webPages\mom15.jar"
                                                 del /q com\hobart\scale\mom\webPages\mom15.jar 
 					         if NOT %errorlevel%==0 goto error_handler )


set cmd="attrib -r classes\*.*"
attrib -r classes\*.*
set cmd="del /S /q classes\*.*"
del /S /q classes\*.*
if NOT %errorlevel%==0 goto error_handler

set CLASSPATH=.;%LXSCALE_SOURCE_BASE%\3RDPartySoftware\jars\ib6js.jar;%LXSCALE_SOURCE_BASE%\3RDPartySoftware\jars\awt.jar;%JDK_BASE%\jre\lib\javaws.jar;%JDK_BASE%\jre\lib\rt.jar;%JDK_BASE%\lib\tools.jar;

if NOT %CAPTURE_FILE%=="" (echo CLASSPATH = %CLASSPATH% %rd:~1,-1%)
echo CLASSPATH = %CLASSPATH%

if NOT %CAPTURE_FILE%=="" (echo Compile Java Source - com\hobart\scale\mom\*.java %rd:~1,-1%)
echo Compile Java Source - com\hobart\scale\mom\*.java

set cmd="%JDK_BASE%\bin\javac -d classes %cd%\com\hobart\scale\mom\*.java"
if NOT %CAPTURE_FILE%=="" ("%JDK_BASE%"\bin\javac -d classes %cd%\com\hobart\scale\mom\*.java %rd:~1,-1%) else ("%JDK_BASE%"\bin\javac -d classes %cd%\com\hobart\scale\mom\*.java)
if NOT %errorlevel%==0 goto error_handler


if NOT %CAPTURE_FILE%=="" (echo Compile Java Source - com\hobart\scale\marqueeeditor\*.java %rd:~1,-1%)
echo Compile Java Source - com\hobart\scale\marqueeeditor\*.java

set cmd="%JDK_BASE%\bin\javac -d classes %cd%\com\hobart\scale\marqueeeditor\*.java"
if NOT %CAPTURE_FILE%=="" ("%JDK_BASE%"\bin\javac -d classes %cd%\com\hobart\scale\marqueeeditor\*.java %rd:~1,-1%) else ("%JDK_BASE%"\bin\javac -d classes %cd%\com\hobart\scale\marqueeeditor\*.java)
if NOT %errorlevel%==0 goto error_handler


if NOT %CAPTURE_FILE%=="" (echo Compile Java Source - com\hobart\scale\util\*.java %rd:~1,-1%)
echo Compile Java Source - com\hobart\scale\util\*.java

set cmd="%JDK_BASE%\bin\javac -d classes %cd%\com\hobart\scale\util\*.java"
if NOT %CAPTURE_FILE%=="" ("%JDK_BASE%"\bin\javac -d classes %cd%\com\hobart\scale\util\*.java %rd:~1,-1%) else ("%JDK_BASE%"\bin\javac -d classes %cd%\com\hobart\scale\util\*.java)
if NOT %errorlevel%==0 goto error_handler


if NOT %CAPTURE_FILE%=="" (echo Compile Java Source - com\hobart\scale\awt\*.java %rd:~1,-1%)
echo Compile Java Source - com\hobart\scale\awt\*.java

set cmd="%JDK_BASE%\bin\javac -d classes %cd%\com\hobart\scale\awt\*.java"
if NOT %CAPTURE_FILE%=="" ("%JDK_BASE%"\bin\javac -d classes %cd%\com\hobart\scale\awt\*.java %rd:~1,-1%) else ("%JDK_BASE%"\bin\javac -d classes %cd%\com\hobart\scale\awt\*.java)
if NOT %errorlevel%==0 goto error_handler


if NOT %CAPTURE_FILE%=="" (echo Compile Java Source - com\hobart\scale\menu\*.java %rd:~1,-1%)
echo Compile Java Source - com\hobart\scale\menu\*.java

set cmd="%JDK_BASE%\bin\javac -d classes %cd%\com\hobart\scale\menu\*.java"
if NOT %CAPTURE_FILE%=="" ("%JDK_BASE%"\bin\javac -d classes %cd%\com\hobart\scale\menu\*.java %rd:~1,-1%) else ("%JDK_BASE%"\bin\javac -d classes %cd%\com\hobart\scale\menu\*.java)
if NOT %errorlevel%==0 goto error_handler


if NOT %CAPTURE_FILE%=="" (echo Compile Java Source - com\hobart\scale\parser\*.java %rd:~1,-1%)
echo Compile Java Source - com\hobart\scale\parser\*.java

set cmd="%JDK_BASE%\bin\javac -d classes %cd%\com\hobart\scale\parser\*.java"
if NOT %CAPTURE_FILE%=="" ("%JDK_BASE%"\bin\javac -d classes %cd%\com\hobart\scale\parser\*.java %rd:~1,-1%) else ("%JDK_BASE%"\bin\javac -d classes %cd%\com\hobart\scale\parser\*.java)
if NOT %errorlevel%==0 goto error_handler


if NOT %CAPTURE_FILE%=="" (echo Compile Java Source - com\hobart\scale\swing\plaf\evolution\remote\*.java %rd:~1,-1%)
echo Compile Java Source - com\hobart\scale\swing\plaf\evolution\remote\*.java

set cmd="%JDK_BASE%\bin\javac -d classes %cd%\com\hobart\scale\swing\plaf\evolution\remote\*.java"
if NOT %CAPTURE_FILE%=="" ("%JDK_BASE%"\bin\javac -d classes %cd%\com\hobart\scale\swing\plaf\evolution\remote\*.java %rd:~1,-1%) else ("%JDK_BASE%"\bin\javac -d classes %cd%\com\hobart\scale\swing\plaf\evolution\remote\*.java)
if NOT %errorlevel%==0 goto error_handler


if NOT %CAPTURE_FILE%=="" (echo Compile Java Source - com\hobart\scale\label\*.java %rd:~1,-1%)
echo Compile Java Source - com\hobart\scale\label\*.java

set cmd="%JDK_BASE%\bin\javac -d classes %cd%\com\hobart\scale\label\*.java"
if NOT %CAPTURE_FILE%=="" ("%JDK_BASE%"\bin\javac -d classes %cd%\com\hobart\scale\label\*.java %rd:~1,-1%) else ("%JDK_BASE%"\bin\javac -d classes %cd%\com\hobart\scale\label\*.java)
if NOT %errorlevel%==0 goto error_handler


if NOT %CAPTURE_FILE%=="" (echo Compile Java Source - com\hobart\scale\labeltext\*.java %rd:~1,-1%)
echo Compile Java Source - com\hobart\scale\labeltext\*.java

set cmd="%JDK_BASE%\bin\javac -d classes %cd%\com\hobart\scale\labeltext\*.java"
if NOT %CAPTURE_FILE%=="" ("%JDK_BASE%"\bin\javac -d classes %cd%\com\hobart\scale\labeltext\*.java %rd:~1,-1%) else ("%JDK_BASE%"\bin\javac -d classes %cd%\com\hobart\scale\labeltext\*.java)
if NOT %errorlevel%==0 goto error_handler


if NOT %CAPTURE_FILE%=="" (echo Compile Java Source - com\hobart\scale\nutrition\*.java %rd:~1,-1%)
echo Compile Java Source - com\hobart\scale\nutrition\*.java

set cmd="%JDK_BASE%\bin\javac -d classes %cd%\com\hobart\scale\nutrition\*.java"
if NOT %CAPTURE_FILE%=="" ("%JDK_BASE%"\bin\javac -d classes %cd%\com\hobart\scale\nutrition\*.java %rd:~1,-1%) else ("%JDK_BASE%"\bin\javac -d classes %cd%\com\hobart\scale\nutrition\*.java)
if NOT %errorlevel%==0 goto error_handler


if NOT %CAPTURE_FILE%=="" (echo Compile Java Source - com\hobart\scale\graphic\*.java %rd:~1,-1%)
echo Compile Java Source - com\hobart\scale\graphic\*.java

set cmd="%JDK_BASE%\bin\javac -d classes %cd%\com\hobart\scale\graphic\*.java"
if NOT %CAPTURE_FILE%=="" ("%JDK_BASE%"\bin\javac -d classes %cd%\com\hobart\scale\graphic\*.java %rd:~1,-1%) else ("%JDK_BASE%"\bin\javac -d classes %cd%\com\hobart\scale\graphic\*.java)
if NOT %errorlevel%==0 goto error_handler


if NOT %CAPTURE_FILE%=="" (echo Compile Java Source - com\hobart\scale\datastore\*.java %rd:~1,-1%)
echo Compile Java Source - com\hobart\scale\datastore\*.java

set cmd="%JDK_BASE%\bin\javac -d classes %cd%\com\hobart\scale\datastore\*.java"
if NOT %CAPTURE_FILE%=="" ("%JDK_BASE%"\bin\javac -d classes %cd%\com\hobart\scale\datastore\*.java %rd:~1,-1%) else ("%JDK_BASE%"\bin\javac -d classes %cd%\com\hobart\scale\datastore\*.java)
if NOT %errorlevel%==0 goto error_handler


if NOT %CAPTURE_FILE%=="" (echo Compile Java Source - com\hobart\scale\datastore\mds\*.java %rd:~1,-1%)
echo Compile Java Source - com\hobart\scale\datastore\mds\*.java

set cmd="%JDK_BASE%\bin\javac -d classes %cd%\com\hobart\scale\datastore\mds\*.java"
if NOT %CAPTURE_FILE%=="" ("%JDK_BASE%"\bin\javac -d classes %cd%\com\hobart\scale\datastore\mds\*.java %rd:~1,-1%) else ("%JDK_BASE%"\bin\javac -d classes %cd%\com\hobart\scale\datastore\mds\*.java)
if NOT %errorlevel%==0 goto error_handler


if NOT %CAPTURE_FILE%=="" (echo Compile Java Source - com\hobart\scale\labeleditor\*.java %rd:~1,-1%)
echo Compile Java Source - com\hobart\scale\labeleditor\*.java

set cmd="%JDK_BASE%\bin\javac -d classes %cd%\com\hobart\scale\labeleditor\*.java"
if NOT %CAPTURE_FILE%=="" ("%JDK_BASE%"\bin\javac -d classes %cd%\com\hobart\scale\labeleditor\*.java %rd:~1,-1%) else ("%JDK_BASE%"\bin\javac -d classes %cd%\com\hobart\scale\labeleditor\*.java)
if NOT %errorlevel%==0 goto error_handler


if NOT %CAPTURE_FILE%=="" (echo Compile Java Source - com\hobart\scale\graphicviewer\*.java %rd:~1,-1%)
echo Compile Java Source - com\hobart\scale\graphicviewer\*.java

set cmd="%JDK_BASE%\bin\javac -d classes %cd%\com\hobart\scale\graphicviewer\*.java"
if NOT %CAPTURE_FILE%=="" ("%JDK_BASE%"\bin\javac -d classes %cd%\com\hobart\scale\graphicviewer\*.java %rd:~1,-1%) else ("%JDK_BASE%"\bin\javac -d classes %cd%\com\hobart\scale\graphicviewer\*.java)
if NOT %errorlevel%==0 goto error_handler


if NOT %CAPTURE_FILE%=="" (echo Compile Java Source - com\hobart\scale\applet\*.java %rd:~1,-1%)
echo Compile Java Source - com\hobart\scale\applet\*.java

set cmd="%JDK_BASE%\bin\javac -d classes %cd%\com\hobart\scale\applet\*.java"
if NOT %CAPTURE_FILE%=="" ("%JDK_BASE%"\bin\javac -d classes %cd%\com\hobart\scale\applet\*.java %rd:~1,-1%) else ("%JDK_BASE%"\bin\javac -d classes %cd%\com\hobart\scale\applet\*.java)
if NOT %errorlevel%==0 goto error_handler



set cmd="cd classes"
cd classes
if NOT %errorlevel%==0 goto error_handler

if NOT %CAPTURE_FILE%=="" (echo Jarring Classes %rd:~1,-1%)
echo Jarring Classes

set cmd="%JDK_BASE%\bin\jar cvf ..\com\hobart\scale\mom\mom15.jar com"
if NOT %CAPTURE_FILE%=="" ("%JDK_BASE%"\bin\jar cvf ..\com\hobart\scale\mom\mom15.jar com %rd:~1,-1%) else ("%JDK_BASE%"\bin\jar cvf ..\com\hobart\scale\mom\mom15.jar com)
if NOT %errorlevel%==0 goto error_handler

cd ..

set cmd="copy /Y com\hobart\scale\mom\mom15.jar com\hobart\scale\mom\webPages\mom15.jar"
copy /Y %CD%\com\hobart\scale\mom\mom15.jar %CD%\com\hobart\scale\mom\webPages\mom15.jar
if NOT %errorlevel%==0 goto error_handler



goto end

:nada_to_do
rem ***************************************************************
rem
rem  The user did not pass in the project directory and or the output file.
rem
rem ***************************************************************
echo * * * * * * * W A R N I N G * * * * * * * 
echo INVALID USAGE -- NOT ALL PARAMETERS ARE DEFINED.
echo USAGE: Build_Moms p1 p2 p3 p4 p5
echo .
echo        p1 is the "LXSCALE_SOURCE_BASE" value.
echo        p2 is the "JDK_BASE" value.
echo        p3 is the "INTERACTIVE" value.
echo        p4 is the "PAUSE_ON_CMPL" value.
echo        p5 is the output capture file (optional)
echo .
set RTN_ERRORLEVEL=1
goto end

:error_handler
set RTN_ERRORLEVEL=1

:end

if NOT %CAPTURE_FILE%=="" (
	if %RTN_ERRORLEVEL%==0 (  echo .
                                  echo Build MOM Jars COMPLETED at %DATE% %TIME%. )
	if NOT %RTN_ERRORLEVEL%==0  ( echo .  
        	                      echo Build had errors.  
                	              echo .  
                        	      echo Error trying to execute: %cmd% )
                            )


if %RTN_ERRORLEVEL%==0 (  echo .
                          echo Build MOM Jars COMPLETED at %DATE% %TIME%.  %rd:~1,-1% )
if NOT %RTN_ERRORLEVEL%==0  ( echo .  %rd:~1,-1%
                              echo Build had errors.  %rd:~1,-1%
                              echo .  %rd:~1,-1%
                              echo Error trying to execute: %cmd%  %rd:~1,-1%)
echo .
cd\
cd %INIT_DIR% 
%INIT_DIR:~0,2%
Path=%SAVED_PATH%
if "%PAUSE_ON_CMPL%"=="Y" pause

