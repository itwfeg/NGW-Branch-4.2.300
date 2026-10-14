#pragma once
#include "EvolMessage.h"
#include "HobString.h"
#include "MessageFormat.h"
#include <string>
#include <vector>

class GCDFtpUpdateMsg :	public EvolMessage
{
public:
	GCDFtpUpdateMsg(const std::string& ftpAddr, const std::string& ftpUser, const std::string& ftpPwd,
		const std::string& ftpDir, const std::string& playlist, const bool automatic, const std::string& downloadProtocol, Hobart::CEvolMsgReceiver *pHobThread = NULL);
	GCDFtpUpdateMsg(EvolDeviceType deviceType, UINT8 eventType,	const HobString &str, int formatType = BrowserFormat);
	~GCDFtpUpdateMsg(void);

	const char *serialize() const;

	void setFtpAddress(std::string val) {m_ftpAddr = val;}
	const std::string& getFtpAddress() const { return m_ftpAddr;}

	void setFtpUser(std::string val) {m_ftpUser = val;}
	const std::string& getFtpUser() const {return m_ftpUser;}

	void setFtpPassword(std::string val) {m_ftpPwd = val;}
	const std::string& getFtpPassword() const {return m_ftpPwd;}

	void setFtpDirectory(std::string val) {m_ftpDir = val;}
	const std::string& getFtpDirectory() const {return m_ftpDir;}

	void setPlayList(std::string val) {m_playlist = val;}
	const std::string& getPlayList() const {return m_playlist;}

	void setAutomatic(const bool val) {m_automatic = val;}
	bool getAutomatic() const {return m_automatic;}

	void setDownloadProtocol(std::string val) {m_downloadProtocol = val;}
	const std::string& getDownloadProtocol() const {return m_downloadProtocol;}



private:
	std::string m_ftpAddr;
	std::string m_ftpUser;
	std::string m_ftpPwd;
	std::string m_ftpDir;
	std::string m_playlist;
	bool m_automatic;
	std::string m_downloadProtocol;
};
