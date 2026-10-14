set output1=%CD%\Full_Build_%date:~4%_at_%time:~0,2%
set output1=%output1:/=_%
set output1=%output1: =_%.log
set output2=%CD%\Full_Build_Int_%date:~4%_at_%time:~0,2%
set output2=%output2:/=_%
set output2=%output2: =_%.log
CALL Main "Y" "\\troyeng1\vss databases\HLX" "$\Windows\MAINBRANCH" "*" "c:\Hobart_release" "C:\vss\win32" "C:\Program Files\Microsoft Visual Studio 8" "Y" "Release" "C:\Program Files\Java\jdk1.5.0_09" "Y" "C:\Ass Image" "C:\WINDDK\3790.1830" "Y" "\\troyeng1\Firmware Images\Access Service Counter Scale\MainBranch" "\\bondxjr1\ftp-root" "Y" "Y" "C:\Program Files\7-Zip" "N" "N" %output2% >%output1%