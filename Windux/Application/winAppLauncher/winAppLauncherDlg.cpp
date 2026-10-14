// winAppLauncherDlg.cpp : implementation file
//

#include "stdafx.h"
#include "winAppLauncher.h"
#include "winAppLauncherDlg.h"
#include "fbwfapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CwinAppLauncherDlg dialog




CwinAppLauncherDlg::CwinAppLauncherDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CwinAppLauncherDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CwinAppLauncherDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CwinAppLauncherDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()




BOOL CwinAppLauncherDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_wBrush.CreateSolidBrush(RGB(255,255,255));
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	int cxscreen = GetSystemMetrics(SM_CXSCREEN);
	int cyscreen = GetSystemMetrics(SM_CYSCREEN);

	MoveWindow(0, 0,cxscreen,cyscreen);

	
	CStatic * pPicture = (CStatic *)GetDlgItem(IDC_PICTURE);
	{
		if (!CwinAppLauncherApp::isFBWFEnabled())
		{
			HBITMAP bm = ::LoadBitmap(AfxGetResourceHandle(),MAKEINTRESOURCE(IDB_BITMAP_SERVICE_MODE));
			pPicture->SetBitmap(bm);

		}

	}

	pPicture->CenterWindow();



	RECT hobRect;
	pPicture->GetWindowRect(&hobRect);


	CProgressCtrl * pProgWebServ = (CProgressCtrl *)GetDlgItem(IDC_PROGRESS_WEBSERVER);

	RECT origProgRect1, newProgRect1;
	pProgWebServ->GetWindowRect(&origProgRect1);

	newProgRect1.left = hobRect.left;
	newProgRect1.top = hobRect.bottom + 20;
	newProgRect1.right = newProgRect1.left + (origProgRect1.right - origProgRect1.left);
	newProgRect1.bottom = newProgRect1.top + (origProgRect1.bottom - origProgRect1.top);

	pProgWebServ->MoveWindow(&newProgRect1,0);
	pProgWebServ->SetRange((short)0, (short) PROGRESS_MAX_VALUE);
	pProgWebServ->SetPos(0);
	pProgWebServ->SetStep(1);

	CProgressCtrl * pProgScaleServ = (CProgressCtrl *)GetDlgItem(IDC_PROGRESS_SCALESERVICE);
	RECT origProgRect2, newProgRect2;
	pProgScaleServ->GetWindowRect(&origProgRect2);

	newProgRect2.left = newProgRect1.left;
	newProgRect2.top = newProgRect1.bottom + 20;
	newProgRect2.right = newProgRect2.left + (origProgRect2.right - origProgRect2.left);
	newProgRect2.bottom = newProgRect2.top + (origProgRect2.bottom - origProgRect2.top);

	pProgScaleServ->MoveWindow(&newProgRect2,0);

	pProgScaleServ->SetRange((short)0, (short) PROGRESS_MAX_VALUE);
	pProgScaleServ->SetPos(0);
	pProgScaleServ->SetStep(1);

	CProgressCtrl * pProgScaleUI = (CProgressCtrl *)GetDlgItem(IDC_PROGRESS_SCALEUI);
	RECT origProgRect3, newProgRect3;
	pProgScaleUI->GetWindowRect(&origProgRect3);

	newProgRect3.left = newProgRect2.left;
	newProgRect3.top = newProgRect2.bottom + 20;
	newProgRect3.right = newProgRect3.left + (origProgRect3.right - origProgRect3.left);
	newProgRect3.bottom = newProgRect3.top + (origProgRect3.bottom - origProgRect3.top);

	pProgScaleUI->MoveWindow(&newProgRect3,0);

	pProgScaleUI->SetRange((short)0, (short) SCALEUI_MAX_DELAY);
	pProgScaleUI->SetPos(0);
	pProgScaleUI->SetStep(1);

	CWnd * pStaticWebServ = GetDlgItem(IDC_TEXT_WEBSERVER);
	RECT origProgRect4, newProgRect4;
	pStaticWebServ->GetWindowRect(&origProgRect4);

	newProgRect4.left = newProgRect1.right + 20;
	newProgRect4.top = newProgRect1.top;
	newProgRect4.right = newProgRect4.left + (origProgRect4.right - origProgRect4.left);
	newProgRect4.bottom = newProgRect4.top + (origProgRect4.bottom - origProgRect4.top);

	pStaticWebServ->MoveWindow(&newProgRect4,0);

	CWnd * pStaticScaleServ = GetDlgItem(IDC_TEXT_SCALESERVICE);
	RECT origProgRect5, newProgRect5;
	pStaticScaleServ->GetWindowRect(&origProgRect5);

	newProgRect5.left = newProgRect2.right + 20;
	newProgRect5.top = newProgRect2.top;
	newProgRect5.right = newProgRect5.left + (origProgRect5.right - origProgRect5.left);
	newProgRect5.bottom = newProgRect5.top + (origProgRect5.bottom - origProgRect5.top);

	pStaticScaleServ->MoveWindow(&newProgRect5,0);

	CWnd * pStaticScaleUI = GetDlgItem(IDC_TEXT_SCALEUI);
	RECT origProgRect6, newProgRect6;
	pStaticScaleUI->GetWindowRect(&origProgRect6);

	newProgRect6.left = newProgRect3.right + 20;
	newProgRect6.top = newProgRect3.top;
	newProgRect6.right = newProgRect6.left + (origProgRect6.right - origProgRect6.left);
	newProgRect6.bottom = newProgRect6.top + (origProgRect6.bottom - origProgRect6.top);

	pStaticScaleUI->MoveWindow(&newProgRect6,0);


	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CwinAppLauncherDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CwinAppLauncherDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


HBRUSH CwinAppLauncherDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
//	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here

	// TODO:  Return a different brush if the default is not desired
	return m_wBrush;
}
