// WifiProfilesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WifiProfiles.h"
#include "WifiProfilesDlg.h"
#include "afxdlgs.h"
#include "windows.h"
#include "wlanapi.h"
#include "afx.h"

#pragma warning(disable:4996)

using namespace std;

typedef DWORD (CALLBACK* FUNCOPENHANDLE) (DWORD, PVOID, PDWORD, PHANDLE);
typedef DWORD (CALLBACK* FUNCCLOSEHANDLE) (HANDLE, PVOID);
typedef DWORD (CALLBACK* FUNCENUMINTERFACES) (HANDLE, PVOID, PWLAN_INTERFACE_INFO_LIST*);
typedef DWORD (CALLBACK* FUNCGETPROFILELIST) (HANDLE, const GUID*, PVOID, PWLAN_PROFILE_INFO_LIST*);
typedef DWORD (CALLBACK* FUNCGETPROFILE) (HANDLE, const GUID*, LPCWSTR, PVOID, LPWSTR*, DWORD*, PDWORD);
typedef DWORD (CALLBACK* FUNCSETPROFILE) (HANDLE, const GUID*, DWORD, LPCWSTR, LPCWSTR, BOOL, PVOID, DWORD*);
typedef VOID (CALLBACK* FUNCFREEMEMORY) (PVOID);

#define SIZEID 30

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CWifiProfilesDlg dialog




CWifiProfilesDlg::CWifiProfilesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWifiProfilesDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWifiProfilesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWifiProfilesDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_RESTORE, &CWifiProfilesDlg::OnBnClickedButtonRestore)
	ON_BN_CLICKED(IDC_BUTTON_BACKUP, &CWifiProfilesDlg::OnBnClickedButtonBackup)
END_MESSAGE_MAP()


// CWifiProfilesDlg message handlers

BOOL CWifiProfilesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CWifiProfilesDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CWifiProfilesDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CWifiProfilesDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CWifiProfilesDlg::OnBnClickedButtonRestore()
{

CFileDialog fLoad(TRUE, L"*.xml", L"", OFN_FILEMUSTEXIST|OFN_NONETWORKBUTTON|OFN_ALLOWMULTISELECT, L"profile xml files (*.xml)|*.xml|all files (*.*)|*.*||");

	{
		CString currentText("");
		CEdit *pReport = (CEdit *)GetDlgItem(IDC_EDIT_RESTORED);

		pReport->SetWindowTextW(currentText);
	}

	if (fLoad.DoModal() == IDOK)
	{


		DWORD status = RestoreWirelessProfiles(&fLoad);

		AfxMessageBox(L"Restore complete!", MB_OK);
	}
}


DWORD CWifiProfilesDlg::RestoreWirelessProfiles(CFileDialog * tmp)
{
	
	DWORD status;
	DWORD negVer;
	HANDLE winAPIHandle;
	FUNCOPENHANDLE pFuncOpenHandle;
	FUNCCLOSEHANDLE pFuncCloseHanlde;


	HMODULE hinstLib = LoadLibrary(L"wlanapi.dll");
	if (hinstLib == NULL)
	{
		AfxMessageBox(L"Failed to load library wlanapi.dll!", MB_OK);

		return false;
	}

    pFuncOpenHandle = (FUNCOPENHANDLE) GetProcAddress(hinstLib, "WlanOpenHandle");
	pFuncCloseHanlde = (FUNCCLOSEHANDLE) GetProcAddress(hinstLib, "WlanCloseHandle");

	if (!pFuncOpenHandle || !pFuncCloseHanlde )
	{
		AfxMessageBox(L"Failed to get function handle in wlanapi.dll!", MB_OK);

		status = FreeLibrary(hinstLib);

		return false;
	}

	status = pFuncOpenHandle (1, NULL, &negVer,&winAPIHandle);
	if (status == ERROR_SUCCESS)
	{


		status = RestoreProfilesFromDirectory(tmp, hinstLib, winAPIHandle);


		pFuncCloseHanlde (winAPIHandle, NULL);
	}
	else
	{
		AfxMessageBox(L"Failed to open connection with wlanapi server!", MB_OK);
	}


	FreeLibrary(hinstLib);

	return status;

}

