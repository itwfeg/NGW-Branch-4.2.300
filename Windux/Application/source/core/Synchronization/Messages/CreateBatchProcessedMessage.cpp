
#if defined( __linux__ )
    #include <string.h>
#endif

#include "CreateBatchProcessedMessage.h"


#include "SynchronizationDevEvent.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int CreateBatchProcessedMessage::_dataType_ = 0;
	std::string CreateBatchProcessedMessage::_dataTypeName_("synchronization::CreateBatchProcessedMessage"); 
#endif


	CreateBatchProcessedMessage::CreateBatchProcessedMessage(int status, int batchId)
		: ProcessedMessage(SynchronizationDev, CreateBatchProcessedEvent, status), batchId_(batchId), batchVersion_(0)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	CreateBatchProcessedMessage::CreateBatchProcessedMessage(const char* buffer)
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
		buffer += sizeof(batchId_);

		memcpy(&batchVersion_, buffer, sizeof(batchVersion_));
	}


	CreateBatchProcessedMessage::~CreateBatchProcessedMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


	const char* CreateBatchProcessedMessage::serialize() const
	{
		ProcessedMessage::serialize();

		serializedData.append((char*)&batchId_, sizeof(batchId_));
		serializedData.append((char*)&batchVersion_, sizeof(batchVersion_));

		return serializedData.data();
	}


}