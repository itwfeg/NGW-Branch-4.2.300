#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tchar.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#if defined ( __linux__ )
#include <Event.h>
#else
#include "Event.h"
#endif // __linux__
#include <ShellAPI.h>
#include <Strsafe.h>

#include <stdio.h>
#include <objbase.h>
#include <msxml6.h>

SERVICE_STATUS        g_ServiceStatus = {0};
SERVICE_STATUS_HANDLE g_StatusHandle = NULL;
HANDLE                g_ServiceStopEvent = INVALID_HANDLE_VALUE;

HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;
HANDLE g_hChildStd_ERR_Rd = NULL;
HANDLE g_hChildStd_ERR_Wr = NULL;
HANDLE g_hChildStd_IN_Rd = NULL;
HANDLE g_hChildStd_IN_Wr = NULL;

VOID WINAPI ServiceMain (DWORD argc, LPTSTR *argv);
VOID WINAPI ServiceCtrlHandler (DWORD);
DWORD WINAPI ServiceWorkerThread (LPVOID lpParam);
DWORD WINAPI WinsockThread (LPVOID lpParam);

DWORD ExecuteProcess (LPTSTR szCommandLine, LPSTR *pszStdOut, LPSTR *pszStdErr);
LPSTR ReadStreamToEnd (HANDLE hStream);
LPSTR FormatError (LPTSTR lpszFunction, DWORD dwError);

#define SOCKIP 127.0.0.1
#define SOCKPORT "5080"
#define DEFAULT_BUFLEN 512

#define SERVICE_NAME  _T("Hobart HD")

int _tmain (int argc, TCHAR *argv[])
{
	SERVICE_TABLE_ENTRY ServiceTable[] = 
	{
		{SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION) ServiceMain},
		{NULL, NULL}
	};

	if (StartServiceCtrlDispatcher (ServiceTable) == FALSE)
	{
		return GetLastError ();
	}

	return 0;
}

VOID WINAPI ServiceMain (DWORD argc, LPTSTR *argv)
{
	DWORD Status = E_FAIL;

	g_StatusHandle = RegisterServiceCtrlHandler (SERVICE_NAME, ServiceCtrlHandler);

	if (g_StatusHandle == NULL) 
	{
		return;
	}

	// Tell the service controller we are starting
	ZeroMemory (&g_ServiceStatus, sizeof (g_ServiceStatus));
	g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	g_ServiceStatus.dwControlsAccepted = 0;
	g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	g_ServiceStatus.dwWin32ExitCode = 0;
	g_ServiceStatus.dwServiceSpecificExitCode = 0;
	g_ServiceStatus.dwCheckPoint = 0;

	SetServiceStatus (g_StatusHandle, &g_ServiceStatus);

	// Create stop event to wait on later.
	g_ServiceStopEvent = CreateEvent (NULL, TRUE, FALSE, NULL);
	if (g_ServiceStopEvent == NULL) 
	{
		g_ServiceStatus.dwControlsAccepted = 0;
		g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		g_ServiceStatus.dwWin32ExitCode = GetLastError();
		g_ServiceStatus.dwCheckPoint = 1;

		SetServiceStatus (g_StatusHandle, &g_ServiceStatus);
		return; 
	}

	// Tell the service controller we are started
	g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	g_ServiceStatus.dwWin32ExitCode = 0;
	g_ServiceStatus.dwCheckPoint = 0;

	SetServiceStatus (g_StatusHandle, &g_ServiceStatus);

	// Start the thread that will check for stop messages
	HANDLE hThread = CreateThread (NULL, 0, ServiceWorkerThread, NULL, 0, NULL);

	// Start the thread that will run the winsock server
	HANDLE hThreadWinSock = CreateThread (NULL, 0, WinsockThread, NULL, 0, NULL);

	// Wait until our worker thread exits effectively signaling that the service needs to stop
	WaitForSingleObject (hThread, INFINITE);

	CloseHandle (g_ServiceStopEvent);

	g_ServiceStatus.dwControlsAccepted = 0;
	g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
	g_ServiceStatus.dwWin32ExitCode = 0;
	g_ServiceStatus.dwCheckPoint = 3;

	SetServiceStatus (g_StatusHandle, &g_ServiceStatus);

	return;
}

