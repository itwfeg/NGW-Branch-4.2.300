#include "RequestDeregistrationCommand.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int RequestDeregistrationCommand::_dataType_ = 0;
	std::string RequestDeregistrationCommand::_dataTypeName_("synchronization::RequestDeregistrationCommand"); 
#endif


	RequestDeregistrationCommand::RequestDeregistrationCommand(int targetScaleId, ProcessedMessage* message)
			: CommCommand(RequestDeregistrationCommandType, targetScaleId, message) 
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	RequestDeregistrationCommand::RequestDeregistrationCommand(const char * buffer)
		: CommCommand(buffer)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	RequestDeregistrationCommand::~RequestDeregistrationCommand()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


}