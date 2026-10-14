unit SoftekBarCode;

{ (c)2003 Marian Aldenhövel MBA Software-Consulting
     Rosenhain 23
     53123 Bonn

     marian@mba-software.de

  This unit dynamically imports the functions from the Softek Barcode Library.

  Usage:

  - Call InitSoftekLibrary from somewhere in your program before calling any of
    the functions.
  - See the documentation from Softek for the use of the actual functions.

AJB: 7/6/05 - Added the stuff necessary for SkewTolerance.

Joe Skeen 3-May-2010 - Added missing functions in the current toolkit
        jskeen@utah.gov   joe_skeen@hotmail.com

AJB: 8/3/2012
  - converted "st" function to "mt" functions
  - added mtCreateBarcodeInstance and mtDestroyBarcodeInstance
  - changed short=shortint to short=smallint
  - added databar and qrcode functions
}

interface

uses Windows,SysUtils;

type short=smallint;
     long=integer;
     PLong=^long;

var mtCreateBarcodeInstance:function():Pointer; stdcall;
    mtDestroyBarcodeInstance:function(hBarcode:Pointer):Integer;stdcall;
    mtGetBitmapResolution:function(hBarcode:Pointer):Integer; stdcall;
		mtSetBitmapResolution:function(hBarcode:Pointer;newValue:Integer):Integer; stdcall;
    mtGetDebugTraceFile:function(hBarcode:Pointer):PAnsiChar; stdcall;
    mtSetDebugTraceFile:function(hBarcode:Pointer;lpszNewValue:PAnsiChar):PAnsiChar; stdcall;
    mtSetLicenseKey:function(hBarcode:Pointer;lpszNewValue:PAnsiChar):PAnsiChar; stdcall;
    mtGetLineJump:function(hBarcode:Pointer):integer; stdcall;
    mtSetLineJump:function(hBarcode:Pointer;nNewValue:integer):integer; stdcall;
    mtGetLowerRatio:function(hBarcode:Pointer):double; stdcall;
    mtSetLowerRatio:function(hBarcode:Pointer;newValue:double):double; stdcall;
    mtGetMinOccurrence:function(hBarcode:Pointer):short; stdcall;
    mtSetMinOccurrence:function(hBarcode:Pointer;nNewValue:short):short; stdcall;
    mtGetMultipleRead:function(hBarcode:Pointer):bool; stdcall;
    mtSetMultipleRead:function(hBarcode:Pointer;bNewValue:bool):bool; stdcall;
    mtGetNoiseReduction:function(hBarcode:Pointer):short; stdcall;
    mtSetNoiseReduction:function(hBarcode:Pointer;nNewValue:short):short; stdcall;
    mtGetPageNo:function(hBarcode:Pointer):short; stdcall;
    mtSetPageNo:function(hBarcode:Pointer;nNewValue:integer):integer; stdcall;
    mtGetPrefOccurrence:function(hBarcode:Pointer):short; stdcall;
    mtSetPrefOccurrence:function(hBarcode:Pointer;nNewValue:short):short; stdcall;
    mtGetQuietZoneSize:function(hBarcode:Pointer):short; stdcall;
    mtSetQuietZoneSize:function(hBarcode:Pointer;nNewValue:short):short; stdcall;
    mtGetRotation:function(hBarcode:Pointer):short; stdcall;
    mtSetRotation:function(hBarcode:Pointer;nNewValue:short):short; stdcall;
    mtGetUpperRatio:function(hBarcode:Pointer):double; stdcall;
    mtSetUpperRatio:function(hBarcode:Pointer;newValue:double):double; stdcall;
    mtGetReadCode39:function(hBarcode:Pointer):bool; stdcall;
    mtSetReadCode39:function(hBarcode:Pointer;bNewValue:bool):bool; stdcall;
    {Only functions with the 2-D version of the toolkit}
    mtGetReadPDF417:function(hBarcode:Pointer):bool; stdcall;
    mtSetReadPDF417:function(hBarcode:Pointer;bNewValue:bool):bool; stdcall;
    mtGetReadEAN13:function(hBarcode:Pointer):bool; stdcall;
    mtSetReadEAN13:function(hBarcode:Pointer;bNewValue:bool):bool; stdcall;
    mtGetReadEAN8:function(hBarcode:Pointer):bool; stdcall;
    mtSetReadEAN8:function(hBarcode:Pointer;bNewValue:bool):bool; stdcall;
    mtGetReadUPCA:function(hBarcode:Pointer):bool; stdcall;
    mtSetReadUPCA:function(hBarcode:Pointer;bNewValue:bool):bool; stdcall;
    mtGetReadUPCE:function(hBarcode:Pointer):bool; stdcall;
    mtSetReadUPCE:function(hBarcode:Pointer;bNewValue:bool):bool; stdcall;
    mtGetShowCheckDigit:function(hBarcode:Pointer):bool; stdcall;
    mtSetShowCheckDigit:function(hBarcode:Pointer;bNewValue:bool):bool; stdcall;
		mtGetReadCode128:function(hBarcode:Pointer):bool; stdcall;
		mtSetReadCode128:function(hBarcode:Pointer;bNewValue:bool):bool; stdcall;
		mtGetCode39NeedStartStop:function(hBarcode:Pointer):bool; stdcall;
		mtSetCode39NeedStartStop:function(hBarcode:Pointer;bNewValue:bool):bool; stdcall;
		mtGetReadCode25:function(hBarcode:Pointer):bool; stdcall;
		mtSetReadCode25:function(hBarcode:Pointer;bNewValue:bool):bool; stdcall;
		mtGetColorThreshold:function(hBarcode:Pointer):short; stdcall;
		mtSetColorThreshold:function(hBarcode:Pointer;nNewValue:short):short; stdcall;
		mtGetColorProcessingLevel:function(hBarcode:Pointer):short; stdcall;
		mtSetColorProcessingLevel:function(hBarcode:Pointer;nNewValue:short):short; stdcall;
		mtGetBarString:function(hBarcode:Pointer;index:short):PAnsiChar; stdcall;
		//stScanBarCode:function(hBarcode:Pointer;FileName:PChar):short; stdcall;
		mtScanBarCodeFromBitmap:function(hBarcode:Pointer;hBitmap:long):short; stdcall;
		mtScanBarCodeFromDIB:function(hBarcode:Pointer;hDib:long):short; stdcall;
		mtGetBarStringType:function(hBarcode:Pointer;index:short):PAnsiChar; stdcall;
    mtGetBarStringPos:function(hBarcode:Pointer;nBarCode:short;var pTopLeftX,pTopLeftY,pBotRightX,pBotRightY:Long):short; stdcall;
    mtSetScanRect:function(hBarcode:Pointer;TopLeftX,TopLeftY,BottomRightX,BottomRightY:Long;MappingMode:short):bool; stdcall;
    mtGetReadPatchCodes:function(hBarcode:Pointer):bool; stdcall;
    mtSetReadPatchCodes:function(hBarcode:Pointer;bNewValue:bool):bool; stdcall;
		stSaveBitmap:function(hBarcode:Pointer;hBitmap:Long;strFilePath:PAnsiChar):bool; stdcall;

		{DT 09/06/04}
		mtGetReadCodabar:function(hBarcode:Pointer):bool; stdcall;
		mtSetReadCodabar:function(hBarcode:Pointer;bNewValue:bool):bool; stdcall;
		mtGetMaxLength:function(hBarcode:Pointer):short; stdcall;
		mtSetMaxLength:function(hBarcode:Pointer;nNewValue:integer):short; stdcall;
		mtGetMinLength:function(hBarcode:Pointer):short; stdcall;
		mtSetMinLength:function(hBarcode:Pointer;nNewValue:short):short; stdcall;
		mtGetScanDirection:function(hBarcode:Pointer):short; stdcall;
		mtSetScanDirection:function(hBarcode:Pointer;nNewValue:short):short; stdcall;
	{AJB 7/4/05}
    mtGetSkewTolerance:function(hBarcode:Pointer):short; stdcall;
    mtSetSkewTolerance:function(hBarcode:Pointer;nNewValue:short):short; stdcall;

    {Joe Skeen 3-May-2010}  //missing functions - some don't show in the documentation yet.
    mtScanBarCode:function(hBarcode:Pointer;FileName:PAnsiChar):short; stdcall; //corrected for use with unicode or non-unicode versions of Delphi
    mtGetBarCodeConfidence: function(hBarcode:Pointer;nBarcode: short): short; stdcall;
    mtGetReadDataMatrix:function(hBarcode:Pointer):bool; stdcall;
    mtSetReadDataMatrix:function(hBarcode:Pointer;bNewValue:bool):bool; stdcall;
    mtGetConvertUPCEToEAN13: function(hBarcode:Pointer): bool; stdcall;
    mtSetConvertUPCEToEAN13: function(hBarcode:Pointer;bNewValue: bool): bool; stdcall;
    //stConvertCode39Extended
    mtExportXMLSettings: function(hBarcode:Pointer;var filePath: PAnsiChar): Integer; stdcall;
    mtGetAllowDuplicateValues: function(hBarcode:Pointer): bool; stdcall;
    //mtGetBarStringA
    mtGetBarStringDirection: function(hBarcode:Pointer;nBarCode: short): short; stdcall;
    mtGetCode25Checksum: function(hBarcode:Pointer): bool; stdcall;
    mtGetCode39Checksum: function(hBarcode:Pointer): bool; stdcall;
    mtGetDespeckle: function(hBarcode:Pointer): bool; stdcall;
    mtGetEncoding: function(hBarcode:Pointer): short; stdcall;
    mtGetErrorCorrection: function(hBarcode:Pointer): bool; stdcall;
    mtGetExtendedCode39: function(hBarcode:Pointer): bool; stdcall;
    mtGetGammaCorrection: function(hBarcode:Pointer): short; stdcall;
    mtGetMedianFilter: function(hBarcode:Pointer): bool; stdcall;
    mtGetMinSeparation: function(hBarcode:Pointer): short; stdcall;
    mtGetMinSpaceBarWidth: function(hBarcode:Pointer): short; stdcall;
    mtGetPattern: function(hBarcode:Pointer): PAnsiChar; stdcall;
    //mtGetPDFTool
    mtGetPdfBpp: function(hBarcode:Pointer): short; stdcall;
    mtGetPdfDpi: function(hBarcode:Pointer): short; stdcall;
    mtGetPhotometric: function(hBarcode:Pointer): bool; stdcall;
    mtGetReadCode25ni: function(hBarcode:Pointer): bool; stdcall;
    mtGetReadMicroPDF417: function(hBarcode:Pointer): bool; stdcall;
    mtGetReadNumeric: function(hBarcode:Pointer): bool; stdcall;
    mtGetReadShortCode128: function(hBarcode:Pointer): bool; stdcall;
    mtGetShortCode128MinLength: function(hBarcode:Pointer): short; stdcall;
    mtGetSkewLineJump: function(hBarcode:Pointer): short; stdcall;
    mtGetTifSplitMode: function(hBarcode:Pointer): short; stdcall;
    mtGetTifSplitPath: function(hBarcode:Pointer): PAnsiChar; stdcall;
    mtGetUseOverSampling: function(hBarcode:Pointer): bool; stdcall;
    mtLoadXMLSettings: function(hBarcode:Pointer;filePath: PAnsiChar; silent: Byte): Integer; stdcall;
    mtProcessXML: function(hBarcode:Pointer;inputFilePath: PAnsiChar; outputFilePath: PAnsiChar; silent: Byte): Integer; stdcall;
    mtSaveResults: function(hBarcode:Pointer;var filePath: PAnsiChar): Integer; stdcall;
    mtSetAllowDuplicateValues: function(hBarcode:Pointer;bNewValue: bool): bool; stdcall;
    mtSetCode25Checksum: function(hBarcode:Pointer;bNewValue: bool): bool; stdcall;
    mtSetCode39Checksum: function(hBarcode:Pointer;bNewValue: bool): bool; stdcall;
    mtSetDespeckle: function(hBarcode:Pointer;bNewValue: bool): bool; stdcall;
    mtSetEncoding: function(hBarcode:Pointer;nNewValue: short): short; stdcall;
    mtSetErrorCorrection: function(hBarcode:Pointer;bNewValue: bool): bool; stdcall;
    mtSetExtendedCode39: function(hBarcode:Pointer;bNewValue: bool): bool; stdcall;
    mtSetGammaCorrection: function(hBarcode:Pointer;nNewValue: short): short; stdcall;
    mtSetMedianFilter: function(hBarcode:Pointer;bNewValue: bool): bool; stdcall;
    mtSetMinSeparation: function(hBarcode:Pointer;nNewValue: short): short; stdcall;
    mtSetMinSpaceBarWidth: function(hBarcode:Pointer;nNewValue: short): short; stdcall;
    mtSetPattern: function(hBarcode:Pointer;var lpszNewValue: PAnsiChar):PAnsiChar; stdcall;
    mtSetPdfBpp: function(hBarcode:Pointer;nNewValue: short): short; stdcall;
    mtSetPdfDpi: function(hBarcode:Pointer;nNewValue: short): short; stdcall;
    mtSetPhotometric: function(hBarcode:Pointer;bPhotometric: bool): bool; stdcall;
    mtSetReadCode25ni: function(hBarcode:Pointer;bNewValue: bool): bool; stdcall;
    mtSetReadMicroPDF417: function(hBarcode:Pointer;bNewValue: bool): bool; stdcall;
    mtSetReadNumeric: function(hBarcode:Pointer;bNewValue: bool): bool; stdcall;
    mtSetReadShortCode128: function(hBarcode:Pointer;bNewValue: bool): bool; stdcall;
    mtSetShortCode128MinLength: function(hBarcode:Pointer;sNewValue: short): short; stdcall;
    mtSetSkewLineJump: function(hBarcode:Pointer;nNewValue: short): short; stdcall;
    //mtSetTempFolder
    mtSetTifSplitMode: function(hBarcode:Pointer;sNewValue: short): short; stdcall;
    mtSetTifSplitPath: function(hBarcode:Pointer;szPath: PAnsiChar): PAnsiChar; stdcall;
    mtSetUseOverSampling: function(hBarcode:Pointer;bNewValue: bool): bool; stdcall;

    // AJB - new functions added 8/3/2012
    mtSetReadDatabar: function(hBarcode:Pointer;bNewValue: bool): bool; stdcall;
    mtGetReadDatabar: function(hBarcode:Pointer): bool; stdcall;
    mtSetReadQRCode: function(hBarcode:Pointer;bNewValue: bool): bool; stdcall;
    mtGetReadQRCode: function(hBarcode:Pointer): bool; stdcall;
    mtSetDatabarOptions:  function(hBarcode:Pointer;iNewValue: integer): short; stdcall;
    mtGetDatabarOptions:  function(hBarcode:Pointer): short; stdcall;
    mtSetPdfImageOnly: function(hBarcode:Pointer;bNewValue: bool): bool; stdcall;
    mtGetPdfImageOnly: function(hBarcode:Pointer): bool; stdcall;
    mtSetPdfImageExtractOptions:  function(hBarcode:Pointer;sNewValue: short): short; stdcall;
    mtGetPdfImageExtractOptions:  function(hBarcode:Pointer): short; stdcall;
    mtSetPdfImageRasterOptions:  function(hBarcode:Pointer;sNewValue: short): short; stdcall;
    mtGetPdfImageRasterOptions:  function(hBarcode:Pointer): short; stdcall;

