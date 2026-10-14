//#include "vxWorks.h"

#include "Event.h"
#include "VersionManagerEvent.h"
#include "ProcessDBFilesRequest.h"
//#include "HobThread.h"
#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int ProcessDBFilesRequest::dataType = 0;
std::string ProcessDBFilesRequest::dataTypeName("ProcessDBFilesRequest"); 
#endif

ProcessDBFilesRequest::	ProcessDBFilesRequest(const std::string& dbDir, Hobart::CEvolMsgReceiver *pHobThread) 
	: EvolMessage(VersionManagerDev, ProcessDBFilesReq, pHobThread), tmpDBDir_(dbDir)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

ProcessDBFilesRequest::ProcessDBFilesRequest(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

unsigned int j = 0;

	EvolMessage::serialize();

	j = getSerializedLength();

	// Now, for the derived class items

	tmpDBDir_.append(&buffer[j]);

}

ProcessDBFilesRequest::~ProcessDBFilesRequest()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


const char *ProcessDBFilesRequest::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items

	if (tmpDBDir_.size() > 0)
		serializedData.append(tmpDBDir_.c_str(), tmpDBDir_.size() + 1);

	return serializedData.data();
}

void ProcessDBFilesRequest::show()
{
	EvolMessage::show();

	//cout << "DBPaths: " << tmpDBDir_ << endl;

}
