package Softek ;

import java.util.*;

public class Barcode {
//Native method declaration
	private native int DoScanBarCode(long handle, String name);
	private native int DoLoadXMLSettings(long handle, String name);
	private native String DoGetBarString(long handle, int index);
	private native String DoGetBarStringType(long handle, int index);
	private native int SetLineJump(long handle, int val);
	private native int GetLineJump(long handle) ;
	private native int SetMinLength(long handle, int val);
	private native int GetMinLength(long handle) ;
	private native int SetMultipleRead(long handle, int val);
	private native int GetMultipleRead(long handle) ;
	private native int SetOrientation(long handle, int val);
	private native int GetOrientation(long handle) ;
	private native int SetMinOccurrence(long handle, int val);
	private native int GetMinOccurrence(long handle) ;
	private native int SetPrefOccurrence(long handle, int val);
	private native int GetPrefOccurrence(long handle) ;
	private native int SetDespeckle(long handle, int val);
	private native int GetDespeckle(long handle) ;
	private native int SetNoiseReduction(long handle, int val);
	private native int GetNoiseReduction(long handle) ;
	private native int SetQuietZoneSize(long handle, int val);
	private native int GetQuietZoneSize(long handle) ;
	private native int SetPageNo(long handle, int val);
	private native int GetPageNo(long handle) ;
	private native int SetReadCode39(long handle, int val);
	private native int GetReadCode39(long handle) ;
	private native int SetReadPDF417(long handle, int val);
	private native int GetReadPDF417(long handle) ;
	private native int SetReadDataMatrix(long handle, int val);
	private native int GetReadDataMatrix(long handle) ;
	private native int SetExtendedCode39(long handle, int val);
	private native int GetExtendedCode39(long handle) ;
	private native int SetCode39Checksum(long handle, int val);
	private native int GetCode39Checksum(long handle) ;
	private native int SetReadCodabar(long handle, int val);
	private native int GetReadCodabar(long handle) ;
	private native int SetReadCode128(long handle, int val);
	private native int GetReadCode128(long handle) ;
	private native int SetReadEAN13(long handle, int val);
	private native int GetReadEAN13(long handle) ;
	private native int SetReadEAN8(long handle, int val);
	private native int GetReadEAN8(long handle) ;
	private native int SetReadPatchCodes(long handle, int val);
	private native int GetReadPatchCodes(long handle) ;
	private native int SetReadUPCA(long handle, int val);
	private native int GetReadUPCA(long handle) ;
	private native int SetReadUPCE(long handle, int val);
	private native int GetReadUPCE(long handle) ;
	private native int SetConvertUPCEToEAN13(long handle, int val);
	private native int GetConvertUPCEToEAN13(long handle) ;
	private native int SetReadCode25(long handle, int val);
	private native int GetReadCode25(long handle) ;
	private native int SetReadCode25ni(long handle, int val);
	private native int GetReadCode25ni(long handle) ;
	private native int SetShowCheckDigit(long handle, int val);
	private native int GetShowCheckDigit(long handle) ;
	private native int SetCode39NeedStartStop(long handle, int val);
	private native int GetCode39NeedStartStop(long handle) ;
	private native int SetAllowDuplicateValues(long handle, int val);
	private native int GetAllowDuplicateValues(long handle) ;
	private native int SetUseOverSampling(long handle, int val);
	private native int GetUseOverSampling(long handle) ;
	private native int SetErrorCorrection(long handle, int val);
	private native int GetErrorCorrection(long handle) ;
	private native int SetReadNumeric(long handle, int val);
	private native int GetReadNumeric(long handle) ;
	private native int SetMaxLength(long handle, int val);
	private native int GetMaxLength(long handle) ;
	private native int SetMinSeparation(long handle, int val);
	private native int GetMinSeparation(long handle) ;
	private native int SetMinSpaceBarWidth(long handle, int val);
	private native int GetMinSpaceBarWidth(long handle) ;
	private native int SetMedianFilter(long handle, int val);
	private native int GetMedianFilter(long handle) ;
	private native int SetColorThreshold(long handle, int val);
	private native int GetColorThreshold(long handle) ;
	private native int SetSkewTolerance(long handle, int val);
	private native int GetSkewTolerance(long handle) ;
	private native int SetScanDirection(long handle, int val);
	private native int GetScanDirection(long handle) ;
	private native String SetPattern(long handle, String val) ;
	private native String GetPattern(long handle) ;
	private native String SetTifSplitPath(long handle, String val) ;
	private native String GetTifSplitPath(long handle) ;	
	private native int SetTifSplitMode(long handle, int val) ;
	private native int GetTifSplitMode(long handle) ;		
	private native long CreateSession(int reserved) ;
	private native long FreeSession(long handle) ;
	private native int DoSetScanRect(long handle, int TopLeftX, int TopLeftY, int BottomRightX, int BottomRightY, int MappingMode);
	private native int DoGetBarStringRect(long handle, int n, int part);
	private native int GetCode25Checksum(long handle);
	private native int SetCode25Checksum(long handle, int val);
	private native int GetEncoding(long handle);
	private native int SetEncoding(long handle, int val);
	private native int GetGammaCorrection(long handle);
	private native int SetGammaCorrection(long handle, int val);
	private native int GetSkewLineJump(long handle);
	private native int SetSkewLineJump(long handle, int val);
	private native String SetDebugTraceFile(long handle, String val) ;
	private native String GetDebugTraceFile(long handle) ;	
	private native int GetPdfBpp(long handle);
	private native int SetPdfBpp(long handle, int val);
	private native int GetPdfDpi(long handle);
	private native int SetPdfDpi(long handle, int val);
	private native int SetColorProcessingLevel(long handle, int val);
	private native int GetColorProcessingLevel(long handle);
	private native int SetDatabarOptions(long handle, int val);
	private native int GetDatabarOptions(long handle);
	private native int SetReadDatabar(long handle, int val);
	private native int GetReadDatabar(long handle);
	private native int SetReadMicroPDF417(long handle, int val);
	private native int GetReadMicroPDF417(long handle);
	private native String SetLicenseKey(long handle, String val) ;
	private long handle ;
	public int LineJump ;
	public int MinLength;
	public int MultipleRead;
	public int MinOccurrence;
	public int PrefOccurrence;
	public int Despeckle;
	public int NoiseReduction;
	public int QuietZoneSize;
	public int PageNo;
	public int ReadCode39;
	public int ReadPDF417;
	public int ReadDataMatrix;
	public int ExtendedCode39;
	public int Code39Checksum;
	public int ReadCodabar;
	public int ReadCode128;
	public int ReadEAN13;
	public int ReadEAN8;
	public int ReadPatchCodes;
	public int ReadUPCA;
	public int ReadUPCE;
	public int ConvertUPCEToEAN13;
	public int ReadCode25;
	public int ReadCode25ni;
	public int ShowCheckDigit;
	public int Code39NeedStartStop;
	public int AllowDuplicateValues;
	public int UseOverSampling;
	public int ErrorCorrection;
	public int ReadNumeric;
	public int MaxLength;
	public int MinSeparation;
	public int MinSpaceBarWidth;
	public int MedianFilter;
	public int ColorThreshold;
	public int SkewTolerance;
	public int ScanDirection;
	public String Pattern ;
	public String TifSplitPath ;
	public int TifSplitMode ;
	public int Code25Checksum ;
	public int Encoding ;
	public int GammaCorrection ;
	public int SkewLineJump;
	public String DebugTraceFile ;
	public int PdfBpp ;
	public int PdfDpi ;
	public int ColorProcessingLevel ;
	public int DatabarOptions ;
	public int ReadDatabar ;
	public int ReadMicroPDF417 ;
	public String LicenseKey ;

