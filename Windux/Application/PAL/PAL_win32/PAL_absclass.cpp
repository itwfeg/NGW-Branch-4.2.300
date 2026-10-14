//
// File : PAL_absclass.cpp
//
// Implemetns the win32 abstraction layer for APIs which are non-portable.
//
#include"PAL_platformheader.h"
#include <locale.h>
#include <wininet.h>
#include <process.h>
#include <string.h>

/*static*/ void PAL_API PAL_absclass::PAL_showVideoPlayer(const char *title)
{
}

/**************not being used at the moment
int _stprintf(TCHAR *buf, const TCHAR *format, ...)
{
	int ret;

	val_list	args;
	
	val_start(args, format);

	strnlen(format);
	ret = swprintf(buf, format, args);

	va_end(args);

	return ret;
}
***************************************/
DWORD PAL_absclass::PAL_logging;


void PAL_absclass::PAL_OutputDbgStrA(const char *str)
{
    ::OutputDebugStringA( str );
}

void PAL_absclass::PAL_OutputDbgStr(const wchar_t *str)
{
    ::OutputDebugString( str );
}

void PAL_absclass::PAL_OutputDbgStr(const std::string &str)
{
    ::OutputDebugStringA( str.c_str() );
}

void PAL_API PAL_absclass::PAL_raiseException(DWORD dwExceptionCode, DWORD dwExceptionFlags, DWORD nNumberOfArguments, const ULONG_PTR *lpArguments)
{
    ::RaiseException( dwExceptionCode, dwExceptionFlags, nNumberOfArguments, lpArguments );
}

DWORD PAL_API PAL_absclass::PAL_setThreadIdealProcessor(HANDLE hThread, DWORD dwIdealProcessor)
{
    return ::SetThreadIdealProcessor( hThread, dwIdealProcessor );
}

DWORD PAL_API PAL_absclass::PAL_resumeThread(HANDLE hThread)
{
    return ::ResumeThread( hThread );
}

BOOL PAL_API PAL_absclass::PAL_setThreadPriority(HANDLE hThread, int nPriority)
{
    return ::SetThreadPriority( hThread, nPriority );
}

HANDLE PAL_absclass::PAL_createThread(
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
    SIZE_T dwStackSize,
    LPTHREAD_START_ROUTINE lpStartAddress,
    LPVOID lpParameter,
    DWORD dwCreationFlags,
    LPDWORD lpThreadId)
{
	return ::CreateThread(
		lpThreadAttributes,
		dwStackSize,
		lpStartAddress,
		lpParameter,
		dwCreationFlags,
		lpThreadId);
}

uintptr_t PAL_absclass::PAL_beginthreadex(VOID *security, UINT stack_size, UINT (__stdcall * start_address)(void *), VOID *arg_list, UINT init_flag, UINT *thread_addr)
{
    return _beginthreadex( security, stack_size, start_address, arg_list, init_flag, thread_addr );
}

void PAL_absclass::PAL_endthreadex( unsigned retval )
{
    _endthreadex( retval );
}

BOOL PAL_API PAL_absclass::PAL_getExitCodeThread(HANDLE hThread, LPDWORD lpExitCode)
{
    return ::GetExitCodeThread( hThread, lpExitCode );
}

BOOL PAL_API PAL_absclass::PAL_getThreadTimes(HANDLE hThread, PAL_LPFILETIME lpCreationTime, PAL_LPFILETIME lpExitTime, PAL_LPFILETIME lpKernelTime, PAL_LPFILETIME lpUserTime)
{
    return GetThreadTimes( hThread, lpCreationTime, lpExitTime, lpKernelTime, lpUserTime );
}

BOOL PAL_API PAL_absclass::PAL_terminateThread(HANDLE hThread, DWORD dwExitCode)
{
    return ::TerminateThread( hThread, dwExitCode );
}



//////////////////////////////////////////////////////////////////
//Wrapper used to provide atomic incremenet on the Win32 platform
//
//@param: val- pointer to value to increment atomically
//@return: incremented value
//
long PAL_absclass::PAL_InterlockedIncrement(long *val)
{
	return ::InterlockedIncrement(val);
}

//////////////////////////////////////////////////////////////////
//Wrapper used to provide atomic decrement on the Win32 platform
//
//@param: val- pointer to value to decrement atomically
//@return: decremented value
//
long PAL_absclass::PAL_InterlockedDecrement(long *val)
{
	return ::InterlockedDecrement(val);
}

