//#include "vxWorks.h"

#include "ShutdownSystemResponse.h"
#include "VersionManagerEvent.h"
#include "HobVersion.h"
//#include "HobThread.h"

#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int ShutdownSystemResponse::dataType = 0;
std::string ShutdownSystemResponse::dataTypeName("ShutdownSystemResponse"); 
#endif

ShutdownSystemResponse::ShutdownSystemResponse(bool OKToShutdown, Hobart::CEvolMsgReceiver *pHobThread) 
	: EvolMessage(VersionManagerDev, SystemShutDownResponse, pHobThread), OKToShutdown_(OKToShutdown)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

ShutdownSystemResponse::ShutdownSystemResponse(const char *buffer) : EvolMessage(buffer)
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

	char *pChar = (char *) &OKToShutdown_;
	for (i = 0 ; i < sizeof(OKToShutdown_); i++)
		*pChar++ = buffer[j++];

}

ShutdownSystemResponse::~ShutdownSystemResponse()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


const char *ShutdownSystemResponse::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items

	char *pChar = (char *) & OKToShutdown_;
	for (unsigned int i = 0; i < sizeof(OKToShutdown_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void ShutdownSystemResponse::show()
{
	EvolMessage::show();

	//cout << "BackupDB: " << OKToShutdown_ <<endl;

}
