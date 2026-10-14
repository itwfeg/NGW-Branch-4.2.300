#ifndef UPDATESOFTWAREAUTOMATICALLY_H
#define UPDATESOFTWAREAUTOMATICALLY_H

#include "EvolMessage.h"
#include "CEvolMsgReceiver.h"
#include <string>

class UpdateSoftwareAutomatically : public EvolMessage
{
	public:
		UpdateSoftwareAutomatically(const std::string& ftpServerName, const std::string& ftpUserName, 
			const std::string& ftpPassword, const std::string& path, const std::string& upgradeServer, Hobart::CEvolMsgReceiver *pHobThread = NULL);
		UpdateSoftwareAutomatically(const char *buffer, int sizeOfMessage);		
		~UpdateSoftwareAutomatically();

		const std::string& getFtpServerName() {return ftpServerName_;}
		void setFtpServerName(const std::string& newServerName) {ftpServerName_ = newServerName;}

		const std::string& getFtpUserName() {return ftpUserName_;}
		void setFtpUserName(const std::string& newUserName) {ftpUserName_ = newUserName;}

		const std::string& getFtpPassword() {return ftpPassword_;}
		void setFtpPassword(const std::string& newPassword) {ftpPassword_ = newPassword;}

		const std::string& getPath() {return path_;}
		void setPath(const std::string& newPath) {path_ = newPath;}

		const std::string& getUpgradeServer() {return upgradeServer_;}
		void setUpgradeServer(const std::string& newUpgradeServer) {upgradeServer_ = newUpgradeServer;}
			
		const char *serialize() const;
		void show();

	private:

		std::string ftpServerName_;
		std::string ftpUserName_;
		std::string ftpPassword_;
		std::string path_;
		std::string upgradeServer_;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};



#endif	//UPDATESOFTWAREAUTOMATICALLY_H
