// SampleBarcodeReader.h : main header file for the SAMPLEBARCODEREADER application
//

#if !defined(AFX_SAMPLEBARCODEREADER_H__E9B8D96E_5AAB_4033_81A1_9ACD2C8CB20C__INCLUDED_)
#define AFX_SAMPLEBARCODEREADER_H__E9B8D96E_5AAB_4033_81A1_9ACD2C8CB20C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSampleBarcodeReaderApp:
// See SampleBarcodeReader.cpp for the implementation of this class
//

class CSampleBarcodeReaderApp : public CWinApp
{
public:
	CSampleBarcodeReaderApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSampleBarcodeReaderApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSampleBarcodeReaderApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAMPLEBARCODEREADER_H__E9B8D96E_5AAB_4033_81A1_9ACD2C8CB20C__INCLUDED_)