	public Barcode() {
  		handle = CreateSession(0) ;
		GetProperties() ;}

	protected void finalize() {
		FreeSession(handle) ;
	}

	private void GetProperties() {
		LineJump = GetLineJump(handle) ;
		MinLength = GetMinLength(handle) ;
		MultipleRead = GetMultipleRead(handle) ;
		MinOccurrence = GetMinOccurrence(handle) ;
		PrefOccurrence = GetPrefOccurrence(handle) ;
		Despeckle = GetDespeckle(handle) ;
		NoiseReduction = GetNoiseReduction(handle) ;
		QuietZoneSize = GetQuietZoneSize(handle) ;
		PageNo = GetPageNo(handle) ;
		ReadCode39 = GetReadCode39(handle) ;
		ReadPDF417 = GetReadPDF417(handle) ;
		ReadDataMatrix = GetReadDataMatrix(handle) ;
		ExtendedCode39 = GetExtendedCode39(handle) ;
		Code39Checksum = GetCode39Checksum(handle) ;
		ReadCodabar = GetReadCodabar(handle) ;
		ReadCode128 = GetReadCode128(handle) ;
		ReadEAN13 = GetReadEAN13(handle) ;
		ReadEAN8 = GetReadEAN8(handle) ;
		ReadPatchCodes = GetReadPatchCodes(handle) ;
		ReadUPCA = GetReadUPCA(handle) ;
		ReadUPCE = GetReadUPCE(handle) ;
		ConvertUPCEToEAN13 = GetConvertUPCEToEAN13(handle) ;
		ReadCode25 = GetReadCode25(handle) ;
		ReadCode25ni = GetReadCode25ni(handle) ;
		ShowCheckDigit = GetShowCheckDigit(handle) ;
		Code39NeedStartStop = GetCode39NeedStartStop(handle) ;
		AllowDuplicateValues = GetAllowDuplicateValues(handle) ;
		UseOverSampling = GetUseOverSampling(handle) ;
		ErrorCorrection = GetErrorCorrection(handle) ;
		ReadNumeric = GetReadNumeric(handle) ;
		MaxLength = GetMaxLength(handle) ;
		MinSeparation = GetMinSeparation(handle) ;
		MinSpaceBarWidth = GetMinSpaceBarWidth(handle) ;
		MedianFilter = GetMedianFilter(handle) ;
		ColorThreshold = GetColorThreshold(handle) ;
		SkewTolerance = GetSkewTolerance(handle) ;
		ScanDirection = GetScanDirection(handle) ;
		Pattern = GetPattern(handle) ;
		TifSplitMode = GetTifSplitMode(handle) ;		
		TifSplitPath = null ;
		Encoding = GetEncoding(handle) ;
		GammaCorrection = GetGammaCorrection(handle) ;
		SkewLineJump = GetSkewLineJump(handle) ;
		Code25Checksum = GetCode25Checksum(handle) ;
		DebugTraceFile = null ;
		PdfBpp = GetPdfBpp(handle) ;
		PdfDpi = GetPdfDpi(handle) ;
		ColorProcessingLevel = GetColorProcessingLevel(handle) ;
		DatabarOptions = GetDatabarOptions(handle) ;
		ReadDatabar = GetReadDatabar(handle) ;
		ReadMicroPDF417 = GetReadMicroPDF417(handle) ;
		LicenseKey = null ;
	}

