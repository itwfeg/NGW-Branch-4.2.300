// Header file included by ANTLR UniLexer
#include "PAL_platformheader.h"

#if !defined ( __linux__ )
#include <tchar.h>
#endif      // __linux__

#include "Test.h"

#include "ErrorHandling.h"
#include "CSmartPtr.h"
#include "MsgTime.h"
#include "CSemStream.h"
#include "OutputLogger.h"
#include "CEvolMsgReceiver.h"
#include "CMessageCenter.h"

#include "time.h"
#include "CHobartTimer.h"
#include "Main.h"
#include "ServiceSpeaker.h"

#include "WeigherMode.h"
#include "MemShow.h"
#include "HfcWrapperConfig.h"
#include "CZip.h"
#if !defined ( __linux__ )
#include "DisplayUsbs.h"
#endif
#include "UsbManager.h"
#include "SystemReset.h"
#include "Ping.h"

#include "DBManager.h"
#include "SynchronizationManagerUnitTester.h"
#include "RendererManagerUnitTester.h"
#include "properties/properties.h"
#include "Trace.h"

#include "cgi/http/HTTPUnitTester.h"

#include "TransactionUnitTester.h"

using namespace Util;


namespace Test
{
    class DBTester
    {
        public:
            DBTester(const std::string& configFileName)
            {
                util_env::Properties configFile(configFileName);
                const std::string* property = configFile.getProperty("dbDefaultsFile");
                if ( property )
                {
                    dbConfiguration = new util_env::Properties(*property);
                    DBManager::setConfiguration(dbConfiguration);
                    DBManager::setDebug(Trace::getInstance(configFile));

                    dbManager = edb::DBManager::getInstance();
                    if ( dbManager )
                    {
                        try
                        {
                            dbManager->openDatabase();
                        }
                        catch ( edb::DBException& edb )
                        {
                            char str[255];
                            sprintf( str, "***** DBException: type = %d, info = '%s' *****\n", edb.getType(), edb.info().c_str() );
#ifdef LINUX_FIX
                            PAL_absclass::PAL_OutputDbgStrA( str );
#else
                            OutputDebugStringA( str );
#endif      // LINUX_FIX
                        }
                    }
                }
                else
                {
                    dbConfiguration = 0;
                    dbManager = 0;
                }
            }
            ~DBTester()
            {
                if ( dbManager )
                {
                    dbManager->closeDatabase();
                    edb::DBManager::destroyInstance(dbManager);
                }

                if ( dbConfiguration )
                    delete dbConfiguration;
            }

        private:
            util_env::Properties * dbConfiguration;
            edb::DBManager * dbManager;

    };


    class CHTTPTester : public CUnitTest
    {
        public:
            CHTTPTester() {}
            ~CHTTPTester() {}
            const TCHAR* Name() { return _T("Test HTTP"); }
            const TCHAR* Description() { return _T(""); }

            bool TestCode()
            {
                util_env::Properties configFile(DEFAULT_INTERNAL_CONFIG_FILE_NAME);
                Trace::getInstance(configFile);
                comm::HTTPUnitTester tester;

                if ( !tester.testHTTPURIParsing() )
                    return false;

                if ( !tester.testHTTPURIQueries() )
                    return false;

                if ( !tester.testHTTPResponse() )
                    return false;

                return tester.testHTTPClient();
            }
    };


    class CTransactionUnitTester : public CUnitTest
    {
        public:
            CTransactionUnitTester() {}
            ~CTransactionUnitTester() {}
            const TCHAR* Name() { return _T("Test Transactions"); }
            const TCHAR* Description() { return _T(""); }

            bool TestCode()
            {
                try
                {
                    DBTester dbtester(DEFAULT_INTERNAL_CONFIG_FILE_NAME);

                    hfc::TransactionUnitTester ut;

                    return ut.runTests();
                }
                catch ( const edb::DBException& edb )
                {
                    Debug::trace("DBTester", "***** DBException: type=" +
                            util_string::inttostring(edb.getType()) + " info='" + edb.info() + "'");

                    return false;
                }
                catch ( ... )
                {
                    LogCatch
                        return false;     // an exception was thrown, consider this a failure
                }
            }
    };


    class CPrimarySynchronizationManagerTester : public CUnitTest
    {
        public:
            CPrimarySynchronizationManagerTester() {}
            ~CPrimarySynchronizationManagerTester() {}
            const TCHAR* Name() { return _T("Test Primary Synchronization Manager"); }
            const TCHAR* Description() { return _T(""); }

            bool TestCode()
            {
                try
                {
                    DBTester dbtester(DEFAULT_INTERNAL_CONFIG_FILE_NAME);

                    synchronization::SynchronizationManagerUnitTester smut;

                    return smut.runTestsPrimary();
                }
                catch ( const edb::DBException& edb )
                {
                    Debug::trace("DBTester", "***** DBException: type=" +
                            util_string::inttostring(edb.getType()) + " info='" + edb.info() + "'");

                    return false;
                }
                catch ( ... )
                {
                    LogCatch
                        return false;     // an exception was thrown, consider this a failure
                }
            }
    };


    class CSecondarySynchronizationManagerTester : public CUnitTest
    {
        public:
            CSecondarySynchronizationManagerTester() {}
            ~CSecondarySynchronizationManagerTester() {}
            const TCHAR* Name() { return _T("Test Secondary Synchronization Manager"); }
            const TCHAR* Description() { return _T(""); }

            bool TestCode()
            {
                try
                {
                    DBTester dbtester(DEFAULT_INTERNAL_CONFIG_FILE_NAME);

                    synchronization::SynchronizationManagerUnitTester smut;

                    return smut.runTestsSecondary();
                }
                catch ( const edb::DBException& edb )
                {
                    Debug::trace("DBTester", "***** DBException: type=" +
                            util_string::inttostring(edb.getType()) + " info='" + edb.info() + "'");

                    return false;
                }
                catch ( ... )
                {
                    LogCatch
                        return false;     // an exception was thrown, consider this a failure
                }
            }
    };


    bool validateVirtualKeyboard(HWND UNUSED(hwnd))
    {
#if !defined ( __linux__ )
        /* We should not have any race condition with these static variablee since the
           user interface message that triggers the call to this function only comes from
           the primary console user interface, which is unique. */
        static TCHAR className[1024];
        static WINDOWINFO windowInfo = { sizeof(WINDOWINFO) };

        if ( IsWindow(hwnd) )
        {
            if ( GetClassName(hwnd, className, 1024) )
            {
                if ( _tcsnccmp(className, _T("ThunderRT6FormDC"), 16) == 0 )
                {
                    if ( GetWindowInfo(hwnd, &windowInfo) )
                        return (windowInfo.dwStyle & WS_CAPTION) == WS_CAPTION;
                    else
                        Debug::trace("Session.virtualKeyboard", "GetWindowInfo failed (error = " +
                                util_string::uinttostring(GetLastError()) + ")");
                }
            }
            else
                Debug::trace("Session.virtualKeyboard", "GetClassName failed (error = " +
                        util_string::uinttostring(GetLastError()) + ")");
        }
#endif      // __linux__
        return false;
    }


    BOOL CALLBACK WindowEnumFindVirtualKeyboard(HWND UNUSED(hwnd), LPARAM UNUSED(lParam))
    {
#if !defined ( __linux__ )
        if ( validateVirtualKeyboard(hwnd) )
        {
            *(HWND*)lParam = hwnd;

            SetLastError(ERROR_SUCCESS);

            return FALSE;
        }
#endif      // __linux__
        return TRUE;
    }


    HWND findVirtualKeyboard()
    {
        /* We should not have any race condition with this static variable since the user
           interface message that triggers the call to this function only comes from the
           primary console user interface, which is unique. */
        static HWND handler = NULL;

#if !defined ( __linux__ )
        if ( handler != NULL && validateVirtualKeyboard(handler) )
            return handler;

        handler = NULL;
        if ( !EnumWindows(&WindowEnumFindVirtualKeyboard, (LPARAM)&handler) )
        {
            DWORD error = GetLastError();
            if ( error != ERROR_SUCCESS )
                Debug::trace("Session.virtualKeyboard", "EnumWindows failed (possible error = " +
                        util_string::uinttostring(GetLastError()) + ")");
        }
#endif      // __linux__
        return handler;
    }


#if !defined ( __linux__ )
    class CWindowTester : public CUnitTest
    {
        public:
            CWindowTester() {}
            ~CWindowTester() {}
            const TCHAR* Name() { return _T("Test Windows"); }
            const TCHAR* Description() { return _T(""); }

            bool TestCode()
            {
#if !defined ( __linux__ )
                try
                {
                    EnumWindows( (WNDENUMPROC) &EnumWindowsProc, NULL );

                    //                          HWND hwnd = FindWindow(_T("ThunderRT6FormDC"), NULL);
                    HWND hwnd = findVirtualKeyboard();
                    if ( hwnd != NULL )
                    {
                        OutputDebugStrings( _T("**** Found Window of class ThunderRT6FormDC\n") );

                        ShowWindowDebug(hwnd);

                        while ( 1 )
                        {
                            if ( IsIconic(hwnd) )
                            {
                                WINDOWPLACEMENT windowPlacement = { sizeof(WINDOWPLACEMENT) };

                                OutputDebugStrings( _T("   It is iconic. Restoring it!\n") );
                                // All methods cause the Keyboard window, after manually sending
                                // it down, to come up without focus (only the first time)
                                //                                                      OpenIcon(hwnd);
                                GetWindowPlacement( hwnd, &windowPlacement );
                                windowPlacement.flags = WPF_ASYNCWINDOWPLACEMENT;
                                windowPlacement.showCmd = SW_RESTORE;
                                windowPlacement.rcNormalPosition.left = (800 - 777) / 2; //474;
                                windowPlacement.rcNormalPosition.top  = 600 - 215 - 50; //723;
                                windowPlacement.rcNormalPosition.right = /*474 +*/ 777 - 1;
                                windowPlacement.rcNormalPosition.bottom = /*723 +*/ 215 - 1;
                                SetWindowPlacement( hwnd, &windowPlacement );
                                //                                                      ShowWindow( hwnd, SW_RESTORE );
                                //                                                      ShowWindow( hwnd, SW_SHOWDEFAULT );
                                //                                                      ShowWindow( hwnd, SW_SHOWNOACTIVATE );
                            }
                            else
                            {
                                OutputDebugStrings( _T("   It is not iconic. Iconizing it!\n") );
                                //                                                      ShowWindow( hwnd, SW_SHOWMINNOACTIVE);
                                // With this method it appears that the window underneath
                                // receives focus, even if it did not appear to have it before.
                                ShowWindow( hwnd, SW_MINIMIZE );
                                // The following method cause the Keyboard window to close (???)
                                //                                                      ShowWindow( hwnd, SW_FORCEMINIMIZE );
                            }

                            Sleep(10*1000);
                            break;
                        }
                    }
                    else
                    {
                        hwnd = FindWindowEx( NULL, NULL, NULL, _T("Click-N-Type"));
                        if ( hwnd != NULL )
                        {
                            OutputDebugStrings( _T("**** Found Window of name Click-N-Type\n") );

                            ShowWindowDebug(hwnd);
                        }
                        else
                            OutputDebugStrings( _T("!!!! Did not find Window of class ThunderRT6FormDC or name Click-N-Type\n") );
                    }

                    return true;
                }
                catch ( ... )
                {
                    LogCatch
                        return false;     // an exception was thrown, consider this a failure
                }
#else
                return false;
#endif      // __linux__
            }

