
::-----------------------------------------------------------------------------------------------
::  Executes build_dependency_list.py under Windows.  This python script resides in
::  ScaleManagement\smBackend.
::
::  This requuires Python 2.7, which is not installed on ENGDEV1.
:: 
::  Parameters: None
::
::  Must be run from the Windux directory.  Generates "sm_backend_dependencies.txt" which will be
::  placed in this same directory.
::
::  If the sm_backend_dependencies.txt file that is generated is different than the version
::  in Subversion, then check it in BEFORE running the build using installToTarget.sh
::  on ENGDEV1.
::-----------------------------------------------------------------------------------------------

setlocal

set WORKINGDIRECTORY=%cd%
set PYTHONPATH=%WORKINGDIRECTORY%\..\ScaleManagement;%WORKINGDIRECTORY%\..\ScaleManagement\smBackend;%WORKINGDIRECTORY%\Application\shared\sessionHandler\python;%WORKINGDIRECTORY%\Application\shared\util\dongleKey;%WORKINGDIRECTORY%\Application\shared\util\licensing;%WORKINGDIRECTORY%\..\ScaleManagement\source\payload;%WORKINGDIRECTORY%\..\ScaleManagement\source;%WORKINGDIRECTORY%\..\ScaleManagement\SQLLiteParser
set PATH=C:\Python27;%PATH%

cd ..\ScaleManagement\smBackend

python build_dependency_list.py

cd %WORKINGDIRECTORY%

endlocal
