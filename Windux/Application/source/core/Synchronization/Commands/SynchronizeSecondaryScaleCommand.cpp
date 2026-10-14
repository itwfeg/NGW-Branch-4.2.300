#include "SynchronizeSecondaryScaleCommand.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int SynchronizeSecondaryScaleCommand::_dataType_ = 0;
	std::string SynchronizeSecondaryScaleCommand::_dataTypeName_("synchronization::SynchronizeSecondaryScaleCommand"); 
#endif


	SynchronizeSecondaryScaleCommand::SynchronizeSecondaryScaleCommand(int scaleId, ProcessedMessage* message)
			: CommCommand(SynchronizeSecondaryScaleCommandType, scaleId, message) 
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	SynchronizeSecondaryScaleCommand::SynchronizeSecondaryScaleCommand(const char * buffer)
		: CommCommand(buffer)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	SynchronizeSecondaryScaleCommand::~SynchronizeSecondaryScaleCommand() 
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


}