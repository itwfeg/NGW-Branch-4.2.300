setlocal enableextensions
pushd "%~dp0"
regsvr32 SoftekBarcode64COM.dll
popd
SET /P Choice=Press return to exit