DWORD CWifiProfilesDlg::RestoreProfilesFromDirectory(CFileDialog * tmp, HMODULE hinstLib, HANDLE winAPIHandle)
{

	DWORD status = 0;
	PWLAN_INTERFACE_INFO_LIST pIntfList = 0;
	FUNCENUMINTERFACES funcEnumInterfaces = (FUNCENUMINTERFACES) GetProcAddress(hinstLib, "WlanEnumInterfaces");
	FUNCFREEMEMORY funcFreeMem = (FUNCFREEMEMORY) GetProcAddress(hinstLib, "WlanFreeMemory");

	if (!funcEnumInterfaces || !funcFreeMem)
	{
		AfxMessageBox(L"Failed to load function from wlanapi.dll!", MB_OK);

		return ERROR_INVALID_FUNCTION;
	}


    // enumerate wireless interfaces
    if ((status = funcEnumInterfaces(
                        winAPIHandle,
                        NULL,               // reserved
                        &pIntfList
                        )) != ERROR_SUCCESS)
    {
		AfxMessageBox(L"WlanEnumInterfaces failed!", MB_OK);

		if (pIntfList)
			funcFreeMem(pIntfList);

		return ERROR_ACCESS_DENIED;
    }

	POSITION fileNamesPosition = tmp->GetStartPosition();

	while(fileNamesPosition != NULL)
	{
		status = RestoreProfile(tmp->GetNextPathName(fileNamesPosition), hinstLib, winAPIHandle, &pIntfList->InterfaceInfo[0].InterfaceGuid);
	}

	if (pIntfList)
		funcFreeMem(pIntfList);

	return status;
}

DWORD CWifiProfilesDlg::RestoreProfile(CString tmp, HMODULE hinstLib, HANDLE winAPIHandle, const GUID* pGUID)
{
	DWORD status = ERROR_ACCESS_DENIED;
	CFile sourceFile;
    CFileException ex;


    if (sourceFile.Open(tmp,
        CFile::modeRead , &ex))
    {

		FUNCSETPROFILE funcSetProfile = (FUNCSETPROFILE) GetProcAddress(hinstLib, "WlanSetProfile");
		FUNCFREEMEMORY funcFreeMem = (FUNCFREEMEMORY) GetProcAddress(hinstLib, "WlanFreeMemory");

		if (funcSetProfile && funcFreeMem )
		{

			UINT buffSize = (UINT)sourceFile.GetLength();
			char *xmlStr = new char[buffSize];

			if (xmlStr)
			{

				sourceFile.SeekToBegin();
				UINT bytesRead = sourceFile.Read (xmlStr, buffSize);


				if (buffSize == bytesRead)
				{
					DWORD resultantSize = 0;
					PBYTE pDest = 0;

					pDest = decrypt((PBYTE) xmlStr, buffSize, &resultantSize);

					if ((resultantSize >0) && pDest)
					{

						LPCWSTR pProfileXml = (LPCWSTR)pDest;
						DWORD reasonCode = 0;


						if ((status = funcSetProfile(
											winAPIHandle, 
											pGUID,
											0,
											pProfileXml,    
											NULL,       
											TRUE,    
											NULL,    
											&reasonCode     
											)) != ERROR_SUCCESS)
						{
							AfxMessageBox(L"WlanSetProfile failed!", MB_OK);
	
						}

						{
							CString currentText("");
							CEdit *pReport = (CEdit *)GetDlgItem(IDC_EDIT_RESTORED);

							pReport->GetWindowTextW(currentText);
							currentText+=tmp;
							currentText+="\r\n";
							pReport->SetWindowTextW(currentText);
						}
					}

					if (pDest)
						free (pDest);

				}
				else
					AfxMessageBox(L"Failed to Read all bytes from file!", MB_OK);

				delete[] xmlStr;
			}
			else
			{
				status = ERROR_OUTOFMEMORY;

				AfxMessageBox(L"Failed to allocate memory for xml file contents!", MB_OK);

			}
		}
		else
		{
			AfxMessageBox(L"Failed to get function handle in wlanapi.dll!", MB_OK);

			status = ERROR_INVALID_FUNCTION;
		}
		
		sourceFile.Close();
	}
	else
		AfxMessageBox(L"Failed to open file!", MB_OK);

	return status;
}

PBYTE CWifiProfilesDlg::decrypt(PBYTE src, DWORD size, PDWORD resultantSize)
{
	PBYTE pDest = 0;
	*resultantSize = (size - (SIZEID * sizeof(WCHAR)) ) / 2;
	
	pDest = (PBYTE)malloc(*resultantSize);

	if (pDest)
	{
		for (int s = (int)(SIZEID*sizeof(WCHAR)), d = 0; s < (int)size; s+=2, d++)
		{
			BYTE high = (src[s] - 0x30) << 4;
			BYTE low = (src[s+1] - 0x30);
			pDest[d] = high | low;

		}
	}
	else
		*resultantSize = 0;

	return pDest;
}

