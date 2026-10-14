
#if defined( __linux__ )
    #include <string.h>
#endif

#include "ProcessSynchronizedBatchMessage.h"


#include "SynchronizationDevEvent.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{



#ifdef USE_ACCOUNTANT
	int ProcessSynchronizedBatchMessage::_dataType_ = 0;
	std::string ProcessSynchronizedBatchMessage::_dataTypeName_("synchronization::ProcessSynchronizedBatchMessage"); 
#endif


	ProcessSynchronizedBatchMessage::ProcessSynchronizedBatchMessage(int batchId)
		: EvolMessage(SynchronizationDev, ProcessSynchronizedBatchEvent), batchId_(batchId)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	ProcessSynchronizedBatchMessage::ProcessSynchronizedBatchMessage(const char* buffer)
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


	ProcessSynchronizedBatchMessage::~ProcessSynchronizedBatchMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


	const char* ProcessSynchronizedBatchMessage::serialize() const
	{
		EvolMessage::serialize();

		serializedData.append((char*)&batchId_, sizeof(batchId_));

		return serializedData.data();
	}


}