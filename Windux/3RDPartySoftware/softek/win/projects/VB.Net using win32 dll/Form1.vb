
Public Class Form1
    Inherits System.Windows.Forms.Form

    ' These declarations are only needed if you are using the DLL interface
    ' For 64-bit applications change "SoftekBarcodeDLL" to "SoftekBarcode64DLL.dll" or rename SoftekBarcode64DLL.dll as SoftekBarcodeDLL.dll

    Private Declare Function mtCreateBarcodeInstance Lib "SoftekBarcodeDLL" () As System.IntPtr
    Private Declare Function mtDestroyBarcodeInstance Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Short
    Private Declare Function mtGetAllowDuplicateValues Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtGetBarStringDirection Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal nBarCode As Short) As Short
    Private Declare Function mtGetBarString Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal nBarCode As Short) As System.IntPtr
    Private Declare Function mtGetBarStringPos Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal nBarCode As Short, ByRef TopLeftX As Integer, ByRef TopLeftY As Integer, ByRef BottomRightX As Integer, ByRef BottomRightY As Integer) As Short
    Private Declare Function mtGetBarStringType Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal nBarCode As Short) As System.IntPtr
    Private Declare Function mtGetBitmapResolution Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Long
    Private Declare Function mtGetCode39Checksum Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtGetCode39NeedStartStop Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtGetColorThreshold Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Short
    Private Declare Function mtGetColorProcessingLevel Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Short
    Private Declare Function mtGetConvertUPCEToEAN13 Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtGetDebugTraceFile Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As System.IntPtr
    Private Declare Function mtGetErrorCorrection Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtGetExtendedCode39 Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtGetLicenseKey Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As System.IntPtr
    Private Declare Function mtGetLineJump Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Integer
    Private Declare Function mtGetLowerRatio Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Double
    Private Declare Function mtGetMaxLength Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Short
    Private Declare Function mtGetMedianFilter Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtGetMinLength Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Short
    Private Declare Function mtGetMinOccurrence Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Short
    Private Declare Function mtGetMinSpaceBarWidth Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Short
    Private Declare Function mtGetMultipleRead Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtGetNoiseReduction Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Integer
    Private Declare Function mtGetPageNo Lib "SoftekBarcodeDLL" (ByVal hBarcode As String) As Short
    Private Declare Function mtGetPattern Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As System.IntPtr
    Private Declare Function mtGetPdfBpp Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Integer
    Private Declare Function mtGetPdfImageOnly Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtGetPdfImageExtractOptions Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Short
    Private Declare Function mtGetPdfImageRasterOptions Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Short
    Private Declare Function mtGetPdfDpi Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Short
    Private Declare Function mtGetPhotometric Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtGetPrefOccurrence Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Short
    Private Declare Function mtGetQuietZoneSize Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Short
    Private Declare Function mtGetReadCodabar Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtGetReadCode128 Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtGetReadCode25 Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtGetReadCode25ni Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtGetReadCode39 Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtGetReadEAN13 Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtGetReadEAN8 Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtGetReadNumeric Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtGetReadPatchCodes Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtGetReadPDF417 Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtGetReadDataMatrix Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtGetReadQRCode Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtGetReadDatabar Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtGetDatabarOptions Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Integer
    Private Declare Function mtGetReadUPCA Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtGetReadUPCE Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtGetScanDirection Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Short
    Private Declare Function mtGetShowCheckDigit Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtGetSkewTolerance Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Short
    Private Declare Function mtGetTifSplitMode Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Short
    Private Declare Function mtGetTifSplitPath Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As System.IntPtr
    Private Declare Function mtGetUpperRatio Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Double
    Private Declare Function mtGetUseOverSampling Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtScanBarCodeFromBitmap Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal hBitmap As IntPtr) As Short
    Private Declare Function mtScanBarCode Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal strFile As String) As Short
    Private Declare Function mtScanBarCodeInBackground Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal strFile As String) As Short
    Private Declare Function mtScanBarCodeWait Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal milliSeconds As Integer) As Integer
    Private Declare Function mtScanBarCodeAbort Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Integer
    Private Declare Function mtGetBarCodeCount Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Integer
    Private Declare Function mtGetScanExitCode Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Integer
    Private Declare Function mtGetProgress Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Integer
    Private Declare Function mtSetAllowDuplicateValues Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtSetBitmapResolution Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Long) As Long
    Private Declare Function mtSetCode39Checksum Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtSetCode39NeedStartStop Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtSetColorThreshold Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Short) As Short
    Private Declare Function mtSetColorProcessingLevel Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Short) As Short
    Private Declare Function mtSetConvertUPCEToEAN13 Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtSetDebugTraceFile Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal strFile As String) As System.IntPtr
    Private Declare Function mtSetErrorCorrection Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtSetExtendedCode39 Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtSetLicenseKey Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal strFile As String) As System.IntPtr
    Private Declare Function mtSetLineJump Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Integer) As Integer
    Private Declare Function mtSetLowerRatio Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Double) As Double
    Private Declare Function mtSetMaxLength Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Short) As Short
    Private Declare Function mtSetMedianFilter Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtSetMinLength Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Short) As Short
    Private Declare Function mtSetMinOccurrence Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Short) As Short
    Private Declare Function mtSetMinSpaceBarWidth Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Short) As Short
    Private Declare Function mtSetMultipleRead Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtSetNoiseReduction Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Integer) As Integer
    Private Declare Function mtSetPageNo Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Short) As Short
    Private Declare Function mtSetPattern Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal strFile As String) As System.IntPtr
    Private Declare Function mtSetPdfBpp Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Short) As Short
    Private Declare Function mtSetPdfDpi Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Short) As Short
    Private Declare Function mtSetPdfImageOnly Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtSetPdfImageExtractOptions Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Short) As Short
    Private Declare Function mtSetPdfImageRasterOptions Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Short) As Short
    Private Declare Function mtSetPhotometric Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtSetPrefOccurrence Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Short) As Short
    Private Declare Function mtSetQuietZoneSize Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Short) As Short
    Private Declare Function mtSetReadCodabar Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtSetReadCode128 Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtSetReadCode25 Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtSetReadCode25ni Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtSetReadCode39 Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtSetReadEAN13 Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtSetReadEAN8 Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtSetReadNumeric Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtSetReadPatchCodes Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtSetReadPDF417 Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtSetReadDataMatrix Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtSetReadQRCode Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtSetDatabarOptions Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Integer) As Integer
    Private Declare Function mtSetReadDatabar Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtSetReadUPCA Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtSetReadUPCE Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtSetScanDirection Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Short) As Short
    Private Declare Function mtSetScanRect Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal TopLeftX As Integer, ByVal TopLeftY As Integer, ByVal BottomRightX As Integer, ByVal BottomRightY As Integer, ByVal MappingMode As Short) As Boolean
    Private Declare Function mtSetShowCheckDigit Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtSetSkewTolerance Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Short) As Short
    Private Declare Function mtSetTifSplitMode Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Short) As Short
    Private Declare Function mtSetTifSplitPath Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As String) As System.IntPtr
    Private Declare Function mtSetUpperRatio Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Double) As Double
    Private Declare Function mtSetUseOverSampling Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtLoadXMLSettings Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal strFile As String, ByVal silent As Boolean) As Short
    Private Declare Function mtSaveResults Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal strOutputFile As String) As Short
    Private Declare Function mtProcessXML Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal strInputFile As String, ByVal strOutputFile As String) As Short
    Private Declare Function mtExportXMLSettings Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal strFile As String) As Short
    Private Declare Function mtSetGammaCorrection Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Short) As Short
    Private Declare Function mtSetDespeckle Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Short) As Short
    Private Declare Function mtSetSkewLineJump Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Short) As Short
    Private Declare Function mtSetCode25Checksum Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtSetReadMicroPDF417 Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtSetReadShortCode128 Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtSetShortCode128MinLength Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Short) As Short
    Private Declare Function mtSetEncoding Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Short) As Short
    Private Declare Function mtGetGammaCorrection Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Short
    Private Declare Function mtGetDespeckle Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Short
    Private Declare Function mtGetSkewLineJump Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Short
    Private Declare Function mtGetCode25Checksum Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtGetReadMicroPDF417 Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtGetReadShortCode128 Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtGetShortCode128MinLength Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Short
    Private Declare Function mtGetEncoding Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Short
    Private Declare Function mtGetLastError Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Short
    Private Declare Function mtGetLastWinError Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Short
    Private Declare Function mtGetUseFastScan Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtSetUseFastScan Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtGetBarcodesAtTopOfPage Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtSetBarcodesAtTopOfPage Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtGetFastScanLineJump Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Short
    Private Declare Function mtSetFastScanLineJump Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Short) As Short
    Private Declare Function mtGetMaxBarcodesPerPage Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Short
    Private Declare Function mtSetMaxBarcodesPerPage Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Short) As Short
    Private Declare Function mtGetSkewedDatamatrix Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtSetSkewedDatamatrix Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtGetTimeOut Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Integer
    Private Declare Function mtSetTimeOut Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Integer) As Integer
    Private Declare Function mtGetSkewedLinear Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Boolean
    Private Declare Function mtSetSkewedLinear Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Boolean) As Boolean
    Private Declare Function mtGetReportUnreadBarcodes Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Short
    Private Declare Function mtSetReportUnreadBarcodes Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Short) As Short
    Private Declare Function mtGetShowCodabarStartStop Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr) As Short
    Private Declare Function mtSetShowCodabarStartStop Lib "SoftekBarcodeDLL" (ByVal hBarcode As System.IntPtr, ByVal newValue As Short) As Short
    ' Required to call DeleteObject for the DLL interface
    Declare Function DeleteObject Lib "gdi32.dll" (ByVal hObject As IntPtr) As Boolean
    Declare Function LoadLibrary Lib "kernel32.dll" Alias "LoadLibraryA" (ByVal lpLibFileName As String) As IntPtr
    Dim hBarcode As IntPtr
    Delegate Sub SetTextCallback(ByVal [text] As String)
    Delegate Sub SetButtonCallback(ByVal [enable] As Boolean)
    Delegate Sub SetProgressCallback(ByVal [value] As Integer)
    Dim aTimer As System.Timers.Timer
    Dim nBarCodes As Integer


