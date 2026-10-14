
#if defined( __linux__ )
    #include <string.h>
#endif

#include "DeregisterSecondaryScaleProcessedMessage.h"


#include "SynchronizationDevEvent.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int DeregisterSecondaryScaleProcessedMessage::_dataType_ = 0;
	std::string DeregisterSecondaryScaleProcessedMessage::_dataTypeName_("synchronization::DeregisterSecondaryScaleProcessedMessage"); 
#endif


	DeregisterSecondaryScaleProcessedMessage::DeregisterSecondaryScaleProcessedMessage(int status, int scaleId)
		: ProcessedMessage(SynchronizationDev, DeregisterSecondaryScaleProcessedEvent, status), scaleId_(scaleId)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	DeregisterSecondaryScaleProcessedMessage::DeregisterSecondaryScaleProcessedMessage(const char* buffer)
		: ProcessedMessage(buffer)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif


		ProcessedMessage::serialize();
		buffer += getSerializedLength();

		// Now, for the derived class items
		memcpy(&scaleId_, buffer, sizeof(scaleId_));
	}


	DeregisterSecondaryScaleProcessedMessage::~DeregisterSecondaryScaleProcessedMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


	const char* DeregisterSecondaryScaleProcessedMessage::serialize() const
	{
		ProcessedMessage::serialize();

		serializedData.append((char*)&scaleId_, sizeof(scaleId_));

		return serializedData.data();
	}


}