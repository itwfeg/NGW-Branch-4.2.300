// winAppLauncher.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include <string>

// CwinAppLauncherApp:
// See winAppLauncher.cpp for the implementation of this class
//

class CwinAppLauncherApp : public CWinApp
{
public:
	CwinAppLauncherApp();
	static bool isHKCUSetup();
// Overrides
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	static bool isFBWFEnabled(){return fbwfEnabled;}
	static bool isFBWFActivated(){return fbwfActivated;}

// Implementation

	DECLARE_MESSAGE_MAP()

	private:

	void reenableAService (const std::string& serviceName);

	CWinThread * pWinThread;
	static bool	fbwfEnabled;
	static bool fbwfActivated;
	static bool usingRestrictedLogins;
};

extern CwinAppLauncherApp theApp;