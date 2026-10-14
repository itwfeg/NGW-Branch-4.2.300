
#include "RFIDStatus.h"
#include "RFIDEvent.h"
#include "Debug.h"

#include "Hex.h"

#include <string>


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int RFIDStatus::dataType = 0;
std::string RFIDStatus::dataTypeName("RFIDStatus"); 
#endif

RFIDStatus::RFIDStatus(Hobart::CEvolMsgReceiver *pHobThread,
								UINT8 status) : EvolMessage(RFIDReader, RFIDCurrentStatus, pHobThread), comStatus_(status)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif


}

RFIDStatus::RFIDStatus(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &comStatus_;
	for (unsigned int i = 0; i < sizeof(comStatus_); i++)
	{
		*pChar++ = buffer[j++];
	}

}

RFIDStatus::~RFIDStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}



void RFIDStatus::setComStatus(bool online)
{
	if (online)
	{
		comStatus_ |= COM_STATUS_ONLINE;
	}
	else
	{
		comStatus_ &= (UINT8)~COM_STATUS_ONLINE;
	}
}

bool RFIDStatus::getComStatus()
{

	return (((comStatus_ & COM_STATUS_ONLINE) == COM_STATUS_ONLINE) ? true : false);
}

const char *RFIDStatus::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &comStatus_;
	for (unsigned int i = 0; i < sizeof(comStatus_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void RFIDStatus::show()
{
	Debug::addTraceCategory("RFIDStatus");
	Debug::trace("RFIDStatus", "RFIDStatus: ");

	EvolMessage::show();

	if (comStatus_ & COM_STATUS_ONLINE)
		Debug::trace("RFIDStatus", std::string("comStatus = true"));
	else
		Debug::trace("RFIDStatus", std::string("comStatus = false"));


	Debug::removeTraceCategory("RFIDStatus");
}




		