//////////////////////////////////////////////////////////////////
//Wrapper used to provide atomic exchange on the Win32 platform
//
//@param: dest- pointer to var to receive the new value atomically
//@param: val- value to copy automically
//@return: Orig value in dest
//
long PAL_absclass::PAL_InterlockedExchange(long *dest, long val)
{
	return ::InterlockedExchange(dest, val);
}




BOOL PAL_API PAL_absclass::PAL_getProcessAffinityMask(HANDLE hProcess, PDWORD_PTR lpProcessAffinityMask, PDWORD_PTR lpSystemAffinityMask)
{
    return ::GetProcessAffinityMask( hProcess, lpProcessAffinityMask, lpSystemAffinityMask );
}

#if 0   // Was removed from PAL_absclass.h & PAL_absclass.cpp for Linux
BOOL PAL_API PAL_absclass::PAL_setProcessAffinityMask(HANDLE hProcess, DWORD_PTR dwProcessAffinityMask)
{
    return ::SetProcessAffinityMask( hProcess, dwProcessAffinityMask );
}
#endif

BOOL PAL_API PAL_absclass::PAL_terminateProcess( HANDLE hProcess, UINT uExitCode)
{
    return ::TerminateProcess( hProcess, uExitCode );
}

void PAL_API PAL_absclass::PAL_initializeCriticalSection(PAL_LPCRITICAL_SECTION lpCriticalSection)
{
    InitializeCriticalSection( lpCriticalSection );
}

void PAL_API PAL_absclass::PAL_enterCriticalSection( PAL_LPCRITICAL_SECTION lpCriticalSection)
{
    EnterCriticalSection( lpCriticalSection );
}

void PAL_API PAL_absclass::PAL_leaveCriticalSection( PAL_LPCRITICAL_SECTION lpCriticalSection)
{
    LeaveCriticalSection( lpCriticalSection );
}

void PAL_API PAL_absclass::PAL_deleteCriticalSection( PAL_LPCRITICAL_SECTION lpCriticalSection)
{
    DeleteCriticalSection( lpCriticalSection );
}


//////////////////////////////////////////////////////////////////
//Create mutex wrapper for win32 platform
//
//@param: attrib- Win32 Security attributes
//@param: owner- initial owner of the mutex. TRUE signifies the caller. 
//@param: mutex_name- Name of the mutex
//@param: mutex-handle to the mutex
//
//@return: Mutex Handle
//
HANDLE PAL_absclass::PAL_createMutex(PAL_PMUTEX_ATTRIB attrib, BOOL owner, 
					LPCTSTR mutex_name)
{
	return ::CreateMutex(attrib, owner, mutex_name);
}

//////////////////////////////////////////////////////////////////
//Releases/destroys the mutex
//
//@param: hndl- Handle to the mutex
//
//@return: TRUE if successful else FALSE
//
BOOL PAL_absclass::PAL_releaseMutex(HANDLE hndl)
{
        return ::ReleaseMutex(hndl);
}

//////////////////////////////////////////////////////////////////
//Creates a named semaphore.
//
//@param: name - Name of the semaphore.
//@param: init_cnt - initial value of the semaphore.
//@param: max_cnt - Max value for the semaphore.
//               parameter varification in linux.
//@param: attrib - attributes for the semaphore.
//
//@return: HANDLE - Handle to the semaphore or error returned
//			by CreateSemaphore API.
//
static HANDLE PAL_createSemaphore(LPCTSTR name, LONG i, LONG j,
		PAL_PSEM_ATTRIB attrib, int asd)
{
	i=2;
	return NULL;
}
HANDLE PAL_absclass::PAL_createSemaphore(LPCTSTR name, LONG init_cnt,
                                        LONG max_cnt,
                                        PAL_PSEM_ATTRIB attrib)
{

	return ::CreateSemaphore(attrib, init_cnt, max_cnt, name);

}

//////////////////////////////////////////////////////////////////
//Releases/increments a named semaphore by the required ammount. 
//
//@param: hndl - Handle of the semaphore.
//@param: cnt - increment amount.
//@param: prev_cnt - Previouse value of the semaphore
//
//@return: BOOL - TRUE if success else FALSE.
//
BOOL PAL_absclass::PAL_incSemaphore(HANDLE hndl, LONG cnt, LPLONG prev_cnt)
{
	return ::ReleaseSemaphore(hndl, cnt, prev_cnt);
}

