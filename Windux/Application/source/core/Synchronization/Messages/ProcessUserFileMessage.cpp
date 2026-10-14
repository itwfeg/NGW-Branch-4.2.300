
#if defined( __linux__ )
    #include <string.h>
#endif

#include "ProcessUserFileMessage.h"


#include "SynchronizationDevEvent.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif

namespace synchronization
{

#ifdef USE_ACCOUNTANT
	int ProcessUserFileMessage::_dataType_ = 0;
	std::string ProcessUserFileMessage::_dataTypeName_("synchronization::ProcessUserFileMessage"); 
#endif

	ProcessUserFileMessage::ProcessUserFileMessage(int id)
		: EvolMessage(SynchronizationDev, ProcessUserFileEvent), batchId_(id)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	ProcessUserFileMessage::ProcessUserFileMessage(const char* buffer)
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


	ProcessUserFileMessage::~ProcessUserFileMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


	const char* ProcessUserFileMessage::serialize() const
	{
		EvolMessage::serialize();

		serializedData.append((char*)&batchId_, sizeof(batchId_));

		return serializedData.data();
	}


}