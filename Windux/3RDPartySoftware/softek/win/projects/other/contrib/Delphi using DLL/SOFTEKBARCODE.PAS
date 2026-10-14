unit SoftekBarCode;

{ (c)2003 Marian Aldenhövel MBA Software-Consulting
     Rosenhain 23
     53123 Bonn

     marian@mba-software.de

  This unit dynamically imports the functions from the Softek Barcode Library.

  Usage:

  - Call InitLibrary from somewhere in your program before calling any of
    the functions.
  - See the documentation from Softek for the use of the actual functions.

AJB: 7/6/05 - Added the stuff necessary for SkewTolerance.

Joe Skeen 3-May-2010 - Added missing functions in the current toolkit
        jskeen@utah.gov   joe_skeen@hotmail.com

}

interface

uses Windows,SysUtils;

type short=shortint;
     long=integer;
     PLong=^long;

var stGetBitmapResolution:function():Integer; stdcall;
		stSetBitmapResolution:function(newValue:Integer):Integer; stdcall;
    stGetDebugTraceFile:function():PAnsiChar; stdcall;
    stSetDebugTraceFile:function(lpszNewValue:PAnsiChar):PAnsiChar; stdcall;
    stSetLicenseKey:function(lpszNewValue:PAnsiChar):PAnsiChar; stdcall;
    stGetLineJump:function():integer; stdcall;
    stSetLineJump:function(nNewValue:integer):integer; stdcall;
    stGetLowerRatio:function():double; stdcall;
    stSetLowerRatio:function(newValue:double):double; stdcall;
    stGetMinOccurrence:function():short; stdcall;    
    stSetMinOccurrence:function(nNewValue:short):short; stdcall;    
    stGetMultipleRead:function():bool; stdcall;    
    stSetMultipleRead:function(bNewValue:bool):bool; stdcall;
    stGetNoiseReduction:function():short; stdcall;
    stSetNoiseReduction:function(nNewValue:short):short; stdcall;
    stGetPageNo:function():short; stdcall;    
    stSetPageNo:function(nNewValue:integer):integer; stdcall;    
    stGetPrefOccurrence:function():short; stdcall;    
    stSetPrefOccurrence:function(nNewValue:short):short; stdcall;    
    stGetQuietZoneSize:function():short; stdcall;    
    stSetQuietZoneSize:function(nNewValue:short):short; stdcall;    
    stGetRotation:function():short; stdcall;    
    stSetRotation:function(nNewValue:short):short; stdcall;    
    stGetUpperRatio:function():double; stdcall;    
    stSetUpperRatio:function(newValue:double):double; stdcall;    
    stGetReadCode39:function():bool; stdcall;
    stSetReadCode39:function(bNewValue:bool):bool; stdcall;
    {Only functions with the 2-D version of the toolkit}
    stGetReadPDF417:function():bool; stdcall;
    stSetReadPDF417:function(bNewValue:bool):bool; stdcall;
    stGetReadEAN13:function():bool; stdcall;
    stSetReadEAN13:function(bNewValue:bool):bool; stdcall;
    stGetReadEAN8:function():bool; stdcall;
    stSetReadEAN8:function(bNewValue:bool):bool; stdcall;
    stGetReadUPCA:function():bool; stdcall;
    stSetReadUPCA:function(bNewValue:bool):bool; stdcall;
    stGetReadUPCE:function():bool; stdcall;
    stSetReadUPCE:function(bNewValue:bool):bool; stdcall;
    stGetShowCheckDigit:function():bool; stdcall;
    stSetShowCheckDigit:function(bNewValue:bool):bool; stdcall;
		stGetReadCode128:function():bool; stdcall;
		stSetReadCode128:function(bNewValue:bool):bool; stdcall;
		stGetCode39NeedStartStop:function():bool; stdcall;
		stSetCode39NeedStartStop:function(bNewValue:bool):bool; stdcall;
		stGetReadCode25:function():bool; stdcall;
		stSetReadCode25:function(bNewValue:bool):bool; stdcall;
		stGetColorThreshold:function():short; stdcall;
		stSetColorThreshold:function(nNewValue:short):short; stdcall;
		stGetColorProcessingLevel:function():short; stdcall;
		stSetColorProcessingLevel:function(nNewValue:short):short; stdcall;
		stGetBarString:function(index:short):PAnsiChar; stdcall;
		//stScanBarCode:function(FileName:PChar):short; stdcall;
		stScanBarCodeFromBitmap:function(hBitmap:long):short; stdcall;
		stScanBarCodeFromDIB:function(hDib:long):short; stdcall;
		stGetBarStringType:function(index:short):PAnsiChar; stdcall;
    stGetBarStringPos:function(nBarCode:short;var pTopLeftX,pTopLeftY,pBotRightX,pBotRightY:Long):short; stdcall;
    stSetScanRect:function(TopLeftX,TopLeftY,BottomRightX,BottomRightY:Long;MappingMode:short):bool; stdcall;
    stGetReadPatchCodes:function():bool; stdcall;
    stSetReadPatchCodes:function(bNewValue:bool):bool; stdcall;
		stSaveBitmap:function(hBitmap:Long;strFilePath:PAnsiChar):bool; stdcall;

		{DT 09/06/04}
		stGetReadCodabar:function():bool; stdcall;
		stSetReadCodabar:function(bNewValue:bool):bool; stdcall;
		stGetMaxLength:function():short; stdcall;
		stSetMaxLength:function(nNewValue:short):short; stdcall;
		stGetMinLength:function():short; stdcall;
		stSetMinLength:function(nNewValue:short):short; stdcall;
		stGetScanDirection:function():short; stdcall;
		stSetScanDirection:function(nNewValue:short):short; stdcall;
	{AJB 7/4/05}
    stGetSkewTolerance:function():short; stdcall;
    stSetSkewTolerance:function(nNewValue:short):short; stdcall;

    {Joe Skeen 3-May-2010}  //missing functions - some don't show in the documentation yet.
    stScanBarCode:function(FileName:PAnsiChar):short; stdcall; //corrected for use with unicode or non-unicode versions of Delphi
    stGetBarCodeConfidence: function(nBarcode: short): short; stdcall;
    stGetReadDataMatrix:function():bool; stdcall;
    stSetReadDataMatrix:function(bNewValue:bool):bool; stdcall;
    stGetConvertUPCEToEAN13: function(): bool; stdcall;
    stSetConvertUPCEToEAN13: function(bNewValue: bool): bool; stdcall;
    //stConvertCode39Extended
    stExportXMLSettings: function(var filePath: PAnsiChar): Integer; stdcall;
    stGetAllowDuplicateValues: function: bool; stdcall;
    //stGetBarStringA
    stGetBarStringDirection: function(nBarCode: short): short; stdcall;
    stGetCode25Checksum: function: bool; stdcall;
    stGetCode39Checksum: function: bool; stdcall;
    stGetDespeckle: function: bool; stdcall;
    stGetEncoding: function: short; stdcall;
    stGetErrorCorrection: function: bool; stdcall;
    stGetExtendedCode39: function: bool; stdcall;
    stGetGammaCorrection: function: short; stdcall;
    stGetMedianFilter: function: bool; stdcall;
    stGetMinSeparation: function: short; stdcall;
    stGetMinSpaceBarWidth: function: short; stdcall;
    stGetPattern: function: PAnsiChar; stdcall;
    //stGetPDFTool
    stGetPdfBpp: function: short; stdcall;
    stGetPdfDpi: function: short; stdcall;
    stGetPhotometric: function: bool; stdcall;
    stGetReadCode25ni: function: bool; stdcall;
    stGetReadMicroPDF417: function: bool; stdcall;
    stGetReadNumeric: function: bool; stdcall;
    stGetReadShortCode128: function: bool; stdcall;
    stGetShortCode128MinLength: function: short; stdcall;
    stGetSkewLineJump: function: short; stdcall;
    //stGetTIFPageCount
    stGetTifSplitMode: function: short; stdcall;
    stGetTifSplitPath: function: PAnsiChar; stdcall;
    stGetUseOverSampling: function: bool; stdcall;
    //*stIsEval
    //*stLibBarCodeFree
    //*stLoadGIFImage
    //*stLoadJPEGImage
    //*stLoadPDFImage
    //*stLoadPDFImageH
    //*stLoadPNGImage
    //*stLoadTIFImage
    stLoadXMLSettings: function(filePath: PAnsiChar; silent: Byte): Integer; stdcall;
    stProcessXML: function(inputFilePath: PAnsiChar; outputFilePath: PAnsiChar; silent: Byte): Integer; stdcall;
    //stSaveDIBtoBMP
    stSaveResults: function(var filePath: PAnsiChar): Integer; stdcall;
    //stScanBarCodeFromBitmapDump
    stSetAllowDuplicateValues: function(bNewValue: bool): bool; stdcall;
    stSetCode25Checksum: function(bNewValue: bool): bool; stdcall;
    stSetCode39Checksum: function(bNewValue: bool): bool; stdcall;
    stSetDespeckle: function(bNewValue: bool): bool; stdcall;
    stSetEncoding: function(nNewValue: short): short; stdcall;
    stSetErrorCorrection: function(bNewValue: bool): bool; stdcall;
    stSetExtendedCode39: function(bNewValue: bool): bool; stdcall;
    stSetGammaCorrection: function(nNewValue: short): short; stdcall;
    stSetMedianFilter: function(bNewValue: bool): bool; stdcall;
    stSetMinSeparation: function(nNewValue: short): short; stdcall;
    stSetMinSpaceBarWidth: function(nNewValue: short): short; stdcall;
    //stSetPDFTool
    stSetPattern: function(var lpszNewValue: PAnsiChar):PAnsiChar; stdcall;
    stSetPdfBpp: function(nNewValue: short): short; stdcall;
    stSetPdfDpi: function(nNewValue: short): short; stdcall;
    stSetPhotometric: function(bPhotometric: bool): bool; stdcall;
    stSetReadCode25ni: function(bNewValue: bool): bool; stdcall;
    stSetReadMicroPDF417: function(bNewValue: bool): bool; stdcall;
    stSetReadNumeric: function(bNewValue: bool): bool; stdcall;
    stSetReadShortCode128: function(bNewValue: bool): bool; stdcall;
    stSetShortCode128MinLength: function(sNewValue: short): short; stdcall;
    stSetSkewLineJump: function(nNewValue: short): short; stdcall;
    //stSetTempFolder
    stSetTifSplitMode: function(sNewValue: short): short; stdcall;
    stSetTifSplitPath: function(szPath: PAnsiChar): PAnsiChar; stdcall;
    stSetUseOverSampling: function(bNewValue: bool): bool; stdcall;