//////////////////////////////////////////////////////////////////
//Initializes an event object.
//
//@param: LPCTSTR - Event name.
//@param: reset_type - FALSE indicates request for event that auto
//                      resets.  Manual resets are not supported.
//@param: init_state - TRUE indicates non-blocking or semaphore
//                      value of 0 for the event.
//@param: attrib - Is always NUll in both implementations.
//
//@return: HANDLE - Handle to the event or NULL upon error.
//
HANDLE PAL_absclass::PAL_createEvent(LPCTSTR l_name, BOOL reset_type,
                        BOOL init_state, PAL_PEVENT_ATTRIB attrib)
{
        return ::CreateEvent(attrib, reset_type, init_state, 
							l_name);

}

//////////////////////////////////////////////////////////////////
//Singals the event in order to release thread(s) waiting on the
//the event.
//
//@param: hndl - Pointer to PAL_PtrMan handle containing the event
//              handle.
//
//@return: BOOL - TRUE if success else failure.
//
BOOL PAL_absclass::PAL_setEvent(HANDLE hndl)
{
        return ::SetEvent(hndl);
}

//////////////////////////////////////////////////////////////////
//Resets the event to a non-signaled state.
//
//@param: hndl - Pointer to PAL_PtrMan handle containing the event
//              handle.
//
//@return: BOOL - TRUE if success else failure.
//
BOOL PAL_absclass::PAL_resetEvent(HANDLE hndl)
{
        return ::ResetEvent(hndl); 
}

//////////////////////////////////////////////////////////////////
//Pulses the event by signaling the event object and the reseting
//it to non-signal state after releasing appropriate number of 
//waiting threads.
//
//@param: hndl - Pointer to PAL_PtrMan handle containing the event
//              handle.
//
//@return: BOOL - TRUE if success else failure.
//
BOOL PAL_absclass::PAL_pulseEvent(HANDLE hndl)
{
        return ::PulseEvent(hndl); 
}

DWORD PAL_absclass::PAL_waitForMultipleObjs(
        DWORD nCount,
        const HANDLE *lpHandles,
        BOOL bWaitAll,
        DWORD dwMilliseconds)
{
	return ::WaitForMultipleObjects(nCount, lpHandles, bWaitAll, dwMilliseconds);
}
//////////////////////////////////////////////////////////////////
//Blocks on the handle passed or on timeout.
//
//@param: hndl- Handle to use to block on.
//
//@return: Indicates the event that caused the function to return.
//
DWORD PAL_absclass::PAL_waitForSingleObj(HANDLE hndl, DWORD time)
{
	return ::WaitForSingleObject(hndl, time);
}

//////////////////////////////////////////////////////////////////
//Retrieves and returns the handle to the current process.
//
//@param: hndl - Handle to close.
//
//@return: BOOL - TRUE if success else failure.
//
BOOL PAL_absclass::PAL_closeHandle(HANDLE hndl)
{
        return ::CloseHandle(hndl);
}

//////////////////////////////////////////////////////////////////
//Retrieves and returns the handle to the current process.
//
//@param: None
//@return: PAL_PROC_HNDL - platform specific handle to the current proc
//
HANDLE PAL_absclass::PAL_getCurrProc(void)
{
        return ::GetCurrentProcess();
}

//////////////////////////////////////////////////////////////////
//Retrieves and returns the handle to the current process.
//
//@param: src_proc - Process to which orig handle belongs.
//@param: src_hndl - Handle to copy.
//@param: dest_proc - The proc receiving the duplicate handle.
//@param: desired_access - Win32 specific access flag for the handle.
//@param: inherit_hndl - Inheritability of the handle.
//@param: opt - Win32 specific options for the handle.
//
//@return: BOOL - True if Success else failure.
//
BOOL PAL_absclass::PAL_dupHandle(HANDLE src_proc, HANDLE src_hndl,
                                HANDLE dest_proc, LPHANDLE dest_hndl,
                                DWORD desired_access, BOOL inherit_hndl,
                                DWORD opt)
{
        return ::DuplicateHandle(src_proc, src_hndl, dest_proc, dest_hndl,
					desired_access, inherit_hndl, opt);
}

