#include "ChangeSynchronizationTypeMessage.h"


#include "SessionEvent.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



#ifdef USE_ACCOUNTANT
	int ChangeSynchronizationTypeMessage::_dataType_ = 0;
	std::string ChangeSynchronizationTypeMessage::_dataTypeName_("ChangeSynchronizationTypeMessage");
#endif


ChangeSynchronizationTypeMessage::ChangeSynchronizationTypeMessage(ScaleSynchronizationType type, const std::string& primary)
	: EvolMessage(SessionDev, SnChangeSynchronizationType), syncType_(type), primaryScale_(primary)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


ChangeSynchronizationTypeMessage::ChangeSynchronizationTypeMessage(const char * buffer)
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
	memcpy(&syncType_, buffer, sizeof(syncType_));
	buffer += sizeof(syncType_);

	int stringSize = 0;
	memcpy(&stringSize, buffer, sizeof(stringSize));
	buffer += sizeof(stringSize);
	primaryScale_.assign(buffer, stringSize);
}


ChangeSynchronizationTypeMessage::~ChangeSynchronizationTypeMessage()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


const char *ChangeSynchronizationTypeMessage::serialize() const
{
	EvolMessage::serialize();

	serializedData.append((char*)&syncType_, sizeof(syncType_));

	int stringSize = (int)primaryScale_.size();
	serializedData.append((char*)&stringSize, sizeof(stringSize));
	serializedData.append(primaryScale_, 0, stringSize);

	return serializedData.data();
}
