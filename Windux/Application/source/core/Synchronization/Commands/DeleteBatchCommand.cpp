#include "DeleteBatchCommand.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int DeleteBatchCommand::_dataType_ = 0;
	std::string DeleteBatchCommand::_dataTypeName_("synchronization::DeleteBatchCommand"); 
#endif


	DeleteBatchCommand::DeleteBatchCommand(int batchId, ProcessedMessage* message)
			: Command(DeleteBatchCommandType, batchId, message)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	DeleteBatchCommand::DeleteBatchCommand(const char * buffer)
		: Command(buffer)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	DeleteBatchCommand::~DeleteBatchCommand() 
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


}