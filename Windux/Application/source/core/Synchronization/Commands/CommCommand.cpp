#include "CommCommand.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int CommCommand::_dataType_ = 0;
	std::string CommCommand::_dataTypeName_("synchronization::CommCommand"); 
#endif


	CommCommand::CommCommand(CommandType type, int targetScaleId, ProcessedMessage* message)
			: Command(type, targetScaleId, message) 
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	CommCommand::CommCommand(const char * buffer)
		: Command(buffer)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	CommCommand::~CommCommand()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


}