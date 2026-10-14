//#include "vxWorks.h"
#include "SendGcdFtpErrorEmailEvent.h"
#include "EmailManagerEvents.h"
#include "CMReaderWriter.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"

	int SendGcdFtpErrorEmailEvent::_dataType_ = 0;
	std::string SendGcdFtpErrorEmailEvent::_dataTypeName_("SendGcdFtpErrorEmailEvent"); 
#endif

	SendGcdFtpErrorEmailEvent::SendGcdFtpErrorEmailEvent(HobString& msg, Hobart::CEvolMsgReceiver *pHobThread) : 
	EvolMessage(EmailManagerDev, SendGcdFtpErrorEmail, pHobThread), err_msg(msg)
{

#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
}

SendGcdFtpErrorEmailEvent::SendGcdFtpErrorEmailEvent(const char *buffer): 
	EvolMessage(EmailManagerDev, SendGcdFtpErrorEmail)
{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif

	EvolMessage::serialize();
	int j = getSerializedLength();

	err_msg += &buffer[j];

}

SendGcdFtpErrorEmailEvent::~SendGcdFtpErrorEmailEvent() 
{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
}


const char *SendGcdFtpErrorEmailEvent::serialize() const
{
	EvolMessage::serialize();

	serializedData.append(err_msg);
	return serializedData.data();

}

void SendGcdFtpErrorEmailEvent::show()
{
	EvolMessage::show();

}
