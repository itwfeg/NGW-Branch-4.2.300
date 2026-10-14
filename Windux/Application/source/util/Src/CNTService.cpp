#include <PAL_platformheader.h>
#include <tchar.h>
#include "dbt.h"

// NTService.cpp
//
// Implementation of CNTService

#include <eh.h>

#include <stdio.h>
#include <stdlib.h>

#include "CNTService.h"
#include "SmartHandle.h"
#include "OutputLogger.h"

#include "CVersionInfo.h"
#include "ErrorHandling.h"
#include "CRelativeTimer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"
#endif

#define myStrCmp(_x, _y) _stricmp(_x, _y)
#define myStrnCmp(_x, _y, _z) strncmp(_x, _y, _z)



#ifdef ENABLE_SERVICE_KILLER

#include "CTLSThread.h"

namespace Util
{

#ifdef USE_ACCOUNTANT
    int CServiceKiller::dataType = 0;
    std::string CServiceKiller::dataTypeName("CServiceKiller"); 
#endif

    CServiceKiller::CServiceKiller(DWORD iTimeout) :  m_Timeout(iTimeout) 
    {
#ifdef USE_ACCOUNTANT
        if (dataType == 0)
            dataType = util_acc::registerDataType(dataTypeName);

        util_acc::addInstance(dataType);
#endif
		PAL_absclass::PAL_OutputDbgStr(L"CServiceKiller::CServiceKiller: entry\r\n");

        try 
        { 
            mp_Thread = new Util::CTLSThread(*this);

        } 

        catch(...) 
        { 
			PAL_absclass::PAL_OutputDbgStr(L"CServiceKiller::CServiceKiller: exception\r\n");
            LogCatch; 
        } 
		PAL_absclass::PAL_OutputDbgStr(L"CServiceKiller::CServiceKiller: exit\r\n");
    }
    CServiceKiller::~CServiceKiller()
    {
#ifdef USE_ACCOUNTANT
        util_acc::removeInstance(dataType);
#endif
		PAL_absclass::PAL_OutputDbgStr(L"CServiceKiller::~CServiceKiller: entry\r\n");

        try 
        { 
			PAL_absclass::PAL_OutputDbgStr(L"CServiceKiller::~CServiceKiller(): stopping thread\r\n");
            mp_Thread->StopThread();
            mp_Thread->WaitForCompletion(100);

        } 
        catch(Util::CBaseException & )
        {
			PAL_absclass::PAL_OutputDbgStr(L"CServiceKiller::~CServiceKiller(): CBaseException\r\n");
        }
        catch(...) 
        { 
			PAL_absclass::PAL_OutputDbgStr(L"CServiceKiller::~CServiceKiller(): exception\r\n");
            LogCatch; 
        } 

    };
    DWORD CServiceKiller::Run()
    {
		PAL_absclass::PAL_OutputDbgStr(L"CServiceKiller::Run: entry\r\n");
        try
        {
            Util::CRelativeTimer l_timer(m_Timeout,m_Timeout);

            DWORD l_ret = Util::MSGWaitForSingleObject(l_timer);
            if( l_ret == WAIT_OBJECT_0)
            {                   
				PAL_absclass::PAL_OutputDbgStr(L"ServiceKiller:Run: calling TerminateProcess\r\n");
                //Util::COutputLogger::ReportToErrorLog(L" TerminateProcess called.",L" Service termination timeout : ");
                PAL_absclass::PAL_terminateProcess(PAL_absclass::PAL_getCurrProc(),2);
                //      ::ExitProcess(2);
            }
			PAL_absclass::PAL_OutputDbgStr(L"ServiceKiller:Run: timer wait returned but object not signaled\r\n");
        }
        catch(Util::CTerminateNow &)
        {
			PAL_absclass::PAL_OutputDbgStr(L"ServiceKiller:Run: CTerminateNow exception\r\n");
        }
        catch(...)
        {
			PAL_absclass::PAL_OutputDbgStr(L"ServiceKiller:Run: other exception\r\n");
            LogCatch;
        }
		PAL_absclass::PAL_OutputDbgStr(L"ServiceKiller:Run: exit\r\n");
        return 1;
    }

    static Util::CSmartPtr< Util::CServiceKiller > g_the_timebomb(NULL);

};

#endif // ENABLE_SERVICE_KILLER

// static variables
Util::CNTService* Util::CNTService::ms_pThis = NULL;

Util::CSmartHandle Util::CNTService::ms_ShutdownEvent = PAL_absclass::PAL_createEvent (
        NULL,           // pointer to a SECURITY_ATTRIBUTES structure           
        TRUE,           // needs manual reset 
        FALSE,          // set initial state to nonsignaled !
        NULL);          // name of object (created without one)

#ifdef USE_ACCOUNTANT
int Util::CNTService::dataType = 0;
std::string Util::CNTService::dataTypeName("CNTService"); 
#endif

