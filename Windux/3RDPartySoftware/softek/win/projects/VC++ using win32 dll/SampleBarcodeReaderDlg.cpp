// SampleBarcodeReaderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SampleBarcodeReader.h"
#include "SampleBarcodeReaderDlg.h"

#include "SoftekBarcodeDLL.h"

#include <direct.h>
#include <sys/stat.h>
#include <io.h>

#define DLLDIR	"\\bin"

#ifdef x64
#define DLLNAME	"SoftekBarcode64DLL.dll"
#else
#define DLLNAME	"SoftekBarcodeDLL.dll"
#endif

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
	, m_progress(0)
	, m_bgRead(1)
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
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_PROGRESS1, m_progressBar);
	DDX_Check(pDX, IDC_CHECK_BACKGROUND, m_bgRead);
}

BEGIN_MESSAGE_MAP(CSampleBarcodeReaderDlg, CDialog)
	//{{AFX_MSG_MAP(CSampleBarcodeReaderDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CSampleBarcodeReaderDlg::OnBnClickedOk)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDABORT, &CSampleBarcodeReaderDlg::OnBnClickedAbort)
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
	
	CheckRadioButton(IDC_RADIO_DLL, IDC_RADIO_COM, IDC_RADIO_DLL) ;

	// For the purposes of this demo the project delays the load on the SoftekBarcode.dll file
	// This means that we can load the file that came in with the download rather than a copy that
	// might already be in the windows system folder.
	LoadSoftekBarcodeDLL();

	initBarcode();

	m_progressBar.SetRange(0, 100);

	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSampleBarcodeReaderDlg::LoadSoftekBarcodeDLL()
{
    // For the purposes of this demo we search back up the tree until the SoftekBarcode.dll file is found.
    // If that fails (perhaps this project has been moved) then we assume the dll file is somewhere on the path.
	char curFolder[MAX_PATH] ;
	_getcwd(curFolder, MAX_PATH) ;
	char *pathPtr ;
	size_t pathLen ;
	_dupenv_s(&pathPtr, &pathLen, "PATH") ;
	CStringA Path = curFolder ;
	struct stat fileInfo ;
	struct stat dirInfo ;


	while (!stat(Path, &dirInfo) && stat(Path + DLLDIR + "\\" + DLLNAME, &fileInfo))
	{
		Path += "\\.." ;
	}

	CString errMsg = "Failed to load " ;
	errMsg += DLLNAME ;
	errMsg += " file" ;

	if (stat(Path, &fileInfo))
	{
		if (LoadLibraryA(DLLNAME) == NULL)
			AfxMessageBox(errMsg) ;
	}
	else
	{
		if (LoadLibraryA(Path + DLLDIR + "\\" + DLLNAME) == NULL)
			AfxMessageBox(errMsg);
	}
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
	UpdateData(TRUE) ;
	CString dialogFileTypes = "Image Files (*.tif,*.tiff,*.pdf,*.bmp,*.jpg,*.jpeg,*.gif,*.png)|*.tif;*.tiff;*.pdf;*.bmp;*.jpg;*.jpeg;*.gif|TIF Images (*.tif)|*.tif;*.tiff|PDF Documents (*.pdf)|*.pdf|JPEG Images (*.jpg)|*.jpg|JPEG Images (*.jpeg)|*.jpeg|BMP Images (*.bmp)|*.bmp|GIF Images (*.gif)|*.gif|PNG Images (*.png)|*.png||" ;

	CFileDialog	FileDialog(TRUE, NULL, _T(""), NULL, dialogFileTypes, NULL) ;

	if (FileDialog.DoModal() != IDOK)
	{
		return ;
	}

	m_ImageFile = FileDialog.GetPathName() ;
	UpdateData(FALSE) ;
}

int CSampleBarcodeReaderDlg::ConvertUTF8Value(LPCSTR in, CString &out)
{
	wchar_t str[1000];
	int r = MultiByteToWideChar(CP_UTF8, 0, in, -1, str, 1000);
	out = str;
	return r ;
}

