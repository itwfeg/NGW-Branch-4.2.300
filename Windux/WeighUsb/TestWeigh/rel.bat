set WEIGHING=c:\WINDDK\3790.1830
set WEIGHINGDRIVE=c:\Hobart\WeighUsb
@rem
@rem
call %WEIGHING%\bin\setenv.bat %WEIGHING% fre
cd %WEIGHINGDRIVE%\WeighUSB\TestWeigh
build -c
copy %WEIGHINGDRIVE%\WeighUSB\TestWeigh\objfre_wxp_x86\i386\TestWeigh.* %WEIGHINGDRIVE%\WeighUSB\TestWeigh\*.*