VOID WINAPI ServiceCtrlHandler (DWORD CtrlCode)
{
	switch (CtrlCode) 
	{
	case SERVICE_CONTROL_STOP :
		if (g_ServiceStatus.dwCurrentState != SERVICE_RUNNING)
			break;

		g_ServiceStatus.dwControlsAccepted = 0;
		g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
		g_ServiceStatus.dwWin32ExitCode = 0;
		g_ServiceStatus.dwCheckPoint = 4;

		SetServiceStatus (g_StatusHandle, &g_ServiceStatus);

		// This will signal the worker thread to start shutting down
		SetEvent (g_ServiceStopEvent);

		break;

	default:
		break;
	}

}

DWORD ExecuteProcess (LPTSTR szCommandLine, LPSTR *pszStdOut, LPSTR *pszStdErr) 
{ 
	// Set the bInheritHandle flag so pipe handles are inherited. 
	SECURITY_ATTRIBUTES saAttr; 
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL; 

	// Create a pipe for the child process's STDOUT. 
	if ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0) )
	{
		DWORD dwError = GetLastError();
		if (pszStdErr)
		{
			*pszStdErr = FormatError(_T("CreatePipe"), dwError);
		}
		return dwError;
	}

	// Ensure the read handle to the pipe for STDOUT is not inherited.
	if ( ! SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )
	{
		DWORD dwError = GetLastError();
		if (pszStdErr)
		{
			*pszStdErr = FormatError(_T("SetHandleInformation"), dwError);
		}
		return dwError;
	}

	// Create a pipe for the child process's STDERR. 
	if (! CreatePipe(&g_hChildStd_ERR_Rd, &g_hChildStd_ERR_Wr, &saAttr, 0))
	{
		DWORD dwError = GetLastError();
		if (pszStdErr)
		{
			*pszStdErr = FormatError(_T("CreatePipe"), dwError);
		}
		return dwError;
	}

	// Ensure the write handle to the pipe for STDERR is not inherited. 
	if ( ! SetHandleInformation(g_hChildStd_ERR_Rd, HANDLE_FLAG_INHERIT, 0) )
	{
		DWORD dwError = GetLastError();
		if (pszStdErr)
		{
			*pszStdErr = FormatError(_T("SetHandleInformation"), dwError);
		}
		return dwError;
	}

	// Create a pipe for the child process's STDIN. 
	if (! CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0))
	{
		DWORD dwError = GetLastError();
		if (pszStdErr)
		{
			*pszStdErr = FormatError(_T("CreatePipe"), dwError);
		}
		return dwError;
	}

	// Ensure the write handle to the pipe for STDIN is not inherited. 
	if ( ! SetHandleInformation(g_hChildStd_IN_Rd, HANDLE_FLAG_INHERIT, 0) )
	{
		DWORD dwError = GetLastError();
		if (pszStdErr)
		{
			*pszStdErr = FormatError(_T("SetHandleInformation"), dwError);
		}
		return dwError;
	}

	// Create the child process. 
	PROCESS_INFORMATION piProcInfo; 
	STARTUPINFO siStartInfo;
	BOOL bSuccess = FALSE; 

	// Set up members of the PROCESS_INFORMATION structure. 
	ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );

	// Set up members of the STARTUPINFO structure. 
	// This structure specifies the STDIN and STDOUT handles for redirection.
	ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
	siStartInfo.cb = sizeof(STARTUPINFO); 
	siStartInfo.hStdError = g_hChildStd_ERR_Wr;
	siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
	siStartInfo.hStdInput = g_hChildStd_IN_Wr;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

	// Create the child process. 
	bSuccess = CreateProcess(NULL, 
		szCommandLine, // command line 
		NULL,          // process security attributes 
		NULL,          // primary thread security attributes 
		TRUE,          // handles are inherited 
		0,             // creation flags 
		NULL,          // use parent's environment 
		NULL,          // use parent's current directory 
		&siStartInfo,  // STARTUPINFO pointer 
		&piProcInfo);  // receives PROCESS_INFORMATION 

	// If an error occurs, exit the application. 
	if (!bSuccess)
	{
		DWORD dwError = GetLastError();
		if (pszStdErr)
		{
			*pszStdErr = FormatError(_T("CreateProcess"), dwError);
		}
		return dwError;
	}

    // Wait until child process exits.
    WaitForSingleObject( piProcInfo.hProcess, INFINITE );
	
	DWORD exitCode = 0;
	GetExitCodeProcess(piProcInfo.hProcess, &exitCode);

	CloseHandle(g_hChildStd_OUT_Wr);
	// Read stdout
	if (pszStdOut)
	{
		*pszStdOut = ReadStreamToEnd(g_hChildStd_OUT_Rd);
	}
	CloseHandle(g_hChildStd_OUT_Rd);
	
	// Read stderr
	CloseHandle(g_hChildStd_ERR_Wr);
	if (pszStdErr)
	{
		*pszStdErr = ReadStreamToEnd(g_hChildStd_ERR_Rd);
	}
	CloseHandle(g_hChildStd_ERR_Rd);

	CloseHandle(g_hChildStd_IN_Wr);
	CloseHandle(g_hChildStd_IN_Rd);

    // Close process and thread handles. 
    CloseHandle( piProcInfo.hProcess );
    CloseHandle( piProcInfo.hThread );

	return exitCode; 
}

