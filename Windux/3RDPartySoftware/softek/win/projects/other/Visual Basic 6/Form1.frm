VERSION 5.00
Object = "{4B12FD42-20B7-4EAF-80DA-A1FB7B33570F}#1.0#0"; "SOFTEK~1.OCX"
Begin VB.Form Form1 
   Caption         =   "Form1"
   ClientHeight    =   6900
   ClientLeft      =   60
   ClientTop       =   360
   ClientWidth     =   8460
   LinkTopic       =   "Form1"
   ScaleHeight     =   6900
   ScaleWidth      =   8460
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton Read 
      Caption         =   "Read"
      Height          =   495
      Left            =   6840
      TabIndex        =   10
      Top             =   6240
      Width           =   1455
   End
   Begin SOFTEKBARCODELib.SoftekBarcode SoftekBarcode1 
      Left            =   3600
      Top             =   6120
      _Version        =   65536
      _ExtentX        =   2355
      _ExtentY        =   1296
      _StockProps     =   0
      LineJump        =   9
      LowerRatio      =   0
      UpperRatio      =   0
      MinOccurrence   =   2
      PrefOccurrence  =   5
      MultipleRead    =   0   'False
      Rotation        =   0
      NoiseReduction  =   0
      BitmapResolution=   200
      DebugTraceFile  =   ""
      QuietZoneSize   =   0
      PageNo          =   0
      ReadCode39      =   -1  'True
      ReadPDF417      =   0   'False
      ReadEAN13       =   -1  'True
      ReadEAN8        =   -1  'True
      ReadUPCA        =   0   'False
      ReadUPCE        =   -1  'True
      ReadCode128     =   -1  'True
      Code39StartStop =   -1  'True
      ShowCheckDigit  =   0   'False
      ReadCode25      =   -1  'True
      ReadCode25ni    =   0   'False
      ReadPatchCodes  =   0   'False
      UseOverSampling =   0   'False
      MinLength       =   4
      MaxLength       =   999
      ScanDirection   =   15
      ReadCodabar     =   -1  'True
      ConvertUPCEToEAN13=   -1  'True
      SkewTolerance   =   0
      AllowDuplicateValues=   -1  'True
      Code39NeedStartStop=   -1  'True
      ColorThreshold  =   128
      TifSplitPath    =   ""
      TifSplitMode    =   0
      Photometric     =   0
      Pattern         =   ""
      PdfBpp          =   1
      PdfDpi          =   300
      PDFTool         =   "verypdf"
      MinSeparation   =   180
      Code39Checksum  =   0   'False
      ExtendedCode39  =   0   'False
      ErrorCorrection =   0   'False
      ReadNumeric     =   0   'False
      MinSpaceWidth   =   0
      MedianFilter    =   0
      GammaCorrection =   100
      Despeckle       =   0
      SkewLineJump    =   9
      Code25Checksum  =   0   'False
      ReadMicroPDF417 =   0   'False
      ReadShortCode128=   0   'False
      ShortCode128MinLength=   2
      Encoding        =   0
      Code39Checksum  =   0   'False
      ReadDataMatrix  =   0   'False
      ReadDatabar     =   0   'False
      DatabarOptions  =   255
      ColorProcessingLevel=   2
      LicenseKey      =   ""
   End
   Begin VB.OptionButton Option_com 
      Caption         =   "COM"
      Height          =   735
      Left            =   2400
      TabIndex        =   8
      Top             =   6120
      Width           =   1095
   End
   Begin VB.OptionButton Option_ocx 
      Caption         =   "OCX"
      Height          =   735
      Left            =   1320
      TabIndex        =   7
      Top             =   6120
      Width           =   975
   End
   Begin VB.OptionButton Option_dll 
      Caption         =   "DLL"
      Height          =   495
      Left            =   240
      TabIndex        =   6
      Top             =   6240
      Value           =   -1  'True
      Width           =   975
   End
   Begin VB.CommandButton Command2 
      Cancel          =   -1  'True
      Caption         =   "Close"
      Height          =   495
      Left            =   5280
      TabIndex        =   4
      Top             =   6240
      Width           =   1455
   End
   Begin VB.TextBox Results 
      Height          =   2655
      Left            =   120
      Locked          =   -1  'True
      MultiLine       =   -1  'True
      ScrollBars      =   2  'Vertical
      TabIndex        =   3
      Top             =   3360
      Width           =   8295
   End
   Begin VB.FileListBox ImageFile 
      Height          =   2235
      Left            =   5400
      Pattern         =   "*.tif;*.jpg;*.gif;*.png;*.bmp"
      TabIndex        =   2
      Top             =   720
      Width           =   3015
   End
   Begin VB.DirListBox Dir1 
      Height          =   1665
      Left            =   120
      TabIndex        =   1
      Top             =   1200
      Width           =   5175
   End
   Begin VB.DriveListBox Drive1 
      Height          =   315
      Left            =   120
      TabIndex        =   0
      Top             =   720
      Width           =   5175
   End
   Begin VB.Label Label2 
      Caption         =   "When you select an image file below or click on Read the toolkit will scan the file for barcodes using the specified interface"
      Height          =   495
      Left            =   120
      TabIndex        =   9
      Top             =   120
      Width           =   8295
   End
   Begin VB.Label Label1 
      Caption         =   "Results"
      Height          =   255
      Left            =   120
      TabIndex        =   5
      Top             =   3000
      Width           =   1935
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False