#if 0   // not support openfile(), use PAL_createFile() instead
//////////////////////////////////////////////////////////////////
//Open a file.
//
//@param: PAL_FILE_PARAM - Parameters to open/create file.
//
//@return: HANDLE - Handle to the FILE or NULL upon error.
//
HANDLE PAL_absclass::PAL_openFile(PAL_FILE_PARAM *pal_file_param)
{
    return ::CreateFile(
                pal_file_param->win_lpFileName,
                pal_file_param->win_dwDesireAccess,
                pal_file_param->win_dwShareMode,
                pal_file_param->win_lpSecurityAttributes,
                pal_file_param->win_dwCreationDisposition,
                pal_file_param->win_dwFlagsAndAttributes,
                pal_file_param->win_hTemplateFile
            );
}
#endif  // not support openfile()

BOOL PAL_absclass::PAL_getFileSize(const char *name, int *fileSize)
{
    struct stat fstat;

    if (stat(name, &fstat) == 0)
    {
        *fileSize = fstat.st_size;
//        *blockSize = fstat.st_blksize;
        return TRUE;
    }
    return FALSE;
}

HANDLE   PAL_absclass::PAL_createFile(LPCSTR name, DWORD access_right, DWORD share_mode, LPSECURITY_ATTRIBUTES secure_attr_p, DWORD disposition, DWORD flag_and_attribute, HANDLE template_file)
{
    return ::CreateFileA(
                name,
                access_right,
                share_mode,
                secure_attr_p,
                disposition,
                flag_and_attribute,
                template_file
            );
}

HANDLE   PAL_absclass::PAL_createFile(LPCTSTR name, DWORD access_right, DWORD share_mode, LPSECURITY_ATTRIBUTES secure_attr_p, DWORD disposition, DWORD flag_and_attribute, HANDLE template_file)
{
    return ::CreateFile(
                name,
                access_right,
                share_mode,
                secure_attr_p,
                disposition,
                flag_and_attribute,
                template_file
            );
}

//////////////////////////////////////////////////////////////////
//Read data from file
//
//@param: hndl          - File handle.
//@param: buf_p         - Pointer to buffer receiving the data read from a file.
//@param: bytes_to_read - Number of bytes to be read.
//@param: bytes_read_p  - Pointer to variable receiving the number of bytes read (Synchronous read)
//@param: overlap_p     - Pointer to OVERLAPPED structure for Asynchronous read.
//
//@return: BOOL - TRUE) Success, FALSE) Fail.
//
BOOL PAL_absclass::PAL_readFile(HANDLE hndl, LPVOID buf_p, DWORD bytes_to_read, LPDWORD bytes_read_p, PAL_POVERLAPPED overlap_p)
{
    return ::ReadFile(
                hndl,
                buf_p,
                bytes_to_read,
                bytes_read_p,
                overlap_p
            );
}

//////////////////////////////////////////////////////////////////
//Write data to file
//
//@param: hndl              - File handle.
//@param: buf_p             - Pointer to buffer containing the data to write to a file.
//@param: bytes_to_write    - Number of bytes to be written.
//@param: bytes_written_p   - Pointer to variable receiving the number of bytes written (Synchronous write)
//@param: overlap_p         - Pointer to OVERLAPPED structure for Asynchronous write.
//
//@return: BOOL - TRUE) Success, FALSE) Fail.
//
BOOL PAL_absclass::PAL_writeFile(HANDLE hndl, LPCVOID buf_p, DWORD bytes_to_write, LPDWORD bytes_written_p, PAL_POVERLAPPED overlap_p)
{
    return ::WriteFile(
                hndl,
                buf_p,
                bytes_to_write,
                bytes_written_p,
                overlap_p
            );
}

DWORD PAL_absclass::PAL_getFileVersionInfoSize ( LPCTSTR lptstrFilename, LPDWORD lpdwHandle)
{
    return ::GetFileVersionInfoSize( lptstrFilename, lpdwHandle );
}

BOOL PAL_absclass::PAL_getFileVersionInfo(
        LPCTSTR lptstrFilename,
        DWORD dwHandle,
        DWORD dwLen,
        LPVOID lpData)
{
    return ::GetFileVersionInfo( lptstrFilename, dwHandle, dwLen, lpData);
}

DWORD PAL_API PAL_absclass::PAL_getFileSize( HANDLE hFile, LPDWORD lpFileSizeHigh)
{
    return ::GetFileSize( hFile, lpFileSizeHigh );
}

DWORD PAL_API PAL_absclass::PAL_setFilePointer(HANDLE hFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod)
{
    return ::SetFilePointer( hFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod );
}