            static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
            {
                static int counter = 0;

                OutputDebugStrings( _T("% 4.4d *** New Window ***\n"), ++counter );

                ShowWindowDebug(hwnd);

                return TRUE;
            }


            static void ShowWindowDebug(HWND hwnd)
            {
                TCHAR* text = 0, className[1024];
                WINDOWINFO windowInfo = { sizeof(WINDOWINFO) };
                WINDOWPLACEMENT windowPlacement = { sizeof(WINDOWPLACEMENT) };
                int textSize = GetWindowTextLength(hwnd);

                if ( GetClassName(hwnd, className, 1024) )
                {
                    OutputDebugStrings( _T("     Window Class: %s\n"), className);
                }
                else
                    ReportError(_T("Calling GetClassName"));

                if ( textSize )
                {
                    text = new TCHAR[textSize + 1];
                    GetWindowText(hwnd, text, textSize + 1);
                    OutputDebugStrings( _T("     Window Text: %s\n"), text );
                }
                else
                    OutputDebugStrings( _T("     Window Text: ** NO NAME **\n") );

                if ( GetWindowInfo(hwnd, &windowInfo) )
                {
                    OutputDebugStrings(_T("     Window Style: %s\n"), WindowStyleText(windowInfo.dwStyle));
                    OutputDebugStrings(_T("     Window ExStyle: %s\n"), WindowsExStyleText(windowInfo.dwExStyle));
                    OutputDebugStrings(_T("     Window Status: %d (%s)\n"), windowInfo.dwWindowStatus,
                            (windowInfo.dwWindowStatus == WS_ACTIVECAPTION ? _T("active") : _T("inactive")));
                    OutputDebugStrings(_T("     Window Type: %d\n"), windowInfo.atomWindowType);
                }
                else
                    ReportError(_T("Calling GetWindowInfo"));

                if ( GetWindowPlacement(hwnd, &windowPlacement) )
                {
                    OutputDebugStrings(_T("     Window Showing: %s\n"), WindowShowingText(windowPlacement.showCmd));
                    OutputDebugStrings(_T("     Window Minimized Position: %d, %d\n"), windowPlacement.ptMinPosition.x, windowPlacement.ptMinPosition.y);
                    OutputDebugStrings(_T("     Window Maximized Position: %d, %d\n"), windowPlacement.ptMaxPosition.x, windowPlacement.ptMaxPosition.y);
                    OutputDebugStrings(_T("     Window Normal Position: %d, %d\n"), windowPlacement.rcNormalPosition.left, windowPlacement.rcNormalPosition.top);
                    OutputDebugStrings(_T("     Window Normal Size: %d, %d\n"), windowPlacement.rcNormalPosition.right - windowPlacement.rcNormalPosition.left,
                            windowPlacement.rcNormalPosition.bottom - windowPlacement.rcNormalPosition.top);
                }
                else
                    ReportError(_T("Calling GetWindowPlacement"));

                if ( IsWindow(hwnd) )
                    OutputDebugStrings( _T("     Window is Window\n"));
                else
                    OutputDebugStrings( _T("     Window is not Window\n"));

                if ( IsWindowUnicode(hwnd) )
                    OutputDebugStrings( _T("     Window is Visible Window\n"));
                else
                    OutputDebugStrings( _T("     Window is not Visible Window\n"));

                if ( IsWindowVisible(hwnd) )
                    OutputDebugStrings( _T("     Window is Unicode Window\n"));
                else
                    OutputDebugStrings( _T("     Window is not Unicode Window\n"));

                if ( IsIconic(hwnd) )
                    OutputDebugStrings( _T("     Window is Iconic\n"));
                else
                    OutputDebugStrings( _T("     Window is not Iconic\n"));

                if ( IsZoomed(hwnd) )
                    OutputDebugStrings( _T("     Window is Zoomed\n"));
                else
                    OutputDebugStrings( _T("     Window is not Zoomed\n"));

                if ( text )
                    delete[] text;
            }

            static TCHAR* WindowStyleText(DWORD style)
            {
                static TCHAR buffer[2048];

                buffer[0] = '\0';

                if ( (style & WS_OVERLAPPEDWINDOW) == WS_OVERLAPPEDWINDOW )
                    _tcscat(buffer, _T("OVERLAPPEDWINDOW"));
                if ( (style & WS_POPUPWINDOW) == WS_POPUPWINDOW )
                    _tcscat(buffer, _T("POPUPWINDOW"));
                if ( (style & WS_CAPTION) == WS_CAPTION )
                    _tcscat(buffer, _T("CAPTION | "));

                if ( style & WS_POPUP )
                    _tcscat(buffer, _T("POPUP | "));
                if ( style & WS_CHILD )
                    _tcscat(buffer, _T("CHILD | "));
                if ( style & WS_MINIMIZE )
                    _tcscat(buffer, _T("MINIMIZE(ICONIC) | "));
                if ( style & WS_VISIBLE )
                    _tcscat(buffer, _T("VISIBLE | "));

                if ( style & WS_DISABLED )
                    _tcscat(buffer, _T("DISABLED | "));
                if ( style & WS_CLIPSIBLINGS )
                    _tcscat(buffer, _T("CLIPSIBLINGS | "));
                if ( style & WS_CLIPCHILDREN )
                    _tcscat(buffer, _T("CLIPCHILDREN | "));
                if ( style & WS_MAXIMIZE )
                    _tcscat(buffer, _T("MAXIMIZE | "));

                if ( style & WS_BORDER )
                    _tcscat(buffer, _T("BORDER | "));
                if ( style & WS_DLGFRAME )
                    _tcscat(buffer, _T("DLGFRAME | "));
                if ( style & WS_VSCROLL )
                    _tcscat(buffer, _T("VSCROLL | "));
                if ( style & WS_HSCROLL )
                    _tcscat(buffer, _T("HSCROLL | "));

                if ( style & WS_SYSMENU )
                    _tcscat(buffer, _T("SYSMENU | "));
                if ( style & WS_THICKFRAME )
                    _tcscat(buffer, _T("THICKFRAME(SIZEBOX) | "));
                if ( style & WS_MINIMIZEBOX )
                    _tcscat(buffer, _T("MINIMIZEBOX | "));
                if ( style & WS_MAXIMIZEBOX )
                    _tcscat(buffer, _T("MAXIMIZEBOX | "));

                return buffer;
            }

            static TCHAR* WindowsExStyleText(DWORD style)
            {
                static TCHAR buffer[2048];

                buffer[0] = '\0';

                if ( (style & WS_EX_PALETTEWINDOW) == WS_EX_PALETTEWINDOW )
                    _tcscat(buffer, _T("PALETTEWINDOW"));
                if ( (style & WS_EX_OVERLAPPEDWINDOW) == WS_EX_OVERLAPPEDWINDOW )
                    _tcscat(buffer, _T("OVERLAPPEDWINDOW"));

                if ( style & WS_EX_DLGMODALFRAME )
                    _tcscat(buffer, _T("DLGMODALFRAME | "));
                if ( style & WS_EX_NOPARENTNOTIFY )
                    _tcscat(buffer, _T("NOPARENTNOTIFY | "));
                if ( style & WS_EX_TOPMOST )
                    _tcscat(buffer, _T("TOPMOST | "));

                if ( style & WS_EX_ACCEPTFILES )
                    _tcscat(buffer, _T("ACCEPTFILES | "));
                if ( style & WS_EX_TRANSPARENT )
                    _tcscat(buffer, _T("TRANSPARENT | "));
                if ( style & WS_EX_MDICHILD )
                    _tcscat(buffer, _T("MDICHILD | "));
                if ( style & WS_EX_TOOLWINDOW )
                    _tcscat(buffer, _T("TOOLWINDOW | "));

                if ( style & WS_EX_WINDOWEDGE )
                    _tcscat(buffer, _T("WINDOWEDGE | "));
                if ( style & WS_EX_CLIENTEDGE )
                    _tcscat(buffer, _T("CLIENTEDGE | "));
                if ( style & WS_EX_CONTEXTHELP )
                    _tcscat(buffer, _T("CONTEXTHELP | "));

                if ( style & WS_EX_RIGHT )
                    _tcscat(buffer, _T("RIGHT | "));
                if ( style & WS_EX_RTLREADING )
                    _tcscat(buffer, _T("RTLREADING | "));
                if ( style & WS_EX_LEFTSCROLLBAR )
                    _tcscat(buffer, _T("LEFTSCROLLBAR | "));

                if ( style & WS_EX_CONTROLPARENT )
                    _tcscat(buffer, _T("CONTROLPARENT | "));
                if ( style & WS_EX_STATICEDGE )
                    _tcscat(buffer, _T("STATICEDGE | "));
                if ( style & WS_EX_APPWINDOW )
                    _tcscat(buffer, _T("APPWINDOW | "));
                if ( style & WS_EX_LAYERED )
                    _tcscat(buffer, _T("LAYERED | "));

                if ( style & WS_EX_NOINHERITLAYOUT )
                    _tcscat(buffer, _T("NOINHERITLAYOUT | "));
                if ( style & WS_EX_LAYOUTRTL )
                    _tcscat(buffer, _T("LAYOUTRTL | "));

                if ( style & WS_EX_COMPOSITED )
                    _tcscat(buffer, _T("COMPOSITED | "));
                if ( style & WS_EX_NOACTIVATE )
                    _tcscat(buffer, _T("NOACTIVATE | "));

                return buffer;
            }


