
#if defined( __linux__ )
    #include <string.h>
#endif

#include "ProcessUserFileProcessedMessage.h"


#include "SynchronizationDevEvent.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int ProcessUserFileProcessedMessage::_dataType_ = 0;
	std::string ProcessUserFileProcessedMessage::_dataTypeName_("synchronization::ProcessUserFileProcessedMessage"); 
#endif


	ProcessUserFileProcessedMessage::ProcessUserFileProcessedMessage(int status, int batchId)
		: ProcessedMessage(SynchronizationDev, ProcessUserFileProcessedEvent, status), batchId_(batchId)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	ProcessUserFileProcessedMessage::ProcessUserFileProcessedMessage(const char* buffer)
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


	ProcessUserFileProcessedMessage::~ProcessUserFileProcessedMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


	const char* ProcessUserFileProcessedMessage::serialize() const
	{
		ProcessedMessage::serialize();

		serializedData.append((char*)&batchId_, sizeof(batchId_));

		return serializedData.data();
	}


}