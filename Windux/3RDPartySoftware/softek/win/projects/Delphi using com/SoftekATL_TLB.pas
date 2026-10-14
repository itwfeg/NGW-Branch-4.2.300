unit SoftekATL_TLB;

// ************************************************************************ //
// WARNING                                                                    
// -------                                                                    
// The types declared in this file were generated from data read from a       
// Type Library. If this type library is explicitly or indirectly (via        
// another type library referring to this type library) re-imported, or the   
// 'Refresh' command of the Type Library Editor activated while editing the   
// Type Library, the contents of this file will be regenerated and all        
// manual modifications will be lost.                                         
// ************************************************************************ //

// $Rev: 45604 $
// File generated on 06/03/2012 18:03:20 from Type Library described below.

// ************************************************************************  //
// Type Lib: C:\Windows\System32\softekatl.dll (1)
// LIBID: {0CBF94E9-24EB-4268-8D21-495C586F3A2E}
// LCID: 0
// Helpfile: 
// HelpString: SoftekATL 7.5.1 Type Library
// DepndLst: 
//   (1) v2.0 stdole, (C:\Windows\system32\stdole2.tlb)
// SYS_KIND: SYS_WIN32
// ************************************************************************ //
{$TYPEDADDRESS OFF} // Unit must be compiled without type-checked pointers. 
{$WARN SYMBOL_PLATFORM OFF}
{$WRITEABLECONST ON}
{$VARPROPSETTER ON}
{$ALIGN 4}

interface

uses Winapi.Windows, System.Classes, System.Variants, System.Win.StdVCL, Vcl.Graphics, Vcl.OleServer, Winapi.ActiveX;
  

// *********************************************************************//
// GUIDS declared in the TypeLibrary. Following prefixes are used:        
//   Type Libraries     : LIBID_xxxx                                      
//   CoClasses          : CLASS_xxxx                                      
//   DISPInterfaces     : DIID_xxxx                                       
//   Non-DISP interfaces: IID_xxxx                                        
// *********************************************************************//
const
  // TypeLibrary Major and minor versions
  SoftekATLMajorVersion = 1;
  SoftekATLMinorVersion = 0;

  LIBID_SoftekATL: TGUID = '{0CBF94E9-24EB-4268-8D21-495C586F3A2E}';

  IID_IBarcode: TGUID = '{92EAAC5E-98EB-4AC6-BB37-932A79F3B7AD}';
  CLASS_CBarcode: TGUID = '{11E7DA45-B56D-4078-89F6-D3D651EC4CD6}';
type

// *********************************************************************//
// Forward declaration of types defined in TypeLibrary                    
// *********************************************************************//
  IBarcode = interface;
  IBarcodeDisp = dispinterface;

// *********************************************************************//
// Declaration of CoClasses defined in Type Library                       
// (NOTE: Here we map each CoClass to its Default Interface)              
// *********************************************************************//
  CBarcode = IBarcode;


// *********************************************************************//
// Declaration of structures, unions and aliases.                         
// *********************************************************************//
  PInteger1 = ^Integer; {*}


