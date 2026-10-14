#include <string>


#include "SynchronizationStatus.h"


#include "Debug.h"
#include "Integer.h"



#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int SynchronizationStatus::dataType = 0;
std::string SynchronizationStatus::dataTypeName("SynchronizationStatus"); 
#endif


SynchronizationStatus::SynchronizationStatus(synchronization::SynchronizationDevError stat, Hobart::CEvolMsgReceiver *pHobThread) :
		EvolMessage(SynchronizationDev, synchronization::SynchronizationStatusUpdateEvent, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	status = stat;
}


SynchronizationStatus::SynchronizationStatus(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();

//	std::string::size_type sz = 0;

	int j = getSerializedLength();

	// Now, for the derived class items
	status = (synchronization::SynchronizationDevError)Integer::valueOf(buffer+j);
}


SynchronizationStatus::~SynchronizationStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *SynchronizationStatus::serialize() const
{
	EvolMessage::serialize();

	serializedData.append(Integer::toString(status));

	return serializedData.data();
}
