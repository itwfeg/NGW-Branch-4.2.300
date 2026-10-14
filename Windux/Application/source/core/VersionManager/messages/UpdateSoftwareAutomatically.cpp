//#include "vxWorks.h"
#include "VersionManagerEvent.h"
#include "UpdateSoftwareAutomatically.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int UpdateSoftwareAutomatically::dataType = 0;
std::string UpdateSoftwareAutomatically::dataTypeName("UpdateSoftwareAutomatically"); 
#endif

UpdateSoftwareAutomatically::UpdateSoftwareAutomatically(const std::string& ftpServerName, const std::string& ftpUserName, 
			const std::string& ftpPassword, const std::string& path, const std::string& upgradeServer,
			Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(VersionManagerDev, UpgradeAutomatic, pHobThread), 
			ftpServerName_(ftpServerName), ftpUserName_(ftpUserName), ftpPassword_(ftpPassword), path_(path), upgradeServer_(upgradeServer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

UpdateSoftwareAutomatically::UpdateSoftwareAutomatically(const char *buffer, int sizeOfMessage) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	int j = 0;
	EvolMessage::serialize();

	// Now, for the derived class items

	ftpServerName_ = "";

	for (j = getSerializedLength(); (j < sizeOfMessage) && (buffer[j] != ','); j++)
	{
		ftpServerName_ += buffer[j];
	}

	ftpUserName_ = "";

	for (j++; (j < sizeOfMessage) && (buffer[j] != ','); j++)
	{
		ftpUserName_ += buffer[j];
	}

	ftpPassword_ = "";

	for (j++; (j < sizeOfMessage) && (buffer[j] != ','); j++)
	{
		ftpPassword_ += buffer[j];
	}

	path_ = "";

	for (j++ ; (j < sizeOfMessage) && (buffer[j] != ','); j++)
	{
		path_ += buffer[j];
	}

	upgradeServer_ = "";

	for (j++; (j < sizeOfMessage); j++)
	{
		upgradeServer_ += buffer[j];
	}
}

UpdateSoftwareAutomatically::~UpdateSoftwareAutomatically()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *UpdateSoftwareAutomatically::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items

	serializedData += ftpServerName_;
	serializedData += ",";
	serializedData += ftpUserName_;
	serializedData += ",";
	serializedData += ftpPassword_;
	serializedData += ",";
	serializedData += path_;
	serializedData += ",";
	serializedData += upgradeServer_;

	return serializedData.data();
}

void UpdateSoftwareAutomatically::show()
{
	EvolMessage::show();

}
