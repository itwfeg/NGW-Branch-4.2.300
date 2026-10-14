// Header file included by ANTLR UniLexer
#if !defined ( __linux__ )
#include <tchar.h>
#endif      // __linux__

#ifndef _TEST_HEADER_
#define _TEST_HEADER_

#if !defined ( __linux__ )
#pragma warning(disable : 4786) 
#endif      // __linux__

#include <vector>

#if !defined ( __linux__ )
#pragma warning(disable: 4275)

#pragma warning(disable: 4786)
#endif      // __linux__
#include <sstream>
#include <map>
#include "CCriticalSection.h"

#if !defined ( __linux__ )
#pragma warning(default: 4275)
#endif      // __linux__


#if defined(LINUX_FIX)

#if defined(PAL_WIN32_RUN)
    #define PCSTR_NEXT_LINE             "\r\n"
    #define PTSTR_NEXT_LINE             L"\r\n"

#elif   defined(__linux__)
    #define PCSTR_NEXT_LINE             "\n"
    #define PTSTR_NEXT_LINE             L"\n"

#else
    #error  "define related VARIABLE"
#endif

    #define PCSTR_NEW_LINE              "\n"
    #define PTSTR_NEW_LINE              L"\n"

// ----- CGetFileSizeTester -----
#if defined(PAL_WIN32_RUN)
    #define TEST_FILENAME               "test.cpp"
#elif   defined(__linux__)
    #define TEST_FILENAME               "Test.cpp"
#else
    #error  "define TEST_FILENAME"
#endif

// ----- ZIP / UNZIP testing -----
#if defined(WIN32)
    #define ZIP_TEST_PATH               "c:\\test\\temp"
    #define ZIP_TEST_FILE               "c:\\test\\z0001.zip"
    #define UNZIP_TEST_PATH             "c:\\test\\t1"

#elif   defined(__linux__)
    // ***** WARNING: If the testing data has security concern, change the testing path
    #define ZIP_TEST_PATH               "/tmp/hobart/zip"
    #define ZIP_TEST_FILE               "/tmp/hobart/z0001.zip"
    #define UNZIP_TEST_PATH             "/tmp/hobart/unzip"

#else
    #error "Define testing files"
#endif

#endif  // LINUX_FIX

namespace Test
{
    class CInternalTestEventLog;

    class CUnitTest
    {
        public:
            CUnitTest() : m_selected( 0 ) {}
            virtual ~CUnitTest() {}

            // Return the test name.
            virtual const TCHAR *Name() = 0;

            // Return a description of the test.
            virtual const TCHAR *Description() = 0;

            // Perform the test.
            virtual bool TestCode() = 0;

            bool m_selected;

        private:
            friend class CUnitTestManager;

            bool TryTest();
    };

    class CUnitTestManager
    {
        public:
            void Register( CUnitTest *i_test )
            {
                m_RegisteredTests.push_back( i_test );
            }
            //  Run the required tests.  Returns number of successes, or -ve for framework problem.
            int Run( const TCHAR *i_log_filename, const TCHAR *i_tester, const TCHAR *i_comment, const TCHAR *i_version );

            int RunSingleTest(const TCHAR *i_log_filename, const int test_no);
            void RunSingleTrailer(const TCHAR *i_log_filename, 
                    const TCHAR *i_tester, const TCHAR *i_comment, const TCHAR *i_version );
            void RunSingleResults(const TCHAR *i_log_filename, 
                    std::map<int,int>& i_cnt);

            // A list of requested tests
            std::vector<CUnitTest*> m_RegisteredTests;
    };

    class CTestEventLog
    {
        public:
            static void Report( const TCHAR *i_event );
    };

    // This function should be implemented by the particular test library.
    // Don't forget to use Test::
    void DoRegistration( Test::CUnitTestManager& manager );


    class CInternalTestEventLog
    {
        public:
            CInternalTestEventLog( const TCHAR *i_filename, bool i_log = true);
            ~CInternalTestEventLog();

            // Called by user-accessable CTestEventLog::Report
            // Column 1 is "Unit Test"
            // Column 2 is current test Name()
            // Column 3 is the event string
            static void Report( const TCHAR *i_event );

            // Called by CUnitTest::TryTest
            // Column 1 is "Unit Test Status"
            // Column 2 is current test Name()
            // Column 3 is the event string
            static void ReportTestStatus( const TCHAR *i_event );

            // Called by CUnitTestManager::Run
            // Column 1 is "Test Run"
            // Column 2 is the event string
            // Column 3 is the status string
            static void ReportTestRunStatus( const TCHAR *i_event, const TCHAR *i_status );

            // Called by CUnitTestManager::Run
            // Column 1 is "Test Run Information"
            // Column 2 is the event string
            // Column 3 is the status string
            static void ReportTestRunInfo( const TCHAR *i_category, const TCHAR *i_info );

            // Write three columns to a .csv file.
            void InternalReportEvent( const TCHAR *i_column1, const TCHAR *i_column2, const TCHAR *i_column3 );

            static CInternalTestEventLog *sm_Open;
            static CUnitTest *sm_CurrentTest;

            HANDLE m_File;
            static bool m_log;
            Util::CSmartCriticalSection         m_CS;
    };

};

#endif
