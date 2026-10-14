pushd .
call %WDKPATH%\bin\setenv.bat %WDKPATH% fre wxp
popd
build -fc