function stGetErrorString(ErrorCode:integer):string;

function InitSoftekLibrary(dllPath:String):boolean;
procedure DoneLibrary;

type ESoftekBarCodeError=class(Exception);

implementation

function stGetErrorString(ErrorCode:integer):string;
begin
  case ErrorCode of
    -1:Result:='Error opening file';
    -2:Result:='File is Multi Plane';
    -3:Result:='Invalid number of bits per sample';
		-4:Result:='Memory allocation error';
    -5:Result:='Invalid TIF photometric value';
    -6,-7,-8:Result:='The trial period on a demo version has either expired or this trial version is no longer valid for installation.';
    else Result:=Format('Unknown error %d',[ErrorCode]);
  end;
end;

var LibraryHandle:THandle=0;


function InitSoftekLibrary(dllPath:String):boolean;

	function DoGetProcAddress(aName:string):pointer;
	begin
		Result:=GetProcAddress(LibraryHandle,PChar(aName));
//    if Result=NIL then RaiseLastOSError;
	end;

begin
	result:=true;
	if LibraryHandle=0 then begin
		LibraryHandle:=LoadLibrary(pChar(dllpath));
		if LibraryHandle>0 then
    begin
 			@mtGetLineJump:=DoGetProcAddress('mtGetLineJump');

			@mtGetBitmapResolution:=DoGetProcAddress('mtGetBitmapResolution');
			@mtSetBitmapResolution:=DoGetProcAddress('mtSetBitmapResolution');
			@mtGetDebugTraceFile:=DoGetProcAddress('mtGetDebugTraceFile');
			@mtSetDebugTraceFile:=DoGetProcAddress('mtSetDebugTraceFile');
			@mtSetLicenseKey:=DoGetProcAddress('mtSetLicenseKey');
			@mtGetLineJump:=DoGetProcAddress('mtGetLineJump');
			@mtSetLineJump:=DoGetProcAddress('mtSetLineJump');
			@mtGetLowerRatio:=DoGetProcAddress('mtGetLowerRatio');
			@mtSetLowerRatio:=DoGetProcAddress('mtSetLowerRatio');
			@mtGetMinOccurrence:=DoGetProcAddress('mtGetMinOccurrence');
			@mtSetMinOccurrence:=DoGetProcAddress('mtSetMinOccurrence');
			@mtGetMultipleRead:=DoGetProcAddress('mtGetMultipleRead');
			@mtSetMultipleRead:=DoGetProcAddress('mtSetMultipleRead');
			@mtGetNoiseReduction:=DoGetProcAddress('mtGetNoiseReduction');
			@mtSetNoiseReduction:=DoGetProcAddress('mtSetNoiseReduction');
			@mtGetPageNo:=DoGetProcAddress('mtGetPageNo');
			@mtSetPageNo:=DoGetProcAddress('mtSetPageNo');
			@mtGetPrefOccurrence:=DoGetProcAddress('mtGetPrefOccurrence');
			@mtSetPrefOccurrence:=DoGetProcAddress('mtSetPrefOccurrence');
			@mtGetQuietZoneSize:=DoGetProcAddress('mtGetQuietZoneSize');
			@mtSetQuietZoneSize:=DoGetProcAddress('mtSetQuietZoneSize');
			@mtGetRotation:=DoGetProcAddress('mtGetRotation');
			@mtSetRotation:=DoGetProcAddress('mtSetRotation');
			@mtGetUpperRatio:=DoGetProcAddress('mtGetUpperRatio');
			@mtSetUpperRatio:=DoGetProcAddress('mtSetUpperRatio');
			@mtGetReadCode39:=DoGetProcAddress('mtGetReadCode39');
			@mtSetReadCode39:=DoGetProcAddress('mtSetReadCode39');
			@mtGetReadEAN13:=DoGetProcAddress('mtGetReadEAN13');
			@mtSetReadEAN13:=DoGetProcAddress('mtSetReadEAN13');
			@mtGetReadEAN8:=DoGetProcAddress('mtGetReadEAN8');
			@mtSetReadEAN8:=DoGetProcAddress('mtSetReadEAN8');
			@mtGetReadUPCA:=DoGetProcAddress('mtGetReadUPCA');
			@mtSetReadUPCA:=DoGetProcAddress('mtSetReadUPCA');
			@mtGetReadUPCE:=DoGetProcAddress('mtGetReadUPCE');
			@mtSetReadUPCE:=DoGetProcAddress('mtSetReadUPCE');
			@mtGetShowCheckDigit:=DoGetProcAddress('mtGetShowCheckDigit');
			@mtSetShowCheckDigit:=DoGetProcAddress('mtSetShowCheckDigit');
			@mtGetReadCode128:=DoGetProcAddress('mtGetReadCode128');
			@mtSetReadCode128:=DoGetProcAddress('mtSetReadCode128');
			@mtGetCode39NeedStartStop:=DoGetProcAddress('mtGetCode39NeedStartStop');
			@mtSetCode39NeedStartStop:=DoGetProcAddress('mtSetCode39NeedStartStop');
			@mtGetReadCode25:=DoGetProcAddress('mtGetReadCode25');
			@mtSetReadCode25:=DoGetProcAddress('mtSetReadCode25');
			@mtGetColorThreshold:=DoGetProcAddress('mtGetColorThreshold');
			@mtSetColorThreshold:=DoGetProcAddress('mtSetColorThreshold');
			@mtGetColorProcessingLevel:=DoGetProcAddress('mtGetColorProcessingLevel');
			@mtSetColorProcessingLevel:=DoGetProcAddress('mtSetColorProcessingLevel');
			@mtGetBarString:=DoGetProcAddress('mtGetBarString');
			@mtScanBarCode:=DoGetProcAddress('mtScanBarCode');
			@mtScanBarCodeFromBitmap:=DoGetProcAddress('mtScanBarCodeFromBitmap');
			@mtScanBarCodeFromDIB:=DoGetProcAddress('mtScanBarCodeFromDIB');
			@mtGetBarStringType:=DoGetProcAddress('mtGetBarStringType');
			@mtGetBarStringPos:=DoGetProcAddress('mtGetBarStringPos');
			@mtSetScanRect:=DoGetProcAddress('mtSetScanRect');
			@mtGetReadPatchCodes:=DoGetProcAddress('mtGetReadPatchCodes');
			@mtSetReadPatchCodes:=DoGetProcAddress('mtSetReadPatchCodes');

			{DT 09/06/04}
			@mtGetReadCodabar:=DoGetProcAddress('mtGetReadCodabar');
			@mtSetReadCodabar:=DoGetProcAddress('mtSetReadCodabar');
			@mtGetMaxLength:=DoGetProcAddress('mtGetMaxLength');
			@mtSetMaxLength:=DoGetProcAddress('mtSetMaxLength');
			@mtGetMinLength:=DoGetProcAddress('mtGetMinLength');
			@mtSetMinLength:=DoGetProcAddress('mtSetMinLength');
			@mtGetScanDirection:=DoGetProcAddress('mtGetScanDirection');
			@mtSetScanDirection:=DoGetProcAddress('mtSetScanDirection');

			{AJB 7/6/05}
			@mtGetSkewTolerance:=DoGetProcAddress('mtGetSkewTolerance');
			@mtSetSkewTolerance:=DoGetProcAddress('mtSetSkewTolerance');

      {Joe Skeen 3-May-2010}
      @mtGetBarCodeConfidence:=DoGetProcAddress('mtGetBarCodeConfidence');
      @mtGetReadDataMatrix:=DoGetProcAddress('mtGetReadDataMatrix');
      @mtSetReadDataMatrix:=DoGetProcAddress('mtSetReadDataMatrix');
      @mtGetReadPDF417:=DoGetProcAddress('mtGetReadPDF417');
      @mtSetReadPDF417:=DoGetProcAddress('mtSetReadPDF417');
      @mtGetConvertUPCEToEAN13:=DoGetProcAddress('mtGetConvertUPCEToEAN13');
      @mtSetConvertUPCEToEAN13:=DoGetProcAddress('mtSetConvertUPCEToEAN13');
      @mtExportXMLSettings:=DoGetProcAddress('mtExportXMLSettings');
      @mtGetAllowDuplicateValues:=DoGetProcAddress('mtGetAllowDuplicateValues');
      @mtGetBarStringDirection:=DoGetProcAddress('mtGetBarStringDirection');
      @mtGetCode25Checksum:=DoGetProcAddress('mtGetCode25Checksum');
      @mtGetCode39Checksum:=DoGetProcAddress('mtGetCode39Checksum');
      @mtGetDespeckle:=DoGetProcAddress('mtGetDespeckle');
      @mtGetEncoding:=DoGetProcAddress('mtGetEncoding');
      @mtGetErrorCorrection:=DoGetProcAddress('mtGetErrorCorrection');
      @mtGetExtendedCode39:=DoGetProcAddress('mtGetExtendedCode39');
      @mtGetGammaCorrection:=DoGetProcAddress('mtGetGammaCorrection');
      @mtGetMedianFilter:=DoGetProcAddress('mtGetMedianFilter');
      @mtGetMinSeparation:=DoGetProcAddress('mtGetMinSeparation');
      @mtGetMinSpaceBarWidth:=DoGetProcAddress('mtGetMinSpaceBarWidth');
      @mtGetPattern:=DoGetProcAddress('mtGetPattern');
      @mtGetPdfBpp:=DoGetProcAddress('mtGetPdfBpp');
      @mtGetPdfDpi:=DoGetProcAddress('mtGetPdfDpi');
      @mtGetPhotometric:=DoGetProcAddress('mtGetPhotometric');
      @mtGetReadCode25ni:=DoGetProcAddress('mtGetReadCode25ni');
      @mtGetReadMicroPDF417:=DoGetProcAddress('mtGetReadMicroPDF417');
      @mtGetReadNumeric:=DoGetProcAddress('mtGetReadNumeric');
      @mtGetReadShortCode128:=DoGetProcAddress('mtGetReadShortCode128');
      @mtGetShortCode128MinLength:=DoGetProcAddress('mtGetShortCode128MinLength');
      @mtGetSkewLineJump:=DoGetProcAddress('mtGetSkewLineJump');
      @mtGetTifSplitMode:=DoGetProcAddress('mtGetTifSplitMode');
      @mtGetTifSplitPath:=DoGetProcAddress('mtGetTifSplitPath');
      @mtGetUseOverSampling:=DoGetProcAddress('mtGetUseOverSampling');
      @mtLoadXMLSettings:=DoGetProcAddress('mtLoadXMLSettings');
      @mtProcessXML:=DoGetProcAddress('mtProcessXML');
      @mtSaveResults:=DoGetProcAddress('mtSaveResults');
      @mtSetAllowDuplicateValues:=DoGetProcAddress('mtSetAllowDuplicateValues');
      @mtSetCode25Checksum:=DoGetProcAddress('mtSetCode25Checksum');
      @mtSetCode39Checksum:=DoGetProcAddress('mtSetCode39Checksum');
      @mtSetDespeckle:=DoGetProcAddress('mtSetDespeckle');
      @mtSetEncoding:=DoGetProcAddress('mtSetEncoding');
      @mtSetErrorCorrection:=DoGetProcAddress('mtSetErrorCorrection');
      @mtSetExtendedCode39:=DoGetProcAddress('mtSetExtendedCode39');
      @mtSetGammaCorrection:=DoGetProcAddress('mtSetGammaCorrection');
      @mtSetMedianFilter:=DoGetProcAddress('mtSetMedianFilter');
      @mtSetMinSeparation:=DoGetProcAddress('mtSetMinSeparation');
      @mtSetMinSpaceBarWidth:=DoGetProcAddress('mtSetMinSpaceBarWidth');
      @mtSetPattern:=DoGetProcAddress('mtSetPattern');
      @mtSetPdfBpp:=DoGetProcAddress('mtSetPdfBpp');
      @mtSetPdfDpi:=DoGetProcAddress('mtSetPdfDpi');
      @mtSetPhotometric:=DoGetProcAddress('mtSetPhotometric');
      @mtSetReadCode25ni:=DoGetProcAddress('mtSetReadCode25ni');
      @mtSetReadMicroPDF417:=DoGetProcAddress('mtSetReadMicroPDF417');
      @mtSetReadNumeric:=DoGetProcAddress('mtSetReadNumeric');
      @mtSetReadShortCode128:=DoGetProcAddress('mtSetReadShortCode128');
      @mtSetShortCode128MinLength:=DoGetProcAddress('mtSetShortCode128MinLength');
      @mtSetSkewLineJump:=DoGetProcAddress('mtSetSkewLineJump');
      @mtSetTifSplitMode:=DoGetProcAddress('mtSetTifSplitMode');
      @mtSetTifSplitPath:=DoGetProcAddress('mtSetTifSplitPath');
      @mtSetUseOverSampling:=DoGetProcAddress('mtSetUseOverSampling');

      {AJB - new functions added 8/3/2012}
      @mtSetReadDatabar:=DoGetProcAddress('mtSetReadDatabar');
      @mtGetReadDatabar:=DoGetProcAddress('mtGetReadDatabar');
      @mtSetReadQRCode:=DoGetProcAddress('mtSetReadQRCode');
      @mtGetReadQRCode:=DoGetProcAddress('mtGetReadQRCode');
      @mtSetDatabarOptions:=DoGetProcAddress('mtSetDatabarOptions');
      @mtGetDatabarOptions:=DoGetProcAddress('mtGetDatabarOptions');
      @mtSetPdfImageOnly:=DoGetProcAddress('mtSetPdfImageOnly');
      @mtGetPdfImageOnly:=DoGetProcAddress('mtGetPdfImageOnly');
      @mtSetPdfImageExtractOptions:=DoGetProcAddress('mtSetPdfImageExtractOptions');
      @mtGetPdfImageExtractOptions:=DoGetProcAddress('mtGetPdfImageExtractOptions');
      @mtSetPdfImageRasterOptions:=DoGetProcAddress('mtSetPdfImageRasterOptions');
      @mtGetPdfImageRasterOptions:=DoGetProcAddress('mtGetPdfImageRasterOptions');

      @mtCreateBarcodeInstance:=DoGetProcAddress('mtCreateBarcodeInstance');
      @mtDestroyBarcodeInstance:=DoGetProcAddress('mtDestroyBarcodeInstance');



    end
		else
    begin
			result:=false;
		end;
	end;
