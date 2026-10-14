@echo OFF
set /p ANS="This procedure will destroy all data on target drive. Do you still want to continue? (Y/N) "
if not "%ANS%"=="Y" if not "%ANS%"=="y" ( echo Aborted.
                                          goto end )

diskpart /s dPartPrepareDriveD.txt

type formatAns.txt | format D: /FS:FAT32

xcopy c:\newImage\*.* d: /R /I /C /H /K /E /Y

:end
echo ...