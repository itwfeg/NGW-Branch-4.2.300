#include "RequestRegistrationProcessedMessage.h"


#include "SynchronizationDevEvent.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int RequestRegistrationProcessedMessage::_dataType_ = 0;
	std::string RequestRegistrationProcessedMessage::_dataTypeName_("synchronization::RequestRegistrationProcessedMessage"); 
#endif


	RequestRegistrationProcessedMessage::RequestRegistrationProcessedMessage(int status)
		: ProcessedMessage(SynchronizationDev, RequestRegistrationProcessedEvent, status)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	RequestRegistrationProcessedMessage::RequestRegistrationProcessedMessage(const char* buffer)
		: ProcessedMessage(buffer)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	RequestRegistrationProcessedMessage::~RequestRegistrationProcessedMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


}