// http://msdn.microsoft.com/en-us/library/ms724205%28v=VS.85%29.aspx
BOOL PAL_API PAL_absclass::PAL_touch(HANDLE hFile)
{
    BOOL bRc = FALSE;
    PAL_TIME_STRUCT st;
    PAL_FILETIME    ft;

    ::GetLocalTime( &st );
    if ( ::SystemTimeToFileTime( &st, &ft ) )
    {
        bRc = ::SetFileTime( hFile, NULL, &ft, &ft );
    }

    return bRc;
}



//////////////////////////////////////////////////////////////////
//
// Cancel all pending I/O operation that are issued by the calling thread.
//
// @param: hndl - Pointer to PAL_PtrMan handle.
//
// @return: BOOL - TRUE if success else failure.
//
BOOL PAL_absclass::PAL_cancelIo(HANDLE hndl)
{
    return ::CancelIo(hndl);
}

BOOL PAL_absclass::PAL_deleteFile(LPCTSTR lpFileName)
{
    return ::DeleteFile( lpFileName );
}

std::vector<std::string> PAL_API PAL_absclass::PAL_getDirectoryList(const char *path)
{
	std::vector<std::string> v;

	std::string pattern = path;
	if ('\\' != pattern[pattern.length() - 1])
	{
		pattern += '\\';
	}
	pattern += '*';

	WIN32_FIND_DATAA findData = { 0 };
	HANDLE hFind = FindFirstFileA(pattern.c_str(), &findData);
	if (INVALID_HANDLE_VALUE != hFind)
	{
		do
		{
			if (0 == (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				v.push_back(findData.cFileName);
			}
		}
		while (FindNextFileA(hFind, &findData));
	}
	FindClose(hFind);
	return v;
}

BOOL PAL_API PAL_absclass::PAL_setCurrentDirectoryA(LPCSTR lpPathName)
{
    return ::SetCurrentDirectoryA(lpPathName);
}

DWORD PAL_API PAL_absclass::PAL_getCurrentDirectoryA(DWORD nBufferLength, LPSTR lpBuffer)
{
    return ::GetCurrentDirectoryA( nBufferLength, lpBuffer );
}

DWORD PAL_API PAL_absclass::PAL_getCurrentDirectoryW(DWORD nBufferLength, LPTSTR lpBuffer)
{
    return ::GetCurrentDirectoryW( nBufferLength, lpBuffer );
}

#if 0   // Alamy: Remove this function
DWORD PAL_API PAL_absclass::PAL_formatMessage(DWORD dwFlags, LPCVOID lpSource, DWORD dwMessageId, DWORD dwLanguageId, LPTSTR lpBuffer, DWORD nSize, va_list *Arguments)
{
    return ::FormatMessage( dwFlags, lpSource, dwMessageId, dwLanguageId, lpBuffer, nSize, Arguments);
}
#endif

HANDLE PAL_API PAL_absclass::PAL_createWaitableTimer(LPSECURITY_ATTRIBUTES lpTimerAttributes, BOOL bManualReset, LPCTSTR lpTimerName)
{
    return ::CreateWaitableTimer( lpTimerAttributes, bManualReset, lpTimerName );
}

BOOL PAL_API PAL_absclass::PAL_cancelWaitableTimer(HANDLE hTimer)
{
    return ::CancelWaitableTimer( hTimer );
}

#if defined ( __linux__ )
BOOL PAL_API PAL_absclass::PAL_setWaitableTimer(HANDLE hTimer, const LARGE_INTEGER *pDueTime, LONG lPeriod, void *pfnCompletionRoutine, LPVOID lpArgToCompletionRoutine, BOOL fResume)
#else
BOOL PAL_API PAL_absclass::PAL_setWaitableTimer(HANDLE hTimer, const LARGE_INTEGER *pDueTime, LONG lPeriod, PAL_PTIMERAPCROUTINE pfnCompletionRoutine, LPVOID lpArgToCompletionRoutine, BOOL fResume)
#endif
{
    return ::SetWaitableTimer( hTimer, pDueTime, lPeriod, (PAL_PTIMERAPCROUTINE)pfnCompletionRoutine, lpArgToCompletionRoutine, fResume );
}

PAL_HMODULE PAL_API PAL_absclass::PAL_getModuleHandle(LPCTSTR lpModuleName)
{
    return ::GetModuleHandle( lpModuleName );
}

DWORD PAL_API PAL_absclass::PAL_getModuleFileName(PAL_HMODULE hModule, LPTSTR lpFilename, DWORD nSize)
{
    return ::GetModuleFileName( hModule, lpFilename, nSize );
}

PAL_HDEVNOTIFY PAL_API PAL_absclass::PAL_registerDeviceNotification(HANDLE hRecipient, LPVOID NotificationFilter, DWORD Flags)
{
    return ::RegisterDeviceNotification( hRecipient, NotificationFilter, Flags );
}

BOOL PAL_API PAL_absclass::PAL_unregisterDeviceNotification(PAL_HDEVNOTIFY Handle)
{
    return ::UnregisterDeviceNotification( Handle );
}



//////////////////////////////////////////////////////////////////
//Wrapper for windows specific sleep routine
//
//@param: dw_msec - time to sleep in msec.
//
VOID PAL_absclass::PAL_sleep(DWORD dw_msec)
{
    ::Sleep(dw_msec);
}

//////////////////////////////////////////////////////////////////
//Wrapper for windows specific Tick Count routine
//@param: NONE
//
//@return: DWORD - number of milliseconds that have elapsed since 
//			the system was started, up to 49.7 days.
//
DWORD PAL_absclass::PAL_getTickCount()
{
    return ::GetTickCount();
}

//////////////////////////////////////////////////////////////////
//Wrapper for windows specific SetLocalTime routine
//@param: time_struct_p - A pointer to SYSTEMTIME (PAL_PTIME_STRUCT) structure that contains the date and time.
//
//@return: BOOL - TRUE (non-zero) on success.
//
BOOL PAL_absclass::PAL_setLocalTime(PAL_PTIME_STRUCT time_struct_p)
{
    return ::SetLocalTime( time_struct_p );
}

BOOL PAL_absclass::PAL_systemTimeToFileTime(const PAL_TIME_STRUCT *lpSystemTime, PAL_LPFILETIME lpFileTime)
{
    return ::SystemTimeToFileTime( lpSystemTime, lpFileTime );
}

BOOL PAL_API PAL_absclass::PAL_fileTimeToDosDateTime(
                const PAL_LPFILETIME lpFiletime,
                LPWORD lpFatDate,
                LPWORD lpFatTime)
{
    return ::FileTimeToDosDateTime(lpFiletime, lpFatDate, lpFatTime);
}

BOOL PAL_absclass::PAL_fileTimeToLocalFileTime(const PAL_FILETIME *lpFileTime, PAL_LPFILETIME lpLocalFileTime)
{
    return ::FileTimeToLocalFileTime( lpFileTime, lpLocalFileTime );
}

BOOL PAL_absclass::PAL_fileTimeToSystemTime(const PAL_FILETIME *lpFileTime, PAL_PTIME_STRUCT lpSystemTime)
{
    return ::FileTimeToSystemTime( lpFileTime, lpSystemTime );
}

void PAL_absclass::PAL_getLocalTime( PAL_PTIME_STRUCT lpSystemTime)
{
    ::GetLocalTime( lpSystemTime );
}

void PAL_absclass::PAL_getSystemTime( PAL_PTIME_STRUCT lpSystemTime)
{
    ::GetSystemTime( lpSystemTime );
}

int PAL_absclass::PAL_getTimeFormat(
                LCID Locale,
                DWORD dwFlags,
                const PAL_TIME_STRUCT *lpTime,
                LPCTSTR lpFormat,
                LPTSTR lpTimeStr,
                int cchTime
                )
{
    return ::GetTimeFormat( Locale, dwFlags, lpTime, lpFormat, lpTimeStr, cchTime );
}

int PAL_absclass::PAL_getDateFormat(
                LCID Locale,
                DWORD dwFlags,
                const PAL_TIME_STRUCT *lpDate,
                LPCTSTR lpFormat,
                LPTSTR lpDateStr,
                int cchDate
                )
{
    return ::GetDateFormat( Locale, dwFlags, lpDate, lpFormat, lpDateStr, cchDate );
}

DWORD PAL_absclass::PAL_getTimeZoneInformation(LPTIME_ZONE_INFORMATION lpTimeZoneInformation)
{
	return ::GetTimeZoneInformation(lpTimeZoneInformation);
}

//////////////////////////////////////////////////////////////////
//@param: category - Category of locale.
//@param: locale - locale name
//
//@return: _locale_t structure.
//
PAL_LOCALE_TYPE PAL_absclass::PAL_createLocale(int category, const char *locale_name)
{
    return ::_create_locale(category, locale_name);
}

//////////////////////////////////////////////////////////////////
//This is a linux specific API and is not used in windows
//
//
void PAL_absclass::PAL_freeLocale(PAL_LOCALE_TYPE locale_to_free)
{
	//not used in windows
}

int PAL_absclass::PAL_MB2WC(UINT CodePage, DWORD dwFlags, LPCSTR lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar)
{
    return ::MultiByteToWideChar( CodePage, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar );
}

int PAL_absclass::PAL_WC2MB(UINT CodePage, DWORD dwFlags, LPCWSTR lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar)
{
    return ::WideCharToMultiByte( CodePage, dwFlags, lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte, lpDefaultChar, lpUsedDefaultChar );
}


//int PAL_absclass::PAL_wsaStartup(WORD version_request, LPWSADATA wsa_data)
//{
//    return ::WSAStartup( version_request, wsa_data );
//}

//////////////////////////////////////////////////////////////////
//
//
// ***** WARNING: need to add comment
//
int PAL_absclass::PAL_wsaGetLastError(void)
{
    return ::WSAGetLastError();
}

int PAL_absclass::PAL_ioctlsocket( PAL_SOCKET s, long cmd, u_long *argp)
{
    return ::ioctlsocket( s, cmd, argp );
}

int PAL_absclass::PAL_closesocket( PAL_SOCKET s)
{
    return ::closesocket( s );
}

BOOL PAL_absclass::PAL_isNetworkAlive( LPDWORD lpdwFlags)
{
    return ::IsNetworkAlive( lpdwFlags );
}

DWORD PAL_absclass::PAL_getAdaptersInfo( PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen)
{
    return ::GetAdaptersInfo( pAdapterInfo, pOutBufLen );
}

char* PAL_absclass::extendedErrorMessageFormat = "cURL FTP error code is %ld";
unsigned long PAL_absclass::extendedError=0;

void PAL_absclass::setExtendedError( unsigned long err )
{
	extendedError = err;
}

unsigned long PAL_absclass::getExtendedError()
{
	return( extendedError );
}

bool PAL_absclass::PAL_InternetGetLastResponseInfo(unsigned long* pdwError, char* data, unsigned long* dwErrorBufLength)
{
	bool retVal = true;
	if( *dwErrorBufLength < 34 )
	{
		*dwErrorBufLength = 34;
		::SetLastError(ERROR_INSUFFICIENT_BUFFER);
		setExtendedError(0);
	}
	sprintf( data, extendedErrorMessageFormat, getExtendedError() );
	return( retVal );
}

//////////////////////////////////////////////////////////////////
//
//
//
PAL_INET_HANDLE PAL_absclass::PAL_internetOpen(
    PAL_INET_STR lpszAgent,
    DWORD dwAccessType,
    PAL_INET_STR lpszProxy,
    PAL_INET_STR lpszProxyBypass,
    DWORD dwFlags
    )
{
    return ::InternetOpenA(
        lpszAgent,dwAccessType,lpszProxy,lpszProxyBypass,dwFlags);
}

PAL_INET_HANDLE PAL_absclass::PAL_internetConnect(
        PAL_INET_HANDLE hInternet,
        PAL_INET_STR lpszServerNameAndPath,
        INTERNET_PORT nServerPort,
        PAL_INET_STR lpszUsername,
        PAL_INET_STR lpszPassword,
        DWORD dwService,
        DWORD dwFlags,
        DWORD_PTR dwContext)
{
    return ::InternetConnectA
            (
                hInternet,
                lpszServerNameAndPath,
                nServerPort,
                lpszUsername,
                lpszPassword,
                dwService,
                dwFlags,
                dwContext
            );
}

bool PAL_absclass::PAL_ftpGetFile(
    PAL_INET_HANDLE inet_handle, 
    PAL_INET_STR lpszRemoteFile,
    PAL_INET_STR lpszNewFile,
    BOOL fFailIfExists,
    DWORD dwFlagsAndAttributes,
    DWORD dwFlags,
    DWORD_PTR dwContext)
{
    return ::FtpGetFileA(
                inet_handle,
                lpszRemoteFile,
                lpszNewFile,
                fFailIfExists,
                dwFlagsAndAttributes,
                dwFlags,
                dwContext
            );
}

bool PAL_absclass::PAL_ftpPutFile(
	PAL_INET_HANDLE inet_handle,
	PAL_INET_STR new_file,
	PAL_INET_STR remote_file,
	DWORD flags,
	DWORD_PTR context)
{
	return ::FtpPutFileA(
		inet_handle,
		new_file,
		remote_file,
		flags,
		context
		);
}

bool PAL_absclass::PAL_ftpCreateDirectory(
	PAL_INET_HANDLE inet_handle,
	PAL_INET_STR remote_directory
	)
{
	return ::FtpCreateDirectoryA(
		inet_handle,
		remote_directory
		);
}

bool PAL_absclass::PAL_internetFindNextFile(PAL_INET_HANDLE inet_handle, PVOID find_data)
{
    return ::InternetFindNextFileA(inet_handle, find_data);
}

BOOL PAL_absclass::PAL_internetCloseHandle(PAL_INET_HANDLE inet_handle)
{
    return ::InternetCloseHandle( inet_handle );
}


//////////////////////////////////////////////////////////////////
//Returns the calling thread's last-error code.
//
//@param: None
//
//@return: DWORD - the error code.
//
DWORD PAL_absclass::PAL_getLastError()
{
    return ::GetLastError();
}

BOOL PAL_absclass::PAL_beep(DWORD freq, DWORD duration)
{
    return ::Beep(freq, duration);
}

//////////////////////////////////////////////////////////////////
//Wrapper for windows specific beep routine
//@param: NONE
//
//@return: BOOL - True if Success else failure.
//
BOOL PAL_API PAL_absclass::PAL_simpleBeep()
{
    return ::MessageBeep( 0xFFFFFFFF );
}

//////////////////////////////////////////////////////////////////
//Wrapper for windows specific font routine
//
//@param: font_hanle - HANDLE of font
//
//@return: BOOL - True if Success else failure.
//
BOOL PAL_absclass::PAL_deleteFont(HANDLE font_handle)
{
    return ::DeleteFont( font_handle );
}

DWORD PAL_API PAL_absclass::PAL_tlsAlloc(void)
{
    return ::TlsAlloc();
}

BOOL PAL_API PAL_absclass::PAL_tlsFree( DWORD dwTlsIndex)
{
    return ::TlsFree( dwTlsIndex );
}

#if 0   // ***** WARNING: Is there a function in Windows ?  What does this function for ?
HANDLE PAL_absclass::PAL_initThrdData(void)
{
    return ::InitThrdData(void);
}
#endif

LPVOID PAL_absclass::PAL_tlsGetValue(DWORD tlsIndex)
{
    return ::TlsGetValue( tlsIndex );
}

BOOL PAL_API PAL_absclass::PAL_tlsSetValue( DWORD dwTlsIndex, LPVOID lpTlsValue)
{
    return TlsSetValue( dwTlsIndex, lpTlsValue );
}

void PAL_API PAL_absclass::PAL_globalMemoryStatus(PAL_LPMEMORYSTATUS lpBuffer)
{
    return GlobalMemoryStatus(lpBuffer);
}


/// Execute an external program and get the results is writes to stdout
int PAL_API PAL_absclass::PAL_executeWithResults(const std::string &command, std::string &commandOutput)
{
	//#include <stdio.h>

    int status = 0, procStatus = 0;

#if defined( __linux__ )
FILE *fp;
int status;
char path[PATH_MAX];


fp = popen("ls *", "r");
if (fp == NULL)
    /* Handle error */;


while (fgets(path, PATH_MAX, fp) != NULL)
    printf("%s", path);


status = pclose(fp);
if (status == -1) {
    /* Error reported by pclose() */
    ...
} else {
    /* Use macros described under wait() to inspect `status' in order
       to determine success/failure of command executed by popen() */
    ...
}

#else



   char   psBuffer[128];
   FILE   *pPipe;

        /* Run DIR so that it writes its output to a pipe. Open this
         * pipe with read text attribute so that we can read it 
         * like a text file. 
         */

   commandOutput = "";

   if ( (pPipe = _popen( command.c_str(), "rt" )) == NULL )
      return 0;

   /* Read pipe until end of file, or an error occurs. */

   while (fgets(psBuffer, 128, pPipe))
   {
	   commandOutput.append(psBuffer);
   }


   /* Close pipe and print return value of pPipe. */
   if (feof( pPipe))
   {
     //printf( "\nProcess returned %d\n", _pclose( pPipe ) );
   }
   else
   {
     //printf( "Error: Failed to read the pipe to the end.\n");
		commandOutput = "";
   }

	status = _pclose( pPipe ); // / 256;
	return status;


#endif

	return 0;
}



