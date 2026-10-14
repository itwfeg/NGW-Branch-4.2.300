rem remove pyinstaller output build and dist directories
rmdir /s /q .\dist\csid
rmdir /s /q .\build

rem generate native Windows EXE from the python files
rem build windowed application
pyinstaller --onefile --icon=csid.ico csid.py
