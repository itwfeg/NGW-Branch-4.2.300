#include "DownloadLocalBatchesCommand.h"



#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int DownloadLocalBatchesCommand::_dataType_ = 0;
	std::string DownloadLocalBatchesCommand::_dataTypeName_("synchronization::DownloadLocalBatchesCommand"); 
#endif



	DownloadLocalBatchesCommand::DownloadLocalBatchesCommand(int scaleId, ProcessedMessage* message)
		: CommCommand(DownloadLocalBatchesCommandType, scaleId, message)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	DownloadLocalBatchesCommand::DownloadLocalBatchesCommand(const char * buffer)
		: CommCommand(buffer)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	DownloadLocalBatchesCommand::~DownloadLocalBatchesCommand()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


}