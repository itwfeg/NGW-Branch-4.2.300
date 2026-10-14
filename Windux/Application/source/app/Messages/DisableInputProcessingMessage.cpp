#include "DisableInputProcessingMessage.h"


#include "SessionEvent.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



#ifdef USE_ACCOUNTANT
	int DisableInputProcessingMessage::_dataType_ = 0;
	std::string DisableInputProcessingMessage::_dataTypeName_("DisableInputProcessingMessage");
#endif


DisableInputProcessingMessage::DisableInputProcessingMessage(int textId)
	: EvolMessage(SessionDev, SnDisableInputEvent), textId_(textId)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


DisableInputProcessingMessage::DisableInputProcessingMessage(const char * buffer)
	: EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	EvolMessage::serialize();
	buffer += getSerializedLength();

	// Now, for the derived class items
	memcpy(&textId_, buffer, sizeof(textId_));
}


DisableInputProcessingMessage::~DisableInputProcessingMessage()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


const char *DisableInputProcessingMessage::serialize() const
{
	EvolMessage::serialize();

	serializedData.append((char*)&textId_, sizeof(textId_));

	return serializedData.data();
}
