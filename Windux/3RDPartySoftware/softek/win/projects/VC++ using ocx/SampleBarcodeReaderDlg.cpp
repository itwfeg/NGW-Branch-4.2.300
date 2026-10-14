// SampleBarcodeReaderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SampleBarcodeReader.h"
#include "SampleBarcodeReaderDlg.h"

#include <direct.h>
#include <sys/stat.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSampleBarcodeReaderDlg dialog

CSampleBarcodeReaderDlg::CSampleBarcodeReaderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSampleBarcodeReaderDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSampleBarcodeReaderDlg)
	m_ImageFile = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSampleBarcodeReaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSampleBarcodeReaderDlg)
	DDX_Control(pDX, IDC_RESULTS, m_Results);
	DDX_Text(pDX, IDC_FILE, m_ImageFile);
	DDX_Control(pDX, IDC_SOFTEKBARCODECTRL1, m_barcode);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSampleBarcodeReaderDlg, CDialog)
	//{{AFX_MSG_MAP(CSampleBarcodeReaderDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSampleBarcodeReaderDlg message handlers

BOOL CSampleBarcodeReaderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	CheckRadioButton(IDC_RADIO_DLL, IDC_RADIO_COM, IDC_RADIO_DLL) ;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSampleBarcodeReaderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSampleBarcodeReaderDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSampleBarcodeReaderDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CSampleBarcodeReaderDlg::OnBrowse() 
{
	CString dialogFileTypes = "Image Files (*.tif,*.tiff,*.pdf,*.bmp,*.jpg,*.jpeg,*.gif,*.png)|*.tif;*.tiff;*.pdf;*.bmp;*.jpg;*.jpeg;*.gif|TIF Images (*.tif)|*.tif;*.tiff|JPEG Images (*.jpg)|*.jpg|JPEG Images (*.jpeg)|*.jpeg|BMP Images (*.bmp)|*.bmp|GIF Images (*.gif)|*.gif|PNG Images (*.png)|*.png||" ;

	CFileDialog	FileDialog(TRUE, NULL, "", NULL, dialogFileTypes, NULL) ;

	if (FileDialog.DoModal() != IDOK)
	{
		return ;
	}

	m_ImageFile = FileDialog.GetPathName() ;
	UpdateData(FALSE) ;
}

void CSampleBarcodeReaderDlg::OnOK() 
{
	m_Results.SetSel(0, -1) ;
	m_Results.ReplaceSel("") ;

	ReadOCX() ;

	m_Results.SetSel(0,0) ;

}