// *********************************************************************//
// Interface: IBarcode
// Flags:     (4416) Dual OleAutomation Dispatchable
// GUID:      {92EAAC5E-98EB-4AC6-BB37-932A79F3B7AD}
// *********************************************************************//
  IBarcode = interface(IDispatch)
    ['{92EAAC5E-98EB-4AC6-BB37-932A79F3B7AD}']
    procedure ScanBarCode(const strFilePath: WideString); safecall;
    function Get_BarCodeCount: Smallint; safecall;
    function Get_BarString(nBarCode: Smallint): WideString; safecall;
    function Get_BitmapResolution: Smallint; safecall;
    procedure Set_BitmapResolution(pVal: Smallint); safecall;
    function Get_DebugTraceFile: WideString; safecall;
    procedure Set_DebugTraceFile(const pVal: WideString); safecall;
    function Get_LineJump: Integer; safecall;
    procedure Set_LineJump(pVal: Integer); safecall;
    function Get_LowerRatio: Double; safecall;
    procedure Set_LowerRatio(pVal: Double); safecall;
    function Get_MinOccurrence: Smallint; safecall;
    procedure Set_MinOccurrence(pVal: Smallint); safecall;
    function Get_MultipleRead: Smallint; safecall;
    procedure Set_MultipleRead(pVal: Smallint); safecall;
    function Get_NoiseReduction: Smallint; safecall;
    procedure Set_NoiseReduction(pVal: Smallint); safecall;
    function Get_PageNo: Smallint; safecall;
    procedure Set_PageNo(pVal: Smallint); safecall;
    function Get_PrefOccurrence: Smallint; safecall;
    procedure Set_PrefOccurrence(pVal: Smallint); safecall;
    function Get_QuietZoneSize: Smallint; safecall;
    procedure Set_QuietZoneSize(pVal: Smallint); safecall;
    function Get_Rotation: Smallint; safecall;
    procedure Set_Rotation(pVal: Smallint); safecall;
    function Get_UpperRatio: Double; safecall;
    procedure Set_UpperRatio(pVal: Double); safecall;
    function Get_ReadCode39: Smallint; safecall;
    procedure Set_ReadCode39(pVal: Smallint); safecall;
    function Get_ReadEAN13: Smallint; safecall;
    procedure Set_ReadEAN13(pVal: Smallint); safecall;
    function Get_ReadEAN8: Smallint; safecall;
    procedure Set_ReadEAN8(pVal: Smallint); safecall;
    function Get_ReadUPCA: Smallint; safecall;
    procedure Set_ReadUPCA(pVal: Smallint); safecall;
    function Get_ReadUPCE: Smallint; safecall;
    procedure Set_ReadUPCE(pVal: Smallint); safecall;
    function Get_ShowCheckDigit: Smallint; safecall;
    procedure Set_ShowCheckDigit(pVal: Smallint); safecall;
    function Get_ReadCode128: Smallint; safecall;
    procedure Set_ReadCode128(pVal: Smallint); safecall;
    function Get_Code39NeedStartStop: Smallint; safecall;
    procedure Set_Code39NeedStartStop(pVal: Smallint); safecall;
    function Get_ReadCode25: Smallint; safecall;
    procedure Set_ReadCode25(pVal: Smallint); safecall;
    function Get_ColorThreshold: Smallint; safecall;
    procedure Set_ColorThreshold(pVal: Smallint); safecall;
    function Get_BarStringType(nBarCode: Smallint): WideString; safecall;
    function Get_ReadPatchCodes: Smallint; safecall;
    procedure Set_ReadPatchCodes(pVal: Smallint); safecall;
    function Get_UseOverSampling: Smallint; safecall;
    procedure Set_UseOverSampling(pVal: Smallint); safecall;
    function Get_ScanDirection: Smallint; safecall;
    procedure Set_ScanDirection(pVal: Smallint); safecall;
    function Get_ConvertUPCEToEAN13: Smallint; safecall;
    procedure Set_ConvertUPCEToEAN13(pVal: Smallint); safecall;
    function Get_BarStringDirection(nBarCode: Smallint): Smallint; safecall;
    function Get_ReadCodabar: Smallint; safecall;
    procedure Set_ReadCodabar(pVal: Smallint); safecall;
    procedure SetScanRect(TopLeftX: Integer; TopLeftY: Integer; BottomRightX: Integer; 
                          BottomRightY: Integer; MappingMode: Smallint); safecall;
    procedure GetBarStringPos(nBarCode: Smallint; var pTopLeftX: Integer; var pTopLeftY: Integer; 
                              var pBottomRightX: Integer; var pBottomRightY: Integer); safecall;
    function Get_MinLength: Smallint; safecall;
    procedure Set_MinLength(pVal: Smallint); safecall;
    function Get_MaxLength: Smallint; safecall;
    procedure Set_MaxLength(pVal: Smallint); safecall;
    function Get_TifSplitMode: Smallint; safecall;
    procedure Set_TifSplitMode(pVal: Smallint); safecall;
    function Get_TifSplitPath: WideString; safecall;
    procedure Set_TifSplitPath(const pVal: WideString); safecall;
    function Get_SkewTolerance: Smallint; safecall;
    procedure Set_SkewTolerance(pVal: Smallint); safecall;
    function Get_ReadCode25ni: Smallint; safecall;
    procedure Set_ReadCode25ni(pVal: Smallint); safecall;
    function Get_Photometric: Smallint; safecall;
    procedure Set_Photometric(pVal: Smallint); safecall;
    function Get_AllowDuplicateValues: Smallint; safecall;
    procedure Set_AllowDuplicateValues(pVal: Smallint); safecall;
    function Get_BarStringPage(nBarCode: Smallint): Smallint; safecall;
    function Get_BarStringTopLeftX(nBarCode: Smallint): Integer; safecall;
    function Get_BarStringTopLeftY(nBarCode: Smallint): Integer; safecall;
    function Get_BarStringBottomRightX(nBarCode: Smallint): Integer; safecall;
    function Get_BarStringBottomRightY(nBarCode: Smallint): Integer; safecall;
    function Get_Code39Checksum: Smallint; safecall;
    procedure Set_Code39Checksum(pVal: Smallint); safecall;
    function Get_ExtendedCode39: Smallint; safecall;
    procedure Set_ExtendedCode39(pVal: Smallint); safecall;
    function Get_ErrorCorrection: Smallint; safecall;
    procedure Set_ErrorCorrection(pVal: Smallint); safecall;
    function Get_ReadNumeric: Smallint; safecall;
    procedure Set_ReadNumeric(pVal: Smallint); safecall;
    function Get_Despeckle: Smallint; safecall;
    procedure Set_Despeckle(pVal: Smallint); safecall;
    function Get_MinSeparation: Smallint; safecall;
    procedure Set_MinSeparation(pVal: Smallint); safecall;
    function Get_PdfDpi: Smallint; safecall;
    procedure Set_PdfDpi(pVal: Smallint); safecall;
    function Get_PdfBpp: Smallint; safecall;
    procedure Set_PdfBpp(pVal: Smallint); safecall;
    function Get_MinSpaceBarWidth: Smallint; safecall;
    procedure Set_MinSpaceBarWidth(pVal: Smallint); safecall;
    function Get_Pattern: WideString; safecall;
    procedure Set_Pattern(const pVal: WideString); safecall;
    function Get_ReadPDF417: Smallint; safecall;
    procedure Set_ReadPDF417(pVal: Smallint); safecall;
    function Get_MedianFilter: Smallint; safecall;
    procedure Set_MedianFilter(pVal: Smallint); safecall;
    procedure SaveXMLResults(const strFilePath: WideString); safecall;
    function Get_XMLRetval: Smallint; safecall;
    procedure Set_XMLRetval(pVal: Smallint); safecall;
    procedure LoadXMLSettings(const strFilePath: WideString; silent: Smallint); safecall;
    procedure ExportXMLSettings(const strFilePath: WideString); safecall;
    procedure ProcessXML(const inputFilePath: WideString; const outputFilePath: WideString; 
                         silent: Smallint); safecall;
    function Get_TifPageCount(const path: WideString): Smallint; safecall;
    function Get_Code25Checksum: Smallint; safecall;
    procedure Set_Code25Checksum(pVal: Smallint); safecall;
    function Get_Encoding: Smallint; safecall;
    procedure Set_Encoding(pVal: Smallint); safecall;
    function Get_GammaCorrection: Smallint; safecall;
    procedure Set_GammaCorrection(pVal: Smallint); safecall;
    function Get_SkewLineJump: Smallint; safecall;
    procedure Set_SkewLineJump(pVal: Smallint); safecall;
    function Get_ReadMicroPDF417: Smallint; safecall;
    procedure Set_ReadMicroPDF417(pVal: Smallint); safecall;
    function Get_ReadShortCode128: Smallint; safecall;
    procedure Set_ReadShortCode128(pVal: Smallint); safecall;
    function Get_ShortCode128MinLength: Smallint; safecall;
    procedure Set_ShortCode128MinLength(pVal: Smallint); safecall;
    function Get_ReadDataMatrix: Smallint; safecall;
    procedure Set_ReadDataMatrix(pVal: Smallint); safecall;
    function Get_ColorProcessingLevel: Smallint; safecall;
    procedure Set_ColorProcessingLevel(pVal: Smallint); safecall;
    function Get_ReadDatabar: Smallint; safecall;
    procedure Set_ReadDatabar(pVal: Smallint); safecall;
    function Get_DatabarOptions: Smallint; safecall;
    procedure Set_DatabarOptions(pVal: Smallint); safecall;
    function Get_LicenseKey: WideString; safecall;
    procedure Set_LicenseKey(const pVal: WideString); safecall;
    function Get_ReadCode93: Smallint; safecall;
    procedure Set_ReadCode93(pVal: Smallint); safecall;
    function Get_PdfImageOnly: Smallint; safecall;
    procedure Set_PdfImageOnly(pVal: Smallint); safecall;
    function Get_PdfImageExtractOptions: Smallint; safecall;
    procedure Set_PdfImageExtractOptions(pVal: Smallint); safecall;
    function Get_PdfImageRasterOptions: Smallint; safecall;
    procedure Set_PdfImageRasterOptions(pVal: Smallint); safecall;
    function Get_LastError: LongWord; safecall;
    function Get_LastWinError: LongWord; safecall;
    function Get_ReadQRCode: Smallint; safecall;
    procedure Set_ReadQRCode(pVal: Smallint); safecall;
    property BarCodeCount: Smallint read Get_BarCodeCount;
    property BarString[nBarCode: Smallint]: WideString read Get_BarString;
    property BitmapResolution: Smallint read Get_BitmapResolution write Set_BitmapResolution;
    property DebugTraceFile: WideString read Get_DebugTraceFile write Set_DebugTraceFile;
    property LineJump: Integer read Get_LineJump write Set_LineJump;
    property LowerRatio: Double read Get_LowerRatio write Set_LowerRatio;
    property MinOccurrence: Smallint read Get_MinOccurrence write Set_MinOccurrence;
    property MultipleRead: Smallint read Get_MultipleRead write Set_MultipleRead;
    property NoiseReduction: Smallint read Get_NoiseReduction write Set_NoiseReduction;
    property PageNo: Smallint read Get_PageNo write Set_PageNo;
    property PrefOccurrence: Smallint read Get_PrefOccurrence write Set_PrefOccurrence;
    property QuietZoneSize: Smallint read Get_QuietZoneSize write Set_QuietZoneSize;
    property Rotation: Smallint read Get_Rotation write Set_Rotation;
    property UpperRatio: Double read Get_UpperRatio write Set_UpperRatio;
    property ReadCode39: Smallint read Get_ReadCode39 write Set_ReadCode39;
    property ReadEAN13: Smallint read Get_ReadEAN13 write Set_ReadEAN13;
    property ReadEAN8: Smallint read Get_ReadEAN8 write Set_ReadEAN8;
    property ReadUPCA: Smallint read Get_ReadUPCA write Set_ReadUPCA;
    property ReadUPCE: Smallint read Get_ReadUPCE write Set_ReadUPCE;
    property ShowCheckDigit: Smallint read Get_ShowCheckDigit write Set_ShowCheckDigit;
    property ReadCode128: Smallint read Get_ReadCode128 write Set_ReadCode128;
    property Code39NeedStartStop: Smallint read Get_Code39NeedStartStop write Set_Code39NeedStartStop;
    property ReadCode25: Smallint read Get_ReadCode25 write Set_ReadCode25;
    property ColorThreshold: Smallint read Get_ColorThreshold write Set_ColorThreshold;
    property BarStringType[nBarCode: Smallint]: WideString read Get_BarStringType;
    property ReadPatchCodes: Smallint read Get_ReadPatchCodes write Set_ReadPatchCodes;
    property UseOverSampling: Smallint read Get_UseOverSampling write Set_UseOverSampling;
    property ScanDirection: Smallint read Get_ScanDirection write Set_ScanDirection;
    property ConvertUPCEToEAN13: Smallint read Get_ConvertUPCEToEAN13 write Set_ConvertUPCEToEAN13;
    property BarStringDirection[nBarCode: Smallint]: Smallint read Get_BarStringDirection;
    property ReadCodabar: Smallint read Get_ReadCodabar write Set_ReadCodabar;
    property MinLength: Smallint read Get_MinLength write Set_MinLength;
    property MaxLength: Smallint read Get_MaxLength write Set_MaxLength;
    property TifSplitMode: Smallint read Get_TifSplitMode write Set_TifSplitMode;
    property TifSplitPath: WideString read Get_TifSplitPath write Set_TifSplitPath;
    property SkewTolerance: Smallint read Get_SkewTolerance write Set_SkewTolerance;
    property ReadCode25ni: Smallint read Get_ReadCode25ni write Set_ReadCode25ni;
    property Photometric: Smallint read Get_Photometric write Set_Photometric;
    property AllowDuplicateValues: Smallint read Get_AllowDuplicateValues write Set_AllowDuplicateValues;
    property BarStringPage[nBarCode: Smallint]: Smallint read Get_BarStringPage;
    property BarStringTopLeftX[nBarCode: Smallint]: Integer read Get_BarStringTopLeftX;
    property BarStringTopLeftY[nBarCode: Smallint]: Integer read Get_BarStringTopLeftY;
    property BarStringBottomRightX[nBarCode: Smallint]: Integer read Get_BarStringBottomRightX;
    property BarStringBottomRightY[nBarCode: Smallint]: Integer read Get_BarStringBottomRightY;
    property Code39Checksum: Smallint read Get_Code39Checksum write Set_Code39Checksum;
    property ExtendedCode39: Smallint read Get_ExtendedCode39 write Set_ExtendedCode39;
    property ErrorCorrection: Smallint read Get_ErrorCorrection write Set_ErrorCorrection;
    property ReadNumeric: Smallint read Get_ReadNumeric write Set_ReadNumeric;
    property Despeckle: Smallint read Get_Despeckle write Set_Despeckle;
    property MinSeparation: Smallint read Get_MinSeparation write Set_MinSeparation;
    property PdfDpi: Smallint read Get_PdfDpi write Set_PdfDpi;
    property PdfBpp: Smallint read Get_PdfBpp write Set_PdfBpp;
    property MinSpaceBarWidth: Smallint read Get_MinSpaceBarWidth write Set_MinSpaceBarWidth;
    property Pattern: WideString read Get_Pattern write Set_Pattern;
    property ReadPDF417: Smallint read Get_ReadPDF417 write Set_ReadPDF417;
    property MedianFilter: Smallint read Get_MedianFilter write Set_MedianFilter;
    property XMLRetval: Smallint read Get_XMLRetval write Set_XMLRetval;
    property TifPageCount[const path: WideString]: Smallint read Get_TifPageCount;
    property Code25Checksum: Smallint read Get_Code25Checksum write Set_Code25Checksum;
    property Encoding: Smallint read Get_Encoding write Set_Encoding;
    property GammaCorrection: Smallint read Get_GammaCorrection write Set_GammaCorrection;
    property SkewLineJump: Smallint read Get_SkewLineJump write Set_SkewLineJump;
    property ReadMicroPDF417: Smallint read Get_ReadMicroPDF417 write Set_ReadMicroPDF417;
    property ReadShortCode128: Smallint read Get_ReadShortCode128 write Set_ReadShortCode128;
    property ShortCode128MinLength: Smallint read Get_ShortCode128MinLength write Set_ShortCode128MinLength;
    property ReadDataMatrix: Smallint read Get_ReadDataMatrix write Set_ReadDataMatrix;
    property ColorProcessingLevel: Smallint read Get_ColorProcessingLevel write Set_ColorProcessingLevel;
    property ReadDatabar: Smallint read Get_ReadDatabar write Set_ReadDatabar;
    property DatabarOptions: Smallint read Get_DatabarOptions write Set_DatabarOptions;
    property LicenseKey: WideString read Get_LicenseKey write Set_LicenseKey;
    property ReadCode93: Smallint read Get_ReadCode93 write Set_ReadCode93;
    property PdfImageOnly: Smallint read Get_PdfImageOnly write Set_PdfImageOnly;
    property PdfImageExtractOptions: Smallint read Get_PdfImageExtractOptions write Set_PdfImageExtractOptions;
    property PdfImageRasterOptions: Smallint read Get_PdfImageRasterOptions write Set_PdfImageRasterOptions;
    property LastError: LongWord read Get_LastError;
    property LastWinError: LongWord read Get_LastWinError;
    property ReadQRCode: Smallint read Get_ReadQRCode write Set_ReadQRCode;
  end;

