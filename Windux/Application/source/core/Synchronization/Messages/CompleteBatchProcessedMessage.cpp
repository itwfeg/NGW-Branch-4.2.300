
#if defined( __linux__ )
    #include <string.h>
#endif

#include "CompleteBatchProcessedMessage.h"


#include "SynchronizationDevEvent.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int CompleteBatchProcessedMessage::_dataType_ = 0;
	std::string CompleteBatchProcessedMessage::_dataTypeName_("synchronization::CompleteBatchProcessedMessage"); 
#endif


	CompleteBatchProcessedMessage::CompleteBatchProcessedMessage(int status, int batchId)
		: ProcessedMessage(SynchronizationDev, CompleteBatchProcessedEvent, status), batchId_(batchId)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	CompleteBatchProcessedMessage::CompleteBatchProcessedMessage(const char* buffer)
		: ProcessedMessage(buffer)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif


		ProcessedMessage::serialize();
		buffer += getSerializedLength();

		// Now, for the derived class items
		memcpy(&batchId_, buffer, sizeof(batchId_));
	}


	CompleteBatchProcessedMessage::~CompleteBatchProcessedMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


	const char* CompleteBatchProcessedMessage::serialize() const
	{
		ProcessedMessage::serialize();

		serializedData.append((char*)&batchId_, sizeof(batchId_));

		return serializedData.data();
	}


}