void CSampleBarcodeReaderDlg::initBarcode()
{

	hBarcode = mtCreateBarcodeInstance() ;

    // Enter your license key here
    // You can get a trial license key from sales@bardecode.com
    // Example:
    // mtSetLicenseKey(hBarcode, "MY LICENSE KEY") ;

	// Turn off the barcode types you don't want to read (this will increase the speed of your application)
	mtSetReadCode128 (hBarcode, 1) ;
	mtSetReadCode39 (hBarcode, 1) ;
	mtSetReadCode25 (hBarcode, 1) ;
	mtSetReadEAN13 (hBarcode, 1) ;
	mtSetReadEAN8 (hBarcode, 1) ;
	mtSetReadUPCA (hBarcode, 1) ;
	mtSetReadUPCE (hBarcode, 1) ;
	mtSetReadCodabar (hBarcode, 0) ;
	mtSetReadPDF417 (hBarcode, 1) ;
	mtSetReadDataMatrix (hBarcode, 1) ;
    mtSetReadDatabar (hBarcode, 1);
    mtSetReadMicroPDF417 (hBarcode, 0);
	mtSetReadQRCode(hBarcode, 1);

    // Databar Options is a mask that controls which type of databar barcodes will be read and whether or not
    // the software will look for a quiet zone around the barcode.
    // 1 = 2D-Linkage flag (handle micro-PDf417 barcodes as supplementary data - requires ReadMicroPDF417 to be true).
    // 2 = Read RSS14
    // 4 = Read RSS14 Stacked
    // 8 = Read Limited
    // 16 = Read Expanded
    // 32 = Read Expanded Stacked
    // 64 = Require quiet zone
    mtSetDatabarOptions (hBarcode, 255);

	// If you want to read more than one barcode then set Multiple Read to 1
	// Setting MutlipleRead to False will make the recognition faster
	mtSetMultipleRead(hBarcode, 1) ;

    // If you know the max number of barcodes for a single page then increase speed by setting MaxBarcodesPerPage
    mtSetMaxBarcodesPerPage(hBarcode,0);

    // In certain conditions (MultipleRead = false or MaxBarcodesPerPage = 1) the SDK can make fast scan of an image before performing the normal scan. This is useful if only 1 bar code is expected per page.
    mtSetUseFastScan(hBarcode,1);

	// Noise reduction takes longer but can make it possible to read some difficult barcodes
	// When using noise reduction a typical value is 10 - the smaller the value the more effect it has.
	// A zero value turns off noise reduction. 
	// If you use NoiseReduction then the ScanDirection mask must be either only horizontal or only
	// vertical (i.e 1, 2, 4, 8, 5 or 10).
	// mtSetNoiseReduction (hBarcode, 0) ;

	// You may need to set a small quiet zone if your barcodes are close to text and pictures in the image.
	// A value of zero uses the default.
	mtSetQuietZoneSize (hBarcode, 0) ;

	// LineJump controls the frequency at which scan lines in an image are sampled.
	// The default is 1 - decrease this for difficult barcodes.
	mtSetLineJump (hBarcode, 1) ;

	// You can restrict your search to a particular area of the image if you wish.
	// stSetSetScanRect(TopLeftX, TopLeftY, BottomRightX, BottomRightY, 0)

	// Set the direction that the barcode reader should scan for barcodes
	// The value is a mask where 1 = Left to Right, 2 = Top to Bottom, 3 = Right To Left, 4 = Bottom to Top
	mtSetScanDirection (hBarcode, 15) ;

	// SkewTolerance controls the angle of skew that the barcode toolkit will tolerate. By default
	// the toolkit checks for barcodes along horizontal and vertical lines in an image. This works 
	// OK for most barcodes because even at an angle it is possible to pass a line through the entire
	// length. SkewTolerance can range from 0 to 5 and allows for barcodes skewed to an angle of 45
	// degrees.
	mtSetSkewTolerance (hBarcode, 0) ;

    // Read most skewed linear barcodes without the need to set SkewTolerance. Currently applies to Codabar, Code 25, Code 39 and Code 128 barcodes only.
    mtSetSkewedLinear(hBarcode,1);

    // Read most skewed datamatrix barcodes without the need to set SkewTolerance
    mtSetSkewedDatamatrix(hBarcode,1);


    // ColorProcessingLevel controls how much time the toolkit will searching a color image for a barcode.
    // The default value is 2 and the range of values is 0 to 5. If ColorThreshold is non-zero then 
    // ColorProcessingLevel is effectively set to 0.
    mtSetColorProcessingLevel (hBarcode, 2);

	// MaxLength and MinLength can be used to specify the number of characters you expect to
	// find in a stSet This can be useful to increase accuracy or if you wish to ignore some
	// barcodes in an image.
	mtSetMinLength (hBarcode, 4) ;
	mtSetMaxLength (hBarcode, 999) ;

	// When the toolkit scans an image it records the number of hits it gets for each barcode that
	// MIGHT be in the image. If the hits recorded for any of the barcodes are >= PrefOccurrence
	// then only these barcodes are returned. Otherwise, any barcode whose hits are >= MinOccurrence
	// are reported. If you have a very poor quality image then try setting MinOccurrence to 1, but you
	// may find that some false positive results are returned.
	// mtSetMinOccurrence (hBarcode, 2) ;
	// mtSetPrefOccurrence (hBarcode, 4) ;

	// Read Code 39 barcodes in extended mode
	// mtSetExtendedCode39(hBarcode, 1) ;

	// Barcode string is numeric
	// mtSetReadNumeric(hBarcode, 1) ;

	// Set a regular expression for the barcode
	// mtSetPattern(hBarcode, "^[A-Z]{2}[0-9]{5}$") ;

	// If you are scanning at a high resolution and the spaces between bars are
	// larger than 1 pixel then set MinSpaceBarWidth to 2 and increase your read rate.
	// mtSetMinSpaceBarWidth(hBarcode, 2) ;

	// MedianFilter is a useful way to clean up higher resolution images where the black bars contain white dots
	// and the spaces contain black dots. It does not work if the space between bars is only 1 pixel wide.
	mtSetMedianFilter(hBarcode, 0) ;

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
    mtSetReportUnreadBarcodes(hBarcode,0);

    // Time out for reading a barcode from a page in ms. Note that this does not include the time to load the page.
    // 0 means no time out.
    mtSetTimeOut(hBarcode,5000);

    // Flags for handling PDF files
    // PdfImageOnly defaults to true and indicates that the PDF documents are simple images.
    mtSetPdfImageOnly(hBarcode, 1);

    // PdfImageExtractOptions is no longer required in version 8 of the SDK but is retained for future possible use.
    // mtSetPdfImageExtractOptions(hBarcode, 0);

    // The PdfImageRasterOptions mask controls how images are rasterized when PdfImageOnly is false or when image extraction fails
    // 0 = Use Debenu to render image
    // 4 = Use VeryPDF to render image (x86 only)
    mtSetPdfImageRasterOptions(hBarcode, 0);

    // PdfDpi and PdfBpp control what sort of image the PDF document is rasterized into
    mtSetPdfDpi(hBarcode, 300);
    mtSetPdfBpp(hBarcode, 8);
}

