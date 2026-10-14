// Header file included by ANTLR UniLexer
#include <tchar.h>
// UnitTesterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "UnitTester.h"
#include "UnitTesterDlg.h"

#include "CTLSThread.h"
#include "CSmartPtr.h"
#include <map>
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUnitTesterDlg dialog

    CUnitTesterDlg::CUnitTesterDlg(CWnd* pParent /*=NULL*/)
: CDialog(CUnitTesterDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CUnitTesterDlg)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    Test::DoRegistration(m_Manager);

}

CUnitTesterDlg::~CUnitTesterDlg()
{
    for (unsigned int i = 0; i < m_Manager.m_RegisteredTests.size(); i++)
    {
        delete m_Manager.m_RegisteredTests[i];
    }
}

void CUnitTesterDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CUnitTesterDlg)
    DDX_Control(pDX, IDC_TESTER, m_Tester);
    DDX_Control(pDX, IDC_VERSION, m_Version);
    DDX_Control(pDX, IDC_COMMENTS, m_RunComment);
    DDX_Control(pDX, IDC_OUTPUT_FILE, m_OutputFileName);
    DDX_Control(pDX, IDC_SELECTED_TEST, m_SelectedTestName);
    DDX_Control(pDX, IDC_DESCRIPTION, m_Description);
    DDX_Control(pDX, IDC_TESTS_LIST, m_TestsList);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CUnitTesterDlg, CDialog)
    //{{AFX_MSG_MAP(CUnitTesterDlg)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDRUNTESTS, OnRuntests)
    ON_BN_CLICKED(IDRUNLTTESTS, OnRunLTtests)
ON_LBN_SELCHANGE(IDC_TESTS_LIST, OnSelchangeTestsList)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CUnitTesterDlg message handlers

BOOL CUnitTesterDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    SetIcon(m_hIcon, TRUE);                     // Set big icon
    SetIcon(m_hIcon, FALSE);            // Set small icon

    for (unsigned int i = 0; i < m_Manager.m_RegisteredTests.size(); i++)
    {
        m_TestsList.AddString( m_Manager.m_RegisteredTests[i]->Name() );
    }

    OnSelchangeTestsList();

    const TCHAR *command_line = GetCommandLine();

    const TCHAR *p = command_line;

    if (*p == '\"')
    {
        p++;

        while (*p != '\"' && *p != '\0')
            p++;

        if (*p != '\0')
            p++;
    }
    else
    {
        while (*p != ' ' && *p != '\0')
            p++;
    }

    while (*p == ' ' && *p != '\0')
        p++;

    if (*p != '\0')
        m_OutputFileName.SetWindowText( p );
    else
        m_OutputFileName.SetWindowText(_T("testout.txt"));

    return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CUnitTesterDlg::OnPaint() 
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

HCURSOR CUnitTesterDlg::OnQueryDragIcon()
{
    return (HCURSOR) m_hIcon;
}

void CUnitTesterDlg::OnRuntests() 
{
    try
    {
        TCHAR tester[80];
        TCHAR version[80];
        TCHAR filename[256];
        TCHAR comment[10240];

        m_Tester.GetWindowText( tester, sizeof( tester )/sizeof(TCHAR) );
        m_Version.GetWindowText( version, sizeof( version )/sizeof(TCHAR) );
        m_RunComment.GetWindowText( comment, sizeof( comment ) /sizeof(TCHAR));
        m_OutputFileName.GetWindowText( filename, sizeof( filename )/sizeof(TCHAR) );

        int selected = m_TestsList.GetSelCount();

        if (selected == 0
                && IDYES == AfxMessageBox( _T("No tests have been selected; do you want to run all tests?"), MB_YESNOCANCEL | MB_ICONQUESTION ))
        {
            selected = m_TestsList.GetCount();
            for (int i = 0; i < selected; i++)
            {
                m_TestsList.SelItemRange( true, 0, m_TestsList.GetCount() );
                // The above doesn't call back OnSelchangeTestsList()
                m_Manager.m_RegisteredTests[i]->m_selected = true;
            }
        }

        if (selected != 0)
        {
            CWaitCursor wait;

            int run_result = m_Manager.Run( filename, tester, comment, version );

            if (run_result >= 0)
            {
                if (run_result < selected)
                {
                    std::wstring err;
                    TCHAR number[32];
                    err += _itot( selected - run_result, number, 10 );
                    err += _T("tests failed out of ");
                    err += _itot( selected, number, 10 );
                    err += _T(" selected");
                    AfxMessageBox( err.c_str(), MB_ICONSTOP );
                }
                else
                    AfxMessageBox( _T("All tests reported success.") );
            }
        }
    }
    catch (...)
    {
        AfxMessageBox( _T("Problem with Unit Test Framework when running tests") );
    }
}


