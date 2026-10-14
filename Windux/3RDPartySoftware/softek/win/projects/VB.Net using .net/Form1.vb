Public Class Form1
    Inherits System.Windows.Forms.Form

    Declare Function DeleteObject Lib "gdi32.dll" (ByVal hObject As IntPtr) As Boolean
    Declare Function LoadLibrary Lib "kernel32.dll" Alias "LoadLibraryA" (ByVal lpLibFileName As String) As IntPtr

    Dim nBarCodes As Integer
    Dim aTimer As System.Timers.Timer
    Friend WithEvents ProgressBar1 As System.Windows.Forms.ProgressBar
    Friend WithEvents CheckBoxbackground As System.Windows.Forms.CheckBox
    Friend WithEvents ButtonAbort As System.Windows.Forms.Button
    Dim barcode As SoftekBarcodeNet.BarcodeReader
    Delegate Sub SetTextCallback(ByVal [text] As String)
    Delegate Sub SetButtonCallback(ByVal [enable] As Boolean)
    Delegate Sub SetProgressCallback(ByVal [value] As Integer)



#Region " Windows Form Designer generated code "

    Public Sub New()
        MyBase.New()

        'This call is required by the Windows Form Designer.
        InitializeComponent()

        initBarcode()

        aTimer = New System.Timers.Timer(1)
        AddHandler aTimer.Elapsed, New System.Timers.ElapsedEventHandler(AddressOf checkRead)

        'Add any initialization after the InitializeComponent() call

    End Sub

    'Form overrides dispose to clean up the component list.
    Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing Then
            If Not (components Is Nothing) Then
                components.Dispose()
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
        Me.ButtonAbort = New System.Windows.Forms.Button
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
        Me.Results.Location = New System.Drawing.Point(12, 64)
        Me.Results.Multiline = True
        Me.Results.Name = "Results"
        Me.Results.ReadOnly = True
        Me.Results.ScrollBars = System.Windows.Forms.ScrollBars.Vertical
        Me.Results.Size = New System.Drawing.Size(569, 203)
        Me.Results.TabIndex = 3
        '
        'Button1
        '
        Me.Button1.Location = New System.Drawing.Point(269, 303)
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
        Me.ProgressBar1.Location = New System.Drawing.Point(15, 35)
        Me.ProgressBar1.Name = "ProgressBar1"
        Me.ProgressBar1.Size = New System.Drawing.Size(566, 23)
        Me.ProgressBar1.TabIndex = 10
        '
        'CheckBoxbackground
        '
        Me.CheckBoxbackground.AutoSize = True
        Me.CheckBoxbackground.Checked = True
        Me.CheckBoxbackground.CheckState = System.Windows.Forms.CheckState.Checked
        Me.CheckBoxbackground.Location = New System.Drawing.Point(12, 310)
        Me.CheckBoxbackground.Name = "CheckBoxbackground"
        Me.CheckBoxbackground.Size = New System.Drawing.Size(159, 17)
        Me.CheckBoxbackground.TabIndex = 11
        Me.CheckBoxbackground.Text = "Launch scan in background"
        Me.CheckBoxbackground.UseVisualStyleBackColor = True
        '
        'ButtonAbort
        '
        Me.ButtonAbort.Location = New System.Drawing.Point(371, 303)
        Me.ButtonAbort.Name = "ButtonAbort"
        Me.ButtonAbort.Size = New System.Drawing.Size(102, 23)
        Me.ButtonAbort.TabIndex = 12
        Me.ButtonAbort.Text = "Abort"
        Me.ButtonAbort.UseVisualStyleBackColor = True
        '
        'Form1
        '
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.ClientSize = New System.Drawing.Size(593, 338)
        Me.Controls.Add(Me.ButtonAbort)
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
        If (barcode.ScanBarCodeWait(100) = 1) Then
            SetProgress(barcode.GetProgress())
            aTimer.Start()
        Else
            SetProgress(barcode.GetProgress())
            nBarCodes = barcode.GetScanExitCode()
            getResults()
            SetButton(True)
        End If
    End Sub

    Private Sub initBarcode()
        ' For the purposes of this demo we first give a path to the installation folder
        ' and the class adds either x86 or x64 to the end and tries to load the other dll files.
        ' If that fails (perhaps this project has been moved) then we give no path and the class
        ' assumes that the dll files will be somewhere on the PATH.
        '
        ' In this project we give the path to the Softek DLL files when we create the BarcodeReader class
        ' Another way is to include the following files with your project:
        ' SoftekBarcodeDLL.dll
        ' SoftekBarcode64DLL.dll
        ' DebenuPDFLibraryDLLXXXX.dll
        ' DebenuPDFLibrary64DLLXXXX.dll

        barcode = New SoftekBarcodeNet.BarcodeReader("..\..\..\bin")

        ' Enter your license key here
        ' You can get a trial license key from sales@bardecode.com
        ' Example:
        ' barcode.LicenseKey = "MY LICENSE KEY"

        ' Turn on the barcode types you want to read.
        ' Turn off the barcode types you don't want to read (this will increase the speed of your application)
        barcode.ReadCode128 = True
        barcode.ReadCode39 = True
        barcode.ReadCode25 = True
        barcode.ReadEAN13 = True
        barcode.ReadEAN8 = True
        barcode.ReadUPCA = True
        barcode.ReadUPCE = True
        barcode.ReadCodabar = True
        barcode.ReadPDF417 = True
        barcode.ReadDataMatrix = True
        barcode.ReadDatabar = True
        barcode.ReadMicroPDF417 = False
        barcode.ReadQRCode = True

        ' Databar Options is a mask that controls which type of databar barcodes will be read and whether or not
        ' the software will look for a quiet zone around the barcode.
        ' 1 = 2D-Linkage flag (handle micro-PDf417 barcodes as supplementary data - requires ReadMicroPDF417 to be true).
        ' 2 = Read RSS14
        ' 4 = Read RSS14 Stacked
        ' 8 = Read Limited
        ' 16 = Read Expanded
        ' 32 = Read Expanded Stacked
        ' 64 = Require quiet zone
        barcode.DatabarOptions = 255

        ' If you want to read more than one barcode then set Multiple Read to 1
        ' Setting MutlipleRead to False will make the recognition faster
        barcode.MultipleRead = True

        ' If you know the max number of barcodes for a single page then increase speed by setting MaxBarcodesPerPage
        barcode.MaxBarcodesPerPage = 0

        ' In certain conditions (MultipleRead = false or MaxBarcodesPerPage = 1) the SDK can make fast scan of an image before performing the normal scan. This is useful if only 1 bar code is expected per page.
        barcode.UseFastScan = True

        ' If MultipleRead = false or MaxBarcodesPerPage = 1 and the bar code is always closer to the top of a page then set BarcodesAtTopOfPage to True to increase speed. 
        barcode.BarcodesAtTopOfPage = False


        ' Noise reduction takes longer but can make it possible to read some difficult barcodes
        ' When using noise reduction a typical value is 10 - the smaller the value the more effect it has.
        ' A zero value turns off noise reduction.
        ' barcode.NoiseReduction = 0

        ' You may need to set a small quiet zone if your barcodes are close to text and pictures in the image.
        ' A value of zero uses the default.
        barcode.QuietZoneSize = 0

        ' LineJump controls the frequency at which scan lines in an image are sampled.
        ' The default is 1.
        barcode.LineJump = 1

        ' You can restrict your search to a particular area of the image if you wish.
        ' This example limits the search to the upper half of the page
        ' Dim scanArea As System.Drawing.Rectangle
        ' scanArea = New System.Drawing.Rectangle(0, 0, 100, 50)
        ' barcode.SetScanRect(scanArea, 1)

        ' Set the direction that the barcode reader should scan for barcodes
        ' The value is a mask where 1 = Left to Right, 2 = Top to Bottom, 4 = Right To Left, 8 = Bottom to Top
        barcode.ScanDirection = 15

        ' Set the page number to read from in a multi-page TIF file. The default is 0, which will make the
        ' toolkit check every page.
        ' barcode.PageNo = 1

        ' SkewTolerance controls the angle of skew that the barcode toolkit will tolerate and ranges from 0 to 5 (45 degrees)
        ' Note that from version 7.6.1 the SDK is able to read some skewed barcodes with this setting. See below.
        barcode.SkewTolerance = 0

        ' Read most skewed linear barcodes without the need to set SkewTolerance. Currently applies to Codabar, Code 25, Code 39 and Code 128 barcodes only.
        barcode.SkewedLinear = True

        ' Read most skewed datamatrix barcodes without the need to set SkewTolerance
        barcode.SkewedDatamatrix = True


        ' ColorProcessingLevel controls how much time the toolkit will searching a color image for a barcode.
        ' The default value is 2 and the range of values is 0 to 5. If ColorThreshold is non-zero then 
        ' ColorProcessingLevel is effectively set to 0.
        barcode.ColorProcessingLevel = 2

        ' MaxLength and MinLength can be used to specify the number of characters you expect to
        ' find in a barcode. This can be useful to increase accuracy or if you wish to ignore some
        ' barcodes in an image.
        barcode.MinLength = 4
        barcode.MaxLength = 999

        ' When the toolkit scans an image it records the number of hits it gets for each barcode that
        ' MIGHT be in the image. If the hits recorded for any of the barcodes are >= PrefOccurrence
        ' then only these barcodes are returned. Otherwise, any barcode whose hits are >= MinOccurrence
        ' are reported. If you have a very poor quality image then try setting MinOccurrence to 1, but you
        ' may find that some false positive results are returned.
        ' barcode.MinOccurrence = 2
        ' barcode.PrefOccurrence = 4

        ' Read Code 39 barcodes in extended mode
        ' barcode.ExtendedCode39 = True

        ' Barcode string is numeric
        ' barcode.ReadNumeric = True

        ' Set a regular expression for the barcode
        ' barcode.Pattern = "^[A-Z]{2}[0-9]{5}$"

        ' If you are scanning at a high resolution and the spaces between bars are
        ' larger than 1 pixel then set MinSpaceBarWidth to 2 and increase your read rate.
        ' barcode.MinSpaceBarWidth = 2

        ' MedianFilter is a useful way to clean up higher resolution images where the black bars contain white dots
        ' and the spaces contain black dots. It does not work if the space between bars is only 1 pixel wide.
        barcode.MedianFilter = False

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
        barcode.ReportUnreadBarcodes = 0

        ' Time out for reading a barcode from a page in ms. Note that this does not include the time to load the page.
        ' 0 means no time out.
        barcode.TimeOut = 5000



        ' Flags for handling PDF files
        ' PdfImageOnly defaults to true and indicates that the PDF documents are simple images.
        barcode.PdfImageOnly = True

        ' PdfImageExtractOptions is no longer required in version 8 of the SDK but is retained for future possible use.
        ' barcode.PdfImageExtractOptions = 0

        ' The PdfImageRasterOptions mask controls how images are rasterized when PdfImageOnly is false or when image extraction fails
        ' 0 = Use Debenu to render image
        ' 4 = Use VeryPDF to render image (x86 only)
        barcode.PdfImageRasterOptions = 0

        ' PdfDpi and PdfBpp control what sort of image the PDF document is rasterized into
        barcode.PdfDpi = 300
        barcode.PdfBpp = 8

        ' Or you can load the settings from a xml format file
        ' See the manual for more details.
        ' barcode.LoadXMLSettings("settings.xml")
    End Sub

    Private Sub getResults()
        Dim i As Integer
        Dim nDirection As Integer
        Dim nPage As Integer
        Dim rect As System.Drawing.Rectangle

        If nBarCodes <= -6 Then
            SetText("License key error: either an evaluation key has expired or the license key is not valid for processing pdf documents")
        ElseIf nBarCodes < 0 Then
            SetText("ScanBarCode returned error number " & nBarCodes)
            SetText("Last Softek Error Number = " & barcode.GetLastError())
            SetText("Last Windows Error Number = " & barcode.GetLastWinError())
        End If

        If nBarCodes = 0 Then
            SetText("Sorry - no barcodes were found in this image")
        End If

        For i = 1 To nBarCodes
            SetText("Barcode " & i & ":")

            SetText("Value = " & barcode.GetBarString(i))

            SetText("Type = " & barcode.GetBarStringType(i))

            nDirection = barcode.GetBarStringDirection(i)
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

            nPage = barcode.GetBarStringPage(i)
            rect = barcode.GetBarStringRect(i)
            SetText("Page = " & nPage)
            SetText("Top Left = (" & rect.X & "," & rect.Y & ")")
            SetText("Bottom Right = (" & (rect.X + rect.Width) & "," & (rect.Y + rect.Height) & ")")
            SetText("")

        Next i

        GC.Collect()
    End Sub

    Private Sub ReadBarcode_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ReadBarcode.Click

        ProgressBar1.Value = 0

        Results.Text = ""

        If CheckBitmap.Checked Then
            If (ImageFile.Text.EndsWith(".pdf", System.StringComparison.CurrentCultureIgnoreCase)) Then
                MessageBox.Show("Cannot load PDF files into bitmaps")
                Return
            End If
            Dim bm As Bitmap
            SetText("Loading image into a bitmap and calling ScanBarCodeFromBitmap")
            bm = New Bitmap(ImageFile.Text)
            nBarCodes = barcode.ScanBarCodeFromBitmap(bm)
            getResults()
        Else
            If (CheckBoxbackground.Checked) Then
                barcode.ScanBarCodeInBackground(ImageFile.Text)
                aTimer.Start()
                ReadBarcode.Enabled = False
            Else
                nBarCodes = barcode.ScanBarCode(ImageFile.Text)
                ' The above line could be rewritten using background scan as follows:
                ' barcode.ScanBarCodeInBackground(ImageFile.Text) - start the background scan
                ' barcode.ScanBarCodeWait(-1) - wait until it finishes
                ' nBarCodes = barcode.GetScanExitCode() - get the exit code (number of barcodes or error code)
                getResults()
            End If
        End If
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

    Private Sub ButtonAbort_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ButtonAbort.Click
        barcode.ScanBarCodeAbort()
    End Sub
End Class


