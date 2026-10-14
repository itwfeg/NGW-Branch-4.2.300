#include "RequestSynchronizationCommand.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif

namespace synchronization
{
#ifdef USE_ACCOUNTANT
	int RequestSynchronizationCommand::_dataType_ = 0;
	std::string RequestSynchronizationCommand::_dataTypeName_("synchronization::RequestSynchronizationCommand"); 
#endif

	RequestSynchronizationCommand::RequestSynchronizationCommand(EvolMessage* message)
			: CommCommand(RequestSynchronizationCommandType, -1, message) 
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}

	RequestSynchronizationCommand::RequestSynchronizationCommand(const char * buffer)
		: CommCommand(buffer)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}

	RequestSynchronizationCommand::~RequestSynchronizationCommand() 
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}

}