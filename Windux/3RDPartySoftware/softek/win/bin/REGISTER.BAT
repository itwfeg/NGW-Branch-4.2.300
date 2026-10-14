setlocal enableextensions
pushd "%~dp0"
regsvr32 SoftekBarcodeCOM.dll
regsvr32 SoftekBarcode.ocx
popd
SET /P Choice=Press return to exit
