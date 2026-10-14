#include "DeleteBatchMessage.h"


#include "SynchronizationDevEvent.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif

namespace synchronization
{

#ifdef USE_ACCOUNTANT
	int DeleteBatchMessage::_dataType_ = 0;
	std::string DeleteBatchMessage::_dataTypeName_("synchronization::DeleteBatchMessage"); 
#endif

	DeleteBatchMessage::DeleteBatchMessage(int id, DeleteBatchProcessedMessage::DeleteBatchMethod deleteMethod)
		: EvolMessage(SynchronizationDev, DeleteBatchEvent), batchId_(id), deleteMethod_(deleteMethod)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	DeleteBatchMessage::DeleteBatchMessage(const char* buffer)
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
		buffer += sizeof(batchId_);

		memcpy(&deleteMethod_, buffer, sizeof(deleteMethod_));
	}


	DeleteBatchMessage::~DeleteBatchMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


	const char* DeleteBatchMessage::serialize() const
	{
		EvolMessage::serialize();

		serializedData.append((char*)&batchId_, sizeof(batchId_));
		serializedData.append((char*)&deleteMethod_, sizeof(deleteMethod_));

		return serializedData.data();
	}


}