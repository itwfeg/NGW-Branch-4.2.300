// Header file included by ANTLR UniLexer
#include <tchar.h>
// UnitTester.cpp : Defines the class behaviors for the application.
//
#include "stdafx.h"
#include "UnitTester.h"
#include "UnitTesterDlg.h"
#include <crtdbg.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUnitTesterApp

BEGIN_MESSAGE_MAP(CUnitTesterApp, CWinApp)
    //{{AFX_MSG_MAP(CUnitTesterApp)
    //}}AFX_MSG
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CUnitTesterApp construction

CUnitTesterApp::CUnitTesterApp()
{
    // 
    //_CrtSetBreakAlloc(62);

}

/////////////////////////////////////////////////////////////////////////////
// The one and only CUnitTesterApp object

CUnitTesterApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CUnitTesterApp initialization

BOOL CUnitTesterApp::InitInstance()
{
    // Standard initialization

#ifdef _AFXDLL
    Enable3dControls();                 // Call this when using MFC in a shared DLL
#else
    Enable3dControlsStatic();   // Call this when linking to MFC statically
#endif

    CUnitTesterDlg dlg;
    m_pMainWnd = &dlg;
    int nResponse = dlg.DoModal();
    if (nResponse == IDOK)
    {
    }
    else if (nResponse == IDCANCEL)
    {
    }

    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.
    return FALSE;
}
