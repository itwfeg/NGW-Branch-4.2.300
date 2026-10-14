
#if defined( __linux__ )
    #include <string.h>
#endif

#include "ProcessedMessage.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif

namespace synchronization
{

#ifdef USE_ACCOUNTANT
	int ProcessedMessage::_dataType_ = 0;
	std::string ProcessedMessage::_dataTypeName_("synchronization::ProcessedMessage"); 
#endif

	ProcessedMessage::ProcessedMessage(EvolDeviceType deviceType, UINT8 eventType, int status)
		: EvolMessage(deviceType, eventType), status_(status)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	ProcessedMessage::ProcessedMessage(const char* buffer)
		: EvolMessage(buffer)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
		EvolMessage::serialize();
		buffer += getSerializedLength();

		// Now, for the derived class items
		memcpy(&status_, buffer, sizeof(status_));
	}


	ProcessedMessage::~ProcessedMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


	const char* ProcessedMessage::serialize() const
	{
		EvolMessage::serialize();

		serializedData.append((char*)&status_, sizeof(status_));

		return serializedData.data();
	}


}