#include "ChangePrimaryScaleMessage.h"


#include "SessionEvent.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



#ifdef USE_ACCOUNTANT
	int ChangePrimaryScaleMessage::_dataType_ = 0;
	std::string ChangePrimaryScaleMessage::_dataTypeName_("ChangePrimaryScaleMessage");
#endif


ChangePrimaryScaleMessage::ChangePrimaryScaleMessage(const std::string& primary)
	: EvolMessage(SessionDev, SnChangePrimaryScale), primaryScale_(primary)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


ChangePrimaryScaleMessage::ChangePrimaryScaleMessage(const char * buffer)
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
	int stringSize = 0;
	memcpy(&stringSize, buffer, sizeof(stringSize));
	buffer += sizeof(stringSize);
	primaryScale_.assign(buffer, stringSize);
}


ChangePrimaryScaleMessage::~ChangePrimaryScaleMessage()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


const char *ChangePrimaryScaleMessage::serialize() const
{
	EvolMessage::serialize();

	int stringSize = (int)primaryScale_.size();
	serializedData.append((char*)&stringSize, sizeof(stringSize));
	serializedData.append(primaryScale_, 0, stringSize);

	return serializedData.data();
}
