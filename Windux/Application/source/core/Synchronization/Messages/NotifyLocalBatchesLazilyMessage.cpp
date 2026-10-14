#include "NotifyLocalBatchesLazilyMessage.h"
#include "SynchronizationDevEvent.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif

namespace synchronization
{

#ifdef USE_ACCOUNTANT
	int NotifyLocalBatchesLazilyMessage::_dataType_ = 0;
	std::string NotifyLocalBatchesLazilyMessage::_dataTypeName_("synchronization::NotifyLocalBatchesLazilyMessage"); 
#endif

	NotifyLocalBatchesLazilyMessage::NotifyLocalBatchesLazilyMessage()
		: EvolMessage(SynchronizationDev, NotifyLocalBatchesLazilyEvent)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	NotifyLocalBatchesLazilyMessage::NotifyLocalBatchesLazilyMessage(const char* buffer)
		: EvolMessage(buffer)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	NotifyLocalBatchesLazilyMessage::~NotifyLocalBatchesLazilyMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


}