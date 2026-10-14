#include "RequestRegistrationCommand.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT

	int RequestRegistrationCommand::_dataType_ = 0;
	std::string RequestRegistrationCommand::_dataTypeName_("synchronization::RequestRegistrationCommand"); 
#endif


	RequestRegistrationCommand::RequestRegistrationCommand(int targetScaleId, ProcessedMessage* message)
			: CommCommand(RequestRegistrationCommandType, targetScaleId, message) 
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	RequestRegistrationCommand::RequestRegistrationCommand(const char * buffer)
		: CommCommand(buffer)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	RequestRegistrationCommand::~RequestRegistrationCommand() 
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


}