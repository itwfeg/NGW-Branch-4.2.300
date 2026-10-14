//#include "vxWorks.h"
#include "SendGcdPlaylistErrorEmailEvent.h"
#include "EmailManagerEvents.h"
#include "CMReaderWriterGCD.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"

	int SendGcdPlaylistErrorEmailEvent::_dataType_ = 0;
	std::string SendGcdPlaylistErrorEmailEvent::_dataTypeName_("SendGcdPlaylistErrorEmailEvent"); 
#endif

SendGcdPlaylistErrorEmailEvent::SendGcdPlaylistErrorEmailEvent(HobString& msg, Hobart::CEvolMsgReceiver *pHobThread) : 
	EvolMessage(EmailManagerDev, SendGcdPlaylistErrorEmail, pHobThread), err_msg(msg)
{

#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
}

SendGcdPlaylistErrorEmailEvent::SendGcdPlaylistErrorEmailEvent(HobString& msg, HobString& url, Hobart::CEvolMsgReceiver *pHobThread) : 
	EvolMessage(EmailManagerDev, SendGcdPlaylistErrorEmail, pHobThread), err_msg(msg), file_url(url)
{

#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
}

SendGcdPlaylistErrorEmailEvent::SendGcdPlaylistErrorEmailEvent(const char *buffer, int length): 
	EvolMessage(EmailManagerDev, SendGcdPlaylistErrorEmail)
{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif

	EvolMessage::serialize();
	int j = getSerializedLength();


	int i = j;
	int fieldIndex = 0;
	while (i < length)
	{
		int n;
		for (n = i; (n < length) && buffer[n] != CMReaderWriterGCD::CH_DELIMETER; ++ n);
		int fieldLength = n - i;

		switch (fieldIndex)
		{
		case 0:
			err_msg.clear();
			err_msg.append(&buffer[i], fieldLength);
			break;
		case 1:
			file_url.clear();
			file_url.append(&buffer[i], fieldLength);
			break;
		}

		i += fieldLength + 1;
		++ fieldIndex;
	}
}

SendGcdPlaylistErrorEmailEvent::~SendGcdPlaylistErrorEmailEvent() 
{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
}


const char *SendGcdPlaylistErrorEmailEvent::serialize() const
{
	EvolMessage::serialize();

	if (err_msg.size() > 0) 
		serializedData.append(err_msg);

	if (file_url.size() > 0)
	{
		serializedData.push_back(CMReaderWriterGCD::CH_DELIMETER);
		serializedData.append(file_url);
	}

	return serializedData.data();
}

void SendGcdPlaylistErrorEmailEvent::show()
{
	EvolMessage::show();

}