#Region " Windows Form Designer generated code "

    Public Sub New()
        MyBase.New()

        'This call is required by the Windows Form Designer.
        InitializeComponent()

        aTimer = New System.Timers.Timer(1)
        AddHandler aTimer.Elapsed, New System.Timers.ElapsedEventHandler(AddressOf checkRead)

        'Add any initialization after the InitializeComponent() call
        initBarcode()
    End Sub

    'Form overrides dispose to clean up the component list.
    Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing Then
            If Not (components Is Nothing) Then
                components.Dispose()
                mtDestroyBarcodeInstance(hBarcode)
            End If
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    Friend WithEvents Button1 As System.Windows.Forms.Button
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents CheckBitmap As System.Windows.Forms.CheckBox
    Friend WithEvents ImageFile As System.Windows.Forms.TextBox
    Friend WithEvents ImageBtn As System.Windows.Forms.Button
    Friend WithEvents OpenFileDialog1 As System.Windows.Forms.OpenFileDialog
    Friend WithEvents ReadBarcode As System.Windows.Forms.Button
    Friend WithEvents ProgressBar1 As System.Windows.Forms.ProgressBar
    Friend WithEvents CheckBoxbackground As System.Windows.Forms.CheckBox
    Friend WithEvents Results As System.Windows.Forms.TextBox
    <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
        Me.ImageFile = New System.Windows.Forms.TextBox
        Me.ImageBtn = New System.Windows.Forms.Button
        Me.OpenFileDialog1 = New System.Windows.Forms.OpenFileDialog
        Me.ReadBarcode = New System.Windows.Forms.Button
        Me.Results = New System.Windows.Forms.TextBox
        Me.Button1 = New System.Windows.Forms.Button
        Me.Label1 = New System.Windows.Forms.Label
        Me.CheckBitmap = New System.Windows.Forms.CheckBox
        Me.ProgressBar1 = New System.Windows.Forms.ProgressBar
        Me.CheckBoxbackground = New System.Windows.Forms.CheckBox
        Me.SuspendLayout()
        '
        'ImageFile
        '
        Me.ImageFile.Location = New System.Drawing.Point(114, 9)
        Me.ImageFile.Name = "ImageFile"
        Me.ImageFile.ReadOnly = True
        Me.ImageFile.Size = New System.Drawing.Size(344, 20)
        Me.ImageFile.TabIndex = 0
        '
        'ImageBtn
        '
        Me.ImageBtn.Location = New System.Drawing.Point(479, 7)
        Me.ImageBtn.Name = "ImageBtn"
        Me.ImageBtn.Size = New System.Drawing.Size(102, 23)
        Me.ImageBtn.TabIndex = 1
        Me.ImageBtn.Text = "Browse"
        '
        'OpenFileDialog1
        '
        Me.OpenFileDialog1.DefaultExt = "tif"
        Me.OpenFileDialog1.Filter = "Images (*.tif,*.pdf,*.bmp,*.jpg)|*.tif;*.jpg;*.tiff;*.pdf;*.bmp;*.jpeg"
        '
        'ReadBarcode
        '
        Me.ReadBarcode.Location = New System.Drawing.Point(479, 303)
        Me.ReadBarcode.Name = "ReadBarcode"
        Me.ReadBarcode.Size = New System.Drawing.Size(102, 24)
        Me.ReadBarcode.TabIndex = 2
        Me.ReadBarcode.Text = "Read"
        '
        'Results
        '
        Me.Results.Location = New System.Drawing.Point(12, 65)
        Me.Results.Multiline = True
        Me.Results.Name = "Results"
        Me.Results.ReadOnly = True
        Me.Results.ScrollBars = System.Windows.Forms.ScrollBars.Vertical
        Me.Results.Size = New System.Drawing.Size(569, 187)
        Me.Results.TabIndex = 3
        '
        'Button1
        '
        Me.Button1.Location = New System.Drawing.Point(362, 303)
        Me.Button1.Name = "Button1"
        Me.Button1.Size = New System.Drawing.Size(96, 23)
        Me.Button1.TabIndex = 4
        Me.Button1.Text = "Close"
        Me.Button1.UseVisualStyleBackColor = True
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(12, 9)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(23, 13)
        Me.Label1.TabIndex = 5
        Me.Label1.Text = "File"
        '
        'CheckBitmap
        '
        Me.CheckBitmap.AutoSize = True
        Me.CheckBitmap.Location = New System.Drawing.Point(12, 283)
        Me.CheckBitmap.Name = "CheckBitmap"
        Me.CheckBitmap.Size = New System.Drawing.Size(389, 17)
        Me.CheckBitmap.TabIndex = 9
        Me.CheckBitmap.Text = "Load page 1 of image into a Bitmap object and call ScanBarCodeFromBitmap"
        Me.CheckBitmap.UseVisualStyleBackColor = True
        '
        'ProgressBar1
        '
        Me.ProgressBar1.Location = New System.Drawing.Point(12, 36)
        Me.ProgressBar1.Name = "ProgressBar1"
        Me.ProgressBar1.Size = New System.Drawing.Size(566, 23)
        Me.ProgressBar1.TabIndex = 11
        '
        'CheckBoxbackground
        '
        Me.CheckBoxbackground.AutoSize = True
        Me.CheckBoxbackground.Checked = True
        Me.CheckBoxbackground.CheckState = System.Windows.Forms.CheckState.Checked
        Me.CheckBoxbackground.Location = New System.Drawing.Point(12, 308)
        Me.CheckBoxbackground.Name = "CheckBoxbackground"
        Me.CheckBoxbackground.Size = New System.Drawing.Size(159, 17)
        Me.CheckBoxbackground.TabIndex = 12
        Me.CheckBoxbackground.Text = "Launch scan in background"
        Me.CheckBoxbackground.UseVisualStyleBackColor = True
        '
        'Form1
        '
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.ClientSize = New System.Drawing.Size(593, 338)
        Me.Controls.Add(Me.CheckBoxbackground)
        Me.Controls.Add(Me.ProgressBar1)
        Me.Controls.Add(Me.CheckBitmap)
        Me.Controls.Add(Me.Label1)
        Me.Controls.Add(Me.Button1)
        Me.Controls.Add(Me.Results)
        Me.Controls.Add(Me.ImageFile)
        Me.Controls.Add(Me.ReadBarcode)
        Me.Controls.Add(Me.ImageBtn)
        Me.Name = "Form1"
        Me.Text = "Barcode Reader"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub

#End Region

    Private Sub ImageBtn_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ImageBtn.Click
        OpenFileDialog1.ShowDialog()
        ImageFile.Text = OpenFileDialog1.FileName
    End Sub
    Private Sub checkRead(ByVal source As Object, ByVal e As System.Timers.ElapsedEventArgs)
        aTimer.Stop()
        ' Wait for up to 100 ms
        If (mtScanBarCodeWait(hBarcode, 100) = 1) Then
            SetProgress(mtGetProgress(hBarcode))
            aTimer.Start()
        Else
            SetProgress(mtGetProgress(hBarcode))
            nBarCodes = mtGetScanExitCode(hBarcode)
            getResults()
            SetButton(True)
        End If
    End Sub
    Private Sub initBarcode()

        LoadSoftekBarcodeDLL()

        hBarcode = mtCreateBarcodeInstance()

        ' Enter your license key here
        ' You can get a trial license key from sales@bardecode.com
        ' Example:
        ' mtSetLicenseKey(hBarcode, "MY LICENSE KEY")

        ' Turn on the barcode types you want to read.
        ' Turn off the barcode types you don't want to read (this will increase the speed of your application)
        mtSetReadCode128(hBarcode, True)
        mtSetReadCode39(hBarcode, True)
        mtSetReadCode25(hBarcode, True)
        mtSetReadEAN13(hBarcode, True)
        mtSetReadEAN8(hBarcode, True)
        mtSetReadUPCA(hBarcode, True)
        mtSetReadUPCE(hBarcode, True)
        mtSetReadCodabar(hBarcode, False)
        mtSetReadPDF417(hBarcode, True)
        mtSetReadDataMatrix(hBarcode, True)
        mtSetReadDatabar(hBarcode, True)
        mtSetReadMicroPDF417(hBarcode, False)
        mtSetReadQRCode(hBarcode, True)

        ' Databar Options is a mask that controls which type of databar barcodes will be read and whether or not
        ' the software will look for a quiet zone around the barcode.
        ' 1 = 2D-Linkage flag (handle micro-PDf417 barcodes as supplementary data - requires ReadMicroPDF417 to be true).
        ' 2 = Read RSS14
        ' 4 = Read RSS14 Stacked
        ' 8 = Read Limited
        ' 16 = Read Expanded
        ' 32 = Read Expanded Stacked
        ' 64 = Require quiet zone
        mtSetDatabarOptions(hBarcode, 255)

        ' If you want to read more than one barcode then set Multiple Read to 1
        ' Setting MutlipleRead to False will make the recognition faster
        mtSetMultipleRead(hBarcode, True)

        ' If you know the max number of barcodes for a single page then increase speed by setting MaxBarcodesPerPage
        mtSetMaxBarcodesPerPage(hBarcode, 0)

        ' In certain conditions (MultipleRead = false or MaxBarcodesPerPage = 1) the SDK can make fast scan of an image before performing the normal scan. This is useful if only 1 bar code is expected per page.
        mtSetUseFastScan(hBarcode, True)

        ' Noise reduction takes longer but can make it possible to read some difficult barcodes
        ' When using noise reduction a typical value is 10 - the smaller the value the more effect it has.
        ' A zero value turns off noise reduction.
        ' If you use NoiseReduction then the ScanDirection mask must be either only horizontal or only
        ' vertical (i.e 1, 2, 4, 8, 5 or 10).
        ' mtSetNoiseReduction(hBarcode, 0)

        ' You may need to set a small quiet zone if your barcodes are close to text and pictures in the image.
        ' A value of zero uses the default.
        mtSetQuietZoneSize(hBarcode, 0)

        ' LineJump controls the frequency at which scan lines in an image are sampled.
        ' The default is 1.
        mtSetLineJump(hBarcode, 1)

        ' You can restrict your search to a particular area of the image if you wish.
        ' retval = stSetScanRect(TopLeftX, TopLeftY, BottomRightX, BottomRightY, 0)

        ' Set the direction that the barcode reader should scan for barcodes
        ' The value is a mask where 1 = Left to Right, 2 = Top to Bottom, 4 = Right To Left, 8 = Bottom to Top
        mtSetScanDirection(hBarcode, 15)

        ' Set the page number to read from in a multi-page TIF file. The default is 0, which will make the
        ' toolkit check every page.
        ' mtSetPageNo(hBarcode, 1)

        ' SkewTolerance controls the angle of skew that the barcode toolkit will tolerate. By default
        ' the toolkit checks for barcodes along horizontal and vertical lines in an image. This works
        ' OK for most barcodes because even at an angle it is possible to pass a line through the entire
        ' length. SkewTolerance can range from 0 to 5 and allows for barcodes skewed to an angle of 45
        ' degrees.
        mtSetSkewTolerance(hBarcode, 0)

        ' If you know the max number of barcodes for a single page then increase speed by setting MaxBarcodesPerPage
        mtSetMaxBarcodesPerPage(hBarcode, 0)

        ' In certain conditions (MultipleRead = false or MaxBarcodesPerPage = 1) the SDK can make fast scan of an image before performing the normal scan. This is useful if only 1 bar code is expected per page.
        mtSetUseFastScan(hBarcode, True)

        ' ColorProcessingLevel controls how much time the toolkit will searching a color image for a barcode.
        ' The default value is 2 and the range of values is 0 to 5. If ColorThreshold is non-zero then 
        ' ColorProcessingLevel is effectively set to 0.
        mtSetColorProcessingLevel(hBarcode, 2)

        ' MaxLength and MinLength can be used to specify the number of characters you expect to
        ' find in a barcode. This can be useful to increase accuracy or if you wish to ignore some
        ' barcodes in an image.
        mtSetMinLength(hBarcode, 4)
        mtSetMaxLength(hBarcode, 999)

        ' When the toolkit scans an image it records the number of hits it gets for each barcode that
        ' MIGHT be in the image. If the hits recorded for any of the barcodes are >= PrefOccurrence
        ' then only these barcodes are returned. Otherwise, any barcode whose hits are >= MinOccurrence
        ' are reported. If you have a very poor quality image then try setting MinOccurrence to 1, but you
        ' may find that some false positive results are returned.
        'mtSetMinOccurrence (hBarcode, 2)
        'mtSetPrefOccurrence (hBarcode, 4)

        ' Read Code 39 barcodes in extended mode
        ' mtSetExtendedCode39(hBarcode, True)

        ' Barcode string is numeric
        ' mtSetReadNumeric(hBarcode, True)

        ' Set a regular expression for the barcode
        ' mtSetPattern(hBarcode, "^[A-Z]{2}[0-9]{5}$")

        ' If you are scanning at a high resolution and the spaces between bars are
        ' larger than 1 pixel then set MinSpaceBarWidth to 2 and increase your read rate.
        ' mtSetMinSpaceBarWidth(hBarcode, 2)

        ' MedianFilter is a useful way to clean up higher resolution images where the black bars contain white dots
        ' and the spaces contain black dots. It does not work if the space between bars is only 1 pixel wide.
        mtSetMedianFilter(hBarcode, False)

        ' ReportUnreadBarcodes can be used to warn of the presence of a barcode on a page that the SDK has not been able to decode.
        ' It currently has the following important limitations:
        ' 1. An unread linear barcode will only be reported if no other barcode was found in the same page.
        ' 2. The height of the area for an unread linear barcode will only cover a portion of the barcode.
        ' 3. Only 2-D barcodes that fail to error correct will be reported.
        ' 4. The barcode type and value will both be set to UNREAD for all unread barcodes.
        ' 5. The reporting of unread linear barcodes takes no account of settings for individual barcode types. For example, if ReadCode39 is True and 
        ' an image contains a single Code 39 barcode then this will be reported as an unread barcode.
        ' 6. 2-D barcodes are only reported as unread if the correct barcode types have been enabled.
        ' 7. Not all unread barcodes will be detected. 
        '
        ' The value is a mask with the following values: 1 = linear barcodes, 2 = Datamatrix, 4 = QR-Code, 8 = PDF-417
        mtSetReportUnreadBarcodes(hBarcode, 0)

        ' Time out for reading a barcode from a page in ms. Note that this does not include the time to load the page.
        ' 0 means no time out.
        mtSetTimeOut(hBarcode, 5000)

        ' Flags for handling PDF files
        ' PdfImageOnly defaults to true and indicates that the PDF documents are simple images.
        mtSetPdfImageOnly(hBarcode, True)

        ' PdfImageExtractOptions is no longer required in version 8 of the SDK but is retained for future possible use.
        ' mtSetPdfImageExtractOptions(hBarcode, 0)

        ' The PdfImageRasterOptions mask controls how images are rasterized when PdfImageOnly is false or when image extraction fails
        ' 0 = Use Debenu to render image
        ' 4 = Use VeryPDF to render image (x86 only)
        mtSetPdfImageRasterOptions(hBarcode, 0)

        ' PdfDpi and PdfBpp control what sort of image the PDF document is rasterized into
        mtSetPdfDpi(hBarcode, 300)
        mtSetPdfBpp(hBarcode, 8)

        ' Or you can load the settings from a xml format file
        ' See the manual for more details.
        ' mtLoadXMLSettings(hBarcode, "settings.xml")
    End Sub
    Private Sub LoadSoftekBarcodeDLL()
        ' For the purposes of this demo we search back up the tree until the SoftekBarcode.dll file is found.
        ' If that fails (perhaps this project has been moved) then we assume the dll file is somewhere on the path.

        Dim Path As String
        Dim dllName As String
        Path = System.Environment.CurrentDirectory
        If IntPtr.Size = 8 Then
            dllName = "SoftekBarcode64DLL.dll"
        Else
            dllName = "SoftekBarcodeDLL.dll"
        End If

        Do Until (Not System.IO.Directory.Exists(Path) Or System.IO.File.Exists(Path + "\bin\" + dllName))
            Path += "\.."
        Loop
        Path += "\bin\" + dllName
        If (System.IO.File.Exists(Path)) Then
            LoadLibrary(Path)
        Else
            If (LoadLibrary(dllName) = IntPtr.Zero) Then
                MsgBox("Failed to load " + dllName + " file")
            End If
        End If

    End Sub
    Private Sub getResults()
        Dim i As Integer
        Dim nDirection As Integer
        Dim TopLeftX As Long
        Dim TopLeftY As Long
        Dim BottomRightX As Long
        Dim BottomRightY As Long
        Dim nPage As Integer

        ' You can also save the results to an XML file
        ' stSaveXMLResults("results.xml")

        If nBarCodes <= -6 Then
            SetText("License key error: either an evaluation key has expired or the license key is not valid for processing pdf documents")
        ElseIf nBarCodes < 0 Then
            SetText("ScanBarCode returned error number " & nBarCodes)
            SetText("Last Softek Error Number = " & mtGetLastError(hBarcode))
            SetText("Last Windows Error Number = " & mtGetLastWinError(hBarcode))
        End If

        If nBarCodes = 0 Then
            SetText("Sorry - no barcodes were found in this image")
        End If

        For i = 1 To nBarCodes
            SetText("")
            SetText("Barcode " & i & ":")

            SetText("Value = " & ConvertUTF8IntPtrtoString(mtGetBarString(hBarcode, i)))

            SetText("Type = " & System.Runtime.InteropServices.Marshal.PtrToStringAnsi(mtGetBarStringType(hBarcode, i)))

            nDirection = mtGetBarStringDirection(hBarcode, i)
            If nDirection = 1 Then
                SetText("Direction = Left to Right")
            Else
                If nDirection = 2 Then
                    SetText("Direction = Top to Bottom")
                Else
                    If nDirection = 4 Then
                        SetText("Direction = Right to Left")
                    Else
                        If nDirection = 8 Then
                            SetText("Direction = Bottom to Top")
                        End If
                    End If
                End If
            End If

            nPage = mtGetBarStringPos(hBarcode, i, TopLeftX, TopLeftY, BottomRightX, BottomRightY)
            SetText("Page = " & nPage)
            SetText("Top Left = (" & TopLeftX & "," & TopLeftY & ")")
            SetText("Bottom Right = (" & BottomRightX & "," & BottomRightY & ")")

        Next i

    End Sub

    

    Private Sub ReadDLL()
        Dim Path As String

        Results.Text = ""

        SetProgress(0)

        Path = ImageFile.Text

        SetText("Using DLL Interface to read barcodes")

        If CheckBitmap.Checked Then
            If (Path.EndsWith(".pdf", System.StringComparison.CurrentCultureIgnoreCase)) Then
                MessageBox.Show("Cannot load PDF files into bitmaps")
                Return
            End If

            Dim bm As System.Drawing.Bitmap
            SetText("Loading image into a bitmap and calling ScanBarCodeFromBitmap")
            bm = New Bitmap(Path)
            Dim hBitmap As IntPtr
            hBitmap = bm.GetHbitmap()
            nBarCodes = mtScanBarCodeFromBitmap(hBarcode, hBitmap)
            DeleteObject(hBitmap)
            bm.Dispose()
            getResults()
        ElseIf CheckBoxbackground.Checked Then
            mtScanBarCodeInBackground(hBarcode, Path)
            aTimer.Start()
            ReadBarcode.Enabled = False
        Else
            nBarCodes = mtScanBarCode(hBarcode, Path)
            getResults()
        End If
    End Sub

    Public Function ConvertUTF8IntPtrtoString(ByVal ptr As System.IntPtr) As String
        Dim l As Integer
        l = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(ptr).Length
        Dim utf8data(l) As Byte
        System.Runtime.InteropServices.Marshal.Copy(ptr, utf8data, 0, l)
        Return System.Text.Encoding.UTF8.GetString(utf8data)
    End Function
    Private Sub ReadBarcode_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ReadBarcode.Click
        ReadDLL()
    End Sub
    
    Private Sub SetButton(ByVal [enabled] As Boolean)
        If Me.Results.InvokeRequired Then
            Dim d As New SetButtonCallback(AddressOf SetButton)
            Me.Invoke(d, New Object() {[enabled]})
        Else
            Me.ReadBarcode.Enabled = [enabled]
        End If

    End Sub

    Private Sub SetProgress(ByVal [value] As Integer)
        If Me.Results.InvokeRequired Then
            Dim d As New SetProgressCallback(AddressOf SetProgress)
            Me.Invoke(d, New Object() {[value]})
        Else
            Me.ProgressBar1.Value = [value]
        End If

    End Sub

    Private Sub SetText(ByVal [text] As String)

        ' InvokeRequired required compares the thread ID of the
        ' calling thread to the thread ID of the creating thread.
        ' If these threads are different, it returns true.
        If Me.Results.InvokeRequired Then
            Dim d As New SetTextCallback(AddressOf SetText)
            Me.Invoke(d, New Object() {[text]})
        Else
            Me.Results.Text += [text] + vbNewLine
        End If
    End Sub

    Private Sub Label2_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)

    End Sub

    Private Sub FilePath_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ImageFile.TextChanged

    End Sub

    Private Sub Label3_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)

    End Sub

    Private Sub Result_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Results.TextChanged

    End Sub

    Private Sub Label1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)

    End Sub

    Private Sub Panel2_Paint(ByVal sender As System.Object, ByVal e As System.Windows.Forms.PaintEventArgs)

    End Sub

    Private Sub ResultFromPicture_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)

    End Sub

    Private Sub Panel1_Paint(ByVal sender As System.Object, ByVal e As System.Windows.Forms.PaintEventArgs)

    End Sub

    Private Sub OpenFileDialog1_FileOk(ByVal sender As System.Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles OpenFileDialog1.FileOk

    End Sub

    Private Sub PictureBox1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)

    End Sub

    Private Sub Button1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button1.Click
        End
    End Sub
End Class