            static TCHAR* WindowShowingText(UINT show)
            {
                static TCHAR buffer[2048];

                buffer[0] = '\0';

                if ( show == SW_SHOWNORMAL )
                    _tcscat(buffer, _T("NORMAL"));
                else if ( show == SW_SHOWMINIMIZED )
                    _tcscat(buffer, _T("MINIMIZED"));
                else if ( show == SW_SHOWMAXIMIZED )
                    _tcscat(buffer, _T("MAXIMIZED"));
                else
                {
                    TCHAR number[34];

                    _tcscat(buffer, _T("unknown ("));
                    _ultot(show, number, 10);
                    _tcscat(buffer, number);
                    _tcscat(buffer, _T(")"));
                }

                return buffer;
            }


            static void OutputDebugStrings(TCHAR* format, ...)
            {
                TCHAR buffer[10*1024];
                va_list paramList;

                va_start(paramList, format);
                _vstprintf(buffer, format, paramList);

                OutputDebugString(buffer);
            }

            static void ReportError(LPCTSTR userMessage)
            {
                DWORD error = GetLastError();
                LPTSTR lpvSysMsg;

                if ( FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                            NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpvSysMsg, 0, NULL) )
                {
                    TCHAR str[255];
#if defined(LINUX_FIX)
                    swprintf(str, sizeof(str)/sizeof(TCHAR), _T("%s : error %d\n\t%s"), userMessage, error , lpvSysMsg);
#else
                    _stprintf( str, _T("%s : error %d\n\t%s"), userMessage, error , lpvSysMsg);
#endif
                    OutputDebugString( str );

                    HeapFree(GetProcessHeap(), 0, lpvSysMsg);
                }
            }
    };
#endif      // __linux__

    class CShutdownTester : public CUnitTest
    {
        public:
            CShutdownTester() {}
            ~CShutdownTester() {}
            const TCHAR* Name() { return _T("Test Shutting down"); }
            const TCHAR* Description() { return _T(""); }

            bool TestCode()
            {
                try
                {
#if !defined ( __linux__ )
                    bool error = false;
                    HANDLE hToken; 
                    TOKEN_PRIVILEGES tkp; 


                    // Open the process token to enable adjusting the process' privileges
                    if ( OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken) )
                    {
                        tkp.PrivilegeCount = 1; 
                        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
                        LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid); 

                        // Adjust the privileges to enable shutting down
                        if ( AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0) )
                        {
                            //                                                  ExitWindowsEx(EWX_SHUTDOWN | EWX_POWEROFF | EWX_FORCE, 0);
                            //                                                  ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0);
                            if ( !ExitWindowsEx(EWX_REBOOT, 0) )
                                error = true;
                            //                                                  InitiateSystemShutdownEx(NULL, NULL /*_T("User-requested Shutdown")*/, 100, TRUE, TRUE, 0);
                        }
                        else error = true;
                    }
                    else error = true;

                    if ( error )
                    {// if any error occurs the report to the user
                        int err = GetLastError();
                        LPVOID lpMsgBuf;
                        FormatMessage(    FORMAT_MESSAGE_ALLOCATE_BUFFER|
                                FORMAT_MESSAGE_FROM_SYSTEM, 
                                NULL,
                                err, 
                                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                (LPTSTR) & lpMsgBuf, 
                                0, 
                                NULL);
                        MessageBox(NULL, (LPTSTR)lpMsgBuf, _T("Shutdown"), MB_OK | MB_ICONERROR);
                        LocalFree(lpMsgBuf);

                        return false;
                    }


                    //                          if ( !InitiateSystemShutdownEx(NULL, NULL /*_T("User-requested Shutdown")*/, 10, TRUE, TRUE, 0) )
                    /*                          if ( !ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0) )
                                                {
                                                ReportError(_T("Failed to shutdown"));

                                                return false;
                                                }
                                                */
#endif      // __linux__
                    return true;
                }
                catch ( ... )
                {
                    LogCatch
                        return false;     // an exception was thrown, consider this a failure
                }
            }

            void ReportError(LPCTSTR UNUSED(userMessage))
            {
#ifdef LINUX_FIX
//                TODO: This ain't going to fly $TRS
//                DWORD error = PAL_absclass::PAL_getLastError();
//                LPTSTR lpvSysMsg;
//
//                if ( PAL_absclass::PAL_formatMessage(
//                            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
//                            NULL, 
//                            error, 
//                            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
//                            (LPTSTR)&lpvSysMsg, 
//                            0, 
//                            NULL) )
//                {
//                    TCHAR str[255];
//
//#if   defined(LINUX_FIX)
//                    swprintf(str, sizeof(str)/sizeof(TCHAR), _T("%s : error %d\n\t%s"), userMessage, error , lpvSysMsg);
//#else
//                    _stprintf( str, _T("%s : error %d\n\t%s"), userMessage, error , lpvSysMsg);
//#endif
//                    PAL_absclass::PAL_OutputDbgStr( str );
//
//                    //TODO: Linux function?
//                    //HeapFree(GetProcessHeap(), 0, lpvSysMsg);
//                }

#if 0
                PAL_absclass::PAL_OutputDbgStr( userMessage );
                PAL_absclass::PAL_OutputDbgStr( " : " );
                Util::LogAndThrow( userMessage, Util::CPlatformException(), _T(__FILE__), __LINE__ );
#endif // 0

#else
                DWORD error = GetLastError();
                LPTSTR lpvSysMsg;

                if ( FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                            NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpvSysMsg, 0, NULL) )
                {
                    TCHAR str[255];

#if defined(LINUX_FIX)
                    swprintf( str, sizeof(str)/sizeof(TCHAR), _T("%s : error %d\n\t%s"), userMessage, error , lpvSysMsg);
#else
                    _stprintf( str, _T("%s : error %d\n\t%s"), userMessage, error , lpvSysMsg);
#endif
                    OutputDebugString( str );

                    HeapFree(GetProcessHeap(), 0, lpvSysMsg);
                }
#endif      // LINUX_FIX
            }
    };

#if 0
    class CRendererManagerLineTester : public CUnitTest
    {
        public:
            CRendererManagerLineTester() {}
            ~CRendererManagerLineTester() {}
            const TCHAR* Name() { return _T("Test Line Rendering"); }
            const TCHAR* Description() { return _T(""); }

            bool TestCode()
            {
                try
                {
                    RendererManagerUnitTester rmut;
                    return rmut.runLineTests();
                }
                catch ( ... )
                {
                    LogCatch
                        return false;     // an exception was thrown, consider this a failure
                }
            }
    };
    class CRendererManagerBarcodeTester : public CUnitTest
    {
        public:
            CRendererManagerBarcodeTester() {}
            ~CRendererManagerBarcodeTester() {}
            const TCHAR* Name() { return _T("Test Barcode Rendering"); }
            const TCHAR* Description() { return _T(""); }

            bool TestCode()
            {
                try
                {
                    RendererManagerUnitTester rmut;
                    return rmut.runBarcodeTests();
                }
                catch ( ... )
                {
                    LogCatch
                        return false;     // an exception was thrown, consider this a failure
                }
            }
    };

    class CRendererManagerGraphicTester : public CUnitTest
    {
        public:
            CRendererManagerGraphicTester() {}
            ~CRendererManagerGraphicTester() {}
            const TCHAR* Name() { return _T("Test Graphic Rendering"); }
            const TCHAR* Description() { return _T(""); }

            bool TestCode()
            {
                try
                {
                    RendererManagerUnitTester rmut;
                    return rmut.runGraphicTests();
                }
                catch ( ... )
                {
                    LogCatch
                        return false;     // an exception was thrown, consider this a failure
                }
            }
    };

    class CRendererManagerFontTester : public CUnitTest
    {
        public:
            CRendererManagerFontTester() {}
            ~CRendererManagerFontTester() {}
            const TCHAR* Name() { return _T("Test Font Rendering"); }
            const TCHAR* Description() { return _T(""); }

            bool TestCode()
            {
                try
                {
                    RendererManagerUnitTester rmut;
                    return rmut.runFontTests();
                }
                catch ( ... )
                {
                    LogCatch
                        return false;     // an exception was thrown, consider this a failure
                }
            }
    };

    class CRendererManagerTextTester : public CUnitTest
    {
        public:
            CRendererManagerTextTester() {}
            ~CRendererManagerTextTester() {}
            const TCHAR* Name() { return _T("Test Text Rendering"); }
            const TCHAR* Description() { return _T(""); }

            bool TestCode()
            {
                try
                {
                    RendererManagerUnitTester rmut;
                    return rmut.runTextTests();
                }
                catch ( ... )
                {
                    LogCatch
                        return false;     // an exception was thrown, consider this a failure
                }
            }
    };

    class CRendererManagerLabelTester : public CUnitTest
    {
        public:
            CRendererManagerLabelTester() {}
            ~CRendererManagerLabelTester() {}
            const TCHAR* Name() { return _T("Test Label Rendering"); }
            const TCHAR* Description() { return _T(""); }

            bool TestCode()
            {
                try
                {
                    std::string cfgFileName(DEFAULT_INTERNAL_CONFIG_FILE_NAME);
                    util_env::Properties traceProps(cfgFileName);
                    Trace::getInstance(traceProps);


                    RendererManagerUnitTester rmut;
                    return rmut.runLabelTests();
                }
                catch ( ... )
                {
                    LogCatch
                        return false;     // an exception was thrown, consider this a failure
                }
            }
    };
