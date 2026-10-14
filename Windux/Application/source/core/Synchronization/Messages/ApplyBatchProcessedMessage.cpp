
#if defined( __linux__ )
    #include <string.h>
#endif

#include "ApplyBatchProcessedMessage.h"


#include "SynchronizationDevEvent.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int ApplyBatchProcessedMessage::_dataType_ = 0;
	std::string ApplyBatchProcessedMessage::_dataTypeName_("synchronization::ApplyBatchProcessedMessage");
#endif


	ApplyBatchProcessedMessage::ApplyBatchProcessedMessage(int batchId)
		: ProcessedMessage(SynchronizationDev, ApplyBatchProcessedEvent), batchId_(batchId)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif
	}


	ApplyBatchProcessedMessage::ApplyBatchProcessedMessage(const char* buffer)
		: ProcessedMessage(buffer)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif

			
		ProcessedMessage::serialize();
		buffer += getSerializedLength();

		// Now, for the derived class items
		memcpy(&batchId_, buffer, sizeof(batchId_));
	}


	ApplyBatchProcessedMessage::~ApplyBatchProcessedMessage()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif
	}


	const char* ApplyBatchProcessedMessage::serialize() const
	{
		ProcessedMessage::serialize();

		serializedData.append((char*)&batchId_, sizeof(batchId_));

		return serializedData.data();
	}


}