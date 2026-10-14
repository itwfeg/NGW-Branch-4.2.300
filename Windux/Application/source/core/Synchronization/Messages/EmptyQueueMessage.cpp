
#if defined( __linux__ )
    #include <string.h>
#endif

#include "EmptyQueueMessage.h"


#include "SynchronizationDevEvent.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int EmptyQueueMessage::_dataType_ = 0;
	std::string EmptyQueueMessage::_dataTypeName_("synchronization::EmptyQueueMessage"); 
#endif



	EmptyQueueMessage::EmptyQueueMessage(QueueManager::QueueType queue)
		: EvolMessage(SynchronizationDev, ApplyBatchEvent), queue_(queue)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	EmptyQueueMessage::EmptyQueueMessage(const char* buffer)
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
		memcpy(&queue_, buffer, sizeof(queue_));
	}


	EmptyQueueMessage::~EmptyQueueMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


	const char* EmptyQueueMessage::serialize() const
	{
		EvolMessage::serialize();

		serializedData.append((char*)&queue_, sizeof(queue_));

		return serializedData.data();
	}


}