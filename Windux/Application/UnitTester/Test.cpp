// Header file included by ANTLR UniLexer
#if !defined ( __linux__ )
#include <tchar.h>
#endif      // __linux__
/*
   $Log: /Test/Test.cpp $
 * 
 * 8     17/09/98 19:51 Dadf-sw
 * 
 * 7     17/09/98 17:33 Dadf-sw
 * 
 * 6     17/09/98 14:29 Dadf-sw
 * More tests on VSS stuff.
 * 
 * 5     17/09/98 14:26 Dadf-sw
 * Testing keyword expansion
 * 
 * 4     17/09/98 14:24 Dadf-sw
 * Added additional logging of available and selected tests.

 $History: Test.cpp $
 * 
 * *****************  Version 8  *****************
 * User: Dadf-sw      Date: 17/09/98   Time: 19:51
 * Updated in $/Test
 * 
 * *****************  Version 7  *****************
 * User: Dadf-sw      Date: 17/09/98   Time: 17:33
 * Updated in $/Test
 * 
 * *****************  Version 6  *****************
 * User: Dadf-sw      Date: 17/09/98   Time: 14:29
 * Updated in $/Test
 * More tests on VSS stuff.
 * 
 * *****************  Version 5  *****************
 * User: Dadf-sw      Date: 17/09/98   Time: 14:26
 * Updated in $/Test
 * Testing keyword expansion

 $Header: /Test/Test.cpp 8     17/09/98 19:51 Dadf-sw $

*/

#include <iostream>

#include "PAL_platformheader.h"
#include "Test.h"
#include "MSGMutex.h"
#include <fstream>
#include <time.h>
#include "utilunicode.h"
using namespace Test;

namespace Test {
};

CInternalTestEventLog *CInternalTestEventLog::sm_Open = 0;
CUnitTest *CInternalTestEventLog::sm_CurrentTest = 0;
bool CInternalTestEventLog::m_log = true;

CInternalTestEventLog::CInternalTestEventLog( const TCHAR *i_filename, bool i_log )
{
    Util::CLockCriticalSection Lock(m_CS);
    m_log = i_log;
    sm_Open = this;

#ifdef LINUX_FIX
    m_File = PAL_absclass::PAL_createFile( 
            i_filename, 
            PAL_FILE_GEN_WR, 
            PAL_FILE_SHARE_RD, 
            0, 
            PAL_FILE_OPEN_CREATE, 
            PAL_FILE_ATTR_NORMAL, 
            0 );
#else
    m_File = CreateFile( i_filename, GENERIC_WRITE, FILE_SHARE_READ 
            , 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );
#endif

    if (m_File == INVALID_HANDLE_VALUE)
        throw Util::CPlatformException();

#ifdef LINUX_FIX
    PAL_absclass::PAL_setFilePointer( m_File, 0, 0, PAL_FILE_END );   // Just in case appending
#else
    SetFilePointer( m_File, 0, 0, FILE_END );   // Just in case appending
#endif
}

CInternalTestEventLog::~CInternalTestEventLog()
{
    Util::CLockCriticalSection Lock(m_CS);
    sm_Open = 0;
#ifdef LINUX_FIX
    PAL_absclass::PAL_closeHandle( m_File );
#else
    CloseHandle( m_File );
#endif
}

void CInternalTestEventLog::Report( const TCHAR *i_event )
{
    if (m_log)
    {
        CInternalTestEventLog::sm_Open->InternalReportEvent( _T("Unit Test"), sm_CurrentTest->Name(), i_event );
    }
}

void CTestEventLog::Report( const TCHAR *i_event )
{
    CInternalTestEventLog::Report( i_event );
}

void CInternalTestEventLog::ReportTestStatus( const TCHAR *i_event )
{
    if (m_log)
    {
        CInternalTestEventLog::sm_Open->InternalReportEvent( _T("Unit Test Status"), sm_CurrentTest->Name(), i_event );
    }
}

void CInternalTestEventLog::ReportTestRunStatus( const TCHAR *i_event, const TCHAR *i_status )
{
    if (m_log)
    {
        CInternalTestEventLog::sm_Open->InternalReportEvent( _T("Test Run"), i_event, i_status );
    }
}

