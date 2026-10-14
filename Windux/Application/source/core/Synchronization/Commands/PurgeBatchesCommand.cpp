#include "PurgeBatchesCommand.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int PurgeBatchesCommand::_dataType_ = 0;
	std::string PurgeBatchesCommand::_dataTypeName_("synchronization::PurgeBatchesCommand"); 
#endif


	PurgeBatchesCommand::PurgeBatchesCommand(ProcessedMessage* message)
			: Command(PurgeBatchesCommandType, -1, message) 
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	PurgeBatchesCommand::PurgeBatchesCommand(const char * buffer)
		: Command(buffer)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	PurgeBatchesCommand::~PurgeBatchesCommand() 
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


}