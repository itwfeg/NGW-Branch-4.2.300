// SampleBarcodeReaderDlg.h : header file
//
//{{AFX_INCLUDES()
//}}AFX_INCLUDES

#include "afxcmn.h"
#include "afxwin.h"
#if !defined(AFX_SAMPLEBARCODEREADERDLG_H__99EA1BEF_AFB1_4B08_ABD9_D1A8045A2680__INCLUDED_)
#define AFX_SAMPLEBARCODEREADERDLG_H__99EA1BEF_AFB1_4B08_ABD9_D1A8045A2680__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CSampleBarcodeReaderDlg dialog

class CSampleBarcodeReaderDlg : public CDialog
{
// Construction
public:
	CSampleBarcodeReaderDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSampleBarcodeReaderDlg)
	enum { IDD = IDD_SAMPLEBARCODEREADER_DIALOG };
	CEdit	m_Results;
	CString	m_ImageFile;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSampleBarcodeReaderDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSampleBarcodeReaderDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void ReadDLL() ;
	void LoadSoftekBarcodeDLL() ;
	void WriteResult(CString msg) ;
	int ConvertUTF8Value(LPCSTR in, CString &out);
	HANDLE hBarcode ;
	void initBarcode();
	void getResults();
	int nBarCodes ;


public:
	afx_msg void OnBnClickedOk();
	CProgressCtrl m_progressBar;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	int m_progress;
	int m_bgRead;
	afx_msg void OnBnClickedAbort();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAMPLEBARCODEREADERDLG_H__99EA1BEF_AFB1_4B08_ABD9_D1A8045A2680__INCLUDED_)
