// winAppLauncher.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "winAppLauncher.h"
#include "winAppLauncherDlg.h"

#ifndef NOFBWF
#include "fbwfapi.h"
#endif

#include "properties\properties.h"
#include "Bool.h"
#include "Integer.h"
#include "fbwfapi.h"

#include <PAL_platformheader.h>
#include <winsvc.h>
#include <string>
#include <iostream>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CwinAppLauncherApp

BEGIN_MESSAGE_MAP(CwinAppLauncherApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CwinAppLauncherApp construction

CwinAppLauncherApp::CwinAppLauncherApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	pWinThread = 0;
	m_pMainWnd = 0;
}


// The one and only CwinAppLauncherApp object

CwinAppLauncherApp theApp;

bool connectToPort(int portNum)
{
	int retStatus;
	bool connected = false;

	SOCKET socketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketHandle != INVALID_SOCKET )
	{
		sockaddr_in address;

		address.sin_family = AF_INET;
		address.sin_port = htons(portNum);
		address.sin_addr.s_addr = inet_addr("127.0.0.1");
		if (connect(socketHandle, (struct sockaddr *) &address, sizeof(address)) != SOCKET_ERROR)
		{
			connected = true;
		}

		retStatus = closesocket(socketHandle);
	}

	return connected;
}


UINT UpdaterThread(LPVOID pParam)
{
	HWND hwnd = (HWND)pParam;
	int maxDelay = SCALEUI_MAX_DELAY;

	CProgressCtrl * pProgWebServ = (CProgressCtrl *)(CWnd::FromHandle(GetDlgItem(hwnd, IDC_PROGRESS_WEBSERVER)));

	CProgressCtrl * pProgScaleServ = (CProgressCtrl *)(CWnd::FromHandle(GetDlgItem(hwnd, IDC_PROGRESS_SCALESERVICE)));

	CProgressCtrl * pProgScaleUI = (CProgressCtrl *)(CWnd::FromHandle(GetDlgItem(hwnd, IDC_PROGRESS_SCALEUI)));


	while (!CwinAppLauncherApp::isFBWFActivated() || CwinAppLauncherApp::isFBWFEnabled())
	{
		if (connectToPort(80))
		{
			pProgWebServ->SetPos(PROGRESS_MAX_VALUE);
			break;
		}
		else
			pProgWebServ->StepIt();

		Sleep(1000);
	}


	while (!CwinAppLauncherApp::isFBWFActivated() || CwinAppLauncherApp::isFBWFEnabled())
	{
		if (connectToPort(6000))
		{
			pProgScaleServ->SetPos(PROGRESS_MAX_VALUE);
			break;
		}
		else
			pProgScaleServ->StepIt();

		Sleep(1000);
	}

	Sleep (1000);

	if (!CwinAppLauncherApp::isFBWFActivated() || CwinAppLauncherApp::isFBWFEnabled())
	{
		std::string path ("\"c:/program files/hobart/access scale/scripts/launchVB.vbs\"");

		HINSTANCE res = ShellExecuteA (NULL,"open",path.c_str(),"","",SW_SHOW);
	}


	if (CwinAppLauncherApp::isFBWFActivated() && !CwinAppLauncherApp::isFBWFEnabled())
		maxDelay = 5;

	for (int i = 0; i < maxDelay; i++)
	{
		pProgScaleUI->StepIt();
		Sleep(1000);
	}

	AfxGetMainWnd()->PostMessage(WM_QUIT,0,0);
	return 0;
}


