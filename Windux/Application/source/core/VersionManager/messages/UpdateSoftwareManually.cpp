//#include "vxWorks.h"
#include "VersionManagerEvent.h"
#include "UpdateSoftwareManually.h"

#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int UpdateSoftwareManually::dataType = 0;
std::string UpdateSoftwareManually::dataTypeName("UpdateSoftwareManually"); 
#endif

UpdateSoftwareManually::UpdateSoftwareManually(std::string& path, Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(VersionManagerDev, UpgradeManual, pHobThread), path_(path)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

UpdateSoftwareManually::UpdateSoftwareManually(const char *buffer, int sizeOfMessage) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	EvolMessage::serialize();

	// Now, for the derived class items
	path_ = "";

	for (int j = getSerializedLength(); j < sizeOfMessage; j++)
	{
		path_ += buffer[j];
	}

}

UpdateSoftwareManually::~UpdateSoftwareManually()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *UpdateSoftwareManually::serialize() const
{

	EvolMessage::serialize();

	// Now, for the derived class items
	serializedData += path_;


	return serializedData.data();
}

void UpdateSoftwareManually::show()
{
	EvolMessage::show();

	//cout << "Path to Evolution.ini: " << path_ << endl;

}
