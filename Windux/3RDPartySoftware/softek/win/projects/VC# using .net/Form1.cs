using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.Runtime.InteropServices;
using System.Threading;
using System.Timers;

namespace BarcodeReader
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
    /// 

	public class Form1 : System.Windows.Forms.Form
    {
        [DllImport("kernel32")]
        public extern static int LoadLibrary(string librayName);

		internal System.Windows.Forms.OpenFileDialog OpenFileDialog1;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
        internal TextBox Result;
        internal Button ImageBtn;
        internal Button ReadBarcode;
        internal TextBox FilePath;
        private Label label1;
        internal Button buttonClose;
        public SoftekBarcodeNet.BarcodeReader barcode;
        private Button buttonAbort;
        private ProgressBar progressBar1;
        internal CheckBox CheckBoxbackground;
        internal CheckBox CheckBitmap;
        private int nBarCode;
        private static System.Timers.Timer aTimer;
        delegate void SetTextCallback(string text);
        delegate void SetProgressCallback(int value);
        delegate void SetButtonCallback(bool enabled);


		public Form1()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//

            initBarcode();
            aTimer = new System.Timers.Timer(100);
            aTimer.Elapsed += checkRead;
            aTimer.Enabled = false;

		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
            this.OpenFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.Result = new System.Windows.Forms.TextBox();
            this.ImageBtn = new System.Windows.Forms.Button();
            this.ReadBarcode = new System.Windows.Forms.Button();
            this.FilePath = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.buttonClose = new System.Windows.Forms.Button();
            this.buttonAbort = new System.Windows.Forms.Button();
            this.progressBar1 = new System.Windows.Forms.ProgressBar();
            this.CheckBoxbackground = new System.Windows.Forms.CheckBox();
            this.CheckBitmap = new System.Windows.Forms.CheckBox();
            this.SuspendLayout();
            // 
            // OpenFileDialog1
            // 
            this.OpenFileDialog1.DefaultExt = "tif";
            this.OpenFileDialog1.Filter = "Images (*.tif,*.pdf,*.bmp,*.jpg)|*.tif;*.pdf;*.jpg;*.tiff;*.bmp;*.jpeg";
            this.OpenFileDialog1.FileOk += new System.ComponentModel.CancelEventHandler(this.OpenFileDialog1_FileOk);
            // 
            // Result
            // 
            this.Result.Location = new System.Drawing.Point(12, 83);
            this.Result.Multiline = true;
            this.Result.Name = "Result";
            this.Result.ReadOnly = true;
            this.Result.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.Result.Size = new System.Drawing.Size(608, 214);
            this.Result.TabIndex = 3;
            this.Result.TextChanged += new System.EventHandler(this.Result_TextChanged);
            // 
            // ImageBtn
            // 
            this.ImageBtn.Location = new System.Drawing.Point(518, 12);
            this.ImageBtn.Name = "ImageBtn";
            this.ImageBtn.Size = new System.Drawing.Size(102, 23);
            this.ImageBtn.TabIndex = 1;
            this.ImageBtn.Text = "Browse";
            this.ImageBtn.Click += new System.EventHandler(this.ImageBtn_Click);
            // 
            // ReadBarcode
            // 
            this.ReadBarcode.Location = new System.Drawing.Point(518, 342);
            this.ReadBarcode.Name = "ReadBarcode";
            this.ReadBarcode.Size = new System.Drawing.Size(102, 24);
            this.ReadBarcode.TabIndex = 2;
            this.ReadBarcode.Text = "Start Scan";
            this.ReadBarcode.Click += new System.EventHandler(this.ReadBarcode_Click);
            // 
            // FilePath
            // 
            this.FilePath.Location = new System.Drawing.Point(66, 14);
            this.FilePath.Name = "FilePath";
            this.FilePath.ReadOnly = true;
            this.FilePath.Size = new System.Drawing.Size(446, 20);
            this.FilePath.TabIndex = 0;
            this.FilePath.TextChanged += new System.EventHandler(this.FilePath_TextChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 17);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(23, 13);
            this.label1.TabIndex = 4;
            this.label1.Text = "File";
            // 
            // buttonClose
            // 
            this.buttonClose.Location = new System.Drawing.Point(302, 342);
            this.buttonClose.Name = "buttonClose";
            this.buttonClose.Size = new System.Drawing.Size(102, 24);
            this.buttonClose.TabIndex = 11;
            this.buttonClose.Text = "Close";
            this.buttonClose.Click += new System.EventHandler(this.buttonClose_Click);
            // 
            // buttonAbort
            // 
            this.buttonAbort.Location = new System.Drawing.Point(410, 342);
            this.buttonAbort.Name = "buttonAbort";
            this.buttonAbort.Size = new System.Drawing.Size(102, 23);
            this.buttonAbort.TabIndex = 12;
            this.buttonAbort.Text = "Abort";
            this.buttonAbort.UseVisualStyleBackColor = true;
            this.buttonAbort.Click += new System.EventHandler(this.button1_Click);
            // 
            // progressBar1
            // 
            this.progressBar1.Location = new System.Drawing.Point(15, 47);
            this.progressBar1.Name = "progressBar1";
            this.progressBar1.Size = new System.Drawing.Size(605, 23);
            this.progressBar1.TabIndex = 13;
            // 
            // CheckBoxbackground
            // 
            this.CheckBoxbackground.AutoSize = true;
            this.CheckBoxbackground.Checked = true;
            this.CheckBoxbackground.CheckState = System.Windows.Forms.CheckState.Checked;
            this.CheckBoxbackground.Location = new System.Drawing.Point(12, 342);
            this.CheckBoxbackground.Name = "CheckBoxbackground";
            this.CheckBoxbackground.Size = new System.Drawing.Size(159, 17);
            this.CheckBoxbackground.TabIndex = 15;
            this.CheckBoxbackground.Text = "Launch scan in background";
            this.CheckBoxbackground.UseVisualStyleBackColor = true;
            // 
            // CheckBitmap
            // 
            this.CheckBitmap.AutoSize = true;
            this.CheckBitmap.Location = new System.Drawing.Point(12, 315);
            this.CheckBitmap.Name = "CheckBitmap";
            this.CheckBitmap.Size = new System.Drawing.Size(389, 17);
            this.CheckBitmap.TabIndex = 14;
            this.CheckBitmap.Text = "Load page 1 of image into a Bitmap object and call ScanBarCodeFromBitmap";
            this.CheckBitmap.UseVisualStyleBackColor = true;
            // 
            // Form1
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(632, 376);
            this.Controls.Add(this.CheckBoxbackground);
            this.Controls.Add(this.CheckBitmap);
            this.Controls.Add(this.progressBar1);
            this.Controls.Add(this.buttonAbort);
            this.Controls.Add(this.buttonClose);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.Result);
            this.Controls.Add(this.ReadBarcode);
            this.Controls.Add(this.FilePath);
            this.Controls.Add(this.ImageBtn);
            this.Name = "Form1";
            this.Text = "Barcode Reader C#";
            this.ResumeLayout(false);
            this.PerformLayout();

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new Form1());
		}

		private void ImageBtn_Click(object sender, System.EventArgs e)
		{
			OpenFileDialog1.ShowDialog() ;
			FilePath.Text = OpenFileDialog1.FileName ;
		}

        private void checkRead(Object source, ElapsedEventArgs e)
        {
            if (barcode.ScanBarCodeWait(0) == 1)
            {
                SetProgress(barcode.GetProgress());
                return;
            }
            SetProgress(barcode.GetProgress());
            aTimer.Stop();
            nBarCode = barcode.GetScanExitCode();
            getResults();
            SetButton(true);
        }

        private void getResults()
        {
            if (nBarCode <= -6)
            {
                SetText("License key error: either an evaluation key has expired or the license key is not valid for processing pdf documents\r\n");
            }
            else if (nBarCode < 0)
            {
                SetText("ScanBarCode returned error number ");
                SetText(nBarCode.ToString());
                SetText("\r\n");
                SetText("Last Softek Error Number = ");
                SetText(barcode.GetLastError().ToString());
                SetText("\r\n");
                SetText("Last Windows Error Number = ");
                SetText(barcode.GetLastWinError().ToString());
                SetText("\r\n");
            }
            else if (nBarCode == 0)
            {
                SetText("No barcode found on this image\r\n");
            }
            else
            {
                SetText("\r\n");
                for (int i = 1; i <= nBarCode; i++)
                {
                    SetText(String.Format("Barcode {0}\r\n", i));
                    SetText(String.Format("Value = {0}\r\n", barcode.GetBarString(i)));
                    SetText(String.Format("Type = {0}\r\n", barcode.GetBarStringType(i)));
                    int nDirection = barcode.GetBarStringDirection(i);
                    if (nDirection == 1)
                        SetText("Direction = Left to Right\r\n");
                    else if (nDirection == 2)
                        SetText("Direction = Top to Bottom\r\n");
                    else if (nDirection == 4)
                        SetText("Direction = Right to Left\r\n");
                    else if (nDirection == 8)
                        SetText("Direction = Bottom to Top\r\n");
                    int nPage = barcode.GetBarStringPage(i);
                    SetText(String.Format("Page = {0}\r\n", nPage));
                    System.Drawing.Rectangle rect = barcode.GetBarStringRect(i);
                    SetText(String.Format("Top Left = ({0},{1})\r\n", rect.X, rect.Y));
                    SetText(String.Format("Bottom Right = ({0},{1})\r\n", rect.X + rect.Width, rect.Y + rect.Height));
                    SetText("\r\n");
                }
            }

            // No clean up necessary - all resources freed each time you call ScanBarCode or 
            // when the object is destroyed.
        }

        private void SetButton(bool enabled)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (this.ReadBarcode.InvokeRequired)
            {
                SetButtonCallback d = new SetButtonCallback(SetButton);
                this.Invoke(d, new object[] { enabled });
            }
            else
            {
                this.ReadBarcode.Enabled = enabled;
            }
        }

        private void SetText(string text)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (this.Result.InvokeRequired)
            {
                SetTextCallback d = new SetTextCallback(SetText);
                this.Invoke(d, new object[] { text });
            }
            else
            {
                this.Result.Text += text;
            }
        }

        private void SetProgress(int value)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (this.progressBar1.InvokeRequired)
            {
                SetProgressCallback d = new SetProgressCallback(SetProgress);
                this.Invoke(d, new object[] { value });
            }
            else
            {
                this.progressBar1.Value = value;
            }
        }

        private void ReadBarcode_Click(object sender, System.EventArgs e)
        {
            Result.Text = "";
            SetText("Using .Net Interface to read barcodes\r\n");
            SetProgress(0);

            if (CheckBitmap.Checked)
            {
                if (FilePath.Text.EndsWith(".pdf", System.StringComparison.CurrentCultureIgnoreCase))
                {
                    MessageBox.Show("Cannot load PDF files into bitmaps");
                    return;
                }
                System.Drawing.Bitmap bm = new System.Drawing.Bitmap(FilePath.Text);
                SetText("Loading image into a bitmap and calling ScanBarCodeFromBitmap\r\n");
                nBarCode = barcode.ScanBarCodeFromBitmap(bm);
                bm.Dispose();
                getResults();
            }
            else
            {
                if (CheckBoxbackground.Checked)
                {
                    SetText("Starting background scan\r\n");

                    SetButton(false);

                    barcode.ScanBarCodeInBackground(FilePath.Text);
                    aTimer.Enabled = true;
                    aTimer.Start();
                }
                else
                {
                    nBarCode = barcode.ScanBarCode(FilePath.Text);
                    getResults();
                }
            }



		}

        private void initBarcode()
        {
            // For the purposes of this demo we first give a path to the installation folder
            // and the class adds either x86 or x64 to the end and tries to load the other dll files.
            // If that fails (perhaps this project has been moved) then we give no path and the class
            // assumes that the dll files will be somewhere on the PATH.


            // In this project we give the path to the Softek DLL files when we create the BarcodeReader class
            // Another way is to include the following files with your project:
            // SoftekBarcodeDLL.dll
            // SoftekBarcode64DLL.dll
            // DebenuPDFLibraryDLLXXXX.dll
            // DebenuPDFLibrary64DLLXXXX.dll

            barcode = new SoftekBarcodeNet.BarcodeReader("..\\..\\..\\..\\bin");


            // Enter your license key here
            // You can get a trial license key from sales@bardecode.com
            // Example:
            // barcode.LicenseKey = "MY LICENSE KEY";

            // Turn on the barcode types you want to read.
            // Turn off the barcode types you don't want to read (this will increase the speed of your application)
            barcode.ReadCode128 = true;
            barcode.ReadCode39 = true;
            barcode.ReadCode25 = true;
            barcode.ReadEAN13 = true;
            barcode.ReadEAN8 = true;
            barcode.ReadUPCA = true;
            barcode.ReadUPCE = true;
            barcode.ReadCodabar = false;
            barcode.ReadPDF417 = true;
            barcode.ReadDataMatrix = true;
            barcode.ReadDatabar = true;
            barcode.ReadMicroPDF417 = false;
            barcode.ReadQRCode = true;

            // Databar Options is a mask that controls which type of databar barcodes will be read and whether or not
            // the software will look for a quiet zone around the barcode.
            // 1 = 2D-Linkage flag (handle micro-PDf417 barcodes as supplementary data - requires ReadMicroPDF417 to be true).
            // 2 = Read RSS14
            // 4 = Read RSS14 Stacked
            // 8 = Read Limited
            // 16 = Read Expanded
            // 32 = Read Expanded Stacked
            // 64 = Require quiet zone
            barcode.DatabarOptions = 255;



            // If you want to read more than one barcode then set Multiple Read to 1
            // Setting MutlipleRead to False will make the recognition faster
            barcode.MultipleRead = true;

            // If you know the max number of barcodes for a single page then increase speed by setting MaxBarcodesPerPage
            barcode.MaxBarcodesPerPage = 0;

            // In certain conditions (MultipleRead = false or MaxBarcodesPerPage = 1) the SDK can make fast scan of an image before performing the normal scan. This is useful if only 1 bar code is expected per page.
            barcode.UseFastScan = true;


            // Noise reduction takes longer but can make it possible to read some difficult barcodes
            // When using noise reduction a typical value is 10 - the smaller the value the more effect it has.
            // A zero value turns off noise reduction. 
            // barcode.NoiseReduction = 0 ;

            // You may need to set a small quiet zone if your barcodes are close to text and pictures in the image.
            // A value of zero uses the default.
            barcode.QuietZoneSize = 0;

            // LineJump controls the frequency at which scan lines in an image are sampled.
            // The default is 9 - decrease this for difficult barcodes.
            barcode.LineJump = 1;

            // You can restrict your search to a particular area of the image if you wish.
            // This example limits the search to the upper half of the page
            // System.Drawing.Rectangle scanArea = new System.Drawing.Rectangle(0, 0, 100, 50);
            // barcode.SetScanRect(scanArea, 1);

            // Set the direction that the barcode reader should scan for barcodes
            // The value is a mask where 1 = Left to Right, 2 = Top to Bottom, 4 = Right To Left, 8 = Bottom to Top
            barcode.ScanDirection = 15;

            // SkewTolerance controls the angle of skew that the barcode toolkit will tolerate. By default
            // the toolkit checks for barcodes along horizontal and vertical lines in an image. This works 
            // OK for most barcodes because even at an angle it is possible to pass a line through the entire
            // length. SkewTolerance can range from 0 to 5 and allows for barcodes skewed to an angle of 45
            // degrees.
            barcode.SkewTolerance = 0;

            // Read most skewed linear barcodes without the need to set SkewTolerance. Currently applies to Codabar, Code 25, Code 39 and Code 128 barcodes only.
            barcode.SkewedLinear = true;

            // Read most skewed datamatrix barcodes without the need to set SkewTolerance
            barcode.SkewedDatamatrix = true;


            // ColorProcessingLevel controls how much time the toolkit will searching a color image for a barcode.
            // The default value is 2 and the range of values is 0 to 5. If ColorThreshold is non-zero then 
            // ColorProcessingLevel is effectively set to 0.
            barcode.ColorProcessingLevel = 2;

            // MaxLength and MinLength can be used to specify the number of characters you expect to
            // find in a barcode. This can be useful to increase accuracy or if you wish to ignore some
            // barcodes in an image.
            barcode.MinLength = 4;
            barcode.MaxLength = 999;

            // When the toolkit scans an image it records the score it gets for each barcode that
            // MIGHT be in the image. If the scores recorded for any of the barcodes are >= PrefOccurrence
            // then only these barcodes are returned. Otherwise, any barcode whose scores are >= MinOccurrence
            // are reported. If you have a very poor quality image then try setting MinOccurrence to 1, but you
            // may find that some false positive results are returned.
            // barcode.MinOccurrence = 2 ;
            // barcode.PrefOccurrence = 4 ;

            // Read Code 39 barcodes in extended mode
            // barcode.ExtendedCode39 = true;

            // Barcode string is numeric
            // barcode.ReadNumeric = true;

            // Set a regular expression for the barcode
            // barcode.Pattern = "^[A-Z]{2}[0-9]{5}$";

            // If you are scanning at a high resolution and the spaces between bars are
            // larger than 1 pixel then set MinSpaceBarWidth to 2 and increase your read rate.
            // barcode.MinSpaceBarWidth = 2;

            // MedianFilter is a useful way to clean up higher resolution images where the black bars contain white dots
            // and the spaces contain black dots. It does not work if the space between bars is only 1 pixel wide.
            barcode.MedianFilter = false;

            // ReportUnreadBarcodes can be used to warn of the presence of a barcode on a page that the SDK has not been able to decode.
            // It currently has the following important limitations:
            // 1. An unread linear barcode will only be reported if no other barcode was found in the same page.
            // 2. The height of the area for an unread linear barcode will only cover a portion of the barcode.
            // 3. Only 2-D barcodes that fail to error correct will be reported.
            // 4. The barcode type and value will both be set to UNREAD for all unread barcodes.
            // 5. The reporting of unread linear barcodes takes no account of settings for individual barcode types. For example, if ReadCode39 is True and 
            // an image contains a single Code 39 barcode then this will be reported as an unread barcode.
            // 6. 2-D barcodes are only reported as unread if the correct barcode types have been enabled.
            // 7. Not all unread barcodes will be detected. 
            //
            // The value is a mask with the following values: 1 = linear barcodes, 2 = Datamatrix, 4 = QR-Code, 8 = PDF-417
            barcode.ReportUnreadBarcodes = 0;

            // Time out for reading a barcode from a page in ms. Note that this does not include the time to load the page.
            // 0 means no time out.
            barcode.TimeOut = 5000;

            // Flags for handling PDF files
            // PdfImageOnly defaults to true and indicates that the PDF documents are simple images.
            barcode.PdfImageOnly = true;

            // PdfImageExtractOptions is no longer required in version 8 of the SDK but is retained for future possible use.
            // barcode.PdfImageExtractOptions = 0;

            // The PdfImageRasterOptions mask controls how images are rasterized when PdfImageOnly is false or when image extraction fails
            // 0 = Use Debenu to render image
            // 4 = Use VeryPDF to render image (x86 only)
            barcode.PdfImageRasterOptions = 0;

            // PdfDpi and PdfBpp control what sort of image the PDF document is rasterized into
            barcode.PdfDpi = 300;
            barcode.PdfBpp = 8;

        }

		private void PictureBox1_Click(object sender, System.EventArgs e)
		{
		
		}

        private void ResultFromPicture_TextChanged(object sender, EventArgs e)
        {

        }

        private void Label3_Click(object sender, EventArgs e)
        {

        }

        private void Label2_Click(object sender, EventArgs e)
        {

        }

        private void Panel2_Paint(object sender, PaintEventArgs e)
        {

        }

        private void FilePath_TextChanged(object sender, EventArgs e)
        {

        }

        private void Panel1_Paint(object sender, PaintEventArgs e)
        {

        }

        private void Result_TextChanged(object sender, EventArgs e)
        {

        }

        private void Label1_Click(object sender, EventArgs e)
        {

        }

        private void OpenFileDialog1_FileOk(object sender, CancelEventArgs e)
        {

        }

        private void buttonClose_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            barcode.ScanBarCodeAbort();
        }

 

	}

 
}