end;

procedure DoneLibrary;
begin
	if LibraryHandle=0 then exit;

//	if not FreeLibrary(LibraryHandle) then RaiseLastOSError;
	FreeLibrary(LibraryHandle);

	mtGetBitmapResolution:=NIL;
	mtsetBitmapResolution:=NIL;
	mtGetDebugTraceFile:=NIL;
	mtsetDebugTraceFile:=NIL;
	mtsetLicenseKey:=NIL;
	mtGetLineJump:=NIL;
  mtsetLineJump:=NIL;
	mtGetLowerRatio:=NIL;
	mtsetLowerRatio:=NIL;
	mtGetMinOccurrence:=NIL;
	mtsetMinOccurrence:=NIL;
	mtGetMultipleRead:=NIL;
  mtsetMultipleRead:=NIL;
  mtGetNoiseReduction:=NIL;
  mtsetNoiseReduction:=NIL;
  mtGetPageNo:=NIL;
  mtsetPageNo:=NIL;
  mtGetPrefOccurrence:=NIL;
  mtsetPrefOccurrence:=NIL;
  mtGetQuietZoneSize:=NIL;
  mtsetQuietZoneSize:=NIL;
  mtGetRotation:=NIL;
  mtsetRotation:=NIL;
  mtGetUpperRatio:=NIL;
  mtsetUpperRatio:=NIL;
  mtGetReadCode39:=NIL;
  mtsetReadCode39:=NIL;
  mtGetReadEAN13:=NIL;
  mtsetReadEAN13:=NIL;
  mtGetReadEAN8:=NIL;
  mtsetReadEAN8:=NIL;
  mtGetReadUPCA:=NIL;
  mtsetReadUPCA:=NIL;
  mtGetReadUPCE:=NIL;
  mtsetReadUPCE:=NIL;
  mtGetShowCheckDigit:=NIL;
  mtsetShowCheckDigit:=NIL;
  mtGetReadCode128:=NIL;
  mtsetReadCode128:=NIL;
  mtGetCode39NeedStartStop:=NIL;
  mtsetCode39NeedStartStop:=NIL;
  mtGetReadCode25:=NIL;
  mtsetReadCode25:=NIL;
  mtGetColorThreshold:=NIL;
  mtsetColorThreshold:=NIL;
  mtGetColorProcessingLevel:=NIL;
  mtsetColorProcessingLevel:=NIL;
  mtGetBarString:=NIL;
  mtScanBarCode:=NIL;
	mtScanBarCodeFromBitmap:=NIL;
	mtScanBarCodeFromDIB:=NIL;
	mtGetBarStringType:=NIL;
  mtGetBarStringPos:=NIL;
  mtsetScanRect:=NIL;
  mtGetReadPatchCodes:=NIL;
  mtsetReadPatchCodes:=NIL;

	{DT 09/06/04}
	mtGetReadCodabar:=NIL;
	mtsetReadCodabar:=NIL;
	mtGetMaxLength:=NIL;
	mtsetMaxLength:=NIL;
	mtGetMinLength:=NIL;
	mtsetMinLength:=NIL;
	mtGetScanDirection:=NIL;
	mtsetScanDirection:=NIL;

	{AJB 7/6/05}
	mtGetSkewTolerance:=NIL;
	mtsetSkewTolerance:=NIL;

  {Joe Skeen 3-May-2010}
  mtGetBarCodeConfidence:=NIL;
  mtGetReadDataMatrix:=NIL;
  mtsetReadDataMatrix:=NIL;
  mtGetReadPDF417:=NIL;
  mtsetReadPDF417:=NIL;
  mtGetConvertUPCEToEAN13:=NIL;
  mtsetConvertUPCEToEAN13:=nil;
  mtExportXMLSettings:=nil;
  mtGetAllowDuplicateValues:=nil;
  mtGetBarStringDirection:=nil;
  mtGetCode25Checksum:=nil;
  mtGetCode39Checksum:=nil;
  mtGetDespeckle:=nil;
  mtGetEncoding:=nil;
  mtGetErrorCorrection:=nil;
  mtGetExtendedCode39:=nil;
  mtGetGammaCorrection:=nil;
  mtGetMedianFilter:=nil;
  mtGetMinSeparation:=nil;
  mtGetMinSpaceBarWidth:=nil;
  mtGetPattern:=nil;
  mtGetPdfBpp:=nil;
  mtGetPdfDpi:=nil;
  mtGetPhotometric:=nil;
  mtGetReadCode25ni:=nil;
  mtGetReadMicroPDF417:=nil;
  mtGetReadNumeric:=nil;
  mtGetReadShortCode128:=nil;
  mtGetShortCode128MinLength:=nil;
  mtGetSkewLineJump:=nil;
  mtGetTifSplitMode:=nil;
  mtGetTifSplitPath:=nil;
  mtGetUseOverSampling:=nil;
  mtLoadXMLSettings:=nil;
  mtProcessXML:=nil;
  mtSaveResults:=nil;
  mtsetAllowDuplicateValues:=nil;
  mtsetCode25Checksum:=nil;
  mtsetCode39Checksum:=nil;
  mtsetDespeckle:=nil;
  mtsetEncoding:=nil;
  mtsetErrorCorrection:=nil;
  mtsetExtendedCode39:=nil;
  mtsetGammaCorrection:=nil;
  mtsetMedianFilter:=nil;
  mtsetMinSeparation:=nil;
  mtsetMinSpaceBarWidth:=nil;
  mtsetPattern:=nil;
  mtsetPdfBpp:=nil;
  mtsetPdfDpi:=nil;
  mtsetPhotometric:=nil;
  mtsetReadCode25ni:=nil;
  mtsetReadMicroPDF417:=nil;
  mtsetReadNumeric:=nil;
  mtsetReadShortCode128:=nil;
  mtsetShortCode128MinLength:=nil;
  mtsetSkewLineJump:=nil;
  mtsetTifSplitMode:=nil;
  mtsetTifSplitPath:=nil;
  mtsetUseOverSampling:=nil;

	{AJB 7/6/05}
  mtsetReadDatabar:=nil;
  mtgetReadDatabar:=nil;
  mtsetReadQRCode:=nil;
  mtgetReadQRCode:=nil;
  mtsetDatabarOptions:=nil;
  mtgetDatabarOptions:=nil;
  mtsetPdfImageOnly:=nil;
  mtgetPdfImageOnly:=nil;
  mtsetPdfImageExtractoptions:=nil;
  mtgetPdfImageExtractoptions:=nil;
  mtsetPdfImageRasterOptions:=nil;
  mtgetPdfImageRasterOptions:=nil;
  mtCreateBarcodeInstance:=NIL;
  mtDestroyBarcodeInstance:=nil;

end;

initialization

finalization
  DoneLibrary;

end.