#ifdef ENABLE_UNHANDLED_EXCEPTION_FILTER
LONG Util::CNTService::UnhandledExceptionFilter(PAL_LPEXCEPTION_POINTERS lpexpExceptionInfo)
{
    TCHAR filename[256];
    TCHAR buf[1024];
    TCHAR code[20];

    PAL_absclass::PAL_simpleBeep();

	PAL_absclass::PAL_OutputDbgStr(L"CNTService:UnhandledExceptionFilter: \r\n");

    _stprintf(filename,_T("exception.log"));
    HANDLE hFile=PAL_absclass::PAL_createFile(
            filename,
            PAL_FILE_GEN_RD,
            PAL_FILE_SHARE_WR,
            NULL,
            PAL_FILE_OPEN_CREATE,
            PAL_FILE_ATTR_ARCHIVE,
            NULL);
    if(hFile == INVALID_HANDLE_VALUE)
    {
		PAL_absclass::PAL_OutputDbgStr(L"CNTService:UnhandledExceptionFilter: couldn't open exception log.\r\n");
        return EXCEPTION_EXECUTE_HANDLER;
    }

    if(lpexpExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
    {
        _stprintf(buf, _T("Exception-code: EXCEPTION_ACCESS_VIOLATION\n"));
    }
    else 
    {
        _ultot( lpexpExceptionInfo->ExceptionRecord->ExceptionCode, code, 16 );
        _stprintf(buf, _T("Exception-code: %s\n"),code);
    }

	PAL_absclass::PAL_OutputDbgStr(L"CNTService:UnhandledExceptionFilter: logging exception\r\n");
    DWORD NumberOfBytesWritten;
    PAL_absclass::PAL_writeFile(hFile,buf,_tcsclen(buf),&NumberOfBytesWritten,NULL);

    _ultot( lpexpExceptionInfo->ExceptionRecord->ExceptionFlags, code, 16 );
    _stprintf(buf, _T("Exception-flag: %s\n"),code);
    PAL_absclass::PAL_writeFile(hFile,buf,_tcsclen(buf),&NumberOfBytesWritten,NULL);

#ifdef _ALPHA_ 
    __int64* pi64Tmp = (__int64*)lpexpExceptionInfo->ExceptionRecord->ExceptionAddress;
    _ui64tot(*pi64Tmp, code, 16 );
#else
    DWORD* pdwTmp = (DWORD*)lpexpExceptionInfo->ExceptionRecord->ExceptionAddress;
    _ultot(*pdwTmp, code, 16 );
#endif

    _stprintf(buf, _T("Exception-address: %s\n"),code);
    PAL_absclass::PAL_writeFile(hFile,buf,_tcsclen(buf),&NumberOfBytesWritten,NULL);

    for (DWORD i=0; i<lpexpExceptionInfo->ExceptionRecord->NumberParameters;++i)
    {
        _ultot(lpexpExceptionInfo->ExceptionRecord->ExceptionInformation[i], code, 16 );
        _stprintf(buf, _T("%d. Exception-information: %s\n"),i,code);
        PAL_absclass::PAL_writeFile(hFile,buf,_tcsclen(buf),&NumberOfBytesWritten,NULL);
    }

    PAL_absclass::PAL_closeHandle(hFile);

	PAL_absclass::PAL_OutputDbgStr(L"CNTService:UnhandledExceptionFilter: exit\r\n");
    return EXCEPTION_EXECUTE_HANDLER;
    //return EXCEPTION_CONTINUE_EXECUTION;
}
#endif // ENABLE_UNHANDLED_EXCEPTION_FILTER

#ifdef ENABLE_TERMINATE_FUNCTION
void Util::CNTService::TerminateFunction()
{
    TCHAR filename[256];
    TCHAR buf[1024];

	PAL_absclass::PAL_OutputDbgStr(L"CNTService:TerminateFunction: entry\r\n");

    PAL_absclass::PAL_simpleBeep();

    _stprintf(filename,_T("exception.log"));

    HANDLE hFile=PAL_absclass::PAL_createFile(
            filename,
            PAL_FILE_GEN_WR,
            PAL_FILE_SHARE_WR,
            NULL,
            PAL_FILE_OPEN_CREATE,
            PAL_FILE_ATTR_ARCHIVE,
            NULL);

    _stprintf(buf,_T("Abnormal program termination"));
    DWORD NumberOfBytesWritten;
    PAL_absclass::PAL_writeFile(hFile,buf,_tcsclen(buf),&NumberOfBytesWritten,NULL);
    PAL_absclass::PAL_closeHandle(hFile);
	PAL_absclass::PAL_OutputDbgStr(L"CNTService:TerminateFunction: exit(-1)\r\n");
    exit(-1);
}
#endif // ENABLE_TERMINATE_FUNCTION


// Constructor
Util::CNTService::CNTService(
        const std::wstring& i_ServiceName,
        bool ibReportVersion) 
        : m_ServiceName(i_ServiceName)
{
	PAL_absclass::PAL_OutputDbgStr(L"CNTService: constructor entry\r\n");

#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);
    util_acc::addInstance(dataType);
#endif

    try
    {
        // copy the address of the current object so we can access it from
        // the static member callback functions. 
        // WARNING: This limits the application to only one CNTService object. 
        ms_pThis = this;

        // initialize SCM handle
        m_hServiceStatus = NULL;

        // initialize service status structure
		m_Status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
        m_Status.dwCurrentState = SERVICE_STOPPED;
        m_Status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
		m_Status.dwWin32ExitCode = 0;
        m_Status.dwServiceSpecificExitCode = 0;
        m_Status.dwCheckPoint = 0;
        m_Status.dwWaitHint = dwShutdownHint;

#ifdef ENABLE_UNHANDLED_EXCEPTION_FILTER
        // set the last handler for unhandled exceptions
        // Let Dr Watson do the job
        //SetUnhandledExceptionFilter(UnhandledExceptionFilter);
#endif // ENABLE_UNHANDLED_EXCEPTION_FILTER

#ifdef ENABLE_TERMINATE_FUNCTION
        //set_terminate(TerminateFunction);
#endif

        // Define handler for Win32 (C structured) exceptions.
        // This translates native exceptions into C++ exceptions.
		_set_se_translator(Util::TranslateSE);

		if( ibReportVersion)
            ReportVersionInfoToErrorLog();
    }
    catch(...)
    {
        LogCatchAndThrow;
    }
	PAL_absclass::PAL_OutputDbgStr(L"CNTService: constructor exit\r\n");
}
// Destructor
Util::CNTService::~CNTService()
{
	PAL_absclass::PAL_OutputDbgStr(L"CNTService: destructor entry\r\n");
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif
	PAL_absclass::PAL_OutputDbgStr(L"CNTService: destructor exit\r\n");
}

////////////////////////////////////////////////////////////////////////////////////////
// Default command line argument parsing

// Returns TRUE if it found an arg it recognised, FALSE if not
// Note: processing some arguments causes output to stdout to be generated.
//
// TODO This is much easier using getopt

bool Util::CNTService::ParseStandardArgs(int argc, char* argv[])
{
    try
    {
        // See if we have any command line args we recognise
        if (argc <= 1) return FALSE;

        if (myStrCmp(argv[1], "-i") == 0)
        {
            // Request to install.
            if (IsInstalled()) {
                _tprintf(_T("%s is already installed\n"), m_ServiceName.c_str());
            } else {
                // Try and install the copy that's running

                // CJC       8/17/00 11:49:28 AM
                // check if an account and password are given
                char * paccount = NULL;
                char * ppass    = NULL;

				//  _tprintf(L" ARGS = %d \n",argc);
                if( argc >= 3)
                {
                    int j;
                    for( j = 2; j < argc; j++)
                    {
                        //_tprintf(_T(" %s \n"),argv[j]);
                        if ( 0 == myStrnCmp(argv[j], "user", 4) )
                        {
                            paccount = ((j+1) < argc) ? argv[j+1] : NULL;
                            //_tprintf(_T( "User = %s\n") ,paccount);
                        }
                        else if ( 0 == myStrnCmp(argv[j], "password", 8) )
                        {
                            ppass = ((j+1) < argc) ? argv[j+1] : NULL;
                            //_tprintf(_T( "pass = %s\n") ,ppass);

                        }
                    }
                }
                if (Install(paccount,ppass)) {
                    _tprintf(_T("%s installed\n"), m_ServiceName.c_str());
                } else {
                    _tprintf(_T("%s failed to install. Error %d\n"), m_ServiceName.c_str(), PAL_absclass::PAL_getLastError());
                }
            }
            return TRUE; // say we processed the argument

        }
        else if ( myStrCmp(argv[1], "-u") == 0)
        {
            // Request to uninstall.
            if (!IsInstalled()) {
                _tprintf(_T("%s is not installed\n"), m_ServiceName.c_str());
            } else {
                // Try and remove the copy that's installed
                if (Uninstall()) {
                    // Get the executable file path
                    TCHAR szFilePath[_MAX_PATH];
                    PAL_absclass::PAL_getModuleFileName(NULL, szFilePath, sizeof(szFilePath)/sizeof(TCHAR));
                    _tprintf(_T("%s removed. (You must delete the file (%s) yourself.)\n"),
                            m_ServiceName.c_str(), szFilePath);
                } else {
                    _tprintf(_T("Could not remove %s. Error %d\n"), m_ServiceName.c_str(), PAL_absclass::PAL_getLastError());
                }
            }
            return TRUE; // say we processed the argument
        }
        else if (myStrCmp(argv[1], "-r") == 0)
        {
            // Request to run
            _tprintf( _T("Running as a command, not a service.\n") );
            Run();
            return TRUE; // say we processed the argument                   
        }

        // Don't recognise the args
        return FALSE;
    }
    catch(...)
    {
        LogCatch;
        return FALSE;
    }
}

////////////////////////////////////////////////////////////////////////////////////////
// Install/uninstall routines

// Test if the service is currently installed
bool Util::CNTService::IsInstalled()
{
    try
    {
        bool bResult = FALSE;

        // Open the Service Control Manager
        SC_HANDLE hSCM = ::OpenSCManager(NULL,  // local machine
                NULL,                                                           // ServicesActive database
                SC_MANAGER_ALL_ACCESS);                         // full access
        if (hSCM) {

            // Try to open the service
            SC_HANDLE hService = ::OpenService(hSCM,
                    m_ServiceName.c_str(),
                    SERVICE_QUERY_CONFIG);
            if (hService) {
                bResult = TRUE;
                ::CloseServiceHandle(hService);
            }

            ::CloseServiceHandle(hSCM);
        }

        return bResult;
    }
    catch(...)
    {
        LogCatchAndThrow;
    }
}

// Install the service
bool Util::CNTService::Install(char * UNUSED(pAccount), char * UNUSED(pPassword))
{
	PAL_absclass::PAL_OutputDbgStr(L"CNTService:Install: entry\r\n");
    try
    {

        TCHAR pwAccount[MAX_ACCOUNT_PASSWORD_SIZE+1];
        TCHAR pwPassword[MAX_ACCOUNT_PASSWORD_SIZE+1];
        if (mbstowcs(pwAccount, pAccount, MAX_ACCOUNT_PASSWORD_SIZE) == -1)     return FALSE;
        if (mbstowcs(pwPassword, pPassword, MAX_ACCOUNT_PASSWORD_SIZE) == -1)   return FALSE;

        // Open the Service Control Manager
        SC_HANDLE hSCM = ::OpenSCManager(NULL,  // local machine
                NULL,                                                           // ServicesActive database
                SC_MANAGER_ALL_ACCESS);                         // full access
        if (!hSCM) 
		{
			PAL_absclass::PAL_OutputDbgStr(L"CNTService:Install: can't open SCM, fail\r\n");
			return false;
		}

        // Get the executable file path
        TCHAR szFilePath[_MAX_PATH];
        if(     ::GetModuleFileName(NULL, szFilePath, sizeof(szFilePath)/sizeof(TCHAR))==0)
        {
            ::CloseServiceHandle(hSCM);
			PAL_absclass::PAL_OutputDbgStr(L"CNTService:Install: can't get module file name, fail\r\n");
            return false;
        }

        // Create the service
        SC_HANDLE hService = ::CreateService(
                hSCM,                                           // handle to SCM database
                m_ServiceName.c_str(),          // name of service to start
                m_ServiceName.c_str(),          // display name
                SERVICE_ALL_ACCESS,                     // type of access
                SERVICE_WIN32_OWN_PROCESS,      // type of service
                SERVICE_DEMAND_START,           // when to start
                SERVICE_ERROR_NORMAL,           // severity if service fails to start
                szFilePath,                                     // path to binary file
                NULL,
                NULL,
                NULL,
                pwAccount,
                pwPassword);
        if (!hService) {
            ::CloseServiceHandle(hSCM);
			PAL_absclass::PAL_OutputDbgStr(L"CNTService:Install: can't start service, fail\r\n");
            return false;
        }

        // make registry entries to support logging messages
        // Add the source name as a subkey under the Application
        // key in the EventLog service portion of the registry.
        TCHAR szKey[256];
        HKEY hKey = NULL;
        _tcscpy(szKey, _T("SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\"));
        _tcscat(szKey, m_ServiceName.c_str());
        if (::RegCreateKey(HKEY_LOCAL_MACHINE, szKey, &hKey) != ERROR_SUCCESS) {
            ::CloseServiceHandle(hService);
            ::CloseServiceHandle(hSCM);
			PAL_absclass::PAL_OutputDbgStr(L"CNTService:Install: can't create reg keys, fail\r\n");
            return false;
        }

        // Add the Event ID message-file name to the 'EventMessageFile' subkey.
        ::RegSetValueEx(hKey,
                _T("EventMessageFile"),
                0,
                REG_EXPAND_SZ, 
                (CONST BYTE*)szFilePath,
                (DWORD)(_tcsclen(szFilePath) * 2 + 2));

        // Set the supported types flags.
        DWORD dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE;
        ::RegSetValueEx(hKey,
                _T("TypesSupported"),
                0,
                REG_DWORD,
                (CONST BYTE*)&dwData,
                sizeof(DWORD));
        ::RegCloseKey(hKey);

        //LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_INSTALLED, m_szServiceName);

        // tidy up
        ::CloseServiceHandle(hService);
        ::CloseServiceHandle(hSCM);

		PAL_absclass::PAL_OutputDbgStr(L"CNTService:Install: exit, success\r\n");
		return true;
    }
    catch(...)
    {
		PAL_absclass::PAL_OutputDbgStr(L"CNTService:Install: exception, fail\r\n");
        LogCatchAndThrow;
    }
	PAL_absclass::PAL_OutputDbgStr(L"CNTService:Install: exit, fail\r\n");
	return false;
}

// Uninstall the service
bool Util::CNTService::Uninstall()
{
	PAL_absclass::PAL_OutputDbgStr(L"CNTService:Uninstall: entry\r\n");
    try
    {

		// Open the Service Control Manager
        SC_HANDLE hSCM = ::OpenSCManager(NULL,  // local machine
                NULL,                                                           // ServicesActive database
                SC_MANAGER_ALL_ACCESS);                         // full access
        if (!hSCM) 
		{
			PAL_absclass::PAL_OutputDbgStr(L"CNTService:Uninstall: can't open SCM, fail\r\n");
			return false;
		}

        bool bResult = false;
        SC_HANDLE hService = ::OpenService(hSCM,
                m_ServiceName.c_str(),
                DELETE);
        if (hService) {
            if (::DeleteService(hService)) {
                //LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_REMOVED, m_szServiceName);
                bResult = TRUE;
            } else {
                //LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_NOTREMOVED, m_szServiceName);
            }
            ::CloseServiceHandle(hService);
        }

        ::CloseServiceHandle(hSCM);
		PAL_absclass::PAL_OutputDbgStr( bResult ? 
			L"CNTService:Uninstall: exit, success\r\n" :
			L"CNTService:Uninstall: exit, fail\r\n" 
		);

        return bResult;

	}
    catch(...)
    {
		PAL_absclass::PAL_OutputDbgStr(L"CNTService:Uninstall: exception, fail\r\n");
        LogCatchAndThrow;
    }
	PAL_absclass::PAL_OutputDbgStr(L"CNTService:Uninstall: exit, fail\r\n");
	return false;
}


//////////////////////////////////////////////////////////////////////////////////////////////
// Service startup and registration

bool Util::CNTService::StartService()
{
	PAL_absclass::PAL_OutputDbgStr(L"CNTService:StartService: entry\r\n");
    try
    {
        SERVICE_TABLE_ENTRY st[] = {
            {(TCHAR *)m_ServiceName.c_str(), (LPSERVICE_MAIN_FUNCTION) ServiceMain},
            {NULL, NULL}
        };

        // Publish the service table to the Service Control Manager.
        // Return the result of ::StartServiceCtrlDispatcher(st);

        BOOL b = ::StartServiceCtrlDispatcher(st);
        if ( b == false )
        {
            // Fetch the Error message string
            VOID  * lpMsgBuf;
            DWORD error_code = GetLastError();
            FormatMessage( 
                    FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                    FORMAT_MESSAGE_FROM_SYSTEM | 
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                    error_code,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                    (LPTSTR) &lpMsgBuf,
                    0,
                    NULL 
                    );
            wchar_t wstr[MAX_PATH];
            wsprintf(wstr, L"CNTService:StartService: error %u '%s'\r\n",error_code, lpMsgBuf);
			PAL_absclass::PAL_OutputDbgStr(wstr);
            LocalFree(lpMsgBuf);
        }
		if (b)
		{
			PAL_absclass::PAL_OutputDbgStr(L"CNTService:StartService: exit, success\r\n");
			return true;
		}
    }
    catch(...)
    {
		PAL_absclass::PAL_OutputDbgStr(L"CNTService:StartService: exception\r\n");
        LogCatch;
    }
	PAL_absclass::PAL_OutputDbgStr(L"CNTService:StartService: exit, fail\r\n");
    return FALSE;
}

// static member function (callback)
// Entry point of the service. 
// Called by Service Control Manager when the service is started.
void Util::CNTService::ServiceMain(DWORD UNUSED(argc), LPTSTR* UNUSED(argv))
{
	PAL_absclass::PAL_OutputDbgStr(L"CNTService:ServiceMain: entry\r\n");

    try
    {
        // Get a pointer to the C++ object
        CNTService* pService = ms_pThis;

        PAL_absclass::PAL_OutputDbgStr(L"CNTService::ServiceMain():  arguments : \r\n");
        for(int i=0; i < (int)argc; i++)
        {
            pService->m_ServiceArguments.push_back( std::wstring(argv[i]) );
#ifdef _DEBUG
            PAL_absclass::PAL_OutputDbgStr(pService->m_ServiceArguments[i].c_str());
            PAL_absclass::PAL_OutputDbgStr(L"\r\n");
#endif
        }

        // Register the control request handler
        pService->m_Status.dwCurrentState = SERVICE_START_PENDING;
        pService->m_hServiceStatus = RegisterServiceCtrlHandlerEx(
            pService->m_ServiceName.c_str(),
            (LPHANDLER_FUNCTION_EX) HandlerEx, 
            (LPVOID) ms_pThis);

        if (pService->m_hServiceStatus == NULL) 
		{
            //pService->LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_CTRLHANDLERNOTINSTALLED);
            PAL_absclass::PAL_OutputDbgStr(L"CNTService::ServiceMain():  failed to register control handler.\r\n");
            return;
        }


        // Start the initialisation
        if (pService->Initialize()) 
        {
            // Do the real work. 
            // When the Run function returns, the service has stopped.
            pService->m_Status.dwWin32ExitCode = 0;
            pService->Run();
            PAL_absclass::PAL_OutputDbgStr(L"CNTService::ServiceMain(): returned from Run()\r\n");
        }
        else
        {
            PAL_absclass::PAL_OutputDbgStr(L"CNTService::ServiceMain(): Initialize() failed\r\n");
        }

        PAL_absclass::PAL_OutputDbgStr(L"CNTService::ServiceMain(): setting state to SERVICE_STOPPED.\r\n");
        // Tell the service manager we are stopped
        pService->SetStatus(SERVICE_STOPPED);
    }
    catch(...)
    {
        PAL_absclass::PAL_OutputDbgStr(L"CNTService::ServiceMain(): exception, setting state to SERVICE_STOPPED.\r\n");
        ms_pThis->SetStatus(SERVICE_STOPPED);
        LogCatch;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
// status functions

void Util::CNTService::SetStatusError(DWORD dwError)
{
    //DebugMsg(L"CNTService::SetStatusError(%lu)\r\n", dwError);
    SetStatusEx(
        m_Status.dwCurrentState,
        m_Status.dwWin32ExitCode = dwError,
        m_Status.dwServiceSpecificExitCode = 0,
        m_Status.dwCheckPoint = 0,
        m_Status.dwWaitHint = dwShutdownHint);
}

void Util::CNTService::SetStatus(DWORD dwState)
{
    //DebugMsg(L"CNTService::SetStatus(%lu)\r\n", dwState);
    SetStatusEx(
        dwState,
        m_Status.dwWin32ExitCode = NO_ERROR,
        m_Status.dwServiceSpecificExitCode = 0,
        m_Status.dwCheckPoint = 0,
        m_Status.dwWaitHint = dwShutdownHint);
}

void Util::CNTService::SetStatusEx(
    DWORD dwState, 
    DWORD dwWin32ExitCode, 
    DWORD dwServiceSpecificExitCode, 
    DWORD dwCheckPoint,
    DWORD dwWaitHint
    )
{
    try
    {
        m_Status.dwCurrentState = dwState;
        m_Status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;

        if (m_Status.dwCurrentState == SERVICE_START_PENDING)
        {
            m_Status.dwControlsAccepted = 0;
        }
        else
        {
            m_Status.dwControlsAccepted =
                SERVICE_ACCEPT_STOP |
                //SERVICE_ACCEPT_PAUSE_CONTINUE |
                SERVICE_ACCEPT_SHUTDOWN;
        }

        // if a specific exit code is defined, 
        // set up the Win32 exit code
        if (m_Status.dwServiceSpecificExitCode == 0)
        {
            m_Status.dwWin32ExitCode = dwWin32ExitCode;
        }
        else
        {
            m_Status.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
        }
        m_Status.dwServiceSpecificExitCode = dwServiceSpecificExitCode;
        m_Status.dwCheckPoint = dwCheckPoint;
        m_Status.dwWaitHint = dwWaitHint;

#if 0 //def _DEBUG
        DebugMsg(L"CNTService:SetStatusEx: dwCurrentState            %u\r\n",m_Status.dwCurrentState);
        DebugMsg(L"CNTService:SetStatusEx: dwWin32ExitCode           %u\r\n",m_Status.dwWin32ExitCode);
        DebugMsg(L"CNTService:SetStatusEx: dwServiceSpecificExitCode %u\r\n",m_Status.dwServiceSpecificExitCode);
        DebugMsg(L"CNTService:SetStatusEx: dwCheckPoint              %u\r\n",m_Status.dwCheckPoint);
        DebugMsg(L"CNTService:SetStatusEx: dwWaitHint                %u\r\n",m_Status.dwWaitHint);
#endif
        // Pass the status record to the SCM
        BOOL success = ::SetServiceStatus (m_hServiceStatus, &m_Status);
        if (!success)
        {
        	PAL_absclass::PAL_OutputDbgStr(L"CNTService:SetStatusEx: SetServiceStatus failed!\r\n");
        }
    }
    catch(...)
    {
    	PAL_absclass::PAL_OutputDbgStr(L"CNTService:SetStatusEx: exception\r\n");
        LogCatchAndThrow;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
// Service initialization

bool Util::CNTService::Initialize()
{
	PAL_absclass::PAL_OutputDbgStr(L"CNTService:Initialize: entry\r\n");
    try
    {
        // Start the initialization
        SetStatus(SERVICE_START_PENDING);

        // Perform the actual initialization
        bool bResult = OnInit(); 

        // Set final state
        m_Status.dwWin32ExitCode = GetLastError();

        if (!bResult) {
            //LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_FAILEDINIT);
			PAL_absclass::PAL_OutputDbgStr(L"CNTService::ServiceInitialize(). OnInit failed. setting state to SERVICE_STOPPED.\r\n");
            SetStatus(SERVICE_STOPPED);
            return false;    
        }

        //LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_STARTED);
        SetStatus(SERVICE_RUNNING);

		PAL_absclass::PAL_OutputDbgStr(L"CNTService:ServiceInitialize: success\r\n");
        return true;
    }
    catch(...)
    {
		PAL_absclass::PAL_OutputDbgStr(L"CNTService:ServiceInitialize: exception\r\n");
        LogCatchAndThrow;
    }
	PAL_absclass::PAL_OutputDbgStr(L"CNTService:ServiceInitialize: fail\r\n");
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// main function to do the real work of the service

// This function performs the main work of the service. 
// When this function returns the service has stopped.
void Util::CNTService::Run()
{
	PAL_absclass::PAL_OutputDbgStr(L"CNTService::Run: entry\r\n");
    try
    {
        PAL_absclass::PAL_waitForSingleObj (ms_ShutdownEvent.GetHandle(), INFINITE);
		PAL_absclass::PAL_OutputDbgStr(L"CNTService::Run: shutdown event signaled.\r\n");

        // nothing more to do
    }
    catch(...)
    {
		PAL_absclass::PAL_OutputDbgStr(L"CNTService::Run(): exception.\r\n");
        LogCatchAndThrow;
    }
	PAL_absclass::PAL_OutputDbgStr(L"CNTService::Run(): exit\r\n");
}

//////////////////////////////////////////////////////////////////////////////////////
// Control request handlers

// static member function (callback) to handle commands from the
// service control manager
DWORD Util::CNTService::HandlerEx(
        DWORD UNUSED(i_Opcode),
        DWORD UNUSED(i_EventType),
        LPVOID UNUSED(i_EventData),
        LPVOID UNUSED(i_Context ))
{
    DWORD dwError = NO_ERROR;

    try
    {
        // Get a pointer to the object
        CNTService* pService = ms_pThis;

        //pService->DebugMsg(L"CNTService::HandlerEx(%lu)\r\n", i_Opcode);

        switch (i_Opcode) {
            case SERVICE_CONTROL_STOP: // 1
                pService->SetStatusEx(SERVICE_STOP_PENDING, NO_ERROR, 0, 1, dwShutdownHint);
                pService->OnStop();
                SetEvent (pService->ms_ShutdownEvent.GetHandle());
                // ServiceMain sets status to SERVICE_STOPPED,
                // so don't call SetStatus here!

#ifdef ENABLE_SERVICE_KILLER
                g_the_timebomb = new Util::CServiceKiller(120000); //set the time bomb to detonate in 2 minutes...
#endif
                break;

            //case SERVICE_CONTROL_PAUSE: // 2
            //    pService->OnPause();
            //    pService->SetStatusError(dwError = NO_ERROR);
            //    break;

            //case SERVICE_CONTROL_CONTINUE: // 3
            //    pService->OnContinue();
            //    pService->SetStatusError(dwError = NO_ERROR);
            //    break;

            case SERVICE_CONTROL_INTERROGATE: // 4
                pService->OnInterrogate();
                pService->SetStatusError(dwError = NO_ERROR);
                break;

            case SERVICE_CONTROL_SHUTDOWN: // 5
                pService->SetStatusEx(SERVICE_STOP_PENDING, NO_ERROR, 0, 1, dwShutdownHint);
                pService->OnShutdown();
                SetEvent (pService->ms_ShutdownEvent.GetHandle());
                // ServiceMain sets status to SERVICE_STOPPED,
                // so don't call SetStatus here!
                break;

            case SERVICE_CONTROL_DEVICEEVENT: // 0x0B
                pService->OnDeviceEvent(i_EventType, i_EventData, i_Context );
                pService->SetStatusError(NO_ERROR);
                break;

            default:
                if (i_Opcode >= SERVICE_ABORT) 
				{
                    if (!pService->OnAbort()) 
					{
                        //pService->LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_BADREQUEST);
                    }
                } 
                else 
				{
                    //pService->LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_BADREQUEST);
                    pService->SetStatusError(dwError = ERROR_CALL_NOT_IMPLEMENTED);
                }
                break;
        }
    }
    catch(...)
    {
    	PAL_absclass::PAL_OutputDbgStr(L"CNTService::HandlerEx: exception caught\r\n");
        LogCatch;
    }

//Return Value
//
//The return value for this function depends on the control code received.
//
//The following list identifies the rules for this return value:
//
// In general, if your service does not handle the control, 
// return ERROR_CALL_NOT_IMPLEMENTED. However, your service should 
// return NO_ERROR for SERVICE_CONTROL_INTERROGATE even if your service 
// does not handle it.
//
// If your service handles SERVICE_CONTROL_STOP 
// or SERVICE_CONTROL_SHUTDOWN, return NO_ERROR.
//
// If your service handles SERVICE_CONTROL_DEVICEEVENT, 
// return NO_ERROR to grant the request and an error code to deny the request.
//
// If your service handles SERVICE_CONTROL_HARDWAREPROFILECHANGE, 
// return NO_ERROR to grant the request and an error code to deny the request.
//
// If your service handles SERVICE_CONTROL_POWEREVENT, 
// return NO_ERROR to grant the request and an error code to deny the request.
//
// For all other control codes your service handles, return NO_ERROR.

    return dwError;

}
// Return the service name
std::wstring Util::CNTService::GetServiceName()
{
    return ms_pThis->m_ServiceName;
}

// Called when the service is first initialized
bool Util::CNTService::OnInit()
{
	PAL_absclass::PAL_OutputDbgStr(L"CNTService::OnInit: entry\r\n");
	try
    {
		PAL_absclass::PAL_OutputDbgStr(L"CNTService::OnInit: success, exit\r\n");
        return true;
    }
    catch(...)
    {
		PAL_absclass::PAL_OutputDbgStr(L"CNTService::OnInit: exception\r\n");
        LogCatchAndThrow;
    }
	PAL_absclass::PAL_OutputDbgStr(L"CNTService::OnInit: fail, exit\r\n");
	return false;
}

// Called when the service control manager wants to stop the service
void Util::CNTService::OnStop()
{
	PAL_absclass::PAL_OutputDbgStr(L"CNTService::OnStop: entry\r\n");
    try
    {
		PAL_absclass::PAL_OutputDbgStr(L"CNTService::OnStop: no action taken\r\n");
    }
    catch(...)
    {
		PAL_absclass::PAL_OutputDbgStr(L"CNTService::OnStop: exception\r\n");
        LogCatchAndThrow;
    }
	PAL_absclass::PAL_OutputDbgStr(L"CNTService::OnStop: exit\r\n");
}

// called when the service is interrogated
void Util::CNTService::OnInterrogate()
{
	PAL_absclass::PAL_OutputDbgStr(L"CNTService::OnInterrogate: entry\r\n");
    try
    {
		PAL_absclass::PAL_OutputDbgStr(L"CNTService::OnInterrogate: no action taken\r\n");
    }
    catch(...)
    {
		PAL_absclass::PAL_OutputDbgStr(L"CNTService::OnInterrogate: exception\r\n");
        LogCatchAndThrow;
    }
	PAL_absclass::PAL_OutputDbgStr(L"CNTService::OnInterrogate: exit\r\n");
}

// called when the service is paused
void Util::CNTService::OnPause()
{
	PAL_absclass::PAL_OutputDbgStr(L"CNTService::OnPause: entry\r\n");
    try
    {
		PAL_absclass::PAL_OutputDbgStr(L"CNTService::OnPause: no action taken\r\n");
    }
    catch(...)
    {
		PAL_absclass::PAL_OutputDbgStr(L"CNTService::OnPause: exception\r\n");
        LogCatchAndThrow;
    }
	PAL_absclass::PAL_OutputDbgStr(L"CNTService::OnPause: exit\r\n");
}

// called when the service is continued
void Util::CNTService::OnContinue()
{
	PAL_absclass::PAL_OutputDbgStr(L"CNTService::OnContinue: entry\r\n");
    try
    {
		PAL_absclass::PAL_OutputDbgStr(L"CNTService::OnContinue: no action taken\r\n");
    }
    catch(...)
    {
		PAL_absclass::PAL_OutputDbgStr(L"CNTService::OnContinue: exception\r\n");
        LogCatchAndThrow;
    }
	PAL_absclass::PAL_OutputDbgStr(L"CNTService::OnContinue: exit\r\n");
}

// called when the service is shut down
void Util::CNTService::OnShutdown()
{
	PAL_absclass::PAL_OutputDbgStr(L"CNTService::OnShutdown: entry\r\n");
    try
    {
		PAL_absclass::PAL_OutputDbgStr(L"CNTService::OnShutdown: no action taken\r\n");
    }
    catch(...)
    {
		PAL_absclass::PAL_OutputDbgStr(L"CNTService::OnShutdown: exception\r\n");
        LogCatchAndThrow;
    }
	PAL_absclass::PAL_OutputDbgStr(L"CNTService::OnShutdown: exit\r\n");
}

void Util::CNTService::OnDeviceEvent(DWORD UNUSED(i_EventType), LPVOID UNUSED(i_EventData), LPVOID UNUSED(i_Context) )
{
	PAL_absclass::PAL_OutputDbgStr(L"CNTService::OnDeviceEvent: \r\n");
}

// called when the service gets a user control message
bool Util::CNTService::OnAbort()
{
	PAL_absclass::PAL_OutputDbgStr(L"CNTService::OnAbort: entry\r\n");
    try
    {
		PAL_absclass::PAL_OutputDbgStr(L"CNTService::OnAbort: calling ExitProcess(EXIT_FAILURE)\r\n");
        // End the process
        ExitProcess (EXIT_FAILURE);
		PAL_absclass::PAL_OutputDbgStr(L"CNTService::OnAbort: ExitProcess returned (??), fail \r\n");
        return false;
    }
    catch(...)
    {
		PAL_absclass::PAL_OutputDbgStr(L"CNTService::OnAbort: exception\r\n");
        LogCatchAndThrow;
    }
	PAL_absclass::PAL_OutputDbgStr(L"CNTService::OnAbort: exit, fail \r\n");
	return false;
}

void Util::CNTService::ReportVersionInfoToErrorLog()
{
	PAL_absclass::PAL_OutputDbgStr(L"CNTService::ReportVersionInfoToErrorLog: entry \r\n");
    try 
    { 
        // The GetCurrentDirectory function returns the windows system directory
        // for a service, we want the error log routed to the sams directory
        wchar_t wbuff[_MAX_PATH]=L"TimsTest";

        PAL_HMODULE hmod = PAL_absclass::PAL_getModuleHandle(NULL);
        Assert( NULL != hmod , Util::CPlatformException());
        Assert( 0 != PAL_absclass::PAL_getModuleFileName(hmod, wbuff, _MAX_PATH), Util::CPlatformException());
        wchar_t wdrive[_MAX_DRIVE], wdir[_MAX_DIR],wname[_MAX_FNAME];
        _wsplitpath(wbuff,wdrive,wdir,wname,NULL);
        std::wstring wpath(wdrive);
        wpath += wdir;
        wpath[wpath.size() - 1] = 0;
        // Make sure the path is set up

        Util::COutputLogger & out = Util::COutputLogger::GetErrorLogger(wpath.c_str(),wname,true);

        Util::CVersionInfo info(wbuff);

        std::wstring wvers = wname;
        wvers += L" Version : ";
        wvers += info.Version();
        if( !info.SpecialBuildDescription().empty() )
        {
            wvers += L" Special Build : ";
            wvers += info.SpecialBuildDescription();
        }
        else
        {
            wvers += L" Standard Build";
        }
        Util::COutputLogger::ReportToErrorLog(wvers.c_str(),L"");
    } 
    catch(...) 
    { 
		PAL_absclass::PAL_OutputDbgStr(L"CNTService::ReportVersionInfoToErrorLog: exception\r\n");
        LogCatchAndThrow; 
    } 
	PAL_absclass::PAL_OutputDbgStr(L"CNTService::ReportVersionInfoToErrorLog: exit \r\n");
}

////////////////////////////////////////////////////////////////////////////////////////////
// Debugging support
#if _DEBUG
void Util::CNTService::DebugMsg(const wchar_t* pszFormat, ...)
{
	try
	{
		wchar_t buf[1024];
		swprintf(buf, L"CNTService.TID %lu: ", GetCurrentThreadId());
		va_list arglist;
		va_start(arglist, pszFormat);
		vswprintf(&buf[wcslen(buf)], pszFormat, arglist);
		va_end(arglist);
		PAL_absclass::PAL_OutputDbgStr(buf);
	}
	catch(...)
	{
		LogCatchAndThrow;
	}
}

#endif