class LTTester : public Util::CRunnableClass
{
    private:
        Test::CUnitTestManager& m_Manager;
        std::map<int,int>&      m_counter;
        Util::CSmartPtr<Util::CTLSThread> m_RunThread;
        const TCHAR*            m_filename;
        const TCHAR*            m_tester;
        const TCHAR*            m_comment;
        const TCHAR*            m_version;
    public:
        LTTester(const TCHAR* i_filename, 
                Test::CUnitTestManager& i_Manager, 
                std::map<int,int>& i_counter,
                const TCHAR *i_tester, 
                const TCHAR *i_comment, 
                const TCHAR *i_version ): 
            m_Manager(i_Manager), m_counter(i_counter), m_filename(i_filename),
            m_tester(i_tester), m_comment(i_comment), m_version(i_version)
            {
                m_RunThread = Util::CSmartPtr<Util::CTLSThread> (new Util::CTLSThread(*this));
            };
        virtual ~LTTester()
        {
            m_RunThread->StopThread();
            m_RunThread->WaitForCompletion();
        };
        virtual DWORD Run()
        {

            for (int i = 0; i < (int)m_Manager.m_RegisteredTests.size(); i++)
            {
                if (m_Manager.m_RegisteredTests[i]->m_selected)
                {
                    m_counter[i] = 0;
                    // Counter for all test runs
                    m_counter[-i] = 0;
                }
            }

            {
                m_Manager.RunSingleTrailer(m_filename, m_tester, m_comment, m_version );
            }

            while (true)
            {
                if (WaitForSingleObject(CRunnableClass::ExitEvent(),0) == WAIT_OBJECT_0)
                    break;
                for ( int i = 0; i < (int)m_Manager.m_RegisteredTests.size(); i++)
                {
                    if (m_Manager.m_RegisteredTests[i]->m_selected)
                    {
                        try
                        {
                            m_counter[i] += m_Manager.RunSingleTest(m_filename,i);
                            m_counter[-i] += 1;
                        }
                        catch(...)
                        {
                            m_counter[-i] += 1;
                        }
                    }
                }
            }
            {
                m_Manager.RunSingleResults(m_filename, m_counter);
            }

            return 0;
        }
};

void CUnitTesterDlg::OnRunLTtests() 
{
    std::map<int,int>           m_counter;
    try
    {
        TCHAR tester[80];
        TCHAR version[80];
        TCHAR filename[256];
        TCHAR comment[10240];


        m_Tester.GetWindowText( tester, sizeof( tester )/sizeof(TCHAR) );
        m_Version.GetWindowText( version, sizeof( version )/sizeof(TCHAR) );
        m_RunComment.GetWindowText( comment, sizeof( comment )/sizeof(TCHAR) );
        m_OutputFileName.GetWindowText( filename, sizeof( filename )/sizeof(TCHAR) );

        int selected = m_TestsList.GetSelCount();

        if (selected == 0
                && IDYES == AfxMessageBox( _T("No tests have been selected; do you want to run all tests?"), MB_YESNOCANCEL ))
        {
            selected = m_TestsList.GetCount();
            for (int i = 0; i < selected; i++)
            {
                m_TestsList.SelItemRange( true, 0, m_TestsList.GetCount() );
                // The above doesn't call back OnSelchangeTestsList()
                m_Manager.m_RegisteredTests[i]->m_selected = true;
            }
        }

        if (selected != 0)
        {
            LTTester    m_Tester(filename, m_Manager, m_counter, tester, comment, version);
            AfxMessageBox(_T("Continuous tests are running. Press OK to finish!"),MB_OK);
        }       
    }
    catch (...)
    {
        AfxMessageBox( _T("Problem with Unit Test Framework when running tests") );
    }
}

void CUnitTesterDlg::OnSelchangeTestsList() 
{
    for (int i = 0; i < m_TestsList.GetCount(); i++)
    {
        m_Manager.m_RegisteredTests[i]->m_selected = m_TestsList.GetSel( i ) != 0;
        int ll1 = m_Manager.m_RegisteredTests[i]->m_selected;
    }

    if (0 != m_TestsList.GetCount())
    {
        m_Description.SetWindowText( m_Manager.m_RegisteredTests[m_TestsList.GetCaretIndex()]->Description() );
        m_SelectedTestName.SetWindowText( m_Manager.m_RegisteredTests[m_TestsList.GetCaretIndex()]->Name() );
    }
}
