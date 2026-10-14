DLL file list:


For 64bit system only:

DebenuPDFLibrary64DLLXXXX.dll		Only required for processing PDF documents
SoftekBarcode64COM.dll			COM object (should be registered and requires SoftekBarcode64DLL.dll)
SoftekBarcode64DLL.dll			Standard Windows DLL (no need to register)
SoftekBarcode64DLL.lib			The lib file for SoftekBarcode64DLL.dll
SoftekBarcode64Lib.dll			A .Net 3.5 component library

For x86 systems only:

DebenuPDFLibraryDLLXXXX.dll		Only required for processing PDF documents
SoftekBarcodeCOM.dll			COM object (should be registered and requires SoftekBarcodeDLL.dll)
SoftekBarcodeDLL.dll			Standard Windows DLL (no need to register)
SoftekBarcodeDLL.lib			The lib file for SoftekBarcodeDLL.dll		
SoftekBarcodeLib.dll			A .Net 3.5 component library		
SoftekBarcode.ocx			OCX object (should be registered and requires SoftekBarcodeDLL.dll
cimage.dll				NOT REQUIRED BY DEFAULT. Only required for processing PDF documents with the VeryPDF renderer.
VeryPDFLibrary.dll			NOT REQUIRED BY DEFAULT. Only required for processing PDF documents with the VeryPDF renderer.

For either x86 or 64bit systems:

SoftekBarcodeNet.dll			A .Net 3.5 wrapper which automatically interfaces with either SoftekBarcodeDLL.dll or SoftekBarcode64DLL.dll

EXE files:

SoftekSDKDemo.exe			Demo application for x86 systems
SoftekSDK64Demo.exe			Demo application for 64bit systems