void CSampleBarcodeReaderDlg::ReadDLL()
{
	UpdateData(TRUE);

	m_progressBar.SetPos(0);

	CString msg ;

	msg.Format(_T("Using DLL Interface to read barcodes\r\n")) ;
	WriteResult(msg) ;

	CStringA imageFile = m_ImageFile;

	if (m_bgRead)
	{
		msg.Format(_T("Starting background read\r\n")) ;
		WriteResult(msg) ;
		mtScanBarCodeInBackground(hBarcode, imageFile);
		SetTimer(1, 1, 0);
	}
	else
	{
		// Call ScanBarCode to process image
		nBarCodes = mtScanBarCode(hBarcode, imageFile) ;
		getResults();
	}


}

void CSampleBarcodeReaderDlg::getResults()
{
	CString msg ;


	if (nBarCodes <= -6)
	{
		msg.Format(_T("License key error: either an evaluation key has expired or the license key is not valid for processing pdf documents\r\n")) ;
		WriteResult(msg) ;
		return ;
	}
	else if (nBarCodes < 0)
	{
		msg.Format(_T("ScanBarCode returned error number %d\r\n"), nBarCodes) ;
		WriteResult(msg) ;
		msg.Format(_T("Last Softek Error Number = %d\r\n"), mtGetLastError(hBarcode)) ;
		WriteResult(msg) ;
		msg.Format(_T("Last Windows Error Number = %d\r\n"), mtGetLastWinError(hBarcode)) ;
		WriteResult(msg) ;
		return ;
	}
	else if (nBarCodes == 0)
	{
		msg.Format(_T("No barcode found on this image\r\n")) ;
		WriteResult(msg) ;
		return ;
	}


	msg.Format(_T("Number of barcodes found = %d\r\n"), nBarCodes) ;
	WriteResult(msg) ;

	for (int i = 1; i <= nBarCodes; i++)
	{
		msg.Format(_T("\r\nBarcode: %d:\r\n"), i) ;
		WriteResult(msg) ;

		// Value of barcode

		
		CString strBarCode ;
		// The ConvertUTF8Value function is only needed for barcodes that might contain
		// characters with values > 127. In practise this means Kanji QR-Codes and some
		// PDF-417 barcodes, so for ordinary barcodes it is fine to simply do:
		// strBarCode = mtGetBarString(hBarcode, i);
		ConvertUTF8Value(mtGetBarString(hBarcode, i), strBarCode) ;
		msg.Format(_T("Value = %s\r\n"), strBarCode) ;
		WriteResult(msg) ;

		// Type
		CString strBarCodeType = mtGetBarStringType(hBarcode, i) ;
		msg.Format(_T("Type = %s\r\n"), strBarCodeType) ;
		WriteResult(msg) ;

		// Direction on page
		int nDirection = mtGetBarStringDirection(hBarcode, i) ;
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
		int nPage = mtGetBarStringPos(hBarcode, i, &topLeftX, &topLeftY, &bottomRightX, &bottomRightY) ;
		msg.Format(_T("Page = %d\r\n"), nPage) ;
		WriteResult(msg) ;

		msg.Format(_T("Top Left = (%ld, %ld)\r\n"), topLeftX, topLeftY) ;
		WriteResult(msg) ;
		msg.Format(_T("Bottom Right = (%ld, %ld)\r\n"), bottomRightX, bottomRightY) ;
		WriteResult(msg) ;
	}

}

