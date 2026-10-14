pushd .
call %WDKPATH%\bin\setenv.bat %WDKPATH% chk wxp
popd
build -fc
