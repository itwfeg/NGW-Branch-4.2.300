
::---------------------------------------------------------------------------------------------------
::  Executes buildMonolithicCorePreInstallFeature.py and buildMonolithicCoreFeature.py under Windows.
::
::  This requuires Python 2.7, which is not installed on ENGDEV1.
:: 
::  Parameters: Release tag and Branch Name, in the format:  NN.NN.NN-MMDDYYYY_HHMMSS  branch-n-xxxxxx
::
::      For example:  runBuildMonolithic.cmd  1.13.110-07012015_125021  branch-timesys-3.10
::
::---------------------------------------------------------------------------------------------------

setlocal

IF [%1]==[] goto missing_parameter
IF [%2]==[] goto missing_parameter

set RELEASETAG=%1
set BRANCHNAME=%2%
set WORKINGDIRECTORY=%cd%
:: set PYTHONPATH=%WORKINGDIRECTORY%\..\ScaleManagement;%WORKINGDIRECTORY%\..\ScaleManagement\source;%WORKINGDIRECTORY%\Application\shared\sessionHandler\python;%WORKINGDIRECTORY%\Application\shared\util\dongleKey;%WORKINGDIRECTORY%\Application\shared\util\licensing
set PYTHONPATH=%WORKINGDIRECTORY%\..\ScaleManagement;%WORKINGDIRECTORY%\..\ScaleManagement\smBackend;%WORKINGDIRECTORY%\Application\shared\sessionHandler\python;%WORKINGDIRECTORY%\Application\shared\util\dongleKey;%WORKINGDIRECTORY%\Application\shared\util\licensing
set PATH=C:\Python27;%PATH%

python buildMonolithicCorePreInstallFeature.py %RELEASETAG% %BRANCHNAME%
if %ERRORLEVEL% NEQ 0 goto error

python buildMonolithicCoreFeature.py %RELEASETAG% %BRANCHNAME%




goto end

:missing_parameter

echo "Missing Parameter ... Need to pass the release tag and branch name"
goto end


:error

echo ERROR: buildMonolithicCorePreInstallFeature FAILED

:end

endlocal
