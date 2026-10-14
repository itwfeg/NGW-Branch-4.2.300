
#if     defined( __linux__ )
    //#include <stdint.h>
    //#include "PAL_typedef.h"
    #include "PAL_platformheader.h"
    #include <string.h>
#endif

#include "AbortBatchProcessedMessage.h"


#include "SynchronizationDevEvent.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int AbortBatchProcessedMessage::_dataType_ = 0;
	std::string AbortBatchProcessedMessage::_dataTypeName_("synchronization::AbortBatchProcessedMessage"); 
#endif


	AbortBatchProcessedMessage::AbortBatchProcessedMessage(int status, int batchId)
		: ProcessedMessage(SynchronizationDev, AbortBatchProcessedEvent, status), batchId_(batchId)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	AbortBatchProcessedMessage::AbortBatchProcessedMessage(const char* buffer)
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


	AbortBatchProcessedMessage::~AbortBatchProcessedMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


	const char* AbortBatchProcessedMessage::serialize() const
	{
		ProcessedMessage::serialize();

		serializedData.append((char*)&batchId_, sizeof(batchId_));

		return serializedData.data();
	}


}