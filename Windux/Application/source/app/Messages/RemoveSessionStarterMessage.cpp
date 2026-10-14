#include "RemoveSessionStarterMessage.h"


#include "SessionEvent.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



#ifdef USE_ACCOUNTANT
	int RemoveSessionStarterMessage::_dataType_ = 0;
	std::string RemoveSessionStarterMessage::_dataTypeName_("RemoveSessionStarterMessage");
#endif


RemoveSessionStarterMessage::RemoveSessionStarterMessage(SessionManager::SessionStarter* starter)
	: EvolMessage(SessionDev, SnRemoveSessionStarter), starter_(starter)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


RemoveSessionStarterMessage::RemoveSessionStarterMessage(const char * buffer)
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
	memcpy(&starter_, buffer, sizeof(starter_));
}


RemoveSessionStarterMessage::~RemoveSessionStarterMessage()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


const char *RemoveSessionStarterMessage::serialize() const
{
	EvolMessage::serialize();

	serializedData.append((char*)&starter_, sizeof(starter_));

	return serializedData.data();
}