void CInternalTestEventLog::ReportTestRunInfo( const TCHAR *i_category, const TCHAR *i_info )
{
    if (m_log)
    {
        CInternalTestEventLog::sm_Open->InternalReportEvent( _T("Test Run Info"), i_category, i_info );
    }
}

void CInternalTestEventLog::InternalReportEvent( const TCHAR *i_column1, const TCHAR *i_column2, const TCHAR *i_column3 )
{
    //static Util::CMSGMutex file_mutex;

    //Util::CMSGMutexClaim claim( file_mutex );
    Util::CLockCriticalSection Lock(m_CS);

    std::wstring str;

    str = _T("\"");
    str += i_column1;
    str += _T("\",\"");
    str += i_column2;
    str += _T("\",\"");
    str += i_column3;
#if defined(LINUX_FIX)
    str += _T("\"");
    str += _T(PCSTR_NEXT_LINE);
#else
    str += _T("\"\x0d\x0a");
#endif

    DWORD written;

#ifdef SEND_TO_DEBUG_LOG
#ifdef LINUX_FIX
    // copy to the PAL Log?
    PAL_absclass::PAL_OutputDbgStr(str.c_str());
#else
    OutputDebugString(str.c_str());
#endif
#endif

#ifdef SEND_TO_FILE
#ifdef LINUX_FIX
    if (!PAL_absclass::PAL_writeFile( m_File, Util::widetocharptr(str.c_str()), str.length(), &written, 0 ))
#else
    if (!WriteFile( m_File, Util::widetocharptr(str.c_str()), str.length(), &written, 0 ))
#endif
    {
        throw Util::CPlatformException();
    }
#else
    printf("%ls", str.c_str());
#endif
}

bool CUnitTest::TryTest()
{
    bool result = false;

    try {
        CInternalTestEventLog::ReportTestStatus( _T("Starting") );
        result = TestCode();
    }
    catch (Util::CBaseException &e)
    {
        result = false;
        CInternalTestEventLog::ReportTestStatus( e.ExceptionMsg().c_str() );
    }
    catch (...)
    {
        result = false;
        CInternalTestEventLog::ReportTestStatus( _T("Exception Thrown") );
    }

    if (result)
        CInternalTestEventLog::ReportTestStatus( _T("Success") );
    else
        CInternalTestEventLog::ReportTestStatus( _T("Failure") );

    return result;
}

