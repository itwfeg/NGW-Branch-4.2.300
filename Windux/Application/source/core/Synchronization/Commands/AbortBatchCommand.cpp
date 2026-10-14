
#if defined( __linux__ )
    #include "PAL_platformheader.h"
#endif

#include "AbortBatchCommand.h"



#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int AbortBatchCommand::_dataType_ = 0;
	std::string AbortBatchCommand::_dataTypeName_("synchronization::AbortBatchCommand"); 
#endif


	AbortBatchCommand::AbortBatchCommand(int batchId, ProcessedMessage* message)
		: Command(AbortBatchCommandType, batchId, message) 
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	AbortBatchCommand::AbortBatchCommand(const char * buffer)
		: Command(buffer)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	AbortBatchCommand::~AbortBatchCommand()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


}