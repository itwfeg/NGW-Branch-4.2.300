#include "StartSyncManagerProcessedMessage.h"


#include "SynchronizationDevEvent.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif

namespace synchronization
{

#ifdef USE_ACCOUNTANT
	int StartSyncManagerProcessedMessage::_dataType_ = 0;
	std::string StartSyncManagerProcessedMessage::_dataTypeName_("synchronization::StartSyncManagerProcessedMessage"); 
#endif

	StartSyncManagerProcessedMessage::StartSyncManagerProcessedMessage(int status)
		: ProcessedMessage(SynchronizationDev, StartSyncManagerProcessedEvent, status)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	StartSyncManagerProcessedMessage::StartSyncManagerProcessedMessage(const char* buffer)
		: ProcessedMessage(buffer)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	StartSyncManagerProcessedMessage::~StartSyncManagerProcessedMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


}