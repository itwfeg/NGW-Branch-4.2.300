#include "ApplyAndDeleteBatchCommand.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif

namespace synchronization
{



#ifdef USE_ACCOUNTANT
	int ApplyAndDeleteBatchCommand::_dataType_ = 0;
	std::string ApplyAndDeleteBatchCommand::_dataTypeName_("synchronization::ApplyAndDeleteBatchCommand"); 
#endif


	ApplyAndDeleteBatchCommand::ApplyAndDeleteBatchCommand(int batchId, ProcessedMessage* message)
			: Command(ApplyAndDeleteBatchCommandType, batchId, message)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	ApplyAndDeleteBatchCommand::ApplyAndDeleteBatchCommand(const char * buffer)
		: Command(buffer)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	ApplyAndDeleteBatchCommand::~ApplyAndDeleteBatchCommand() 
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}



}