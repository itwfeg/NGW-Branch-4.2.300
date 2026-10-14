#include "AddNewSessionMessage.h"


#include "SessionEvent.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



#ifdef USE_ACCOUNTANT
	int AddNewSessionMessage::_dataType_ = 0;
	std::string AddNewSessionMessage::_dataTypeName_("AddNewSessionMessage");
#endif


AddNewSessionMessage::AddNewSessionMessage(AbstractSession* session)
	: EvolMessage(SessionDev, SnAddNewSession), session_(session)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


AddNewSessionMessage::AddNewSessionMessage(const char * buffer)
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
	memcpy(&session_, buffer, sizeof(session_));
}


AddNewSessionMessage::~AddNewSessionMessage()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


const char *AddNewSessionMessage::serialize() const
{
	EvolMessage::serialize();

	serializedData.append((char*)&session_, sizeof(session_));

	return serializedData.data();
}