#endif

    class CEvolMessageWrapperTester : public CUnitTest
    {
        public:
            virtual ~CEvolMessageWrapperTester() {}
            CEvolMessageWrapperTester(){}
            const TCHAR * Name() { return _T("Test EvolMessageWrapper Class");};
            const TCHAR * Description() { return _T("");};        

            bool TestCode() 
            { 
                try
                {                                       
                    WeigherMode mode( NULL, ScaleWeightTrackingMode );
                    EvolMessageWrapper modeWrapper( &mode );

                    WeigherMode mode2( modeWrapper.Data() );

                    Assert( mode.getMode() == mode2.getMode(), Util::CNamedException( L"Mode" ) );
                    Assert( mode.getDeviceType() == mode2.getDeviceType(), Util::CNamedException( L"Device" ) );
                    Assert( mode.getEventType() == mode2.getEventType(), Util::CNamedException( L"Event" ) );
                    Assert( mode.getDeviceType() == modeWrapper.getDeviceType(), Util::CNamedException( L"Device" ) );
                    Assert( mode.getEventType() == modeWrapper.getEventType(), Util::CNamedException( L"Event" ) );

                    return true;
                }
                catch (...)
                {
                    LogCatch
                        return false; // an exception was thrown, consider this a failure
                }
            }
        private:       

    };

    class CEvolMessageWrapperTester2 : public CUnitTest
    {
        public:
            virtual ~CEvolMessageWrapperTester2() {}
            CEvolMessageWrapperTester2(){}
            const TCHAR * Name() { return _T("Test CEvolMessageWrapper -- Get Sender pointer");};
            const TCHAR * Description() { return _T("");};        

            class OneReceiver : public Hobart::CEvolMsgReceiver
        {
            public:
                OneReceiver(const char * name0 = "FirstReceiver" ):Hobart::CEvolMsgReceiver( name0 ) 
            {
                this->registerEvent( Weigher, 7 );
            };
                virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & UNUSED(msg) )
                {

                }                       

        };

            bool TestCode() 
            { 
                try
                {               
                    OneReceiver one;
#ifndef LINUX_FIX
                    OneReceiver * p1 = &one;
#endif      // LINUX_FIX

                    EvolMessage msg( (EvolDeviceType)1, 1 );
                    msg.setSourceObject( &one );

                    Hobart::CEvolMsgReceiver* sender = msg.getSourceObject();
                    Assert(  sender == &one, Util::CNamedException( L"Event" ) );

                    return true;
                }
                catch (...)
                {
                    LogCatch
                        return false; // an exception was thrown, consider this a failure
                }
            }
        private:       

    };

    class CMessageCenterTester1 : public CUnitTest
    {
        public:
            virtual ~CMessageCenterTester1() {}
            CMessageCenterTester1(){}

            const TCHAR * Name() { return _T("Test EvolReceiver - send to itself");};
            const TCHAR * Description() { return _T("");};

            class FirstReceiver : public Hobart::CEvolMsgReceiver
        {
            public:
                FirstReceiver(const char * name = "FirstReceiver" ):Hobart::CEvolMsgReceiver( name ) 
            {
                this->registerEvent( Weigher, 7 );
            };
                virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
                {
                    lastMsgReceived = msg;
                }

                Util::CSmartPtr< EvolMessageWrapper> lastMsgReceived;

        };

            bool TestCode() 
            { 
                try
                {
                    Hobart::CMessageCenter::instance();
                    FirstReceiver first;
                    WeigherMode mode( NULL, ScaleWeightTrackingMode );
                    first.dispatchEvent( &mode );
#ifdef LINUX_FIX
                    PAL_absclass::PAL_sleep( 10 ); //wait for the message
#else
                    Sleep( 10 ); //wait for the message
#endif      // LINUX_FIX
                    Assert ( first.lastMsgReceived.get() != NULL, Util::CNamedException( L"Deliver failed!" ) );

                    WeigherMode mode2( first.lastMsgReceived->Data() );

                    Assert( mode.getMode() == mode2.getMode(), Util::CNamedException( L"Mode" ) );
                    Assert( mode.getDeviceType() == mode2.getDeviceType(), Util::CNamedException( L"Device" ) );
                    Assert( mode.getEventType() == mode2.getEventType(), Util::CNamedException( L"Event" ) );

                    first.lastMsgReceived = NULL;
                    const char * ptr = mode.serialize();
                    first.dispatchEvent( ptr, mode.getSerializedLength() );
#ifdef LINUX_FIX
                    PAL_absclass::PAL_sleep( 10 ); //wait for the message
#else
                    Sleep( 10 ); //wait for the message
#endif      // LINUX_FIX
                    Assert ( first.lastMsgReceived.get() != NULL, Util::CNamedException( L"Deliver failed!" ) );

                    WeigherMode mode3( first.lastMsgReceived->Data() );

                    Assert( mode.getMode() == mode3.getMode(), Util::CNamedException( L"Mode" ) );
                    Assert( mode.getDeviceType() == mode3.getDeviceType(), Util::CNamedException( L"Device" ) );
                    Assert( mode.getEventType() == mode3.getEventType(), Util::CNamedException( L"Event" ) );

                    return true;
                }
                catch ( Util::CNamedException  )
                {
                    LogCatchAndThrow;
                }
                catch (...)
                {
                    LogCatch
                        return false; // an exception was thrown, consider this a failure
                }
            }
        private:


    };

    class CMessageCenterTester2 : public CUnitTest
    {
        public:
            virtual ~CMessageCenterTester2() {}
            CMessageCenterTester2(){}

            const TCHAR * Name() { return _T("Test EvolReceiver - send by message center");};
            const TCHAR * Description() { return _T("");};

            class FirstReceiver : public Hobart::CEvolMsgReceiver
        {
            public:
                FirstReceiver(const char * name = "FirstReceiver" ):Hobart::CEvolMsgReceiver( name ) 
            {
                this->registerEvent( Weigher, 7 );
            };
                virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
                {
                    lastMsgReceived = msg;
                }

                Util::CSmartPtr< EvolMessageWrapper> lastMsgReceived;

        };

            bool TestCode() 
            { 
                try
                {
                    Hobart::CMessageCenter::instance();
                    FirstReceiver first;
                    WeigherMode mode( NULL, ScaleWeightTrackingMode );
                    Hobart::CMessageCenter::instance() << &mode;
#ifdef LINUX_FIX
                    PAL_absclass::PAL_sleep( 10 ); //wait for the message
#else
                    Sleep( 10 ); //wait for the message
#endif      // LINUX_FIX
                    Assert ( first.lastMsgReceived.get() != NULL, Util::CNamedException( L"Deliver failed!" ) );

                    WeigherMode mode2( first.lastMsgReceived->Data() );

                    Assert( mode.getMode() == mode2.getMode(), Util::CNamedException( L"Mode" ) );
                    Assert( mode.getDeviceType() == mode2.getDeviceType(), Util::CNamedException( L"Device" ) );
                    Assert( mode.getEventType() == mode2.getEventType(), Util::CNamedException( L"Event" ) );

                    return true;
                }
                catch ( Util::CNamedException  )
                {
                    LogCatchAndThrow;
                }
                catch (...)
                {
                    LogCatch
                        return false; // an exception was thrown, consider this a failure
                }
            }
        private:


    };

    class CMessageCenterTester3 : public CUnitTest
    {
        public:
            virtual ~CMessageCenterTester3() {}
            CMessageCenterTester3(){}

            const TCHAR * Name() { return _T("Test EvolReceiver - send by another receiver");};
            const TCHAR * Description() { return _T("");};

            class FirstReceiver : public Hobart::CEvolMsgReceiver
        {
            public:
                FirstReceiver(const char * name = "FirstReceiver" ):Hobart::CEvolMsgReceiver( name ) 
            {
                this->registerEvent( Weigher, 7 );
            };
                virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
                {
                    lastMsgReceived = msg;
                }

                void Unreginster( EvolDeviceType evol_device, UINT8 evol_event )
                {
                    deregisterEvent( evol_device, evol_event );
                }

                Util::CSmartPtr< EvolMessageWrapper> lastMsgReceived;

        };

            class SecondReceiver : public Hobart::CEvolMsgReceiver
        {
            public:
                SecondReceiver(const char * name = "SecondReceiver" ):Hobart::CEvolMsgReceiver( name ) 
            {
            };
                virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & UNUSED(msg) )
                {                               
                }                       
        };

            bool TestCode() 
            { 
                try
                {
                    Hobart::CMessageCenter::instance();
                    FirstReceiver first;
                    SecondReceiver second;

                    WeigherMode mode( NULL, ScaleWeightTrackingMode );
                    second.dispatchEvent( &mode );
#ifdef LINUX_FIX
                    PAL_absclass::PAL_sleep( 10 ); //wait for the message
#else
                    Sleep( 10 ); //wait for the message
#endif      // LINUX_FIX
                    Assert ( first.lastMsgReceived.get() != NULL, Util::CNamedException( L"Deliver failed!" ) );

                    WeigherMode mode2( first.lastMsgReceived->Data() );

                    Assert( mode.getMode() == mode2.getMode(), Util::CNamedException( L"Mode" ) );
                    Assert( mode.getDeviceType() == mode2.getDeviceType(), Util::CNamedException( L"Device" ) );
                    Assert( mode.getEventType() == mode2.getEventType(), Util::CNamedException( L"Event" ) );

                    return true;
                }
                catch ( Util::CNamedException  )
                {
                    LogCatchAndThrow;
                }
                catch (...)
                {
                    LogCatch
                        return false; // an exception was thrown, consider this a failure
                }
            }
        private:


    };

    class CMessageCenterTester4 : public CUnitTest
    {
        public:
            virtual ~CMessageCenterTester4() {}
            CMessageCenterTester4(){}

            const TCHAR * Name() { return _T("Test EvolReceiver - deregister test");};
            const TCHAR * Description() { return _T("");};

            class FirstReceiver : public Hobart::CEvolMsgReceiver
        {
            public:
                FirstReceiver(const char * name = "FirstReceiver" ):Hobart::CEvolMsgReceiver( name ) 
            {
                this->registerEvent( (EvolDeviceType)1, 1 );
                this->registerEvent( (EvolDeviceType)1, 2 );
                this->registerEvent( (EvolDeviceType)1, 3 );
                this->registerEvent( (EvolDeviceType)1, 4 );
            };
                virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
                {
                    char str[255];
                    sprintf( str, "\tFirst got message (%d, %d )\r\n", msg->getDeviceType(), msg->getEventType() );
#ifdef LINUX_FIX
                    PAL_absclass::PAL_OutputDbgStrA( str );
#else
                    OutputDebugStringA( str );
#endif      // LINUX_FIX
                }

                void Unreginster( EvolDeviceType evol_device, UINT8 evol_event )
                {
                    deregisterEvent( evol_device, evol_event );
                }

        };

            class SecondReceiver : public Hobart::CEvolMsgReceiver
        {
            public:
                SecondReceiver(const char * name = "SecondReceiver" ):Hobart::CEvolMsgReceiver( name ) 
            {
            };
                virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & UNUSED(msg) )
                {                               
                }                       
        };

            bool TestCode() 
            { 
                try
                {
                    Hobart::CMessageCenter::instance();
                    FirstReceiver first;
                    SecondReceiver second;

                    EvolMessage m11( (EvolDeviceType)1, 1 );
                    EvolMessage m12( (EvolDeviceType)1, 2 );
                    EvolMessage m13( (EvolDeviceType)1, 3 );
                    EvolMessage m14( (EvolDeviceType)1, 4 );

#ifdef LINUX_FIX
                    PAL_absclass::PAL_OutputDbgStr( L"2 Sends message:( 1, 1)\r\n" );
                    second.dispatchEvent( &m11 );
                    PAL_absclass::PAL_OutputDbgStr( L"2 Sends message:( 1, 2)\r\n" );
                    second.dispatchEvent( &m12 );
                    PAL_absclass::PAL_OutputDbgStr( L"2 Sends message:( 1, 3)\r\n" );
                    second.dispatchEvent( &m13 );
                    PAL_absclass::PAL_OutputDbgStr( L"2 Sends message:( 1, 4)\r\n" );
                    second.dispatchEvent( &m14 );
                    PAL_absclass::PAL_sleep( 1000 ); //wait for the message
#else
                    OutputDebugString( L"2 Sends message:( 1, 1)\r\n" );
                    second.dispatchEvent( &m11 );
                    OutputDebugString( L"2 Sends message:( 1, 2)\r\n" );
                    second.dispatchEvent( &m12 );
                    OutputDebugString( L"2 Sends message:( 1, 3)\r\n" );
                    second.dispatchEvent( &m13 );
                    OutputDebugString( L"2 Sends message:( 1, 4)\r\n" );
                    second.dispatchEvent( &m14 );
                    Sleep( 1000 ); //wait for the message
#endif      // LINUX_FIX

                    first.Unreginster( (EvolDeviceType)1, 1 );
                    first.Unreginster( (EvolDeviceType)1, 3 );

#ifdef LINUX_FIX
                    PAL_absclass::PAL_OutputDbgStr( L"2 Sends message:( 1, 1)\r\n" );
                    second.dispatchEvent( &m11 );
                    PAL_absclass::PAL_OutputDbgStr( L"2 Sends message:( 1, 2)\r\n" );
                    second.dispatchEvent( &m12 );
                    PAL_absclass::PAL_OutputDbgStr( L"2 Sends message:( 1, 3)\r\n" );
                    second.dispatchEvent( &m13 );
                    PAL_absclass::PAL_OutputDbgStr( L"2 Sends message:( 1, 4)\r\n" );
                    second.dispatchEvent( &m14 );
                    PAL_absclass::PAL_sleep( 1000 ); //wait for the message
#else
                    OutputDebugString( L"2 Sends message:( 1, 1)\r\n" );
                    second.dispatchEvent( &m11 );
                    OutputDebugString( L"2 Sends message:( 1, 2)\r\n" );
                    second.dispatchEvent( &m12 );
                    OutputDebugString( L"2 Sends message:( 1, 3)\r\n" );
                    second.dispatchEvent( &m13 );
                    OutputDebugString( L"2 Sends message:( 1, 4)\r\n" );
                    second.dispatchEvent( &m14 );
                    Sleep( 1000 ); //wait for the message
#endif      // LINUX_FIX

                    return true;
                }
                catch ( Util::CNamedException  )
                {
                    LogCatchAndThrow;
                }
                catch (...)
                {
                    LogCatch
                        return false; // an exception was thrown, consider this a failure
                }
            }
        private:


    };
    class CMessageCenterTester5 : public CUnitTest
    {
        public:
            virtual ~CMessageCenterTester5() {}
            CMessageCenterTester5(){}

            const TCHAR * Name() { return _T("Test EvolReceiver - register temporarily test");};
            const TCHAR * Description() { return _T("");};

            class OneReceiver : public Hobart::CEvolMsgReceiver
        {
            public:
                OneReceiver(const char * name = "FirstReceiver" ):Hobart::CEvolMsgReceiver( name ) 
            {                           
            };
                virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
                {
                    char str[255];
                    sprintf( str, "\tReceive message %d \r\n", msg->getEventType() );
#ifdef LINUX_FIX
                    PAL_absclass::PAL_OutputDbgStrA( str );
#else
                    OutputDebugStringA( str );
#endif
                }                       
        };

            void SendMessages()
            {
                EvolMessage m11( (EvolDeviceType)300, 1 );
                EvolMessage m12( (EvolDeviceType)1, 2 );
                EvolMessage m13( (EvolDeviceType)1, 3 );
                EvolMessage m14( (EvolDeviceType)1, 4 );

#ifdef LINUX_FIX
                PAL_absclass::PAL_OutputDbgStr( L"Sends messages: 1, 2, 3, 4\r\n" );
#else
                OutputDebugString( L"Sends messages: 1, 2, 3, 4\r\n" );
#endif
                Hobart::CMessageCenter::instance() <<  &m11 << &m12 << &m13 << &m14;
            };

            bool TestCode() 
            { 
                try
                {
                    Hobart::CMessageCenter::instance();
                    OneReceiver one;

                    one.registerEvent( (EvolDeviceType)300, 1 );
                    one.registerEvent( (EvolDeviceType)1, 2, false );
                    one.registerEvent( (EvolDeviceType)1, 3 );
                    one.registerEvent( (EvolDeviceType)1, 4, false );

                    SendMessages();
#ifdef LINUX_FIX
                    PAL_absclass::PAL_sleep( 100 ); //wait for the message
#else
                    Sleep( 100 ); //wait for the message
#endif      // LINUX_FIX

                    SendMessages();
#ifdef LINUX_FIX
                    PAL_absclass::PAL_sleep( 100 ); //wait for the message
#else
                    Sleep( 100 ); //wait for the message
#endif      // LINUX_FIX

                    one.deregisterEvent( (EvolDeviceType)1, 1 ); 
                    SendMessages();
#ifdef LINUX_FIX
                    PAL_absclass::PAL_sleep( 2000 ); //wait for the message
#else
                    Sleep( 2000 ); //wait for the message
#endif      // LINUX_FIX

                    return true;
                }
                catch ( Util::CNamedException  )
                {
                    LogCatchAndThrow;
                }
                catch (...)
                {
                    LogCatch
                        return false; // an exception was thrown, consider this a failure
                }
            }
        private:


    };

    class CMessageCenterTester6 : public CUnitTest
    {
        public:
            virtual ~CMessageCenterTester6() {}
            CMessageCenterTester6(){}

            const TCHAR * Name() { return _T("Test EvolReceiver - Register same message twice");};
            const TCHAR * Description() { return _T("");};

            class FirstReceiver : public Hobart::CEvolMsgReceiver
        {
            public:
                FirstReceiver(const char * name = "FirstReceiver" ):Hobart::CEvolMsgReceiver( name ) 
            {
                this->registerEvent( Weigher, 7 );
                this->registerEvent( Weigher, 7 );

            };
                virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
                {
                    char str[255];
                    sprintf( str, "\tFirst got message (%d, %d )\r\n", msg->getDeviceType(), msg->getEventType() );
#ifdef LINUX_FIX
                    PAL_absclass::PAL_OutputDbgStrA( str );
#else
                    OutputDebugStringA( str );
#endif      // LINUX_FIX
                }

                Util::CSmartPtr< EvolMessageWrapper> lastMsgReceived;

        };

            bool TestCode() 
            { 
                try
                {
                    Hobart::CMessageCenter::instance();
                    FirstReceiver first;
                    WeigherMode mode( NULL, ScaleWeightTrackingMode );
                    Hobart::CMessageCenter::instance() << &mode;
#ifdef LINUX_FIX
                    DWORD t1 = PAL_absclass::PAL_getTickCount();
                    PAL_absclass::PAL_sleep( 1000 ); //wait for the message
                    t1 = PAL_absclass::PAL_getTickCount() - t1;
#else
                    DWORD t1 = GetTickCount();
                    Sleep( 1000 ); //wait for the message
                    t1 = GetTickCount() - t1;
#endif      // LINUX_FIX
                    return true;
                }
                catch ( Util::CNamedException  )
                {
                    LogCatchAndThrow;
                }
                catch (...)
                {
                    LogCatch
                        return false; // an exception was thrown, consider this a failure
                }
            }
        private:


    };

    class CMessageCenterTester : public CUnitTest
    {
        public:
            virtual ~CMessageCenterTester() {}
            CMessageCenterTester(){}

            const TCHAR * Name() { return _T("Test EvolReceiver and MessageCenter");};
            const TCHAR * Description() { return _T("");};

            class FirstReceiver : public Hobart::CEvolMsgReceiver
        {
            public:
                FirstReceiver(const char * name = "FirstReceiver" ):Hobart::CEvolMsgReceiver( name ) 
            {
                this->registerEvent( (EvolDeviceType)1, 1 );
                this->registerEvent( (EvolDeviceType)2, 1 );
            };
                virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
                {
                    char str[255];
                    sprintf( str, "\t1 got message (%d, %d )\r\n", msg->getDeviceType(), msg->getEventType() );
#ifdef LINUX_FIX
                    PAL_absclass::PAL_OutputDbgStrA( str );
#else
                    OutputDebugStringA( str );
#endif      // LINUX_FIX
                }
        };

            class SecondReceiver : public Hobart::CEvolMsgReceiver
        {
            public:
                SecondReceiver(const char * name = "SecondReceiver" ):Hobart::CEvolMsgReceiver( name ) 
            {
                this->registerEvent( (EvolDeviceType)1, 2 , false);
                this->registerEvent( (EvolDeviceType)2, 2 );
            };
                virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
                {
                    char str[255];
                    sprintf( str, "\t2 got message (%d, %d )\r\n", msg->getDeviceType(), msg->getEventType() );
#ifdef LINUX_FIX
                    PAL_absclass::PAL_OutputDbgStrA( str );
#else
                    OutputDebugStringA( str );
#endif
                }
        };


            bool TestCode() 
            { 
                try
#ifdef LINUX_FIX
                {
                    Hobart::CMessageCenter::instance();

                    FirstReceiver first;
                    PAL_absclass::PAL_OutputDbgStr( L"1 constructed! \r\n" );
                    EvolMessage m11( (EvolDeviceType)1, 1 );
                    EvolMessage m12( (EvolDeviceType)1, 2 );
                    EvolMessage m21( (EvolDeviceType)2, 1 );
                    EvolMessage m22( (EvolDeviceType)2, 2 );

                    PAL_absclass::PAL_OutputDbgStr( L"1 Sends message:( 1, 1)\r\n" );
                    first.dispatchEvent( &m11 );
                    PAL_absclass::PAL_sleep( 100 ); //wait for the message
                    PAL_absclass::PAL_OutputDbgStr( L"1 Sends message:( 1, 2)\r\n" );
                    first.dispatchEvent( &m12 );
                    PAL_absclass::PAL_sleep( 100 ); //wait for the message
                    PAL_absclass::PAL_OutputDbgStr( L"1 Sends message:( 2, 1)\r\n" );
                    first.dispatchEvent( &m21 );
                    PAL_absclass::PAL_sleep( 100 ); //wait for the message
                    PAL_absclass::PAL_OutputDbgStr( L"1 Sends message:( 2, 2)\r\n" );
                    first.dispatchEvent( &m22 );
                    PAL_absclass::PAL_sleep( 1000 ); //wait for the message

                    PAL_absclass::PAL_OutputDbgStr( L"MessageCenter Sends message:( 1, 1 )\r\n" );
                    Hobart::CMessageCenter::instance() << &m11;
                    PAL_absclass::PAL_sleep( 100 ); //wait for the message
                    PAL_absclass::PAL_OutputDbgStr( L"MessageCenter Sends message:( 1, 2 )\r\n" );
                    Hobart::CMessageCenter::instance() << &m12;
                    PAL_absclass::PAL_sleep( 100 ); //wait for the message
                    PAL_absclass::PAL_OutputDbgStr( L"MessageCenter Sends message:( 2, 1 )\r\n" );
                    Hobart::CMessageCenter::instance() << &m21;
                    PAL_absclass::PAL_sleep( 100 ); //wait for the message
                    PAL_absclass::PAL_OutputDbgStr( L"MessageCenter Sends message:( 2, 2 )\r\n" );
                    Hobart::CMessageCenter::instance() << &m22;
                    PAL_absclass::PAL_sleep( 1000 ); //wait for the message

                    {
                        SecondReceiver second;
                        PAL_absclass::PAL_OutputDbgStr( L"1 Sends 4 messages:\r\n" );

                        first.dispatchEvent( &m11 );
                        PAL_absclass::PAL_sleep( 100 ); //wait for the message
                        first.dispatchEvent( &m12 );
                        PAL_absclass::PAL_sleep( 100 ); //wait for the message
                        first.dispatchEvent( &m21 );
                        PAL_absclass::PAL_sleep( 100 ); //wait for the message
                        first.dispatchEvent( &m22 );
                        PAL_absclass::PAL_sleep( 1000 ); //wait for the message

                        PAL_absclass::PAL_OutputDbgStr( L"MessageCenter Sends 4 message:\r\n" );
                        Hobart::CMessageCenter::instance() << &m11;
                        PAL_absclass::PAL_sleep( 100 ); //wait for the message
                        Hobart::CMessageCenter::instance() << &m12;
                        PAL_absclass::PAL_sleep( 100 ); //wait for the message
                        Hobart::CMessageCenter::instance() << &m21;
                        PAL_absclass::PAL_sleep( 100 ); //wait for the message
                        Hobart::CMessageCenter::instance() << &m22;
                        PAL_absclass::PAL_sleep( 1000 ); //wait for the message

                        PAL_absclass::PAL_OutputDbgStr( L"1 Sends 4 messages to itself :\r\n" );

                        first.dispatchEvent( &m11, &first );
                        PAL_absclass::PAL_sleep( 100 ); //wait for the message
                        first.dispatchEvent( &m12, &first );
                        PAL_absclass::PAL_sleep( 100 ); //wait for the message
                        first.dispatchEvent( &m21, &first );
                        PAL_absclass::PAL_sleep( 100 ); //wait for the message
                        first.dispatchEvent( &m22, &first );
                        PAL_absclass::PAL_sleep( 1000 ); //wait for the message

                        PAL_absclass::PAL_OutputDbgStr( L"1 Sends 4 messages to 2 :\r\n" );

                        first.dispatchEvent( &m11, &second );
                        PAL_absclass::PAL_sleep( 100 ); //wait for the message
                        first.dispatchEvent( &m12, &second );
                        PAL_absclass::PAL_sleep( 100 ); //wait for the message
                        first.dispatchEvent( &m21, &second );
                        PAL_absclass::PAL_sleep( 100 ); //wait for the message
                        first.dispatchEvent( &m22, &second );
                        PAL_absclass::PAL_sleep( 1000 ); //wait for the message

                        PAL_absclass::PAL_OutputDbgStr( L"***2 removed from stack !\r\n" );
                    }

                    PAL_absclass::PAL_OutputDbgStr( L"1 Sends 4 messages:\r\n" );
                    //first.deregisterEvent( (EvolDeviceType)1, 1 );
                    first.dispatchEvent( &m11 );
                    PAL_absclass::PAL_sleep( 100 ); //wait for the message
                    first.dispatchEvent( &m12 );
                    PAL_absclass::PAL_sleep( 100 ); //wait for the message
                    first.dispatchEvent( &m21 );
                    PAL_absclass::PAL_sleep( 100 ); //wait for the message
                    first.dispatchEvent( &m22 );
                    PAL_absclass::PAL_sleep( 4000 ); //wait for the message


                    return true;
                }                
#else
                {
                    Hobart::CMessageCenter::instance();

                    FirstReceiver first;
                    OutputDebugString( L"1 constructed! \r\n" );
                    EvolMessage m11( (EvolDeviceType)1, 1 );
                    EvolMessage m12( (EvolDeviceType)1, 2 );
                    EvolMessage m21( (EvolDeviceType)2, 1 );
                    EvolMessage m22( (EvolDeviceType)2, 2 );

                    OutputDebugString( L"1 Sends message:( 1, 1)\r\n" );
                    first.dispatchEvent( &m11 );
                    Sleep( 100 ); //wait for the message
                    OutputDebugString( L"1 Sends message:( 1, 2)\r\n" );
                    first.dispatchEvent( &m12 );
                    Sleep( 100 ); //wait for the message
                    OutputDebugString( L"1 Sends message:( 2, 1)\r\n" );
                    first.dispatchEvent( &m21 );
                    Sleep( 100 ); //wait for the message
                    OutputDebugString( L"1 Sends message:( 2, 2)\r\n" );
                    first.dispatchEvent( &m22 );
                    Sleep( 1000 ); //wait for the message

                    OutputDebugString( L"MessageCenter Sends message:( 1, 1 )\r\n" );
                    Hobart::CMessageCenter::instance() << &m11;
                    Sleep( 100 ); //wait for the message
                    OutputDebugString( L"MessageCenter Sends message:( 1, 2 )\r\n" );
                    Hobart::CMessageCenter::instance() << &m12;
                    Sleep( 100 ); //wait for the message
                    OutputDebugString( L"MessageCenter Sends message:( 2, 1 )\r\n" );
                    Hobart::CMessageCenter::instance() << &m21;
                    Sleep( 100 ); //wait for the message
                    OutputDebugString( L"MessageCenter Sends message:( 2, 2 )\r\n" );
                    Hobart::CMessageCenter::instance() << &m22;
                    Sleep( 1000 ); //wait for the message

                    {
                        SecondReceiver second;
                        OutputDebugString( L"1 Sends 4 messages:\r\n" );

                        first.dispatchEvent( &m11 );
                        Sleep( 100 ); //wait for the message
                        first.dispatchEvent( &m12 );
                        Sleep( 100 ); //wait for the message
                        first.dispatchEvent( &m21 );
                        Sleep( 100 ); //wait for the message
                        first.dispatchEvent( &m22 );
                        Sleep( 1000 ); //wait for the message

                        OutputDebugString( L"MessageCenter Sends 4 message:\r\n" );
                        Hobart::CMessageCenter::instance() << &m11;
                        Sleep( 100 ); //wait for the message
                        Hobart::CMessageCenter::instance() << &m12;
                        Sleep( 100 ); //wait for the message
                        Hobart::CMessageCenter::instance() << &m21;
                        Sleep( 100 ); //wait for the message
                        Hobart::CMessageCenter::instance() << &m22;
                        Sleep( 1000 ); //wait for the message

                        OutputDebugString( L"1 Sends 4 messages to itself :\r\n" );

                        first.dispatchEvent( &m11, &first );
                        Sleep( 100 ); //wait for the message
                        first.dispatchEvent( &m12, &first );
                        Sleep( 100 ); //wait for the message
                        first.dispatchEvent( &m21, &first );
                        Sleep( 100 ); //wait for the message
                        first.dispatchEvent( &m22, &first );
                        Sleep( 1000 ); //wait for the message

                        OutputDebugString( L"1 Sends 4 messages to 2 :\r\n" );

                        first.dispatchEvent( &m11, &second );
                        Sleep( 100 ); //wait for the message
                        first.dispatchEvent( &m12, &second );
                        Sleep( 100 ); //wait for the message
                        first.dispatchEvent( &m21, &second );
                        Sleep( 100 ); //wait for the message
                        first.dispatchEvent( &m22, &second );
                        Sleep( 1000 ); //wait for the message

                        OutputDebugString( L"***2 removed from stack !\r\n" );
                    }

                    OutputDebugString( L"1 Sends 4 messages:\r\n" );
                    //first.deregisterEvent( (EvolDeviceType)1, 1 );
                    first.dispatchEvent( &m11 );
                    Sleep( 100 ); //wait for the message
                    first.dispatchEvent( &m12 );
                    Sleep( 100 ); //wait for the message
                    first.dispatchEvent( &m21 );
                    Sleep( 100 ); //wait for the message
                    first.dispatchEvent( &m22 );
                    Sleep( 4000 ); //wait for the message


                    return true;
                }
#endif      // LINUX_FIX
                catch (...)
                {
                    LogCatch
                        return false; // an exception was thrown, consider this a failure
                }
            }
        private:


    };
    class CHobartTimerTester : public CUnitTest
    {
        public:
            // LIFECYCLE
            virtual ~CHobartTimerTester() {}

            CHobartTimerTester(){}
            // ACCESS
            const TCHAR * Name() { return _T("Test CHobartTimer");};
            const TCHAR * Description() { return _T("");};
            // OPERATIONS

            class MyHolder : public Hobart::CHobartTimer
        {
            public:
                virtual void timeoutProcess( __int32 id )
                {
                    Util::CMSGTime ct = Util::CMSGTime::GetTheCurrentTime();
                    wchar_t s[255];
#ifdef LINUX_FIX
                    swprintf( s, 255, L"Timer %d signaled at: %d:%d:%d - %d\r\n", id,
                            ct.GetHour(), ct.GetMinute(), ct.GetSecond(), ct.GetMilliseconds() );
                    PAL_absclass::PAL_OutputDbgStr( s );
#else
                    wsprintf( s, L"Timer %d signaled at: %d:%d:%d - %d\r\n", id,
                            ct.GetHour(), ct.GetMinute(), ct.GetSecond(), ct.GetMilliseconds() );
                    OutputDebugString( s );
#endif      // LINUX_FIX
                }               
        };
            bool TestCode() 
            { 
                try
                {                                       
                    MyHolder holder;

                    holder.createTimer( 50, 1 );
                    holder.createTimer( 20, 2 );
                    Util::CMSGTime ct = Util::CMSGTime::GetTheCurrentTime();
                    wchar_t s[255];
#ifdef LINUX_FIX
                    swprintf( s, 255, L"Start up timer 1 & 2 at: %d:%d:%d - %d\r\n",
                            ct.GetHour(), ct.GetMinute(), ct.GetSecond(), ct.GetMilliseconds() );
                    PAL_absclass::PAL_OutputDbgStr( s );

                    PAL_absclass::PAL_sleep( 600 );       

                    holder.createTimer( 10, 3 );
                    ct = Util::CMSGTime::GetTheCurrentTime();
                    swprintf( s, 255, L"Start up timer 3 at: %d:%d:%d - %d\r\n",
                            ct.GetHour(), ct.GetMinute(), ct.GetSecond(), ct.GetMilliseconds() );
                    PAL_absclass::PAL_OutputDbgStr( s );                             

                    PAL_absclass::PAL_sleep( 300 );                       
#else
                    wsprintf( s, L"Start up timer 1 & 2 at: %d:%d:%d - %d\r\n",
                            ct.GetHour(), ct.GetMinute(), ct.GetSecond(), ct.GetMilliseconds() );
                    OutputDebugString( s );

                    Sleep( 600 );       

                    holder.createTimer( 10, 3 );
                    ct = Util::CMSGTime::GetTheCurrentTime();
                    wsprintf( s, L"Start up timer 3 at: %d:%d:%d - %d\r\n",
                            ct.GetHour(), ct.GetMinute(), ct.GetSecond(), ct.GetMilliseconds() );
                    OutputDebugString( s );                             

                    Sleep( 300 );                       
#endif      // LINUX_FIX

                    return true;
                }
                catch (...)
                {
                    LogCatch
                        return false; // an exception was thrown, consider this a failure
                }
            }
        private:       

    };

    class CHobartTimerAccuracyTester : public CUnitTest
    {
        public:
            // LIFECYCLE
            virtual ~CHobartTimerAccuracyTester() {}

            CHobartTimerAccuracyTester(){}
            // ACCESS
            const TCHAR * Name() { return _T("Test CHobartTimer accuracy");};
            const TCHAR * Description() { return _T("");};
            // OPERATIONS

            class MyHolder : public Hobart::CHobartTimer
        {
            public:
                virtual void timeoutProcess( __int32 id )
                {
                    static __int32 i = 0;
                    i++;
                    if ( i == 100 )
                    {
                        Util::CMSGTime ct = Util::CMSGTime::GetTheCurrentTime();
                        wchar_t s[255];
#ifdef LINUX_FIX
                        swprintf( s, 255, L"\t*****Timer %d signaled at: %d:%d:%d - %d\r\n", id,
                                ct.GetHour(), ct.GetMinute(), ct.GetSecond(), ct.GetMilliseconds() );
                        PAL_absclass::PAL_OutputDbgStr( s );
#else
                        wsprintf( s, L"\t*****Timer %d signaled at: %d:%d:%d - %d\r\n", id,
                                ct.GetHour(), ct.GetMinute(), ct.GetSecond(), ct.GetMilliseconds() );
                        OutputDebugString( s );
#endif      // LINUX_FIX
                    }
                    else
                        this->createTimer( 10, i );
                }

        };
            bool TestCode() 
            { 
                try
                {                                       
                    MyHolder holder;

                    holder.createTimer( 10, 0 );
                    Util::CMSGTime ct = Util::CMSGTime::GetTheCurrentTime();
                    wchar_t s[255];
#ifdef LINUX_FIX
                    swprintf( s, 255, L"\t*****Start up timer 0 at: %d:%d:%d - %d\r\n",
                            ct.GetHour(), ct.GetMinute(), ct.GetSecond(), ct.GetMilliseconds() );
                    PAL_absclass::PAL_OutputDbgStr( s );

                    PAL_absclass::PAL_sleep( 6000 );                      
#else
                    wsprintf( s, L"\t*****Start up timer 0 at: %d:%d:%d - %d\r\n",
                            ct.GetHour(), ct.GetMinute(), ct.GetSecond(), ct.GetMilliseconds() );
                    OutputDebugString( s );

                    Sleep( 6000 );                      
#endif      // LINUX_FIX
                    return true;
                }
                catch (...)
                {
                    LogCatch
                        return false; // an exception was thrown, consider this a failure
                }
            }
        private:        

    };

    class CHobartTimerCancelFunnctionTester : public CUnitTest
    {
        public:
            virtual ~CHobartTimerCancelFunnctionTester() {}
            CHobartTimerCancelFunnctionTester(){}
            const TCHAR * Name() { return _T("Test CHobartTimer cancelTimer Function");};
            const TCHAR * Description() { return _T("");};

            class MyHolder : public Hobart::CHobartTimer
        {
            public:
                virtual void timeoutProcess( __int32 id )
                {
                    Util::CMSGTime ct = Util::CMSGTime::GetTheCurrentTime();
                    wchar_t s[255];
#ifdef LINUX_FIX
                    swprintf( s, 255, L"\t*****Timer %d signaled at: %d:%d:%d - %d\r\n", id,
                            ct.GetHour(), ct.GetMinute(), ct.GetSecond(), ct.GetMilliseconds() );
                    PAL_absclass::PAL_OutputDbgStr( s );
#else
                    wsprintf( s, L"\t*****Timer %d signaled at: %d:%d:%d - %d\r\n", id,
                            ct.GetHour(), ct.GetMinute(), ct.GetSecond(), ct.GetMilliseconds() );
                    OutputDebugString( s );
#endif      // LINUX_FIX
                }
        };
            bool TestCode() 
            { 
                try
                {
                    MyHolder holder;
                    holder.createTimer( 300, 1 );
                    Util::CMSGTime ct = Util::CMSGTime::GetTheCurrentTime();
                    wchar_t s[255];
#ifdef LINUX_FIX
                    swprintf( s, 255, L"\t*****Start up timer 1 at: %d:%d:%d - %d\r\n",
                            ct.GetHour(), ct.GetMinute(), ct.GetSecond(), ct.GetMilliseconds() );
                    PAL_absclass::PAL_OutputDbgStr( s );

                    holder.createTimer( 400, 2 );
                    Util::CMSGTime::GetTheCurrentTime();
                    swprintf( s, 255, L"\t*****Start up timer 2 at: %d:%d:%d - %d\r\n",
                            ct.GetHour(), ct.GetMinute(), ct.GetSecond(), ct.GetMilliseconds() );
                    PAL_absclass::PAL_OutputDbgStr( s );

                    PAL_absclass::PAL_OutputDbgStr( L"\t!!!CancelTimer 1\r\n" );
                    holder.cancelTimer( 1 );

                    PAL_absclass::PAL_sleep( 3000 );
#else
                    wsprintf( s, L"\t*****Start up timer 1 at: %d:%d:%d - %d\r\n",
                            ct.GetHour(), ct.GetMinute(), ct.GetSecond(), ct.GetMilliseconds() );
                    OutputDebugString( s );

                    holder.createTimer( 400, 2 );
                    Util::CMSGTime::GetTheCurrentTime();
                    wsprintf( s, L"\t*****Start up timer 2 at: %d:%d:%d - %d\r\n",
                            ct.GetHour(), ct.GetMinute(), ct.GetSecond(), ct.GetMilliseconds() );
                    OutputDebugString( s );

                    OutputDebugString( L"\t!!!CancelTimer 1\r\n" );
                    holder.cancelTimer( 1 );

                    Sleep( 3000 );
#endif      // LINUX_FIX
                    return true;
                }
                catch (...)
                {
                    LogCatch
                        return false; // an exception was thrown, consider this a failure
                }
            }
        private:


    };


    class CServiceSpeakerTester : public CUnitTest
    {
        public:
            virtual ~CServiceSpeakerTester() {}
            CServiceSpeakerTester(){}
            const TCHAR * Name() { return _T("Test good and bad beep");};
            const TCHAR * Description() { return _T("");};        

            bool TestCode() 
            { 
                try
                {                                       
                    ServiceSpeaker::goodBeep( 5 );
#ifdef LINUX_FIX
                    PAL_absclass::PAL_sleep( 2000 );
#else
                    Sleep( 2000 );
#endif      // LINUX_FIX
                    ServiceSpeaker::badBeep( 6 );
                    return true;
                }
                catch (...)
                {
                    LogCatch
                        return false; // an exception was thrown, consider this a failure
                }
            }
        private:        

    };

    class CZipFolderTester : public CUnitTest
    {
        public:
            virtual ~CZipFolderTester() {}
            CZipFolderTester(){}
            const TCHAR * Name() { return _T("Test zip all the files within a folder");};
            const TCHAR * Description() { return _T("");};        

            bool TestCode() 
            { 
                try
                {       
#ifdef LINUX_FIX
                    Hobart::CZip::zipDir( ZIP_TEST_PATH, ZIP_TEST_FILE );
                    PAL_absclass::PAL_sleep( 500 );
                    Hobart::CZip::unZip( ZIP_TEST_FILE, UNZIP_TEST_PATH );
#else
                    Hobart::CZip::zipDir( "c:\\test\\temp", "c:\\test\\z0001.zip" );
                    Sleep( 500 );
                    Hobart::CZip::unZip( "c:\\test\\z0001.zip", "c:\\test\\t1" );
#endif      // LINUX_FIX
                    return true;
                }
                catch (...)
                {
                    LogCatch
                        return false; // an exception was thrown, consider this a failure
                }
            }
        private:        

    };

    class CHobartMainTester : public CUnitTest
    {
        public:
            virtual ~CHobartMainTester() {}
            CHobartMainTester(){}
            const TCHAR * Name() { return _T("Test Hobart All");};
            const TCHAR * Description() { return _T("");};        

            bool TestCode() 
            { 
                try
                {                                       
                    EvolMain main;
                    //                          main.startAll( 1, "c:\\hobarttest\\", "c:\\hobarttest\\" );
#if defined(LINUX_FIX)  // UnitTester is not up-to-date
                    main.startAll( 1, NULL, NULL, NULL, NULL );
#else
                    main.startAll( 1, 0, 0 );
#endif

                    while ( true )
                    {
                        static int count = 0;
                        wchar_t str[255];
#ifdef LINUX_FIX
                        swprintf( str, 255, L"Program has been running for %d minutes.\r\n ", count );
                        PAL_absclass::PAL_OutputDbgStr( str );
                        memShow();

                        PAL_absclass::PAL_sleep( 60000);
#else
                        wsprintf( str, L"Program has been running for %d minutes.\r\n ", count );
                        OutputDebugString( str );
                        memShow();

                        Sleep( 60000);
#endif      // LINUX_FIX

                        count++;
                    }
                }
                catch (...)
                {
                    LogCatch
                        return false; // an exception was thrown, consider this a failure
                }
            }
        private:        

    };

    class CUsbDisplayTester : public CUnitTest
    {
        public:
            virtual ~CUsbDisplayTester() {}
            CUsbDisplayTester(){}
            const TCHAR * Name() { return _T("Display all usb devices");};
            const TCHAR * Description() { return _T("");};        

            bool TestCode() 
            { 
#if !defined ( __linux__ )
                try
                {                                       
                    Test::DisplayUsbs dis;
                    dis.main();
                    Sleep( 1000 );
                    return true;
                }
                catch (...)
                {
                    LogCatch
                        return false; // an exception was thrown, consider this a failure
                }
#else
                return false;
#endif      // __linux__
            }
        private:        

    };



    class CGetFileSizeTester : public CUnitTest
    {
        public:
            virtual ~CGetFileSizeTester() {}
            CGetFileSizeTester(){}
            const TCHAR * Name() { return _T("Test get file size function");};
            const TCHAR * Description() { return _T("");};

            bool TestCode()
            {
                try
                {
#ifdef LINUX_FIX
//                    PAL_absclass::PAL_FILE_PARAM param;
//                    DWORD lowsize, highsize = 0;
//                    param.linux_pathname = L"test.cpp";
//                    HANDLE h = PAL_absclass::PAL_openFile(&param);
                    struct stat fstat;
                    if (stat(TEST_FILENAME, &fstat) == 0)
                    {
//                        lowsize = PAL_absclass::PAL_getFileSize(h, &highsize);
//                        PAL_absclass::PAL_closeHandle(h);
//                      if (highsize > 0)
                        if (fstat.st_size > 0xFFFF)     // > 65535 bytes;
                        {
                            PAL_absclass::PAL_OutputDbgStr( L"File is too large\r\n" );
                        }
                        else
                        {
                            wchar_t str[100];
                            swprintf( str, 100, L"File size is : %d\r\n ", fstat.st_size );
                            PAL_absclass::PAL_OutputDbgStr( str );
                        }
                        return true;
                    }
                    else
                        return false;
#else
                    int blockSize = 0;
                    int fileSize = 0; 

                    WIN32_FILE_ATTRIBUTE_DATA attr_data;
                    if ( GetFileAttributesExA(TEST_FILENAME, GetFileExInfoStandard, &attr_data ) > 0 )
                    {
                        if ( attr_data.nFileSizeHigh > 0 )
                        {
                            OutputDebugString( L"File is too large\r\n" );
                        }
                        else
                        {
                            blockSize = 1024;
                            fileSize = attr_data.nFileSizeLow;
                            wchar_t str[100];
                            wsprintf( str, L"File size is : %d\r\n ", fileSize );
                            OutputDebugString( str );
                        }
                    }
                    else
                    {
                        OutputDebugString(L"Failed to get file size.\r\n");
                        return false;
                    }
                    return true;
#endif      // LINUX_FIX
                }
                catch (...)
                {
                    LogCatch
                        return false; // an exception was thrown, consider this a failure
                }
            }
        private:        

    };



    class CWeigherCommManagerTester : public CUnitTest
    {
        public:
            virtual ~CWeigherCommManagerTester() {}
            CWeigherCommManagerTester(){}
            const TCHAR * Name() { return _T("Test WeigherCommManager");};
            const TCHAR * Description() { return _T("");};        

            bool TestCode() 
            { 
                try
                {
                    //UsbManager::getInstance()->requestScanForAPCAtBootUp();

#ifdef LINUX_FIX
                    PAL_absclass::PAL_sleep( 2000 );
#else
                    Sleep( 2000 );
#endif      // LINUX_FIX
                    int i = 0;
                    while ( i++ < 50 )
                    {
                        SystemReset wg_sys_reset;
                        wg_sys_reset.setUsbNodeId( (USBD_NODE_ID ) 2 );
                        Hobart::CMessageCenter::instance() << &wg_sys_reset;

#ifdef LINUX_FIX
                        PAL_absclass::PAL_sleep( 100 );
#else
                        Sleep( 100 );
#endif      // LINUX_FIX
                    }

#ifdef LINUX_FIX
                    PAL_absclass::PAL_sleep( 3000 );
#else
                    Sleep( 3000 );
#endif      // LINUX_FIX

                    return true;
                }
                catch (...)
                {
                    LogCatch
                        return false; // an exception was thrown, consider this a failure
                }
            }
        private:

    };

    class CPingTester : public CUnitTest
    {
        public:
            virtual ~CPingTester() {}
            CPingTester(){}
            const TCHAR * Name() { return _T("Test Ping function");};
            const TCHAR * Description() { return _T("");};

            bool TestCode()
            {
                try
                {
#ifdef LINUX_FIX
                    std::string ip_address("10.8.0.1" );
                    if ( Hobart::CPing::Ping( ip_address ) )
                        PAL_absclass::PAL_OutputDbgStr( L"Got response from 10.8.0.1\r\n" );
                    else
                        PAL_absclass::PAL_OutputDbgStr( L"Failed to Get response from 10.8.0.1\r\n" );

                    ip_address = "10.8.0.3";
                    if ( Hobart::CPing::Ping( ip_address ) )
                        PAL_absclass::PAL_OutputDbgStr( L"Got response from 10.8.0.3\r\n" );
                    else
                        PAL_absclass::PAL_OutputDbgStr( L"Failed to Get response from 10.8.0.3\r\n" );
#else
                    std::string ip_address("10.5.0.103" );
                    if ( Hobart::CPing::Ping( ip_address ) )
                        OutputDebugString( L"Got response from 10.5.0.103\r\n" );
                    else
                        OutputDebugString( L"Failed to Get response from 10.5.0.103\r\n" );

                    ip_address = "100.5.0.103";
                    if ( Hobart::CPing::Ping( ip_address ) )
                        OutputDebugString( L"Got response from 100.5.0.103\r\n" );
                    else
                        OutputDebugString( L"Failed to Get response from 100.5.0.103\r\n" );
#endif
                    return true;
                }
                catch (...)
                {
                    LogCatch
                        return false; // an exception was thrown, consider this a failure
                }
            }
        private:

    };



    //
    // Register the objects with the TestManager
    //
    void DoRegistration( Test::CUnitTestManager & manager)
    {
        manager.Register(new CHobartMainTester);
        manager.Register(new CTransactionUnitTester);
        manager.Register(new CHTTPTester);
        manager.Register(new CPrimarySynchronizationManagerTester);
        manager.Register(new CSecondarySynchronizationManagerTester);
#if !defined ( __linux__ )
        manager.Register(new CWindowTester);
#endif
        manager.Register(new CShutdownTester);
#if 0
        manager.Register(new CRendererManagerLineTester);
        manager.Register(new CRendererManagerBarcodeTester);
        manager.Register(new CRendererManagerGraphicTester);
        manager.Register(new CRendererManagerFontTester);
        manager.Register(new CRendererManagerTextTester);
        manager.Register(new CRendererManagerLabelTester);
#endif
        manager.Register(new CEvolMessageWrapperTester );
        manager.Register(new CEvolMessageWrapperTester2 );
        manager.Register(new CMessageCenterTester1 );
        manager.Register(new CMessageCenterTester2 );
        manager.Register(new CMessageCenterTester3 );
        manager.Register(new CMessageCenterTester4 );
        manager.Register(new CMessageCenterTester5 );
        manager.Register(new CMessageCenterTester6 );
        manager.Register(new CMessageCenterTester );   
        manager.Register(new CHobartTimerTester );
        manager.Register(new CHobartTimerAccuracyTester );
        manager.Register(new CHobartTimerCancelFunnctionTester );
        manager.Register(new CServiceSpeakerTester);
        manager.Register(new CZipFolderTester);
        manager.Register(new CGetFileSizeTester);
        manager.Register(new CUsbDisplayTester );
        manager.Register(new CWeigherCommManagerTester);
        manager.Register(new CPingTester );
    }

}//namespace Test
