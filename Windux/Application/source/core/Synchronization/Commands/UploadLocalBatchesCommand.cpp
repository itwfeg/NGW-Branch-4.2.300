#include "UploadLocalBatchesCommand.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif

namespace synchronization
{
#ifdef USE_ACCOUNTANT
	int UploadLocalBatchesCommand::_dataType_ = 0;
	std::string UploadLocalBatchesCommand::_dataTypeName_("synchronization::UploadLocalBatchesCommand"); 
#endif

	UploadLocalBatchesCommand::UploadLocalBatchesCommand(int scaleId, int rversion, EvolMessage* message)
			: CommCommand(UploadLocalBatchesCommandType, scaleId, message), remoteVersion_(rversion) 
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}

	UploadLocalBatchesCommand::UploadLocalBatchesCommand(const char * buffer)
		: CommCommand(buffer)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}

	UploadLocalBatchesCommand::~UploadLocalBatchesCommand() 
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}
}