int CUnitTestManager::Run( const TCHAR *i_log_filename, const TCHAR *i_tester, const TCHAR *i_comment, const TCHAR *i_version )
{
    int successes = 0;
    try
    {
        CInternalTestEventLog log( i_log_filename );

        CInternalTestEventLog::ReportTestRunStatus( _T("Tester"), i_tester );
        CInternalTestEventLog::ReportTestRunStatus( _T("Tested software version"), i_version );
        CInternalTestEventLog::ReportTestRunStatus( _T("Comment"), i_comment );

        {
            std::wstring str;
#if defined ( __linux__ )
            char buf[128];
            wchar_t wbuf[128];
            time_t now = time (NULL);
            struct tm *t = localtime(&now);
            strftime(buf, 128, "%c", t);
            mbstowcs(wbuf, buf, 128);
            str = wbuf;
#else
            TCHAR tmpbuf[128];
            _tstrdate( tmpbuf );
            str += tmpbuf;
            str += _T(" ");
            _tstrtime( tmpbuf );
            str += tmpbuf;
#endif      // LINUX_FIX
            CInternalTestEventLog::ReportTestRunStatus( _T("Starting"), str.c_str() );
        }

        unsigned int i;

        for (i = 0; i < m_RegisteredTests.size(); i++)
        {
            CInternalTestEventLog::ReportTestRunInfo( _T("Available Test"), m_RegisteredTests[i]->Name() );
        }

        int selected = 0;

        for (i = 0; i < m_RegisteredTests.size(); i++)
        {
            if (m_RegisteredTests[i]->m_selected)
            {
                selected++;

                CInternalTestEventLog::ReportTestRunInfo( _T("Selected Test"), m_RegisteredTests[i]->Name() );
            }
        }

        for (i = 0; i < m_RegisteredTests.size(); i++)
        {
            if (m_RegisteredTests[i]->m_selected)
            {
                log.sm_CurrentTest = m_RegisteredTests[i];

                try {
                    if (m_RegisteredTests[i]->TryTest())
                        successes++;
                }
                catch (Util::CBaseException &e)
                {
#if !defined ( __linux__ )
                    MessageBox(NULL,e.ExceptionMsg().c_str(),_T("Error"),MB_ICONSTOP |MB_OK);
#endif
                    CInternalTestEventLog::sm_Open->InternalReportEvent( _T("Exception"), m_RegisteredTests[i]->Name(), e.ExceptionMsg().c_str() );
                }
                catch (...)
                {
#if !defined ( __linux__ )
                    MessageBox(NULL,_T("Unexpected exception thrown when running test"),_T("Error"),MB_ICONSTOP |MB_OK);
#endif
                    CInternalTestEventLog::sm_Open->InternalReportEvent( _T("Exception"), m_RegisteredTests[i]->Name(), _T("Unknown") );
                }

                log.sm_CurrentTest = 0;
            }
        }

        {
            std::wstring str;
#if defined ( __linux__ )
            char buf[128];
            wchar_t wbuf[128];
            time_t now = time (NULL);
            struct tm *t = localtime(&now);
            strftime(buf, 128, "%c", t);
            mbstowcs(wbuf, buf, 128);
            str = wbuf;
#else
            TCHAR tmpbuf[128];
            _tstrdate( tmpbuf );
            str += tmpbuf;
            str += _T(" ");
            _tstrtime( tmpbuf );
            str += tmpbuf;
#endif      // LINUX_FIX
            CInternalTestEventLog::ReportTestRunStatus( _T("Completed"), str.c_str() );
        }

        if (successes == selected)
            CInternalTestEventLog::ReportTestRunStatus( _T("Result"), _T("Success") );
        else
            CInternalTestEventLog::ReportTestRunStatus( _T("Result"), _T("Fail") );
    }
    catch (...)
    {
#if !defined ( __linux__ )
        MessageBox(NULL,_T("Problem opening log file"),_T("Error"),MB_ICONSTOP |MB_OK);
#endif      // __linux__
        successes = -1;
    }

    return successes;
}

int CUnitTestManager::RunSingleTest(const TCHAR *i_log_filename, const int test_no)
{
    int successes = 0;
    //    CInternalTestEventLog log( i_log_filename, false );
    CInternalTestEventLog log( i_log_filename );        // Alamy: Enable log

    if (m_RegisteredTests[test_no]->m_selected)
    {
        log.sm_CurrentTest = m_RegisteredTests[test_no];

        try {
            if (m_RegisteredTests[test_no]->TryTest())
            {
                successes++;
            }
        }
        catch (Util::CBaseException &e)
        {
            CInternalTestEventLog::sm_Open->InternalReportEvent( _T("Exception"), m_RegisteredTests[test_no]->Name(), e.ExceptionMsg().c_str() );
        }
        catch (...)
        {
            CInternalTestEventLog::sm_Open->InternalReportEvent( _T("Exception"), m_RegisteredTests[test_no]->Name(), _T("Unknown") );
        }

        log.sm_CurrentTest = 0;
    }
    return successes;
}

