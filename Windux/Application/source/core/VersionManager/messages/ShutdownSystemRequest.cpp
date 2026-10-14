//#include "vxWorks.h"

#include "ShutdownSystemRequest.h"
#include "VersionManagerEvent.h"
#include "HobVersion.h"
//#include "HobThread.h"

#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int ShutdownSystemRequest::dataType = 0;
std::string ShutdownSystemRequest::dataTypeName("ShutdownSystemRequest"); 
#endif


ShutdownSystemRequest::	ShutdownSystemRequest(bool backupDB, Hobart::CEvolMsgReceiver *pHobThread) 
	: EvolMessage(VersionManagerDev, SystemShutDownRequest, pHobThread), backUpDB_(backupDB)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

ShutdownSystemRequest::ShutdownSystemRequest(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
unsigned int j = 0, i = 0;

	EvolMessage::serialize();

	j = getSerializedLength();

	// Now, for the derived class items

	char *pChar = (char *) &backUpDB_;
	for (i = 0 ; i < sizeof(backUpDB_); i++)
		*pChar++ = buffer[j++];

}

ShutdownSystemRequest::~ShutdownSystemRequest()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


const char *ShutdownSystemRequest::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items

	char *pChar = (char *) & backUpDB_;
	for (unsigned int i = 0; i < sizeof(backUpDB_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void ShutdownSystemRequest::show()
{
	EvolMessage::show();

	//cout << "BackupDB: " << backUpDB_ <<endl;

}