LPSTR ReadStreamToEnd (HANDLE hStream)
{
	// Read output from stream, Stop when there is no more data. 
	const DWORD ALLOC_SIZE = 512;
	DWORD dwRead = 0;
	DWORD dwTotalRead = 0;
	CHAR *readBuf = (CHAR *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, ALLOC_SIZE);
	while ((ReadFile(hStream, &readBuf[dwTotalRead], ALLOC_SIZE, &dwRead, NULL)) && (dwRead > 0))
	{
		dwTotalRead += dwRead;
		readBuf = (CHAR *)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, readBuf, dwTotalRead + ALLOC_SIZE);
	}
	return readBuf;
}

LPSTR FormatError (LPTSTR lpszFunction, DWORD dwError) 
{ 
    LPTSTR lpMsgBuf;
    LPTSTR lpDisplayBuf;

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dwError,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0, NULL );

    lpDisplayBuf = (LPTSTR)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf)+lstrlen((LPCTSTR)lpszFunction)+64)*sizeof(TCHAR)); 
    StringCchPrintf(lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("HobartHD error: \"%s\" failed with error %d: \"%s\""), 
        lpszFunction, dwError, lpMsgBuf);

	LocalFree(lpMsgBuf);
	
	DWORD dwAllocSize = WideCharToMultiByte(CP_ACP, 0, (LPTSTR)lpDisplayBuf, -1, 0, 0, NULL, NULL);
	LPSTR lpErrorBuf = (LPSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwAllocSize);
	WideCharToMultiByte(CP_ACP, 0, (LPTSTR)lpDisplayBuf, -1, lpErrorBuf, dwAllocSize, NULL, NULL);

	LocalFree(lpDisplayBuf);

	return lpErrorBuf;
}

