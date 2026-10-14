#include "NotifyOnlineProcessedMessage.h"


#include "SynchronizationDevEvent.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int NotifyOnlineProcessedMessage::_dataType_ = 0;
	std::string NotifyOnlineProcessedMessage::_dataTypeName_("synchronization::NotifyOnlineProcessedMessage"); 
#endif


	NotifyOnlineProcessedMessage::NotifyOnlineProcessedMessage(int status)
		: ProcessedMessage(SynchronizationDev, NotifyOnlineProcessedEvent, status)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	NotifyOnlineProcessedMessage::NotifyOnlineProcessedMessage(const char* buffer)
		: ProcessedMessage(buffer)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	NotifyOnlineProcessedMessage::~NotifyOnlineProcessedMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


}