
Public Class Form1
    Inherits System.Windows.Forms.Form
    Dim oBarcode As SoftekBarcodeCOM.CBarcode
    Friend WithEvents ProgressBar1 As System.Windows.Forms.ProgressBar
    Dim nBarCodes As Integer
    Dim aTimer As System.Timers.Timer
    Delegate Sub SetTextCallback(ByVal [text] As String)
    Delegate Sub SetButtonCallback(ByVal [enable] As Boolean)
    Delegate Sub SetProgressCallback(ByVal [value] As Integer)





#Region " Windows Form Designer generated code "

    Public Sub New()
        MyBase.New()

        'This call is required by the Windows Form Designer.
        InitializeComponent()

        'Add any initialization after the InitializeComponent() call
        initBarcode()
        aTimer = New System.Timers.Timer(1)
        AddHandler aTimer.Elapsed, New System.Timers.ElapsedEventHandler(AddressOf checkRead)

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
    Friend WithEvents ImageFile As System.Windows.Forms.TextBox
    Friend WithEvents ImageBtn As System.Windows.Forms.Button
    Friend WithEvents OpenFileDialog1 As System.Windows.Forms.OpenFileDialog
    Friend WithEvents ReadBarcode As System.Windows.Forms.Button
    Friend WithEvents CheckBoxbackground As System.Windows.Forms.CheckBox
    Friend WithEvents ButtonAbort As System.Windows.Forms.Button
    Friend WithEvents Results As System.Windows.Forms.TextBox
    <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
        Me.ImageFile = New System.Windows.Forms.TextBox
        Me.ImageBtn = New System.Windows.Forms.Button
        Me.OpenFileDialog1 = New System.Windows.Forms.OpenFileDialog
        Me.ReadBarcode = New System.Windows.Forms.Button
        Me.Results = New System.Windows.Forms.TextBox
        Me.Button1 = New System.Windows.Forms.Button
        Me.Label1 = New System.Windows.Forms.Label
        Me.CheckBoxbackground = New System.Windows.Forms.CheckBox
        Me.ButtonAbort = New System.Windows.Forms.Button
        Me.ProgressBar1 = New System.Windows.Forms.ProgressBar
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
        Me.OpenFileDialog1.Filter = "Images (*.tif,*.pdf,*.bmp,*.jpg)|*.tif;*.pdf;*.jpg;*.tiff;*.bmp;*.jpeg"
        '
        'ReadBarcode
        '
        Me.ReadBarcode.Location = New System.Drawing.Point(479, 324)
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
        Me.Button1.Location = New System.Drawing.Point(12, 324)
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
        'CheckBoxbackground
        '
        Me.CheckBoxbackground.AutoSize = True
        Me.CheckBoxbackground.Checked = True
        Me.CheckBoxbackground.CheckState = System.Windows.Forms.CheckState.Checked
        Me.CheckBoxbackground.Location = New System.Drawing.Point(12, 287)
        Me.CheckBoxbackground.Name = "CheckBoxbackground"
        Me.CheckBoxbackground.Size = New System.Drawing.Size(159, 17)
        Me.CheckBoxbackground.TabIndex = 6
        Me.CheckBoxbackground.Text = "Launch scan in background"
        Me.CheckBoxbackground.UseVisualStyleBackColor = True
        '
        'ButtonAbort
        '
        Me.ButtonAbort.Location = New System.Drawing.Point(371, 324)
        Me.ButtonAbort.Name = "ButtonAbort"
        Me.ButtonAbort.Size = New System.Drawing.Size(102, 23)
        Me.ButtonAbort.TabIndex = 7
        Me.ButtonAbort.Text = "Abort"
        Me.ButtonAbort.UseVisualStyleBackColor = True
        '
        'ProgressBar1
        '
        Me.ProgressBar1.Location = New System.Drawing.Point(15, 35)
        Me.ProgressBar1.Name = "ProgressBar1"
        Me.ProgressBar1.Size = New System.Drawing.Size(566, 23)
        Me.ProgressBar1.TabIndex = 8
        '
        'Form1
        '
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.ClientSize = New System.Drawing.Size(593, 360)
        Me.Controls.Add(Me.ProgressBar1)
        Me.Controls.Add(Me.ButtonAbort)
        Me.Controls.Add(Me.CheckBoxbackground)
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
        ' Wait for 100 ms
        If (oBarcode.ScanBarCodeWait(100) = 1) Then
            SetProgress(oBarcode.Progress)
            aTimer.Start()
        Else
            SetProgress(oBarcode.Progress)
            nBarCodes = oBarcode.scanExitCode
            getResults()
            SetButton(True)
        End If
    End Sub
    Private Sub initBarcode()
        oBarcode = New SoftekBarcodeCOM.CBarcode()


        ' Enter your license key here
        ' You can get a trial license key from sales@bardecode.com
        ' Example:
        ' oBarcode.LicenseKey = "MY LICENSE KEY"


        ' Turn on the barcode types you want to read.
        ' Turn off the barcode types you don't want to read (this will increase the speed of your application)
        oBarcode.ReadCode128 = True
        oBarcode.ReadCode39 = True
        oBarcode.ReadCode25 = True
        oBarcode.ReadEAN13 = True
        oBarcode.ReadEAN8 = True
        oBarcode.ReadUPCA = True
        oBarcode.ReadUPCE = True
        oBarcode.ReadCodabar = True
        oBarcode.ReadPDF417 = True
        oBarcode.ReadDataMatrix = True
        oBarcode.ReadDatabar = True
        oBarcode.ReadMicroPDF417 = False
        oBarcode.ReadQRCode = True


        ' Databar Options is a mask that controls which type of databar barcodes will be read and whether or not
        ' the software will look for a quiet zone around the barcode.
        ' 1 = 2D-Linkage flag (handle micro-PDf417 barcodes as supplementary data - requires ReadMicroPDF417 to be true).
        ' 2 = Read RSS14
        ' 4 = Read RSS14 Stacked
        ' 8 = Read Limited
        ' 16 = Read Expanded
        ' 32 = Read Expanded Stacked
        ' 64 = Require quiet zone
        oBarcode.DatabarOptions = 255

        ' If you want to read more than one barcode then set Multiple Read to True
        ' Setting MutlipleRead to False will make the recognition faster
        oBarcode.MultipleRead = True

        ' If you know the max number of barcodes for a single page then increase speed by setting MaxBarcodesPerPage
        oBarcode.MaxBarcodesPerPage = 0

        ' In certain conditions (MultipleRead = false or MaxBarcodesPerPage = 1) the SDK can make fast scan of an image before performing the normal scan. This is useful if only 1 bar code is expected per page.
        oBarcode.UseFastScan = True

        ' If MultipleRead = false or MaxBarcodesPerPage = 1 and the bar code is always closer to the top of a page then set BarcodesAtTopOfPage to True to increase speed. 
        oBarcode.BarcodesAtTopOfPage = False

        ' Noise reduction takes longer but can make it possible to read some difficult barcodes
        ' When using noise reduction a typical value is 10 - the smaller the value the more effect it has.
        ' A zero value turns off noise reduction.
        ' If you use NoiseReduction then the ScanDirection mask must be either only horizontal or only
        ' vertical (i.e 1, 2, 4, 8, 5 or 10).
        ' oBarcode.NoiseReduction = 0

        ' You may need to set a small quiet zone if your barcodes are close to text and pictures in the image.
        ' A value of zero uses the default.
        oBarcode.QuietZoneSize = 0

        ' LineJump controls the frequency at which scan lines in an image are sampled.
        ' The default is 1.
        oBarcode.LineJump = 1

        ' You can restrict your search to a particular area of the image if you wish.
        ' retval = oBarcode.SetScanRect(TopLeftX, TopLeftY, BottomRightX, BottomRightY, 0)

        ' Set the direction that the barcode reader should scan for barcodes
        ' The value is a mask where 1 = Left to Right, 2 = Top to Bottom, 3 = Right To Left, 4 = Bottom to Top
        oBarcode.ScanDirection = 15

        ' Set the page number to read from in a multi-page TIF file. The default is 0, which will make the
        ' toolkit check every page.
        ' oBarcode.PageNo = 1

        ' SkewTolerance controls the angle of skew that the barcode toolkit will tolerate. By default
        ' the toolkit checks for barcodes along horizontal and vertical lines in an image. This works
        ' OK for most barcodes because even at an angle it is possible to pass a line through the entire
        ' length. SkewTolerance can range from 0 to 5 and allows for barcodes skewed to an angle of 45
        ' degrees.
        oBarcode.SkewTolerance = 0

        ' Read most skewed linear barcodes without the need to set SkewTolerance. Currently applies to Codabar, Code 25, Code 39 and Code 128 barcodes only.
        oBarcode.SkewedLinear = True

        ' Read most skewed datamatrix barcodes without the need to set SkewTolerance
        oBarcode.SkewedDatamatrix = True

        ' ColorProcessingLevel controls how much time the toolkit will searching a color image for a barcode.
        ' The default value is 2 and the range of values is 0 to 5. If ColorThreshold is non-zero then 
        ' ColorProcessingLevel is effectively set to 0.
        oBarcode.ColorProcessingLevel = 2

        ' MaxLength and MinLength can be used to specify the number of characters you expect to
        ' find in a barcode. This can be useful to increase accuracy or if you wish to ignore some
        ' barcodes in an image.
        oBarcode.MinLength = 4
        oBarcode.MaxLength = 999

        ' When the toolkit scans an image it records the number of hits it gets for each barcode that
        ' MIGHT be in the image. If the hits recorded for any of the barcodes are >= PrefOccurrence
        ' then only these barcodes are returned. Otherwise, any barcode whose hits are >= MinOccurrence
        ' are reported. If you have a very poor quality image then try setting MinOccurrence to 1, but you
        ' may find that some false positive results are returned.
        'oBarcode.MinOccurrence = 2
        'oBarcode.PrefOccurrence = 4

        ' Read Code 39 barcodes in extended mode
        ' oBarcode.ExtendedCode39 = True

        ' Barcode string is numeric
        ' oBarcode.ReadNumeric = True

        ' Set a regular expression for the barcode
        ' oBarcode.Pattern = "^[A-Z]{2}[0-9]{5}$"

        ' If you are scanning at a high resolution and the spaces between bars are
        ' larger than 1 pixel then set MinSpaceBarWidth to 2 and increase your read rate.
        ' oBarcode.MinSpaceBarWidth = 2

        ' MedianFilter is a useful way to clean up higher resolution images where the black bars contain white dots
        ' and the spaces contain black dots. It does not work if the space between bars is only 1 pixel wide.
        oBarcode.MedianFilter = False

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
        oBarcode.ReportUnreadBarcodes = 0

        ' Time out for reading a barcode from a page in ms. Note that this does not include the time to load the page.
        ' 0 means no time out.
        oBarcode.TimeOut = 5000

        ' Flags for handling PDF files
        ' PdfImageOnly defaults to true and indicates that the PDF documents are simple images.
        oBarcode.PdfImageOnly = True

        ' PdfImageExtractOptions is no longer required in version 8 of the SDK but is retained for future possible use.
        ' oBarcode.PdfImageExtractOptions = 0

        ' The PdfImageRasterOptions mask controls how images are rasterized when PdfImageOnly is false or when image extraction fails
        ' 0 = Use Debenu to render image
        ' 4 = Use VeryPDF to render image (x86 only)
        oBarcode.PdfImageRasterOptions = 0

        ' PdfDpi and PdfBpp control what sort of image the PDF document is rasterized into
        oBarcode.PdfDpi = 300
        oBarcode.PdfBpp = 8


        ' Or you can load the settings from a xml format file
        ' See the manual for more details.
        ' oBarcode.LoadXMLSettings("settings.xml")
        ' The result of any XML operation is stored in oBarcode.XMLRetval (1 = Success, 0 = Failure)


    End Sub

    Private Sub getResults()
        Dim nDirection As Integer
        Dim TopLeftX As Long
        Dim TopLeftY As Long
        Dim BottomRightX As Long
        Dim BottomRightY As Long
        Dim nPage As Integer

        Dim i As Short

        ProgressBar1.Value = oBarcode.Progress

        ' You can also save the results to an XML file
        ' oBarcode.SaveXMLResults("results.xml")

        If nBarCodes <= -6 Then
            SetText("License key error: either an evaluation key has expired or the license key is not valid for processing pdf documents")
        ElseIf nBarCodes < 0 Then
            SetText("ScanBarCode returned error number " & nBarCodes)
            SetText("Last Softek Error Number = " & oBarcode.LastError)
            SetText("Last Windows Error Number = " & oBarcode.LastWinError)
        End If

        If nBarCodes = 0 Then
            SetText("Sorry - no barcodes were found in this image")
        End If

        For i = 1 To nBarCodes
            SetText("")
            SetText("Barcode " & i & ":")

            SetText("Value = " & oBarcode.BarString(i))

            SetText("Type = " & oBarcode.BarStringType(i))

            nDirection = oBarcode.BarStringDirection(i)
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

            nPage = oBarcode.BarStringPage(i)

            oBarcode.GetBarStringPos(i, TopLeftX, TopLeftY, BottomRightX, BottomRightY)
            nPage = oBarcode.BarStringPage(i)
            SetText("Page = " & nPage)
            SetText("Top Left = (" & TopLeftX & "," & TopLeftY & ")")
            SetText("Bottom Right = (" & BottomRightX & "," & BottomRightY & ")")

        Next i
    End Sub

    Private Sub ReadBarcode_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ReadBarcode.Click
        Results.Text = ""
        SetText("Using COM Interface to read barcodes")
        ProgressBar1.Value = 0
        If (CheckBoxbackground.Checked) Then
            oBarcode.ScanBarCodeInBackground(ImageFile.Text)
            aTimer.Start()
            ReadBarcode.Enabled = False
        Else
            oBarcode.ScanBarCode(ImageFile.Text)
            nBarCodes = oBarcode.scanExitCode
            getResults()
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
        oBarcode.ScanBarCodeAbort()

    End Sub
End Class


