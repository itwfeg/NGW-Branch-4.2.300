#include "ProcessUserChangesProcessedMessage.h"


#include "SynchronizationDevEvent.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int ProcessUserChangesProcessedMessage::_dataType_ = 0;
	std::string ProcessUserChangesProcessedMessage::_dataTypeName_("synchronization::ProcessUserChangesProcessedMessage"); 
#endif


	ProcessUserChangesProcessedMessage::ProcessUserChangesProcessedMessage(int status)
		: ProcessedMessage(SynchronizationDev, ProcessUserChangesProcessedEvent, status)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	ProcessUserChangesProcessedMessage::ProcessUserChangesProcessedMessage(const char* buffer)
		: ProcessedMessage(buffer)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	ProcessUserChangesProcessedMessage::~ProcessUserChangesProcessedMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


}