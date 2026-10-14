
#if defined( __linux__ )
    #include <string.h>
#endif

#include "ProcessHQBatchProcessedMessage.h"


#include "SynchronizationDevEvent.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int ProcessHQBatchProcessedMessage::_dataType_ = 0;
	std::string ProcessHQBatchProcessedMessage::_dataTypeName_("synchronization::ProcessHQBatchProcessedMessage"); 
#endif


	ProcessHQBatchProcessedMessage::ProcessHQBatchProcessedMessage(int status, int batchNumber)
		: ProcessedMessage(SynchronizationDev, ProcessHQBatchProcessedEvent, status), batchNumber_(batchNumber)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	ProcessHQBatchProcessedMessage::ProcessHQBatchProcessedMessage(const char* buffer)
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
		memcpy(&batchNumber_, buffer, sizeof(batchNumber_));
	}


	ProcessHQBatchProcessedMessage::~ProcessHQBatchProcessedMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


	const char* ProcessHQBatchProcessedMessage::serialize() const
	{
		ProcessedMessage::serialize();

		serializedData.append((char*)&batchNumber_, sizeof(batchNumber_));

		return serializedData.data();
	}


}