void CUnitTestManager::RunSingleTrailer(
        const TCHAR *i_log_filename, 
        const TCHAR *i_tester, 
        const TCHAR *i_comment, 
        const TCHAR *i_version )
{
    int successes = 0;
    try
    {
        CInternalTestEventLog log( i_log_filename );

        CInternalTestEventLog::ReportTestRunStatus( _T("Tester"), i_tester );
        CInternalTestEventLog::ReportTestRunStatus( _T("Tested software version"), i_version );
        CInternalTestEventLog::ReportTestRunStatus( _T("Comment"), i_comment );

        {
            std::wstring str;
#if defined ( __linux__ )
            char buf[128];
            wchar_t wbuf[128];
            time_t now = time (NULL);
            struct tm *t = localtime(&now);
            strftime(buf, 128, "%c", t);
            mbstowcs(wbuf, buf, 128);
            str = wbuf;
#else
            TCHAR tmpbuf[128];
            _tstrdate( tmpbuf );
            str += tmpbuf;
            str += _T(" ");
            _tstrtime( tmpbuf );
            str += tmpbuf;
#endif      // LINUX_FIX
            CInternalTestEventLog::ReportTestRunStatus( _T("Starting"), str.c_str() );
        }

        unsigned int i;

        for (i = 0; i < m_RegisteredTests.size(); i++)
        {
            CInternalTestEventLog::ReportTestRunInfo( _T("Available Test"), m_RegisteredTests[i]->Name() );
        }

        int selected = 0;

        for (i = 0; i < m_RegisteredTests.size(); i++)
        {
            if (m_RegisteredTests[i]->m_selected)
            {
                selected++;

                CInternalTestEventLog::ReportTestRunInfo( _T("Selected Test"), m_RegisteredTests[i]->Name() );
            }
        }
    }
    catch (...)
    {
#if !defined ( __linux__ )
        MessageBox(NULL,_T("Problem opening log file"),_T("Error"),MB_ICONSTOP |MB_OK);
#endif      // __linux__
        successes = -1;
    }
}

void CUnitTestManager::RunSingleResults(const TCHAR *i_log_filename, std::map<int,int>& i_cnt)
{
    try
    {
        std::ofstream           ost(Util::widetocharptr(i_log_filename), std::ios::app);
        for (int i = 0; (unsigned) i < m_RegisteredTests.size(); i++)
        {
            if (m_RegisteredTests[i]->m_selected)
            {
                if (i_cnt[-i] > 0)
                { 
                    //std::ostringstream                ost;
                    ost << "Test ";
                    std::string l_ww;
                    Util::wchartostring(l_ww,m_RegisteredTests[i]->Name());
                    ost << l_ww;//Util::widetocharptr(m_RegisteredTests[i]->Name());
                    ost << " reliability : ";
                    ost << (float)((float)i_cnt[i]/(float)i_cnt[-i]*100);
                    ost << " %(";
                    ost << i_cnt[i];
                    ost << "/";
                    ost << i_cnt[-i];
                    ost << ")";
                    ost << std::endl;
                    //CInternalTestEventLog log( i_log_filename );
                    //log.Report(ost.str().c_str());
                }
            }
        }
    }
    catch (...)
    {
#if !defined ( __linux__ )
        MessageBox(NULL,_T("Problem opening log file"),_T("Error"),MB_ICONSTOP |MB_OK);
#endif
    }
}

// MSVC2010
#if defined ( __linux__ )
int main(int argc, char** argv)
{
    std::map<int,int>       counter;
    int                     opt;
    uint                    case_index;

    PAL_absclass::PAL_logging = 0xFFFFFFFF;

    Test::CUnitTestManager  manager;

    // default testing case
    case_index = 5;
    while ((opt = getopt(argc, argv, "n:a")) != -1)
    {
        switch (opt)
        {
            case 'n':   case_index = atoi(optarg);  break;
            case 'a':   /* Run All testing cases */ break;
            default:    break;
        }
    }
    Test::DoRegistration(manager);      // ***** Segmentation fault if this statement is executed

    if (case_index >= manager.m_RegisteredTests.size())
    {
        case_index = manager.m_RegisteredTests.size() - 1;
    }
    printf("%d cases registered, selected index %d\n", manager.m_RegisteredTests.size(), case_index);

    manager.m_RegisteredTests[case_index]->m_selected = true;

    manager.RunSingleTrailer(
            _T("out.log"), 
            _T("test"),
            _T("so far"),
            _T("0.1"));

    counter[case_index] = manager.RunSingleTest( _T("out.log"), case_index);
    counter[-case_index] = 1;

    manager.RunSingleResults(_T("out.log"), counter);

    //    Test::UnRegistration( manager );

    // Use this line to detect if we finish the main(), see PAL_log.log
    PAL_PRINT_LOG(PAL_INFO_LOG, "----- End of Test.cpp -----" );

    return 0;
}
#endif
