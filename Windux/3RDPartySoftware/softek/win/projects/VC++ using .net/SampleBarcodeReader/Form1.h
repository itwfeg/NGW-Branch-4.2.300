#pragma once


namespace SampleBarcodeReader {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Timers;

	// Class used to carry a fixed parameter to the event handler - in this case a ref to the form
	public ref class MyTimer : System::Timers::Timer
	{
	public:
		MyTimer(double interval)
		{
		}

		Object^ f ;
	};

	/// <summary>
	/// Summary for Form1
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
		SoftekBarcodeNet::BarcodeReader ^barcode ;
		MyTimer^ aTimer;
	private: System::Windows::Forms::ProgressBar^  progressBar1;
	internal: System::Windows::Forms::CheckBox^  CheckBoxbackground;
	private: 
		int nBarCodes ;
		delegate void SetTextDelegate(String^ text);
		delegate void SetProgressDelegate(int value);
		delegate void SetButtonDelegate(bool enabled);
		

	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//

			initBarcode();


			aTimer = gcnew MyTimer(100);
			// Pass "this" into the timer to be picked up by the event handler
			aTimer->f = this ;
			aTimer->Elapsed += gcnew ElapsedEventHandler (Form1::checkReadStatic );
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			barcode->~BarcodeReader() ;
			if (components)
			{
				delete components;
			}
		}
	internal: System::Windows::Forms::CheckBox^  CheckBitmap;
	protected: 
	internal: System::Windows::Forms::Label^  Label1;
	internal: System::Windows::Forms::Button^  Button1;
	internal: System::Windows::Forms::OpenFileDialog^  OpenFileDialog1;
	internal: System::Windows::Forms::TextBox^  Results;
	internal: System::Windows::Forms::TextBox^  ImageFile;
	internal: System::Windows::Forms::Button^  ReadBarcode;
	internal: System::Windows::Forms::Button^  ImageBtn;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->CheckBitmap = (gcnew System::Windows::Forms::CheckBox());
			this->Label1 = (gcnew System::Windows::Forms::Label());
			this->Button1 = (gcnew System::Windows::Forms::Button());
			this->OpenFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->Results = (gcnew System::Windows::Forms::TextBox());
			this->ImageFile = (gcnew System::Windows::Forms::TextBox());
			this->ReadBarcode = (gcnew System::Windows::Forms::Button());
			this->ImageBtn = (gcnew System::Windows::Forms::Button());
			this->progressBar1 = (gcnew System::Windows::Forms::ProgressBar());
			this->CheckBoxbackground = (gcnew System::Windows::Forms::CheckBox());
			this->SuspendLayout();
			// 
			// CheckBitmap
			// 
			this->CheckBitmap->AutoSize = true;
			this->CheckBitmap->Location = System::Drawing::Point(10, 286);
			this->CheckBitmap->Name = L"CheckBitmap";
			this->CheckBitmap->Size = System::Drawing::Size(389, 17);
			this->CheckBitmap->TabIndex = 19;
			this->CheckBitmap->Text = L"Load page 1 of image into a Bitmap object and call ScanBarCodeFromBitmap";
			this->CheckBitmap->UseVisualStyleBackColor = true;
			this->CheckBitmap->CheckedChanged += gcnew System::EventHandler(this, &Form1::CheckBitmap_CheckedChanged);
			// 
			// Label1
			// 
			this->Label1->AutoSize = true;
			this->Label1->Location = System::Drawing::Point(10, 12);
			this->Label1->Name = L"Label1";
			this->Label1->Size = System::Drawing::Size(23, 13);
			this->Label1->TabIndex = 15;
			this->Label1->Text = L"File";
			this->Label1->Click += gcnew System::EventHandler(this, &Form1::Label1_Click);
			// 
			// Button1
			// 
			this->Button1->Location = System::Drawing::Point(360, 306);
			this->Button1->Name = L"Button1";
			this->Button1->Size = System::Drawing::Size(96, 23);
			this->Button1->TabIndex = 14;
			this->Button1->Text = L"Close";
			this->Button1->UseVisualStyleBackColor = true;
			this->Button1->Click += gcnew System::EventHandler(this, &Form1::Button1_Click);
			// 
			// OpenFileDialog1
			// 
			this->OpenFileDialog1->DefaultExt = L"tif";
			this->OpenFileDialog1->Filter = L"Images (*.tif,*.pdf,*.bmp,*.jpg)|*.tif;*.pdf;*.jpg;*.tiff;*.bmp;*.jpeg";
			this->OpenFileDialog1->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &Form1::OpenFileDialog1_FileOk);
			// 
			// Results
			// 
			this->Results->Location = System::Drawing::Point(10, 68);
			this->Results->Multiline = true;
			this->Results->Name = L"Results";
			this->Results->ReadOnly = true;
			this->Results->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->Results->Size = System::Drawing::Size(569, 202);
			this->Results->TabIndex = 13;
			this->Results->TextChanged += gcnew System::EventHandler(this, &Form1::Results_TextChanged);
			// 
			// ImageFile
			// 
			this->ImageFile->Location = System::Drawing::Point(112, 12);
			this->ImageFile->Name = L"ImageFile";
			this->ImageFile->ReadOnly = true;
			this->ImageFile->Size = System::Drawing::Size(344, 20);
			this->ImageFile->TabIndex = 10;
			this->ImageFile->TextChanged += gcnew System::EventHandler(this, &Form1::ImageFile_TextChanged);
			// 
			// ReadBarcode
			// 
			this->ReadBarcode->Location = System::Drawing::Point(477, 306);
			this->ReadBarcode->Name = L"ReadBarcode";
			this->ReadBarcode->Size = System::Drawing::Size(102, 24);
			this->ReadBarcode->TabIndex = 12;
			this->ReadBarcode->Text = L"Read";
			this->ReadBarcode->Click += gcnew System::EventHandler(this, &Form1::ReadBarcode_Click);
			// 
			// ImageBtn
			// 
			this->ImageBtn->Location = System::Drawing::Point(477, 10);
			this->ImageBtn->Name = L"ImageBtn";
			this->ImageBtn->Size = System::Drawing::Size(102, 23);
			this->ImageBtn->TabIndex = 11;
			this->ImageBtn->Text = L"Browse";
			this->ImageBtn->Click += gcnew System::EventHandler(this, &Form1::ImageBtn_Click);
			// 
			// progressBar1
			// 
			this->progressBar1->Location = System::Drawing::Point(10, 39);
			this->progressBar1->Name = L"progressBar1";
			this->progressBar1->Size = System::Drawing::Size(568, 23);
			this->progressBar1->TabIndex = 20;
			// 
			// CheckBoxbackground
			// 
			this->CheckBoxbackground->AutoSize = true;
			this->CheckBoxbackground->Checked = true;
			this->CheckBoxbackground->CheckState = System::Windows::Forms::CheckState::Checked;
			this->CheckBoxbackground->Location = System::Drawing::Point(10, 310);
			this->CheckBoxbackground->Name = L"CheckBoxbackground";
			this->CheckBoxbackground->Size = System::Drawing::Size(159, 17);
			this->CheckBoxbackground->TabIndex = 21;
			this->CheckBoxbackground->Text = L"Launch scan in background";
			this->CheckBoxbackground->UseVisualStyleBackColor = true;
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(590, 341);
			this->Controls->Add(this->CheckBoxbackground);
			this->Controls->Add(this->progressBar1);
			this->Controls->Add(this->CheckBitmap);
			this->Controls->Add(this->Label1);
			this->Controls->Add(this->Button1);
			this->Controls->Add(this->Results);
			this->Controls->Add(this->ImageFile);
			this->Controls->Add(this->ReadBarcode);
			this->Controls->Add(this->ImageBtn);
			this->Name = L"Form1";
			this->Text = L"Form1";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

    private:
		// Handles call back 
		static void checkReadStatic(System::Object^ sender, ElapsedEventArgs^ e)
        {
			((Form1 ^) (((MyTimer^) sender)->f))->checkRead();
        }

		void checkRead()
		{
            if (barcode->ScanBarCodeWait(0) == 1)
            {
                SetProgress(barcode->GetProgress());
                return;
            }
            SetProgress(barcode->GetProgress());
            aTimer->Stop();
            nBarCodes = barcode->GetScanExitCode();
            getResults();
            SetButton(true);

		}

		void SetText(String^ text)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (this->Results->InvokeRequired)
            {
                SetTextDelegate^ d = 
                    gcnew SetTextDelegate(this, &Form1::SetText);
                this->Invoke(d, gcnew array<Object^> { text });
            }
            else
            {
                this->Results->Text += text;
            }
        }

        void SetProgress(int value)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
			if (this->progressBar1->InvokeRequired)
            {
                SetProgressDelegate^ d = 
                    gcnew SetProgressDelegate(this, &Form1::SetProgress);
                this->Invoke(d, gcnew array<Object^> { value });
            }
            else
            {
				this->progressBar1->Value = value ;
            }
        }

        void SetButton(bool enabled)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
			if (this->ReadBarcode->InvokeRequired)
            {
                SetButtonDelegate^ d = 
                    gcnew SetButtonDelegate(this, &Form1::SetButton);
                this->Invoke(d, gcnew array<Object^> { enabled });
            }
            else
            {
				this->ReadBarcode->Enabled = enabled ;
            }
		}


		void initBarcode()
		{
			// In this project we give the path to the Softek DLL files when we create the BarcodeReader class
			// Another way is to include the following files with your project:
			// SoftekBarcodeDLL.dll
			// SoftekBarcode64DLL.dll
			// DebenuPDFLibraryDLLXXXX.dll
			// DebenuPDFLibrary64DLLXXXX.dll
			barcode = gcnew SoftekBarcodeNet::BarcodeReader("..\\..\\..");

			// Enter your license key here
			// You can get a trial license key from sales@bardecode.com
			// Example:
			// barcode->LicenseKey = "MY LICENSE KEY";

			// Turn on the barcode types you want to read.
			// Turn off the barcode types you don't want to read (this will increase the speed of your application)
			barcode->ReadCode128 = true ;
			barcode->ReadCode39 = true ;
			barcode->ReadCode25 = true ;
			barcode->ReadEAN13 = true ;
			barcode->ReadEAN8 = true ;
			barcode->ReadUPCA = true ;
			barcode->ReadUPCE = true ;
			barcode->ReadCodabar = false ;
			barcode->ReadPDF417 = true ;
			barcode->ReadDataMatrix = true ;
			barcode->ReadDatabar = true;
			barcode->ReadMicroPDF417 = false;
			barcode->ReadQRCode = true;

			// Databar Options is a mask that controls which type of databar barcodes will be read and whether or not
			// the software will look for a quiet zone around the barcode.
			// 1 = 2D-Linkage flag (handle micro-PDf417 barcodes as supplementary data - requires ReadMicroPDF417 to be true).
			// 2 = Read RSS14
			// 4 = Read RSS14 Stacked
			// 8 = Read Limited
			// 16 = Read Expanded
			// 32 = Read Expanded Stacked
			// 64 = Require quiet zone
			barcode->DatabarOptions = 255;

			// If you want to read more than one barcode then set Multiple Read to 1
			// Setting MutlipleRead to False will make the recognition faster
			barcode->MultipleRead = true ;

			// If you know the max number of barcodes for a single page then increase speed by setting MaxBarcodesPerPage
			barcode->MaxBarcodesPerPage = 0;

			// In certain conditions (MultipleRead = false or MaxBarcodesPerPage = 1) the SDK can make fast scan of an image before performing the normal scan. This is useful if only 1 bar code is expected per page.
			barcode->UseFastScan = true;



			// Noise reduction takes longer but can make it possible to read some difficult barcodes
			// When using noise reduction a typical value is 10 - the smaller the value the more effect it has.
			// A zero value turns off noise reduction.
			// barcode->NoiseReduction = 0 ;

			// You may need to set a small quiet zone if your barcodes are close to text and pictures in the image.
			// A value of zero uses the default.
			barcode->QuietZoneSize = 0 ;

			// LineJump controls the frequency at which scan lines in an image are sampled.
			// The default is 1.
			barcode->LineJump = 1 ;

			// You can restrict your search to a particular area of the image if you wish.
			// In this example the scan area is restricted to the top half of the page
			// System::Drawing::Rectangle *scanArea = new System::Drawing::Rectangle(0, 0, 100, 50);
			// barcode->SetScanRect(*scanArea, 1);

			// Set the direction that the barcode reader should scan for barcodes
			// The value is a mask where 1 = Left to Right, 2 = Top to Bottom, 3 = Right To Left, 4 = Bottom to Top
			barcode->ScanDirection = 15 ;

			// Set the page number to read from in a multi-page TIF file. The default is 0, which will make the
			// toolkit check every page.
			// barcode->PageNo = 1 ;

			// SkewTolerance controls the angle of skew that the barcode toolkit will tolerate. By default
			// the toolkit checks for barcodes along horizontal and vertical lines in an image. This works
			// OK for most barcodes because even at an angle it is possible to pass a line through the entire
			// length. SkewTolerance can range from 0 to 5 and allows for barcodes skewed to an angle of 45
			// degrees.
			barcode->SkewTolerance = 0 ;

			// Read most skewed linear barcodes without the need to set SkewTolerance. Currently applies to Codabar, Code 25, Code 39 and Code 128 barcodes only.
			barcode->SkewedLinear = true;

			// Read most skewed datamatrix barcodes without the need to set SkewTolerance
			barcode->SkewedDatamatrix = true;



			// ColorProcessingLevel controls how much time the toolkit will searching a color image for a barcode.
			// The default value is 2 and the range of values is 0 to 5. If ColorThreshold is non-zero then 
			// ColorProcessingLevel is effectively set to 0.
			barcode->ColorProcessingLevel = 2;

			// MaxLength and MinLength can be used to specify the number of characters you expect to
			// find in a barcode. This can be useful to increase accuracy or if you wish to ignore some
			// barcodes in an image.
			barcode->MinLength = 4 ;
			barcode->MaxLength = 999 ;

			// When the toolkit scans an image it records the number of hits it gets for each barcode that
			// MIGHT be in the image. If the hits recorded for any of the barcodes are >= PrefOccurrence
			// then only these barcodes are returned. Otherwise, any barcode whose hits are >= MinOccurrence
			// are reported. If you have a very poor quality image then try setting MinOccurrence to 1, but you
			// may find that some false positive results are returned.
			//barcode->MinOccurrence = 2 ;
			//barcode->PrefOccurrence = 4 ;

			// Read Code 39 barcodes in extended mode
			// barcode->ExtendedCode39 = true ;

			// Barcode string is numeric
			// barcode.ReadNumeric = true ;

			// Set a regular expression for the barcode
			// barcode->Pattern = "^[A-Z]{2}[0-9]{5}$" ;

			// If you are scanning at a high resolution and the spaces between bars are
			// larger than 1 pixel then set MinSpaceBarWidth to 2 and increase your read rate.
			// barcode->MinSpaceBarWidth = 2 ;

			// MedianFilter is a useful way to clean up higher resolution images where the black bars contain white dots
			// and the spaces contain black dots. It does not work if the space between bars is only 1 pixel wide.
			barcode->MedianFilter = false ;	

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
			barcode->ReportUnreadBarcodes = 0;

			// Time out for reading a barcode from a page in ms. Note that this does not include the time to load the page.
			// 0 means no time out.
			barcode->TimeOut = 5000;


			// Flags for handling PDF files
			// PdfImageOnly defaults to true and indicates that the PDF documents are simple images.
			barcode->PdfImageOnly = true;

            // PdfImageExtractOptions is no longer required in version 8 of the SDK but is retained for future possible use.
            // barcode->PdfImageExtractOptions = 0;

            // The PdfImageRasterOptions mask controls how images are rasterized when PdfImageOnly is false or when image extraction fails
            // 0 = Use Debenu to render image
            // 4 = Use VeryPDF to render image (x86 only)
            barcode->PdfImageRasterOptions = 0;

			// PdfDpi and PdfBpp control what sort of image the PDF document is rasterized into
			barcode->PdfDpi = 300 ;
			barcode->PdfBpp = 8 ; 
		}

		void getResults()
		{
			if (nBarCodes <= -6)
			{
				SetText("License key error: either an evaluation key has expired or the license key is not valid for processing pdf documents") ;
			}
			else if (nBarCodes < 0)
			{
				SetText(String::Format("ScanBarCode returned error number {0}\r\n", nBarCodes)) ;
				SetText(String::Format("Last Softek Error Number =  {0}\r\n", barcode->GetLastError())) ;
				SetText(String::Format("Last Windows Error Number =  {0}\r\n", barcode->GetLastWinError())) ;
			}
			else if (nBarCodes == 0)
			{
				SetText("No barcode found on this image") ;
			}
			else
			{
				for (int i = 1; i <= nBarCodes; i++) {
					SetText(String::Format("Barcode {0}:\r\n", i)) ;
					SetText(String::Format("Value = {0}\r\n", barcode->GetBarString(i))) ;
					SetText(String::Format("Type = {0}\r\n", barcode->GetBarStringType(i))) ;

					SetText("Direction = ") ;
					switch (barcode->GetBarStringDirection(i))
					{
					case 1:
						SetText("Left to Right") ;
						break ;
					case 2:
						SetText("Top to Bottom") ;
						break ;
					case 4:
						SetText("Right to Left") ;
						break ;
					case 8:
						SetText("Bottom to Top") ;
						break ;
					}
					SetText("\r\n") ;

					SetText(String::Format("Page = {0}\r\n", barcode->GetBarStringPage(i))) ;
					System::Drawing::Rectangle rect = barcode->GetBarStringRect(i) ;
					SetText(String::Format("Top Left = ({0},{1})\r\n", rect.X, rect.Y)) ;
					SetText(String::Format("Bottom Right = ({0},{1})\r\n\r\n", rect.X + rect.Width, rect.Y + rect.Height)) ;
				}
			}
	}

	System::Void ReadBarcode_Click(System::Object^  sender, System::EventArgs^  e) {

			Results->Text = "" ;
			SetText("Using .Net Interface to read barcodes\r\n");
			SetProgress(0);

			if (CheckBitmap->Checked) {
				SetText("Loading image into a bitmap and calling ScanBarCodeFromBitmap\r\n") ;
				System::Drawing::Bitmap ^bm = gcnew System::Drawing::Bitmap(ImageFile->Text) ;
				nBarCodes = barcode->ScanBarCodeFromBitmap(bm) ;
				bm->~Bitmap() ;
				getResults();
			} else {
				if (CheckBoxbackground->Checked)
				{
					SetButton(false);
					barcode->ScanBarCodeInBackground(ImageFile->Text) ;
					aTimer->Start();
				}
				else
				{
					nBarCodes = barcode->ScanBarCode(ImageFile->Text) ;
					getResults();
				}
			}
		}



private: System::Void ImageBtn_Click(System::Object^  sender, System::EventArgs^  e) {
			OpenFileDialog1->ShowDialog() ;
			ImageFile->Text = OpenFileDialog1->FileName ;
		 }
private: System::Void Button1_Click(System::Object^  sender, System::EventArgs^  e) {
			 this->Close() ;
		 }
private: System::Void Label1_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void OpenFileDialog1_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) {
		 }
private: System::Void Results_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void ImageFile_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void CheckBitmap_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
};



}