	private void SetProperties() {
		SetLineJump(handle, LineJump) ;
		SetMinLength(handle, MinLength) ;
		SetMultipleRead(handle, MultipleRead) ;
		SetMinOccurrence(handle, MinOccurrence) ;
		SetPrefOccurrence(handle, PrefOccurrence) ;
		SetDespeckle(handle, Despeckle) ;
		SetNoiseReduction(handle, NoiseReduction) ;
		SetQuietZoneSize(handle, QuietZoneSize) ;
		SetPageNo(handle, PageNo) ;
		SetReadCode39(handle, ReadCode39) ;
		SetReadPDF417(handle, ReadPDF417) ;
		SetReadDataMatrix(handle, ReadDataMatrix) ;
		SetExtendedCode39(handle, ExtendedCode39) ;
		SetCode39Checksum(handle, Code39Checksum) ;
		SetReadCodabar(handle, ReadCodabar) ;
		SetReadCode128(handle, ReadCode128) ;
		SetReadEAN13(handle, ReadEAN13) ;
		SetReadEAN8(handle, ReadEAN8) ;
		SetReadPatchCodes(handle, ReadPatchCodes) ;
		SetReadUPCA(handle, ReadUPCA) ;
		SetReadUPCE(handle, ReadUPCE) ;
		SetConvertUPCEToEAN13(handle, ConvertUPCEToEAN13) ;
		SetReadCode25(handle, ReadCode25) ;
		SetReadCode25ni(handle, ReadCode25ni) ;
		SetShowCheckDigit(handle, ShowCheckDigit) ;
		SetCode39NeedStartStop(handle, Code39NeedStartStop) ;
		SetAllowDuplicateValues(handle, AllowDuplicateValues) ;
		SetUseOverSampling(handle, UseOverSampling) ;
		SetErrorCorrection(handle, ErrorCorrection) ;
		SetReadNumeric(handle, ReadNumeric) ;
		SetMaxLength(handle, MaxLength) ;
		SetMinSeparation(handle, MinSeparation) ;
		SetMinSpaceBarWidth(handle, MinSpaceBarWidth) ;
		SetMedianFilter(handle, MedianFilter) ;
		SetColorThreshold(handle, ColorThreshold) ;
		SetSkewTolerance(handle, SkewTolerance) ;
		SetScanDirection(handle, ScanDirection) ;
		SetPattern(handle, Pattern) ;
		if (TifSplitPath != null) {SetTifSplitPath(handle, TifSplitPath) ;}
		SetTifSplitMode(handle, TifSplitMode) ;
		SetEncoding(handle, Encoding);
		SetGammaCorrection(handle, GammaCorrection);
		SetSkewLineJump(handle, SkewLineJump);
		SetCode25Checksum(handle, Code25Checksum);
		if (DebugTraceFile != null) {SetDebugTraceFile(handle, DebugTraceFile);}
		SetPdfBpp(handle, PdfBpp);
		SetPdfDpi(handle, PdfDpi);
		SetColorProcessingLevel(handle, ColorProcessingLevel);
		SetDatabarOptions(handle, DatabarOptions);
		SetReadDatabar(handle, ReadDatabar);
		SetReadMicroPDF417(handle, ReadMicroPDF417);
		if (LicenseKey != null) {SetLicenseKey(handle, LicenseKey);}
	}

