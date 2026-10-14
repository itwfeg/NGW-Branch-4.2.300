
#if defined( __linux__ )
    #include <string.h>
#endif

#include "AddRecordToBatchProcessedMessage.h"


#include "SynchronizationDevEvent.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int AddRecordToBatchProcessedMessage::_dataType_ = 0;
	std::string AddRecordToBatchProcessedMessage::_dataTypeName_("synchronization::AddRecordToBatchProcessedMessage");
#endif


	AddRecordToBatchProcessedMessage::AddRecordToBatchProcessedMessage(int status, int batchId)
		: ProcessedMessage(SynchronizationDev, AddRecordToBatchProcessedEvent, status), batchId_(batchId)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif
	}


	AddRecordToBatchProcessedMessage::AddRecordToBatchProcessedMessage(const char* buffer)
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


	AddRecordToBatchProcessedMessage::~AddRecordToBatchProcessedMessage()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif
	}


	const char* AddRecordToBatchProcessedMessage::serialize() const
	{
		ProcessedMessage::serialize();

		serializedData.append((char*)&batchId_, sizeof(batchId_));

		return serializedData.data();
	}


}