DWORD CWifiProfilesDlg::BackupWirelessProfiles(CString path)
{
	DWORD status = ERROR_ACCESS_DENIED;
	DWORD negVer;
	HANDLE winAPIHandle;
	FUNCOPENHANDLE pFuncOpenHandle;
	FUNCCLOSEHANDLE pFuncCloseHanlde;


	HMODULE hinstLib = LoadLibrary(L"wlanapi.dll");
	if (hinstLib == NULL)
	{
		AfxMessageBox(L"Failed to load library wlanapi.dll!", MB_OK);

		return status;
	}

    pFuncOpenHandle = (FUNCOPENHANDLE) GetProcAddress(hinstLib, "WlanOpenHandle");
	pFuncCloseHanlde = (FUNCCLOSEHANDLE) GetProcAddress(hinstLib, "WlanCloseHandle");

	if (!pFuncOpenHandle || !pFuncCloseHanlde )
	{
		AfxMessageBox(L"Failed to get function handle in wlanapi.dll!", MB_OK);

		status = FreeLibrary(hinstLib);

		return status;
	}

	status = pFuncOpenHandle (1, NULL, &negVer,&winAPIHandle);
	if (status == ERROR_SUCCESS)
	{

		status = loadProfilesOnAllInterfaces(path, hinstLib, winAPIHandle);

		pFuncCloseHanlde (winAPIHandle, NULL);
	}
	else
	{
		AfxMessageBox(L"Failed to open connection with wlanapi server!", MB_OK);
	}

	FreeLibrary(hinstLib);

	return status;

}

DWORD CWifiProfilesDlg::loadProfilesOnAllInterfaces(CString path, HMODULE hinstLib, HANDLE winAPIHandle)
{
	DWORD status = 0;
	PWLAN_INTERFACE_INFO_LIST pIntfList = 0;
	FUNCENUMINTERFACES funcEnumInterfaces = (FUNCENUMINTERFACES) GetProcAddress(hinstLib, "WlanEnumInterfaces");
	FUNCFREEMEMORY funcFreeMem = (FUNCFREEMEMORY) GetProcAddress(hinstLib, "WlanFreeMemory");


	if (!funcEnumInterfaces || !funcFreeMem)
	{
		AfxMessageBox(L"Failed to load function from wlanapi.dll!", MB_OK);

		return ERROR_INVALID_FUNCTION;
	}



    // enumerate wireless interfaces
    if ((status = funcEnumInterfaces(
                        winAPIHandle,
                        NULL,               // reserved
                        &pIntfList
                        )) != ERROR_SUCCESS)
    {
		AfxMessageBox(L"WlanEnumInterfaces failed!", MB_OK);

		return status;
    }


    for (int i = 0; i < (int)pIntfList->dwNumberOfItems; i++)
    {
		status = loadProfilesOnInterface(path, hinstLib, winAPIHandle, &pIntfList->InterfaceInfo[i].InterfaceGuid);
	}

	if (pIntfList)
		funcFreeMem(pIntfList);

	return status;
}

DWORD CWifiProfilesDlg::loadProfilesOnInterface(CString path, HMODULE hinstLib, HANDLE winAPIHandle, const GUID* pGUID)
{
	DWORD status = 0;
	PWLAN_PROFILE_INFO_LIST pProfileList = 0;
	FUNCGETPROFILELIST funcGetProfileList = (FUNCGETPROFILELIST) GetProcAddress(hinstLib, "WlanGetProfileList");
	FUNCFREEMEMORY funcFreeMem = (FUNCFREEMEMORY) GetProcAddress(hinstLib, "WlanFreeMemory");

	if (!funcGetProfileList || !funcFreeMem)
	{
		AfxMessageBox(L"Failed to load function from wlanapi.dll!", MB_OK);

		return ERROR_INVALID_FUNCTION;
	}

	if ((status = funcGetProfileList(
						winAPIHandle,
						pGUID,               // reserved
						NULL,
						&pProfileList
						)) != ERROR_SUCCESS)
	{
		AfxMessageBox(L"WlanGetProfileList failed!", MB_OK);


		return status;
	}


	for (int i = 0; i < (int)pProfileList->dwNumberOfItems; i++)
	{

		if ((status = getProfile(path, hinstLib, winAPIHandle, pGUID, 
			pProfileList->ProfileInfo[i].strProfileName, i)) != ERROR_SUCCESS)
			break;

	}

	if (pProfileList)
		funcFreeMem(pProfileList);

	return status;
}