void CSampleBarcodeReaderDlg::ReadOCX()
{

	// Enter your license key here
	// You can get a trial license key from sales@bardecode.com
	// Example:
	// m_barcode.SetLicenseKey ("MY LICENSE KEY") ;


	// Turn off the barcode types you don't want to read (this will increase the speed of your application)
	m_barcode.SetReadCode128 (1) ;
	m_barcode.SetReadCode39 (1) ;
	m_barcode.SetReadCode25 (1) ;
	m_barcode.SetReadEAN13 (1) ;
	m_barcode.SetReadEAN8 (1) ;
	m_barcode.SetReadUPCA (1) ;
	m_barcode.SetReadUPCE (1) ;
	m_barcode.SetReadCodabar (1) ;
	m_barcode.SetReadPDF417 (1) ;
	m_barcode.SetReadDataMatrix (1) ;
    m_barcode.SetReadDatabar (1);
    m_barcode.SetReadMicroPDF417 (0);
	m_barcode.SetReadQRCode(1);

    // Databar Options is a mask that controls which type of databar barcodes will be read and whether or not
    // the software will look for a quiet zone around the barcode.
    // 1 = 2D-Linkage flag (handle micro-PDf417 barcodes as supplementary data - requires ReadMicroPDF417 to be true).
    // 2 = Read RSS14
    // 4 = Read RSS14 Stacked
    // 8 = Read Limited
    // 16 = Read Expanded
    // 32 = Read Expanded Stacked
    // 64 = Require quiet zone
    m_barcode.SetDatabarOptions (255);

	// If you want to read more than one barcode then set Multiple Read to 1
	// Setting MutlipleRead to False will make the recognition faster
	m_barcode.SetMultipleRead (1) ;

    // If you know the max number of barcodes for a single page then increase speed by setting MaxBarcodesPerPage
    m_barcode.SetMaxBarcodesPerPage(0);

    // In certain conditions (MultipleRead = false or MaxBarcodesPerPage = 1) the SDK can make fast scan of an image before performing the normal scan. This is useful if only 1 bar code is expected per page.
    m_barcode.SetUseFastScan(1);


	// Noise reduction takes longer but can make it possible to read some difficult barcodes
	// When using noise reduction a typical value is 10 - the smaller the value the more effect it has.
	// A zero value turns off noise reduction. 
	// If you use NoiseReduction then the ScanDirection mask must be either only horizontal or only
	// vertical (i.e 1, 2, 4, 8, 5 or 10).
	// m_barcode.SetNoiseReduction = 0 ;

	// You may need to set a small quiet zone if your barcodes are close to text and pictures in the image.
	// A value of zero uses the default.
	m_barcode.SetQuietZoneSize (0) ;

	// LineJump controls the frequency at which scan lines in an image are sampled.
	// The default is 1 - decrease this for difficult barcodes.
	m_barcode.SetLineJump (1) ;

	// You can restrict your search to a particular area of the image if you wish.
	// m_barcode.SetScanRect(TopLeftX, TopLeftY, BottomRightX, BottomRightY, 0)

	// Set the direction that the barcode reader should scan for barcodes
	// The value is a mask where 1 = Left to Right, 2 = Top to Bottom, 3 = Right To Left, 4 = Bottom to Top
	m_barcode.SetScanDirection (15) ;

	// SkewTolerance controls the angle of skew that the barcode toolkit will tolerate. By default
	// the toolkit checks for barcodes along horizontal and vertical lines in an image. This works 
	// OK for most barcodes because even at an angle it is possible to pass a line through the entire
	// length. SkewTolerance can range from 0 to 5 and allows for barcodes skewed to an angle of 45
	// degrees.
	m_barcode.SetSkewTolerance (0) ;

    // Read most skewed linear barcodes without the need to set SkewTolerance. Currently applies to Codabar, Code 25, Code 39 and Code 128 barcodes only.
    m_barcode.SetSkewedLinear(1);

    // Read most skewed datamatrix barcodes without the need to set SkewTolerance
    m_barcode.SetSkewedDatamatrix(1);


    // ColorProcessingLevel controls how much time the toolkit will searching a color image for a barcode.
    // The default value is 2 and the range of values is 0 to 5. If ColorThreshold is non-zero then 
    // ColorProcessingLevel is effectively set to 0.
    m_barcode.SetColorProcessingLevel (2);

	// MaxLength and MinLength can be used to specify the number of characters you expect to
	// find in a m_barcode. This can be useful to increase accuracy or if you wish to ignore some
	// barcodes in an image.
	m_barcode.SetMinLength (4) ;
	m_barcode.SetMaxLength (999) ;

	// When the toolkit scans an image it records the number of hits it gets for each barcode that
	// MIGHT be in the image. If the hits recorded for any of the barcodes are >= PrefOccurrence
	// then only these barcodes are returned. Otherwise, any barcode whose hits are >= MinOccurrence
	// are reported. If you have a very poor quality image then try setting MinOccurrence to 1, but you
	// may find that some false positive results are returned.
	// m_barcode.SetMinOccurrence (2) ;
	// m_barcode.SetPrefOccurrence (4) ;

	// Read Code 39 barcodes in extended mode
	// m_barcode.SetExtendedCode39(1) ;

	// Barcode string is numeric
	// m_barcode.SetReadNumeric(1) ;

	// Set a regular expression for the barcode
	// m_barcode.SetPattern("^[A-Z]{2}[0-9]{5}$") ;

	// If you are scanning at a high resolution and the spaces between bars are
	// larger than 1 pixel then set MinSpaceBarWidth to 2 and increase your read rate.
	// m_barcode.SetMinSpaceBarWidth(2) ;

	// MedianFilter is a useful way to clean up higher resolution images where the black bars contain white dots
	// and the spaces contain black dots. It does not work if the space between bars is only 1 pixel wide.
	m_barcode.SetMedianFilter(FALSE) ;

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
    m_barcode.SetReportUnreadBarcodes(0);

    // Time out for reading a barcode from a page in ms. Note that this does not include the time to load the page.
    // 0 means no time out.
    m_barcode.SetTimeOut(5000);


    // Flags for handling PDF files
    // PdfImageOnly defaults to true and indicates that the PDF documents are simple images.
    m_barcode.SetPdfImageOnly(TRUE);

    // PdfImageExtractOptions is no longer required in version 8 of the SDK but is retained for future possible use.
    // m_barcode.SetPdfImageExtractOptions(0);

    // The PdfImageRasterOptions mask controls how images are rasterized when PdfImageOnly is false or when image extraction fails
    // 0 = Use Debenu to render image
    // 4 = Use VeryPDF to render image (x86 only)
    m_barcode.SetPdfImageRasterOptions(0);

    // PdfDpi and PdfBpp control what sort of image the PDF document is rasterized into
    m_barcode.SetPdfDpi(300) ;
    m_barcode.SetPdfBpp(8) ; 

	CString msg ;

	msg.Format("Using OCX Interface to read barcodes\r\n") ;
	WriteResult(msg) ;

	
	// Call ScanBarCode to process image
	int nBarCodes = m_barcode.ScanBarCode(m_ImageFile) ;

	if (nBarCodes <= -6)
	{
		msg.Format("License key error: either an evaluation key has expired or the license key is not valid for processing pdf documents\r\n") ;
		WriteResult(msg) ;
		return ;
	}
	else if (nBarCodes < 0)
	{
		msg.Format("ScanBarCode returned error number %d\r\n", nBarCodes) ;
		WriteResult(msg) ;
		msg.Format("Last Softek Error Number = %d\r\n", m_barcode.GetLastError()) ;
		WriteResult(msg) ;
		msg.Format("Last Windows Error Number = %d\r\n", m_barcode.GetLastWinError()) ;
		WriteResult(msg) ;
		return ;
	}
	else if (nBarCodes == 0)
	{
		msg.Format("No barcode found on this image\r\n") ;
		WriteResult(msg) ;
		return ;
	}

	msg.Format("Number of barcodes found = %d\r\n", nBarCodes) ;
	WriteResult(msg) ;

	for (int i = 1; i <= nBarCodes; i++)
	{
		msg.Format("\r\nBarcode: %d:\r\n", i) ;
		WriteResult(msg) ;

		// Value of barcode
		CString strBarCode = m_barcode.GetBarString(i) ;
		msg.Format("Value = %s\r\n", strBarCode) ;
		WriteResult(msg) ;

		// Type
		CString strBarCodeType = m_barcode.GetBarStringType(i) ;
		msg.Format("Type = %s\r\n", strBarCodeType) ;
		WriteResult(msg) ;

		// Direction on page
		int nDirection = m_barcode.GetBarStringDirection(i) ;
		msg = "Direction = " ;
		switch (nDirection)
		{
		case 1:
			msg += "Left to Right" ;
			break ;
		case 2:
			msg += "Top to Bottom" ;
			break ;
		case 4:
			msg += "Right to Left" ;
			break ;
		case 8:
			msg += "Bottom to Top" ;
			break ;
		default:
			break ;
		}
		msg += "\r\n" ;
		WriteResult(msg) ;

		// Page number and coordinates
		long topLeftX, topLeftY, bottomRightX, bottomRightY ;
		int nPage = m_barcode.GetBarStringPos(i, &topLeftX, &topLeftY, &bottomRightX, &bottomRightY) ;
		msg.Format("Page = %d\r\n", nPage) ;
		WriteResult(msg) ;

		msg.Format("Top Left = (%ld, %ld)\r\n", topLeftX, topLeftY) ;
		WriteResult(msg) ;
		msg.Format("Bottom Right = (%ld, %ld)\r\n", bottomRightX, bottomRightY) ;
		WriteResult(msg) ;
	}
}

void CSampleBarcodeReaderDlg::WriteResult(CString msg)
{
	m_Results.SetSel(-1, -1, TRUE) ;
	m_Results.ReplaceSel(msg) ;
}