// *********************************************************************//
// DispIntf:  IBarcodeDisp
// Flags:     (4416) Dual OleAutomation Dispatchable
// GUID:      {92EAAC5E-98EB-4AC6-BB37-932A79F3B7AD}
// *********************************************************************//
  IBarcodeDisp = dispinterface
    ['{92EAAC5E-98EB-4AC6-BB37-932A79F3B7AD}']
    procedure ScanBarCode(const strFilePath: WideString); dispid 1;
    property BarCodeCount: Smallint readonly dispid 2;
    property BarString[nBarCode: Smallint]: WideString readonly dispid 3;
    property BitmapResolution: Smallint dispid 4;
    property DebugTraceFile: WideString dispid 5;
    property LineJump: Integer dispid 6;
    property LowerRatio: Double dispid 7;
    property MinOccurrence: Smallint dispid 8;
    property MultipleRead: Smallint dispid 9;
    property NoiseReduction: Smallint dispid 10;
    property PageNo: Smallint dispid 11;
    property PrefOccurrence: Smallint dispid 12;
    property QuietZoneSize: Smallint dispid 13;
    property Rotation: Smallint dispid 14;
    property UpperRatio: Double dispid 15;
    property ReadCode39: Smallint dispid 16;
    property ReadEAN13: Smallint dispid 17;
    property ReadEAN8: Smallint dispid 18;
    property ReadUPCA: Smallint dispid 19;
    property ReadUPCE: Smallint dispid 20;
    property ShowCheckDigit: Smallint dispid 21;
    property ReadCode128: Smallint dispid 22;
    property Code39NeedStartStop: Smallint dispid 23;
    property ReadCode25: Smallint dispid 24;
    property ColorThreshold: Smallint dispid 25;
    property BarStringType[nBarCode: Smallint]: WideString readonly dispid 26;
    property ReadPatchCodes: Smallint dispid 27;
    property UseOverSampling: Smallint dispid 28;
    property ScanDirection: Smallint dispid 29;
    property ConvertUPCEToEAN13: Smallint dispid 30;
    property BarStringDirection[nBarCode: Smallint]: Smallint readonly dispid 31;
    property ReadCodabar: Smallint dispid 32;
    procedure SetScanRect(TopLeftX: Integer; TopLeftY: Integer; BottomRightX: Integer; 
                          BottomRightY: Integer; MappingMode: Smallint); dispid 33;
    procedure GetBarStringPos(nBarCode: Smallint; var pTopLeftX: Integer; var pTopLeftY: Integer; 
                              var pBottomRightX: Integer; var pBottomRightY: Integer); dispid 34;
    property MinLength: Smallint dispid 35;
    property MaxLength: Smallint dispid 36;
    property TifSplitMode: Smallint dispid 37;
    property TifSplitPath: WideString dispid 38;
    property SkewTolerance: Smallint dispid 39;
    property ReadCode25ni: Smallint dispid 40;
    property Photometric: Smallint dispid 41;
    property AllowDuplicateValues: Smallint dispid 42;
    property BarStringPage[nBarCode: Smallint]: Smallint readonly dispid 43;
    property BarStringTopLeftX[nBarCode: Smallint]: Integer readonly dispid 44;
    property BarStringTopLeftY[nBarCode: Smallint]: Integer readonly dispid 45;
    property BarStringBottomRightX[nBarCode: Smallint]: Integer readonly dispid 46;
    property BarStringBottomRightY[nBarCode: Smallint]: Integer readonly dispid 47;
    property Code39Checksum: Smallint dispid 48;
    property ExtendedCode39: Smallint dispid 49;
    property ErrorCorrection: Smallint dispid 50;
    property ReadNumeric: Smallint dispid 51;
    property Despeckle: Smallint dispid 52;
    property MinSeparation: Smallint dispid 53;
    property PdfDpi: Smallint dispid 54;
    property PdfBpp: Smallint dispid 55;
    property MinSpaceBarWidth: Smallint dispid 56;
    property Pattern: WideString dispid 57;
    property ReadPDF417: Smallint dispid 58;
    property MedianFilter: Smallint dispid 59;
    procedure SaveXMLResults(const strFilePath: WideString); dispid 60;
    property XMLRetval: Smallint dispid 61;
    procedure LoadXMLSettings(const strFilePath: WideString; silent: Smallint); dispid 62;
    procedure ExportXMLSettings(const strFilePath: WideString); dispid 63;
    procedure ProcessXML(const inputFilePath: WideString; const outputFilePath: WideString; 
                         silent: Smallint); dispid 64;
    property TifPageCount[const path: WideString]: Smallint readonly dispid 65;
    property Code25Checksum: Smallint dispid 66;
    property Encoding: Smallint dispid 67;
    property GammaCorrection: Smallint dispid 68;
    property SkewLineJump: Smallint dispid 69;
    property ReadMicroPDF417: Smallint dispid 70;
    property ReadShortCode128: Smallint dispid 71;
    property ShortCode128MinLength: Smallint dispid 72;
    property ReadDataMatrix: Smallint dispid 73;
    property ColorProcessingLevel: Smallint dispid 74;
    property ReadDatabar: Smallint dispid 75;
    property DatabarOptions: Smallint dispid 76;
    property LicenseKey: WideString dispid 77;
    property ReadCode93: Smallint dispid 78;
    property PdfImageOnly: Smallint dispid 79;
    property PdfImageExtractOptions: Smallint dispid 80;
    property PdfImageRasterOptions: Smallint dispid 81;
    property LastError: LongWord readonly dispid 82;
    property LastWinError: LongWord readonly dispid 83;
    property ReadQRCode: Smallint dispid 84;
  end;

// *********************************************************************//
// The Class CoCBarcode provides a Create and CreateRemote method to          
// create instances of the default interface IBarcode exposed by              
// the CoClass CBarcode. The functions are intended to be used by             
// clients wishing to automate the CoClass objects exposed by the         
// server of this typelibrary.                                            
// *********************************************************************//
  CoCBarcode = class
    class function Create: IBarcode;
    class function CreateRemote(const MachineName: string): IBarcode;
  end;

implementation

uses System.Win.ComObj;

class function CoCBarcode.Create: IBarcode;
begin
  Result := CreateComObject(CLASS_CBarcode) as IBarcode;
end;

class function CoCBarcode.CreateRemote(const MachineName: string): IBarcode;
begin
  Result := CreateRemoteComObject(MachineName, CLASS_CBarcode) as IBarcode;
end;

end.
