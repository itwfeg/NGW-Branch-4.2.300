
#if defined( __linux__ )
    #include <string.h>
#endif

#include "ApplyBatchMessage.h"


#include "SynchronizationDevEvent.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif

namespace synchronization
{

#ifdef USE_ACCOUNTANT
	int ApplyBatchMessage::_dataType_ = 0;
	std::string ApplyBatchMessage::_dataTypeName_("synchronization::ApplyBatchMessage"); 
#endif

	ApplyBatchMessage::ApplyBatchMessage(int id)
		: EvolMessage(SynchronizationDev, ApplyBatchEvent), batchId_(id)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	ApplyBatchMessage::ApplyBatchMessage(const char* buffer)
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


	ApplyBatchMessage::~ApplyBatchMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


	const char* ApplyBatchMessage::serialize() const
	{
		EvolMessage::serialize();

		serializedData.append((char*)&batchId_, sizeof(batchId_));

		return serializedData.data();
	}


}