void CSampleBarcodeReaderDlg::WriteResult(CString msg)
{
	m_Results.SetSel(-1, -1, TRUE) ;
	m_Results.ReplaceSel(msg) ;
}


void CSampleBarcodeReaderDlg::OnBnClickedOk()
{
	m_Results.SetSel(0, -1) ;
	m_Results.ReplaceSel(_T("")) ;

	ReadDLL() ;
	m_Results.SetSel(0,0) ;
}

void CSampleBarcodeReaderDlg::OnTimer(UINT_PTR nIDEvent)
{
	UpdateData(TRUE);
	if (mtScanBarCodeWait(hBarcode, 100) == 1)
	{
		
		m_progressBar.SetPos(mtGetProgress(hBarcode));
		m_Results.SetSel(0, -1) ;
		CString msg ;
		msg.Format(_T("Found %d bar codes so far"), mtGetBarCodeCount(hBarcode));
		m_Results.ReplaceSel(msg) ;
		UpdateData(FALSE);
		return ;
	}
	KillTimer(1);
	m_progressBar.SetPos(mtGetProgress(hBarcode));
	nBarCodes = mtGetScanExitCode(hBarcode);
	getResults();
	CDialog::OnTimer(nIDEvent);
}

void CSampleBarcodeReaderDlg::OnBnClickedAbort()
{
	mtScanBarCodeAbort(hBarcode);
}
