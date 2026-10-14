//#include "vxWorks.h"
#include "VersionManagerEvent.h"
#include "WriteFileToSCEEPMsg.h"

#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int WriteFileToSCEEPMsg::dataType = 0;
std::string WriteFileToSCEEPMsg::dataTypeName("WriteFileToSCEEPMsg"); 
#endif

WriteFileToSCEEPMsg::WriteFileToSCEEPMsg(std::string& path, Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(VersionManagerDev, WriteFileToSCEEP, pHobThread), path_(path)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WriteFileToSCEEPMsg::WriteFileToSCEEPMsg(const char *buffer, int sizeOfMessage) : EvolMessage(buffer)
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

WriteFileToSCEEPMsg::~WriteFileToSCEEPMsg()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *WriteFileToSCEEPMsg::serialize() const
{

	EvolMessage::serialize();

	// Now, for the derived class items
	serializedData += path_;


	return serializedData.data();
}

void WriteFileToSCEEPMsg::show()
{
	EvolMessage::show();

	//cout << "Path to Bin File: " << path_ << endl;

}
