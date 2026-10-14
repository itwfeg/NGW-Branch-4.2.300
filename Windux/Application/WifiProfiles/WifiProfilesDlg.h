// WifiProfilesDlg.h : header file
//

#pragma once


// CWifiProfilesDlg dialog
class CWifiProfilesDlg : public CDialog
{
// Construction
public:
	CWifiProfilesDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_WIFIPROFILES_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	DWORD RestoreWirelessProfiles(CFileDialog * tmp);
	DWORD RestoreProfilesFromDirectory(CFileDialog * tmp, HMODULE hinstLib, HANDLE winAPIHandle);
	DWORD RestoreProfile(CString tmp, HMODULE hinstLib, HANDLE winAPIHandle, const GUID* pGUID);
	PBYTE decrypt(PBYTE src, DWORD size, PDWORD resultantSize);
	DWORD BackupWirelessProfiles(CString path);
	DWORD loadProfilesOnAllInterfaces(CString path, HMODULE hinstLib, HANDLE winAPIHandle);
	DWORD loadProfilesOnInterface(CString path, HMODULE hinstLib, HANDLE winAPIHandle, const GUID* pGUID);
	DWORD getProfile(CString path, HMODULE hinstLib, HANDLE winAPIHandle, const GUID* pGUID, PWCHAR profileName, int profileIndex);
	PBYTE encrypt(PBYTE src, DWORD size, PDWORD resultantSize, PWCHAR identifier);

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonRestore();
	afx_msg void OnBnClickedButtonBackup();
};
