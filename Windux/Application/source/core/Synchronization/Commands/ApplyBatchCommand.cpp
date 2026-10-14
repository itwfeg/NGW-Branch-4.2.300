#include "ApplyBatchCommand.h"



#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int ApplyBatchCommand::_dataType_ = 0;
	std::string ApplyBatchCommand::_dataTypeName_("synchronization::ApplyBatchCommand"); 
#endif


	ApplyBatchCommand::ApplyBatchCommand(int batchId, ProcessedMessage* message)
			: Command(ApplyBatchCommandType, batchId, message) 
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	ApplyBatchCommand::ApplyBatchCommand(const char * buffer)
		: Command(buffer)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	ApplyBatchCommand::~ApplyBatchCommand() 
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


}