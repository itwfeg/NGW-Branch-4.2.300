#include "RebootMessage.h"


#include "SessionEvent.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



#ifdef USE_ACCOUNTANT
	int RebootMessage::_dataType_ = 0;
	std::string RebootMessage::_dataTypeName_("RebootMessage");
#endif


RebootMessage::RebootMessage(bool powerDownOnly)
	: EvolMessage(SessionDev, SnReboot), powerDownOnly_(powerDownOnly)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


RebootMessage::RebootMessage(const char * buffer)
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
	memcpy(&powerDownOnly_, buffer, sizeof(powerDownOnly_));
}


RebootMessage::~RebootMessage()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


const char *RebootMessage::serialize() const
{
	EvolMessage::serialize();

	serializedData.append((char*)&powerDownOnly_, sizeof(powerDownOnly_));

	return serializedData.data();
}
