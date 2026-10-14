#include "GCDFtpUpdateMsg.h"
#include "HobTokenizerCSV.h"
#include "GCDEvent.h"

GCDFtpUpdateMsg::GCDFtpUpdateMsg(const std::string& ftpAddr, const std::string& ftpUser, const std::string& ftpPwd,
								 const std::string& ftpDir, const std::string& playlist, const bool automatic, const std::string& downloadProtocol, Hobart::CEvolMsgReceiver *pHobThread) :
	EvolMessage(GCDSessionDev, GcdSessionUpdateViaFtpNow, pHobThread), 
	m_ftpAddr(ftpAddr), m_ftpUser(ftpUser), m_ftpPwd(ftpPwd), m_ftpDir(ftpDir), m_playlist(playlist), m_automatic(automatic), m_downloadProtocol(downloadProtocol)
{
}

GCDFtpUpdateMsg::GCDFtpUpdateMsg(EvolDeviceType deviceType, UINT8 eventType, const HobString &str, int formatType) :
	EvolMessage(deviceType, eventType),
	m_automatic(false),
	m_downloadProtocol("FTP")
{

	switch (formatType)
	{
		case BrowserFormat:
		{
			HobTokenizerCSV tok(&str, BrowserFormatSeparator);
			HobString token;
			for (int i = 0; (tok.next(&token) != -1); ++ i)
			{
				switch (i)
				{
				case 0:
					m_ftpAddr = token;
					break;
				case 1:
					m_ftpUser = token;
					break;
				case 2:
					m_ftpPwd = token;
					break;
				case 3:
					m_ftpDir = token;
					break;
				case 4:
					m_playlist = token;
					break;
				case 5:
					m_automatic = token.compare("1") == 0;
					break;
				case 6:
					m_downloadProtocol = token;
					break;
				}
			}
			break;
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

GCDFtpUpdateMsg::~GCDFtpUpdateMsg(void)
{
}

const char *GCDFtpUpdateMsg::serialize() const
{
	EvolMessage::serialize();

	serializedData.append(m_ftpAddr);
	serializedData.append(BrowserFormatSeparator);
	serializedData.append(m_ftpUser);
	serializedData.append(BrowserFormatSeparator);
	serializedData.append(m_ftpPwd);
	serializedData.append(BrowserFormatSeparator);
	serializedData.append(m_ftpDir);
	serializedData.append(BrowserFormatSeparator);
	serializedData.append(m_playlist);
	serializedData.append(BrowserFormatSeparator);
	serializedData.append(m_automatic ? "1" : "0");
	serializedData.append(BrowserFormatSeparator);
	serializedData.append(m_downloadProtocol);

	return serializedData.data();
}