DWORD WINAPI ExecuteProcessThread (LPVOID lpParam)
{
	SOCKET ClientSocket = (SOCKET)lpParam;

	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	memset(&recvbuf, 0, recvbuflen);
	int iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
	if (iResult > 0) 
	{
		// Determine number of wide characters to be allocated for the
		// Unicode string.
		int cCharacters =  iResult + 1;

		LPWSTR pszW = NULL;

		pszW = (LPWSTR)LocalAlloc(
			LPTR,
			cCharacters*2);

		memset((LPVOID)pszW, 0, cCharacters*2);

		if (NULL != pszW)
		{
			recvbuf[iResult] = '\0';
			MultiByteToWideChar(CP_ACP, 0, recvbuf, cCharacters, pszW, cCharacters);
				
			LPSTR szStdOut = NULL;
			LPSTR szStdErr = NULL;
			DWORD exitCode = ExecuteProcess(pszW, &szStdOut, &szStdErr);

			CHAR szExitCode[32];
			ZeroMemory(szExitCode, sizeof(szExitCode));
			StringCbPrintfA(szExitCode, sizeof(szExitCode), "%d", exitCode);
			size_t dwExitCodeSzLength = 0;
			StringCbLengthA(szExitCode, sizeof(szExitCode), &dwExitCodeSzLength);
			send(ClientSocket, szExitCode, (int)(dwExitCodeSzLength + 1), 0);

			size_t dwStdOutSzLength = 0;
			if (szStdOut != NULL)
			{
				StringCchLengthA(szStdOut, STRSAFE_MAX_CCH, &dwStdOutSzLength);
				send(ClientSocket, szStdOut, (int)(dwStdOutSzLength + 1), 0);
				HeapFree(GetProcessHeap(), 0, szStdOut);
			}
			else
			{
				send(ClientSocket, "", 1, 0);
			}

			size_t dwStdErrSzLength = 0;
			if (szStdErr != NULL)
			{
				StringCchLengthA(szStdErr, STRSAFE_MAX_CCH, &dwStdErrSzLength);
				send(ClientSocket, szStdErr, (int)(dwStdErrSzLength + 1), 0);
				HeapFree(GetProcessHeap(), 0, szStdErr);
			}
			else
			{
				send(ClientSocket, "", 1, 0);
			}

			LocalFree(pszW);
		}
	}
	shutdown(ClientSocket, SD_BOTH);
	closesocket(ClientSocket);

	return ERROR_SUCCESS;
}

DWORD WINAPI WinsockThread (LPVOID lpParam)
{
	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	while (true)
	{
		// Initialize Winsock
		iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
		if (iResult != 0) 
		{
			return 1;
		}

		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		// Resolve the server address and port
		iResult = getaddrinfo(NULL, SOCKPORT, &hints, &result);
		if ( iResult != 0 ) 
		{
			WSACleanup();
			return 1;
		}

		// Create a SOCKET for connecting to server
		ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (ListenSocket == INVALID_SOCKET) 
		{
			freeaddrinfo(result);
			WSACleanup();
			return 1;
		}

		// Setup the TCP listening socket
		iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR) 
		{
			freeaddrinfo(result);
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}

		freeaddrinfo(result);

		// Receive until the peer shuts down the connection
		while ((iResult = listen(ListenSocket, SOMAXCONN)) != SOCKET_ERROR)
		{
			// Accept a client socket
			SOCKET ClientSocket = ClientSocket = accept(ListenSocket, NULL, NULL);
			if (ClientSocket != INVALID_SOCKET) 
			{
				HANDLE hThreadExecuteProcess = CreateThread (NULL, 0, ExecuteProcessThread, (LPVOID)ClientSocket, 0, NULL);
				CloseHandle( hThreadExecuteProcess );
			}
		}

		// No longer need server socket
		closesocket(ListenSocket);

		// cleanup
		WSACleanup();
	}

	return ERROR_SUCCESS;
}

DWORD WINAPI ServiceWorkerThread (LPVOID lpParam)
{
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	while (WaitForSingleObject(g_ServiceStopEvent, 0) != WAIT_OBJECT_0)
	{
		Sleep(3000);
	}

	CoUninitialize();

	return ERROR_SUCCESS;
}
