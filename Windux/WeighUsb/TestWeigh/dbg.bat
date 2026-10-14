
set WEIGHING=c:\WINDDK\3790.1830
set WEIGHINGDRIVE=c:\Hobart\WeighUsb\WeighUsb
@rem
@rem
call %WEIGHING%\bin\setenv.bat %WEIGHING% chk
cd %WEIGHINGDRIVE%\WeighUSB\TestWeigh
build -c
copy %WEIGHINGDRIVE%\WeighUSB\TestWeigh\objchk_wxp_x86\i386\TestWeigh.* %WEIGHINGDRIVE%\WeighUSB\TestWeigh\*.*