function stGetErrorString(ErrorCode:short):string;

function InitLibrary:boolean;
procedure DoneLibrary;

type ESoftekBarCodeError=class(Exception);

implementation

function stGetErrorString(ErrorCode:short):string;
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

function InitLibrary:boolean;

	function DoGetProcAdress(aName:string):pointer;
	begin
		Result:=GetProcAddress(LibraryHandle,PChar(aName));
//    if Result=NIL then RaiseLastOSError;
	end;

begin
	result:=true;
	if LibraryHandle=0 then begin
		LibraryHandle:=LoadLibrary('SoftekBarCodeDLL.dll');
		if LibraryHandle>0 then
    begin
			@stGetBitmapResolution:=DoGetProcAdress('stGetBitmapResolution');
			@stSetBitmapResolution:=DoGetProcAdress('stSetBitmapResolution');
			@stGetDebugTraceFile:=DoGetProcAdress('stGetDebugTraceFile');
			@stSetDebugTraceFile:=DoGetProcAdress('stSetDebugTraceFile');
			@stSetLicenseKey:=DoGetProcAdress('stSetLicenseKey');
			@stGetLineJump:=DoGetProcAdress('stGetLineJump');
			@stSetLineJump:=DoGetProcAdress('stSetLineJump');
			@stGetLowerRatio:=DoGetProcAdress('stGetLowerRatio');
			@stSetLowerRatio:=DoGetProcAdress('stSetLowerRatio');
			@stGetMinOccurrence:=DoGetProcAdress('stGetMinOccurrence');
			@stSetMinOccurrence:=DoGetProcAdress('stSetMinOccurrence');
			@stGetMultipleRead:=DoGetProcAdress('stGetMultipleRead');
			@stSetMultipleRead:=DoGetProcAdress('stSetMultipleRead');
			@stGetNoiseReduction:=DoGetProcAdress('stGetNoiseReduction');
			@stSetNoiseReduction:=DoGetProcAdress('stSetNoiseReduction');
			@stGetPageNo:=DoGetProcAdress('stGetPageNo');
			@stSetPageNo:=DoGetProcAdress('stSetPageNo');
			@stGetPrefOccurrence:=DoGetProcAdress('stGetPrefOccurrence');
			@stSetPrefOccurrence:=DoGetProcAdress('stSetPrefOccurrence');
			@stGetQuietZoneSize:=DoGetProcAdress('stGetQuietZoneSize');
			@stSetQuietZoneSize:=DoGetProcAdress('stSetQuietZoneSize');
			@stGetRotation:=DoGetProcAdress('stGetRotation');
			@stSetRotation:=DoGetProcAdress('stSetRotation');
			@stGetUpperRatio:=DoGetProcAdress('stGetUpperRatio');
			@stSetUpperRatio:=DoGetProcAdress('stSetUpperRatio');
			@stGetReadCode39:=DoGetProcAdress('stGetReadCode39');
			@stSetReadCode39:=DoGetProcAdress('stSetReadCode39');
			@stGetReadEAN13:=DoGetProcAdress('stGetReadEAN13');
			@stSetReadEAN13:=DoGetProcAdress('stSetReadEAN13');
			@stGetReadEAN8:=DoGetProcAdress('stGetReadEAN8');
			@stSetReadEAN8:=DoGetProcAdress('stSetReadEAN8');
			@stGetReadUPCA:=DoGetProcAdress('stGetReadUPCA');
			@stSetReadUPCA:=DoGetProcAdress('stSetReadUPCA');
			@stGetReadUPCE:=DoGetProcAdress('stGetReadUPCE');
			@stSetReadUPCE:=DoGetProcAdress('stSetReadUPCE');
			@stGetShowCheckDigit:=DoGetProcAdress('stGetShowCheckDigit');
			@stSetShowCheckDigit:=DoGetProcAdress('stSetShowCheckDigit');
			@stGetReadCode128:=DoGetProcAdress('stGetReadCode128');
			@stSetReadCode128:=DoGetProcAdress('stSetReadCode128');
			@stGetCode39NeedStartStop:=DoGetProcAdress('stGetCode39NeedStartStop');
			@stSetCode39NeedStartStop:=DoGetProcAdress('stSetCode39NeedStartStop');
			@stGetReadCode25:=DoGetProcAdress('stGetReadCode25');
			@stSetReadCode25:=DoGetProcAdress('stSetReadCode25');
			@stGetColorThreshold:=DoGetProcAdress('stGetColorThreshold');
			@stSetColorThreshold:=DoGetProcAdress('stSetColorThreshold');
			@stGetColorProcessingLevel:=DoGetProcAdress('stGetColorProcessingLevel');
			@stSetColorProcessingLevel:=DoGetProcAdress('stSetColorProcessingLevel');
			@stGetBarString:=DoGetProcAdress('stGetBarString');
			@stScanBarCode:=DoGetProcAdress('stScanBarCode');
			@stScanBarCodeFromBitmap:=DoGetProcAdress('stScanBarCodeFromBitmap');
			@stScanBarCodeFromDIB:=DoGetProcAdress('stScanBarCodeFromDIB');
			@stGetBarStringType:=DoGetProcAdress('stGetBarStringType');
			@stGetBarStringPos:=DoGetProcAdress('stGetBarStringPos');
			@stSetScanRect:=DoGetProcAdress('stSetScanRect');
			@stGetReadPatchCodes:=DoGetProcAdress('stGetReadPatchCodes');
			@stSetReadPatchCodes:=DoGetProcAdress('stSetReadPatchCodes');
			@stSaveBitmap:=DoGetProcAdress('stSaveBitmap');

			{DT 09/06/04}
			@stGetReadCodabar:=DoGetProcAdress('stGetReadCodabar');
			@stSetReadCodabar:=DoGetProcAdress('stSetReadCodabar');
			@stGetMaxLength:=DoGetProcAdress('stGetMaxLength');
			@stSetMaxLength:=DoGetProcAdress('stSetMaxLength');
			@stGetMinLength:=DoGetProcAdress('stGetMinLength');
			@stSetMinLength:=DoGetProcAdress('stSetMinLength');
			@stGetScanDirection:=DoGetProcAdress('stGetScanDirection');
			@stSetScanDirection:=DoGetProcAdress('stSetScanDirection');

			{AJB 7/6/05}
			@stGetSkewTolerance:=DoGetProcAdress('stGetSkewTolerance');
			@stSetSkewTolerance:=DoGetProcAdress('stSetSkewTolerance');

      {Joe Skeen 3-May-2010}
      @stGetBarCodeConfidence:=DoGetProcAdress('stGetBarCodeConfidence');
      @stGetReadDataMatrix:=DoGetProcAdress('stGetReadDataMatrix');
      @stSetReadDataMatrix:=DoGetProcAdress('stSetReadDataMatrix');
      @stGetReadPDF417:=DoGetProcAdress('stGetReadPDF417');
      @stSetReadPDF417:=DoGetProcAdress('stSetReadPDF417');
      @stGetConvertUPCEToEAN13:=DoGetProcAdress('stGetConvertUPCEToEAN13');
      @stSetConvertUPCEToEAN13:=DoGetProcAdress('stSetConvertUPCEToEAN13');
      @stExportXMLSettings:=DoGetProcAdress('stExportXMLSettings');
      @stGetAllowDuplicateValues:=DoGetProcAdress('stGetAllowDuplicateValues');
      @stGetBarStringDirection:=DoGetProcAdress('stGetBarStringDirection');
      @stGetCode25Checksum:=DoGetProcAdress('stGetCode25Checksum');
      @stGetCode39Checksum:=DoGetProcAdress('stGetCode39Checksum');
      @stGetDespeckle:=DoGetProcAdress('stGetDespeckle');
      @stGetEncoding:=DoGetProcAdress('stGetEncoding');
      @stGetErrorCorrection:=DoGetProcAdress('stGetErrorCorrection');
      @stGetExtendedCode39:=DoGetProcAdress('stGetExtendedCode39');
      @stGetGammaCorrection:=DoGetProcAdress('stGetGammaCorrection');
      @stGetMedianFilter:=DoGetProcAdress('stGetMedianFilter');
      @stGetMinSeparation:=DoGetProcAdress('stGetMinSeparation');
      @stGetMinSpaceBarWidth:=DoGetProcAdress('stGetMinSpaceBarWidth');
      @stGetPattern:=DoGetProcAdress('stGetPattern');
      @stGetPdfBpp:=DoGetProcAdress('stGetPdfBpp');
      @stGetPdfDpi:=DoGetProcAdress('stGetPdfDpi');
      @stGetPhotometric:=DoGetProcAdress('stGetPhotometric');
      @stGetReadCode25ni:=DoGetProcAdress('stGetReadCode25ni');
      @stGetReadMicroPDF417:=DoGetProcAdress('stGetReadMicroPDF417');
      @stGetReadNumeric:=DoGetProcAdress('stGetReadNumeric');
      @stGetReadShortCode128:=DoGetProcAdress('stGetReadShortCode128');
      @stGetShortCode128MinLength:=DoGetProcAdress('stGetShortCode128MinLength');
      @stGetSkewLineJump:=DoGetProcAdress('stGetSkewLineJump');
      @stGetTifSplitMode:=DoGetProcAdress('stGetTifSplitMode');
      @stGetTifSplitPath:=DoGetProcAdress('stGetTifSplitPath');
      @stGetUseOverSampling:=DoGetProcAdress('stGetUseOverSampling');
      @stLoadXMLSettings:=DoGetProcAdress('stLoadXMLSettings');
      @stProcessXML:=DoGetProcAdress('stProcessXML');
      @stSaveResults:=DoGetProcAdress('stSaveResults');
      @stSetAllowDuplicateValues:=DoGetProcAdress('stSetAllowDuplicateValues');
      @stSetCode25Checksum:=DoGetProcAdress('stSetCode25Checksum');
      @stSetCode39Checksum:=DoGetProcAdress('stSetCode39Checksum');
      @stSetDespeckle:=DoGetProcAdress('stSetDespeckle');
      @stSetEncoding:=DoGetProcAdress('stSetEncoding');
      @stSetErrorCorrection:=DoGetProcAdress('stSetErrorCorrection');
      @stSetExtendedCode39:=DoGetProcAdress('stSetExtendedCode39');
      @stSetGammaCorrection:=DoGetProcAdress('stSetGammaCorrection');
      @stSetMedianFilter:=DoGetProcAdress('stSetMedianFilter');
      @stSetMinSeparation:=DoGetProcAdress('stSetMinSeparation');
      @stSetMinSpaceBarWidth:=DoGetProcAdress('stSetMinSpaceBarWidth');
      @stSetPattern:=DoGetProcAdress('stSetPattern');
      @stSetPdfBpp:=DoGetProcAdress('stSetPdfBpp');
      @stSetPdfDpi:=DoGetProcAdress('stSetPdfDpi');
      @stSetPhotometric:=DoGetProcAdress('stSetPhotometric');
      @stSetReadCode25ni:=DoGetProcAdress('stSetReadCode25ni');
      @stSetReadMicroPDF417:=DoGetProcAdress('stSetReadMicroPDF417');
      @stSetReadNumeric:=DoGetProcAdress('stSetReadNumeric');
      @stSetReadShortCode128:=DoGetProcAdress('stSetReadShortCode128');
      @stSetShortCode128MinLength:=DoGetProcAdress('stSetShortCode128MinLength');
      @stSetSkewLineJump:=DoGetProcAdress('stSetSkewLineJump');
      @stSetTifSplitMode:=DoGetProcAdress('stSetTifSplitMode');
      @stSetTifSplitPath:=DoGetProcAdress('stSetTifSplitPath');
      @stSetUseOverSampling:=DoGetProcAdress('stSetUseOverSampling');
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

	stGetBitmapResolution:=NIL;
	stSetBitmapResolution:=NIL;
	stGetDebugTraceFile:=NIL;
	stSetDebugTraceFile:=NIL;
	stSetLicenseKey:=NIL;
	stGetLineJump:=NIL;
  stSetLineJump:=NIL;
	stGetLowerRatio:=NIL;
	stSetLowerRatio:=NIL;
	stGetMinOccurrence:=NIL;
	stSetMinOccurrence:=NIL;
	stGetMultipleRead:=NIL;
  stSetMultipleRead:=NIL;
  stGetNoiseReduction:=NIL;
  stSetNoiseReduction:=NIL;
  stGetPageNo:=NIL;
  stSetPageNo:=NIL;
  stGetPrefOccurrence:=NIL;
  stSetPrefOccurrence:=NIL;
  stGetQuietZoneSize:=NIL;
  stSetQuietZoneSize:=NIL;
  stGetRotation:=NIL;
  stSetRotation:=NIL;
  stGetUpperRatio:=NIL;
  stSetUpperRatio:=NIL;
  stGetReadCode39:=NIL;
  stSetReadCode39:=NIL;
  stGetReadEAN13:=NIL;
  stSetReadEAN13:=NIL;
  stGetReadEAN8:=NIL;
  stSetReadEAN8:=NIL;
  stGetReadUPCA:=NIL;
  stSetReadUPCA:=NIL;
  stGetReadUPCE:=NIL;
  stSetReadUPCE:=NIL;
  stGetShowCheckDigit:=NIL;
  stSetShowCheckDigit:=NIL;
  stGetReadCode128:=NIL;
  stSetReadCode128:=NIL;
  stGetCode39NeedStartStop:=NIL;
  stSetCode39NeedStartStop:=NIL;
  stGetReadCode25:=NIL;
  stSetReadCode25:=NIL;
  stGetColorThreshold:=NIL;
  stSetColorThreshold:=NIL;
  stGetColorProcessingLevel:=NIL;
  stSetColorProcessingLevel:=NIL;
  stGetBarString:=NIL;
  stScanBarCode:=NIL;
	stScanBarCodeFromBitmap:=NIL;
	stScanBarCodeFromDIB:=NIL;
	stGetBarStringType:=NIL;
  stGetBarStringPos:=NIL;
  stSetScanRect:=NIL;
  stGetReadPatchCodes:=NIL;
  stSetReadPatchCodes:=NIL;
	stSaveBitmap:=NIL;

	{DT 09/06/04}
	stGetReadCodabar:=NIL;
	stSetReadCodabar:=NIL;
	stGetMaxLength:=NIL;
	stSetMaxLength:=NIL;
	stGetMinLength:=NIL;
	stSetMinLength:=NIL;
	stGetScanDirection:=NIL;
	stSetScanDirection:=NIL;

	{AJB 7/6/05}
	stGetSkewTolerance:=NIL;
	stSetSkewTolerance:=NIL;

  {Joe Skeen 3-May-2010}
  stGetBarCodeConfidence:=NIL;
  stGetReadDataMatrix:=NIL;
  stSetReadDataMatrix:=NIL;
  stGetReadPDF417:=NIL;
  stSetReadPDF417:=NIL;
  stGetConvertUPCEToEAN13:=NIL;
  stSetConvertUPCEToEAN13:=nil;
  stExportXMLSettings:=nil;
  stGetAllowDuplicateValues:=nil;
  stGetBarStringDirection:=nil;
  stGetCode25Checksum:=nil;
  stGetCode39Checksum:=nil;
  stGetDespeckle:=nil;
  stGetEncoding:=nil;
  stGetErrorCorrection:=nil;
  stGetExtendedCode39:=nil;
  stGetGammaCorrection:=nil;
  stGetMedianFilter:=nil;
  stGetMinSeparation:=nil;
  stGetMinSpaceBarWidth:=nil;
  stGetPattern:=nil;
  stGetPdfBpp:=nil;
  stGetPdfDpi:=nil;
  stGetPhotometric:=nil;
  stGetReadCode25ni:=nil;
  stGetReadMicroPDF417:=nil;
  stGetReadNumeric:=nil;
  stGetReadShortCode128:=nil;
  stGetShortCode128MinLength:=nil;
  stGetSkewLineJump:=nil;
  stGetTifSplitMode:=nil;
  stGetTifSplitPath:=nil;
  stGetUseOverSampling:=nil;
  stLoadXMLSettings:=nil;
  stProcessXML:=nil;
  stSaveResults:=nil;
  stSetAllowDuplicateValues:=nil;
  stSetCode25Checksum:=nil;
  stSetCode39Checksum:=nil;
  stSetDespeckle:=nil;
  stSetEncoding:=nil;
  stSetErrorCorrection:=nil;
  stSetExtendedCode39:=nil;
  stSetGammaCorrection:=nil;
  stSetMedianFilter:=nil;
  stSetMinSeparation:=nil;
  stSetMinSpaceBarWidth:=nil;
  stSetPattern:=nil;
  stSetPdfBpp:=nil;
  stSetPdfDpi:=nil;
  stSetPhotometric:=nil;
  stSetReadCode25ni:=nil;
  stSetReadMicroPDF417:=nil;
  stSetReadNumeric:=nil;
  stSetReadShortCode128:=nil;
  stSetShortCode128MinLength:=nil;
  stSetSkewLineJump:=nil;
  stSetTifSplitMode:=nil;
  stSetTifSplitPath:=nil;
  stSetUseOverSampling:=nil;

end;

initialization

finalization
  DoneLibrary;

end.
