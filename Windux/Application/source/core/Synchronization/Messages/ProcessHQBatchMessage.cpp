
#if defined( __linux__ )
    #include <string.h>
#endif

#include "ProcessHQBatchMessage.h"


#include "SynchronizationDevEvent.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif

namespace synchronization
{

#ifdef USE_ACCOUNTANT
	int ProcessHQBatchMessage::_dataType_ = 0;
	std::string ProcessHQBatchMessage::_dataTypeName_("synchronization::ProcessHQBatchMessage"); 
#endif

	ProcessHQBatchMessage::ProcessHQBatchMessage(int number)
		: EvolMessage(SynchronizationDev, ProcessHQBatchEvent), batchNumber_(number)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	ProcessHQBatchMessage::ProcessHQBatchMessage(const char* buffer)
		: EvolMessage(buffer)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
		EvolMessage::serialize();
		buffer += getSerializedLength();

		// Now, for the derived class items
		memcpy(&batchNumber_, buffer, sizeof(batchNumber_));
	}


	ProcessHQBatchMessage::~ProcessHQBatchMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


	const char* ProcessHQBatchMessage::serialize() const
	{
		EvolMessage::serialize();

		serializedData.append((char*)&batchNumber_, sizeof(batchNumber_));

		return serializedData.data();
	}


}