//#include "vxWorks.h"
#include "SendErrorEmailEvent.h"
#include "EmailManagerEvents.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"

	int SendErrorEmailEvent::_dataType_ = 0;
	std::string SendErrorEmailEvent::_dataTypeName_("SendErrorEmailEvent"); 
#endif

SendErrorEmailEvent::SendErrorEmailEvent(ErrorEmailType type, ErrorEmailCode code, Hobart::CEvolMsgReceiver *pHobThread) : 
	EvolMessage(EmailManagerDev, SendErrorEmail, pHobThread), type_(type), code_(code)
{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
}

SendErrorEmailEvent::SendErrorEmailEvent(const char *buffer): EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
unsigned int j = 0, i = 0;

	EvolMessage::serialize();

	j = getSerializedLength();

	// Now, for the derived class items

	char *pChar = (char *) &type_;
	for (i = 0 ; i < sizeof(type_); i++)
		*pChar++ = buffer[j++];

	pChar = (char *) &code_;
	for (i = 0 ; i < sizeof(code_); i++)
		*pChar++ = buffer[j++];

}

SendErrorEmailEvent::~SendErrorEmailEvent() 
{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
}


const char *SendErrorEmailEvent::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items

	char *pChar = (char *) & type_;
	for (unsigned int i = 0; i < sizeof(type_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) & code_;
	for (unsigned int i = 0; i < sizeof(code_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void SendErrorEmailEvent::show()
{
	EvolMessage::show();

	//cout << "type_: " << type_ << "code_: " << code_ <<endl;

}