void CwinAppLauncherApp::reenableAService (const std::string& serviceName)
{
	SC_HANDLE schSCManager = 0;
	SC_HANDLE schService = 0;
	SC_LOCK sclLock = 0; 

	schSCManager = OpenSCManager( 
								NULL,                    // local machine 
								NULL,                    // ServicesActive database 
								SC_MANAGER_ALL_ACCESS);  // full access rights

	if (!schSCManager)
		return;
	// Need to acquire database lock before reconfiguring. 

	do
	{
		sclLock = LockServiceDatabase(schSCManager);
		if (!sclLock)
			Sleep (1000);
	}
	while (!sclLock);


	if (sclLock)
	{
		schService = OpenServiceA( 
								schSCManager,           // SCManager database 
								serviceName.c_str(),           // name of service 
								SERVICE_ALL_ACCESS); // need CHANGE access 

		if (schService)
		{
		   if (! ChangeServiceConfig( 
								schService,        // handle of service 
								SERVICE_NO_CHANGE, // service type: no change 
								SERVICE_AUTO_START,       // change service start type 
								SERVICE_NO_CHANGE, // error control: no change 
								NULL,              // binary path: no change 
								NULL,              // load order group: no change 
								NULL,              // tag ID: no change 
								NULL,              // dependencies: no change 
								NULL,              // account name: no change 
								NULL,              // password: no change 
								NULL) )            // display name: no change
			{
				std::cout << "Failed to change Service to auto start!!!!" << std::endl;
			}

			CloseServiceHandle(schService);
		}

		UnlockServiceDatabase(sclLock); 
	}

	CloseServiceHandle(schSCManager);
}


bool CwinAppLauncherApp::isHKCUSetup()
{
	bool status = false;
	BYTE tmpBuff[16];
	BYTE * pData;
	DWORD size = 0;
	HKEY cntKey;


	if (RegOpenKeyEx(HKEY_CURRENT_USER, _T ("Software\\VB and VBA Program Settings\\Click-N-Type\\Settings"), 0, KEY_ALL_ACCESS, &cntKey) == ERROR_SUCCESS)
	{
		if (RegQueryValueEx(cntKey, _T("CfgFile"), 0, 0, (LPBYTE)&tmpBuff, &size) == ERROR_MORE_DATA)
		{
			pData = new BYTE[size];

			if (RegQueryValueEx(cntKey, _T("CfgFile"), 0, 0, (LPBYTE)pData, &size) == ERROR_SUCCESS)
			{
				if (_tcsstr((TCHAR *) pData, _T("AccessKeyboard.cfg")) != 0)
					status = true;
			}
			delete [] pData;
		}
	}

	return status;
}


// CwinAppLauncherApp initialization

bool CwinAppLauncherApp::fbwfEnabled = true;
bool CwinAppLauncherApp::fbwfActivated = true;


