#include "StopSynchronizerMessage.h"


#include "SynchronizationDevEvent.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif

namespace synchronization
{

#ifdef USE_ACCOUNTANT
	int StopSynchronizerMessage::_dataType_ = 0;
	std::string StopSynchronizerMessage::_dataTypeName_("synchronization::StopSynchronizerMessage"); 
#endif

	StopSynchronizerMessage::StopSynchronizerMessage()
		: EvolMessage(SynchronizationDev, StopSynchronizerEvent)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	StopSynchronizerMessage::StopSynchronizerMessage(const char* buffer)
		: EvolMessage(buffer)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	StopSynchronizerMessage::~StopSynchronizerMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


}