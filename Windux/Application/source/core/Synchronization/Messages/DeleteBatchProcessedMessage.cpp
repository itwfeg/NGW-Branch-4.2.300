
#if defined( __linux__ )
    #include <string.h>
#endif

#include "DeleteBatchProcessedMessage.h"


#include "SynchronizationDevEvent.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int DeleteBatchProcessedMessage::_dataType_ = 0;
	std::string DeleteBatchProcessedMessage::_dataTypeName_("synchronization::DeleteBatchProcessedMessage");
#endif


	std::string DeleteBatchProcessedMessage::DeleteChangesOnlyName("DeleteChangesOnly");
	std::string DeleteBatchProcessedMessage::DeleteChangesAndBatchName("DeleteChangesAndBatch");
	std::string DeleteBatchProcessedMessage::DeleteInvalidName("DeleteInvalid");


	DeleteBatchProcessedMessage::DeleteBatchProcessedMessage(int batchId, DeleteBatchMethod deleteMethod)
		: ProcessedMessage(SynchronizationDev, DeleteBatchProcessedEvent), batchId_(batchId), deleteMethod_(deleteMethod)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif
	}


	DeleteBatchProcessedMessage::DeleteBatchProcessedMessage(const char* buffer)
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
		buffer += sizeof(batchId_);

		memcpy(&deleteMethod_, buffer, sizeof(deleteMethod_));
	}


	DeleteBatchProcessedMessage::~DeleteBatchProcessedMessage()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif
	}


	const char* DeleteBatchProcessedMessage::serialize() const
	{
		ProcessedMessage::serialize();

		serializedData.append((char*)&batchId_, sizeof(batchId_));
		serializedData.append((char*)&deleteMethod_, sizeof(deleteMethod_));

		return serializedData.data();
	}


	const std::string& DeleteBatchProcessedMessage::getDeleteMethodName(DeleteBatchMethod method)
	{
		switch ( method )
		{
			case DeleteChangesOnly:
				return DeleteChangesOnlyName;
			case DeleteChangesAndBatch:
				return DeleteChangesAndBatchName;
			default:
				return DeleteInvalidName;
		}
	}


}