Private Sub ReadOCX()
    Dim nBarCodes As Integer
    Dim Path As String
    Dim i As Integer
    Dim nDirection As Integer
    Dim TopLeftX As Long
    Dim TopLeftY As Long
    Dim BottomRightX As Long
    Dim BottomRightY As Long
    Dim nPage As Integer
    
    ' Enter your license key here
    ' You can get a trial license key from sales@bardecode.com
    ' Example:
    ' SoftekBarcode1.LicenseKey = "MY LICENSE KEY"
    
    ' Turn on the barcode types you want to read.
    ' Turn off the barcode types you don't want to read (this will increase the speed of your application)
    SoftekBarcode1.ReadCode128 = True
    SoftekBarcode1.ReadCode39 = True
    SoftekBarcode1.ReadCode25 = True
    SoftekBarcode1.ReadEAN13 = True
    SoftekBarcode1.ReadEAN8 = True
    SoftekBarcode1.ReadUPCA = True
    SoftekBarcode1.ReadUPCE = True
    SoftekBarcode1.ReadCodabar = False
    SoftekBarcode1.ReadPDF417 = True
    SoftekBarcode1.ReadDataMatrix = False
    SoftekBarcode1.ReadDatabar = False
    SoftekBarcode1.ReadMicroPDF417 = False

    ' Databar Options is a mask that controls which type of databar barcodes will be read and whether or not
    ' the software will look for a quiet zone around the barcode.
    ' 1 = 2D-Linkage flag (handle micro-PDf417 barcodes as supplementary data - requires ReadMicroPDF417 to be true).
    ' 2 = Read RSS14
    ' 4 = Read RSS14 Stacked
    ' 8 = Read Limited
    ' 16 = Read Expanded
    ' 32 = Read Expanded Stacked
    ' 64 = Require quiet zone
    SoftekBarcode1.DatabarOptions = 255
    
    ' If you want to read more than one barcode then set Multiple Read to 1
    ' Setting MutlipleRead to False will make the recognition faster
    SoftekBarcode1.MultipleRead = True

    ' MedianFilter is a useful way to clean up higher resolution images where the black bars contain white dots
    ' and the spaces contain black dots. It does not work if the space between bars is only 1 pixel wide.
    SoftekBarcode1.MedianFilter = False
    
    ' Noise reduction takes longer but can make it possible to read some difficult barcodes
    ' When using noise reduction a typical value is 10 - the smaller the value the more effect it has.
    ' A zero value turns off noise reduction.
    ' If you use NoiseReduction then the ScanDirection mask must be either only horizontal or only
    ' vertical (i.e 1, 2, 4, 8, 5 or 10).
    ' SoftekBarcode1.NoiseReduction = 0

    ' You may need to set a small quiet zone if your barcodes are close to text and pictures in the image.
    ' A value of zero uses the default.
    SoftekBarcode1.QuietZoneSize = 0

    ' LineJump controls the frequency at which scan lines in an image are sampled.
    ' The default is 1.
    SoftekBarcode1.LineJump = 1

    ' You can restrict your search to a particular area of the image if you wish.
    ' retval = SoftekBarcode1.SetScanRect(TopLeftX, TopLeftY, BottomRightX, BottomRightY, 0)

    ' Set the direction that the barcode reader should scan for barcodes
    ' The value is a mask where 1 = Left to Right, 2 = Top to Bottom, 3 = Right To Left, 4 = Bottom to Top
    SoftekBarcode1.ScanDirection = 15

    ' Set the page number to read from in a multi-page TIF file. The default is 0, which will make the
    ' toolkit check every page.
    ' SoftekBarcode1.PageNo = 1

    ' SkewTolerance controls the angle of skew that the barcode toolkit will tolerate. By default
    ' the toolkit checks for barcodes along horizontal and vertical lines in an image. This works
    ' OK for most barcodes because even at an angle it is possible to pass a line through the entire
    ' length. SkewTolerance can range from 0 to 5 and allows for barcodes skewed to an angle of 45
    ' degrees.
    SoftekBarcode1.SkewTolerance = 0

    ' ColorThreshold is used by the toolkit to determine whether a pixel in a color image should be
    ' considered as black or white. This property can range in value from 0 to 255, with a
    ' default of 128. Use a low value for dark images and a high value for bright images.
    ' A value of 0 means that the toolkit calculates the value from the image.
    SoftekBarcode1.ColorThreshold = 0

    ' MaxLength and MinLength can be used to specify the number of characters you expect to
    ' find in a barcode. This can be useful to increase accuracy or if you wish to ignore some
    ' barcodes in an image.
    SoftekBarcode1.MinLength = 4
    SoftekBarcode1.MaxLength = 99

    ' When the toolkit scans an image it records the number of hits it gets for each barcode that
    ' MIGHT be in the image. If the hits recorded for any of the barcodes are >= PrefOccurrence
    ' then only these barcodes are returned. Otherwise, any barcode whose hits are >= MinOccurrence
    ' are reported. If you have a very poor quality image then try setting MinOccurrence to 1, but you
    ' may find that some false positive results are returned.
    'SoftekBarcode1.MinOccurrence = 2
    'SoftekBarcode1.PrefOccurrence = 4
    
    
    Path = ImageFile.Path
    
    If (Right(Path, 1) <> "\") Then
        Path = Path + "\"
    End If
    
    Path = Path + ImageFile.FileName
    
    Results.Text = ""
    
    WriteResult ("Using OCX Interface to read barcodes")
   
    nBarCodes = SoftekBarcode1.ScanBarCode(Path)
    
    If nBarCodes < 0 Then
        WriteResult ("ScanBarCode returned error number " & nBarCodes)
    End If
    
    If nBarCodes = 0 Then
        WriteResult ("Sorry - no barcodes were found in this image")
    End If
        
    For i = 1 To nBarCodes
        WriteResult ("")
        WriteResult ("Barcode " & i & ":")
        
        WriteResult ("Value = " & SoftekBarcode1.GetBarString(i))
        
        WriteResult ("Type = " & SoftekBarcode1.GetBarStringType(i))
        
        nDirection = SoftekBarcode1.GetBarStringDirection(i)
        If nDirection = 1 Then
            WriteResult ("Direction = Left to Right")
        Else
            If nDirection = 2 Then
                WriteResult ("Direction = Top to Bottom")
            Else
                If nDirection = 4 Then
                    WriteResult ("Direction = Right to Left")
                Else
                    If nDirection = 8 Then
                        WriteResult ("Direction = Bottom to Top")
                    End If
                End If
            End If
        End If
        
        nPage = SoftekBarcode1.GetBarStringPos(i, TopLeftX, TopLeftY, BottomRightX, BottomRightY)
        WriteResult ("Page = " & nPage)
        WriteResult ("Top Left = (" & TopLeftX & "," & TopLeftY & ")")
        WriteResult ("Bottom Right = (" & BottomRightX & "," & BottomRightY & ")")
         
    Next i
    
    Results.SelStart = 0
End Sub


Sub WriteResult(Result As String)
    With Results
        .SelStart = Len(.Text)
        .SelText = vbNewLine & Result
    End With
End Sub
Private Sub Command2_Click()
    End
End Sub

Private Sub Dir1_Change()
    ImageFile.Path = Dir1.Path
End Sub

Private Sub Drive1_Change()
    Dir1.Path = Drive1.Drive
End Sub

Private Sub ImageFile_Click()
    ReadOCX
End Sub

Private Sub Read_Click()
    ImageFile_Click
End Sub

Function nullTrim(s As String) As String
' Trim the string to the null character
Dim n As Integer
n = InStr(s, Chr$(0))
If (n > 0) Then
nullTrim = Trim(Left(s, n - 1))
Else
nullTrim = Trim(s)
End If
End Function