DWORD CWifiProfilesDlg::getProfile(CString path, HMODULE hinstLib, HANDLE winAPIHandle, const GUID* pGUID, PWCHAR profileName, int profileIndex)
{
	DWORD status = 0;
	PWSTR strXml = 0;
	FUNCGETPROFILE funcGetProfile = (FUNCGETPROFILE) GetProcAddress(hinstLib, "WlanGetProfile");
	FUNCFREEMEMORY funcFreeMem = (FUNCFREEMEMORY) GetProcAddress(hinstLib, "WlanFreeMemory");

	if (!funcGetProfile || !funcFreeMem)
	{
		AfxMessageBox(L"Failed to load function from wlanapi.dll!", MB_OK);

		return ERROR_INVALID_FUNCTION;
	}

	if ((status = funcGetProfile(
						winAPIHandle, 
						pGUID, 
						profileName,    // profile name
						NULL,       // reserved
						&strXml,    // XML string of the profile
						NULL,       // not interested in the profile flags
						NULL        // don't care about ACL
						)) != ERROR_SUCCESS)
	{
		AfxMessageBox(L"WlanGetProfile failed!", MB_OK);

		if (strXml)
			funcFreeMem(strXml);

		return status;
	}
	else
	{
		WCHAR idString[128];
		DWORD resultantSize = 0;
		PBYTE pDest = 0;

		swprintf(idString, 128, L"Wireless Profile %s.", profileName);

		//write the null terminating character to file. hence (wcslen(strXml) + 1)
		pDest = encrypt((PBYTE) strXml, (DWORD)((wcslen(strXml) + 1) * sizeof(WCHAR)), &resultantSize, idString);
		if ((resultantSize > 0) && pDest)
		{
			CString tmp("");
			CFile sourceFile;
			CFileException ex;

			if (path[path.GetLength()-1] == '\\')
				tmp.Format(L"%sprofile_%02d.xml", path, profileIndex + 1);
			else
				tmp.Format(L"%s\\profile_%02d.xml", path, profileIndex + 1);


			if (sourceFile.Open(tmp,
				CFile::modeWrite | CFile::modeCreate | CFile::shareExclusive , &ex))
			{


				sourceFile.Write(pDest, resultantSize);

				sourceFile.Flush();

				sourceFile.Close();

				{
					CString currentText("");
					CEdit *pReport = (CEdit *)GetDlgItem(IDC_EDIT_BACKED_UP);

					pReport->GetWindowTextW(currentText);
					currentText+=tmp;
					currentText+="\r\n";
					pReport->SetWindowTextW(currentText);
				}

			}
			else
			{
				AfxMessageBox(L"Failed to save file!", MB_OK);
			}


		}

		if (pDest)
			free(pDest);
		if (strXml)
			funcFreeMem(strXml);
	}

	return ERROR_SUCCESS;
}

PBYTE CWifiProfilesDlg::encrypt(PBYTE src, DWORD size, PDWORD resultantSize, PWCHAR identifier)
{
	PBYTE pDest = 0;
	*resultantSize = SIZEID*sizeof(WCHAR) + (size * 2);
	
	pDest = (PBYTE)malloc(*resultantSize);

	if (pDest)
	{

		wcsncpy((PWCHAR)pDest, identifier, SIZEID);
		
		for (int s = 0, d = (int)(SIZEID*sizeof(WCHAR)); s < (int)size; s++, d+=2)
		{
			BYTE high = ((src[s] & 0xF0)>>4) + 0x30;
			BYTE low = (src[s] & 0x0F) + 0x30;
			pDest[d] = high;
			pDest[d+1] = low;

		}
	}
	else
		*resultantSize = 0;

	return pDest;
}

void CWifiProfilesDlg::OnBnClickedButtonBackup()
{
	CEdit *pBackupPath = (CEdit *)GetDlgItem(IDC_EDIT_BACKUP_PROFILES);

	{
		CString currentText("");
		CEdit *pReport = (CEdit *)GetDlgItem(IDC_EDIT_BACKED_UP);

		pReport->SetWindowTextW(currentText);
	}

	int len = pBackupPath->GetWindowTextLengthW();

	if (len)
	{
		CString backupPath("");

		pBackupPath->GetWindowTextW(backupPath);

		DWORD status = BackupWirelessProfiles(backupPath);

		AfxMessageBox(L"Backup complete!", MB_OK);

	}

}
