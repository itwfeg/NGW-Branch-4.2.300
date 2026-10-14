// winAppLauncherDlg.h : header file
//

#pragma once

#define PROGRESS_MAX_VALUE 10
#define SCALEUI_MAX_DELAY 60
// CwinAppLauncherDlg dialog
class CwinAppLauncherDlg : public CDialog
{
// Construction
public:
	CwinAppLauncherDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_WINAPPLAUNCHER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	CBrush m_wBrush;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