	public int ScanBarCode(String name) {
		SetProperties() ;
		return DoScanBarCode(handle, name) ; }

	public String GetBarString(int index) {
		return DoGetBarString(handle, index) ; }

	public String GetBarStringType(int index) {
		return DoGetBarStringType(handle, index) ; }

	public int SetScanRect(int TopLeftX, int TopLeftY, int BottomRightX, int BottomRightY, int MappingMode)
	{
		return DoSetScanRect(handle, TopLeftX, TopLeftY, BottomRightX, BottomRightY, MappingMode);
	}

	public int SetScanRect(java.awt.Rectangle rect, int MappingMode)
	{
		return DoSetScanRect(handle, rect.x, rect.y, rect.x + rect.width, rect.y + rect.height, MappingMode);
	}

	public java.awt.Rectangle GetBarStringRect(int index)
	{
		java.awt.Rectangle rect = new java.awt.Rectangle();
		rect.x = DoGetBarStringRect(handle, index, 1);
		rect.y = DoGetBarStringRect(handle, index, 2);
		rect.width = DoGetBarStringRect(handle, index, 3);
		rect.height = DoGetBarStringRect(handle, index, 4);
		return rect;
	}

	public int GetBarStringPage(int index)
	{
		return DoGetBarStringRect(handle, index, 0);
	}

	public int LoadXMLSettings(String name) {
		return DoLoadXMLSettings(handle, name) ;
	}

//Load the library
	static {
		System.loadLibrary("bardecode_jni");
	}
}