BOOL CwinAppLauncherApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);

	int doShowTaskbar = 1;			// default is true to show it
	HKEY hKey = NULL;
	DWORD taskbarRegVal;
	DWORD keySize = 4;
	ULONG result;
	ULONG current = 0, next = 0;

	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	bool usingRestrictedLogins = false;
	bool administratorAllowedUIAccess = true;
	{
		string internalConfigFileName(DEFAULT_INTERNAL_CONFIG_FILE_NAME);
		string customInternalConfigFileName(DEFAULT_CUSTOM_INTERNAL_CONFIG_FILE_NAME);
		util_env::Properties *properties = 0;
		properties = new util_env::Properties(customInternalConfigFileName, internalConfigFileName);
		if (!properties)
			return FALSE;

		const std::string *paramValue = 0;
		paramValue = properties->getProperty("startupDelay");
		if (paramValue)
		{
			int startupDelay = Integer::valueOf(*paramValue);   // this value is the number of seconds to delay when starting up (i.e. to give time for the debugger to attach)
			if (startupDelay > 0)
			{
				AfxMessageBox(IDS_STRING_DELAYING_BEFORE_STARTUP);
				//Sleep(startupDelay*1000);
			}
		}

		paramValue = 0;
		paramValue = properties->getProperty("FBWFActivated");
		if (paramValue)
			fbwfActivated = Bool::valueOf(*paramValue);

		paramValue = 0;
		paramValue = properties->getProperty("usingRestrictedLogins");
		if (paramValue)
			usingRestrictedLogins = Bool::valueOf(*paramValue);

		paramValue = 0;
		paramValue = properties->getProperty("AdministratorAllowedUIAccess");
		if (paramValue)
			administratorAllowedUIAccess = Bool::valueOf(*paramValue);
	}


	// If there is a restricted login account (as in Walmart), then don't allow the UI to come up for an Administrator login.
	// The restricted login accounts are intended to limit what the operator can do to the scale, and are implying that they
	// are used to operate the scale.  The Administrator account is used for administering Windows.
	// So by default, we don't want to allow the Administrator to run the UI.
	if (IsUserAnAdmin() && !administratorAllowedUIAccess)
	{
		HWND taskbar = FindWindow(_T("Shell_traywnd"), 0);
		ShowWindow(taskbar, SW_SHOW);

		AfxMessageBox(IDS_STRING_NOT_AS_ADMINISTRATOR);
		return FALSE;
	}
	else
	{
		// Standard initialization
		// If you are not using these features and wish to reduce the size
		// of your final executable, you should remove from the following
		// the specific initialization routines you do not need
		// Change the registry key under which our settings are stored
		// TODO: You should modify this string to be something appropriate
		// such as the name of your company or organization

		if (fbwfActivated)
		{
			result = NO_ERROR;

#ifndef NOFBWF
			result = FbwfIsFilterEnabled(&current, &next);
#endif

			if (result == NO_ERROR)
			{
				if (!current)
					fbwfEnabled = false;
			}
		}


		SetRegistryKey(_T("Local AppWizard-Generated Applications"));
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T ("SYSTEM\\CurrentControlSet\\Services\\Hobart Scale Service"), 0, KEY_QUERY_VALUE, &hKey))
		{
			// This IF is reached if we could not find the key in the registry. do not show taskbar if we cant find the key - ie, there is no access to the key
			doShowTaskbar = 0; 
		}
		else
		{
			// so we found the key, now lets see what the value is
			RegQueryValueExA(hKey, "displayTaskbar", NULL, NULL, (unsigned char*) &taskbarRegVal, &keySize);
		
			doShowTaskbar = taskbarRegVal;
	
			if (hKey)
	 			RegCloseKey(hKey);
		}
		
		if ((!fbwfActivated || fbwfEnabled) && (doShowTaskbar == 0))
		{
			HWND taskbar = FindWindow(_T("Shell_traywnd"), 0);
			ShowWindow(taskbar, SW_HIDE);
		}

		CwinAppLauncherDlg* pDlg = new CwinAppLauncherDlg();

		m_pMainWnd = pDlg;

		BOOL ret = pDlg->Create((UINT)CwinAppLauncherDlg::IDD);

		pDlg->SetWindowTextW(_T("winAppLauncher"));
		pDlg->ShowWindow(SW_SHOW);

		if (fbwfActivated)
		{
			if (!fbwfEnabled)
			{
				int ans = AfxMessageBox(IDS_STRING_UNPROTECTED_FIRST_PROMPT, MB_YESNO | MB_ICONQUESTION);
				{
					if (ans == IDNO)
					{
						//automatically reenable fbwf upon reboot.
#ifndef NOFBWF
						FbwfEnableFilter();
#endif
						reenableAService (std::string("Hobart Scale Service"));
						reenableAService (std::string("GoWebs"));

						AfxMessageBox(IDS_STRING_UNPROTECTED_NO_PROMPT, MB_OK | MB_ICONINFORMATION);
					}
				}
			}
		}

		pWinThread = AfxBeginThread(UpdaterThread, pDlg->m_hWnd, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
		pWinThread->m_bAutoDelete = FALSE;
		pWinThread->ResumeThread();

		
	/*	INT_PTR nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with OK
		}
		else if (nResponse == IDCANCEL)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with Cancel
		}
	*/
		// Since the dialog has been closed, return FALSE so that we exit the
		//  application, rather than start the application's message pump.
	}

	return TRUE;
}

int CwinAppLauncherApp::ExitInstance()
{
	DWORD exitCode = 0;

	do
	{
		if (pWinThread)
			::GetExitCodeThread(pWinThread->m_hThread,&exitCode);
		Sleep (1);
	}
	while (exitCode == STILL_ACTIVE);

	if (pWinThread) 
		delete pWinThread;
	if (m_pMainWnd)
		delete m_pMainWnd;

	return CWinApp::ExitInstance();
}