// Header file included by ANTLR UniLexer
#include <tchar.h>
// UnitTesterDlg.h : header file
//

#if !defined(AFX_UNITTESTERDLG_H__97CDB546_328F_11D2_B97C_00805FA39725__INCLUDED_)
#define AFX_UNITTESTERDLG_H__97CDB546_328F_11D2_B97C_00805FA39725__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Test.h"

/////////////////////////////////////////////////////////////////////////////
// CUnitTesterDlg dialog

class CUnitTesterDlg : public CDialog
{
    // Construction
    public:
        CUnitTesterDlg(CWnd* pParent = NULL);   // standard constructor
        ~CUnitTesterDlg();

        // Dialog Data
        //{{AFX_DATA(CUnitTesterDlg)
        enum { IDD = IDD_UNITTESTER_DIALOG };
        CEdit   m_Tester;
        CEdit   m_Version;
        CEdit   m_RunComment;
        CEdit   m_OutputFileName;
        CEdit   m_SelectedTestName;
        CEdit   m_Description;
        CListBox        m_TestsList;
        //}}AFX_DATA

        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CUnitTesterDlg)
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV support
        //}}AFX_VIRTUAL

        Test::CUnitTestManager m_Manager;

        // Implementation
    protected:
        HICON m_hIcon;

        // Generated message map functions
        //{{AFX_MSG(CUnitTesterDlg)
        virtual BOOL OnInitDialog();
        afx_msg void OnPaint();
        afx_msg HCURSOR OnQueryDragIcon();
        afx_msg void OnRuntests();
        afx_msg void OnRunLTtests();
        afx_msg void OnSelchangeTestsList();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UNITTESTERDLG_H__97CDB546_328F_11D2_B97C_00805FA39725__INCLUDED_)
