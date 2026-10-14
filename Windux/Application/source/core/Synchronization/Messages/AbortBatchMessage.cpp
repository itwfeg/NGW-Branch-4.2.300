
#if defined( __linux__ )
    #include <string.h>
#endif

#include "AbortBatchMessage.h"


#include "SynchronizationDevEvent.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif

namespace synchronization
{
#ifdef USE_ACCOUNTANT
	int AbortBatchMessage::_dataType_ = 0;
	std::string AbortBatchMessage::_dataTypeName_("synchronization::AbortBatchMessage"); 
#endif

	AbortBatchMessage::AbortBatchMessage(int id)
		: EvolMessage(SynchronizationDev, AbortBatchEvent), batchId_(id)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	AbortBatchMessage::AbortBatchMessage(const char* buffer)
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
		memcpy(&batchId_, buffer, sizeof(batchId_));
	}


	AbortBatchMessage::~AbortBatchMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


	const char* AbortBatchMessage::serialize() const
	{
		EvolMessage::serialize();

		serializedData.append((char*)&batchId_, sizeof(batchId_));

		return serializedData.data();
	}


}