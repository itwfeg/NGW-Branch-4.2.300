#include "SynchronizePrimaryScaleCommand.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int SynchronizePrimaryScaleCommand::_dataType_ = 0;
	std::string SynchronizePrimaryScaleCommand::_dataTypeName_("synchronization::SynchronizePrimaryScaleCommand"); 
#endif


	SynchronizePrimaryScaleCommand::SynchronizePrimaryScaleCommand(int scaleId, ProcessedMessage* message)
			: CommCommand(SynchronizePrimaryScaleCommandType, scaleId, message) 
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	SynchronizePrimaryScaleCommand::SynchronizePrimaryScaleCommand(const char * buffer)
		: CommCommand(buffer)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	SynchronizePrimaryScaleCommand::~SynchronizePrimaryScaleCommand() 
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


}