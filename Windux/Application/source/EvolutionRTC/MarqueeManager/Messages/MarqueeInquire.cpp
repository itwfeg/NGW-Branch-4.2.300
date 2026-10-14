//#include "vxWorks.h"
//#include <netinet/in.h>/* for htonl, ntohl etc */

#include <PAL_platformheader.h>
//#include <winsock2.h>      /* for htons, ntohs, etc. */ // replaced by PAL header


#include "MarqueeEvent.h"
#include "MarqueeInquire.h"

#include "Debug.h"
#include "HobString.h"
#include "Hex.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int MarqueeInquire::dataType = 0;
std::string MarqueeInquire::dataTypeName("MarqueeInquire"); 
#endif


MarqueeInquire::MarqueeInquire(MqInquireDisp mqInquireDisp, char * deviceSignature, Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(Marquee, MqInquire, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	this->mqInquireDisp_ = mqInquireDisp;
	memcpy(this->deviceSignature_, deviceSignature, MAX_DEVICE_NAME_STRING);
}

MarqueeInquire::MarqueeInquire(MqInquireMsg *pInquireMsg, Hobart::CEvolMsgReceiver *pHobThread): EvolMessage(Marquee, MqInquire, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	this->mqInquireDisp_ = (MqInquireDisp)ntohs(pInquireMsg->disposition);

	memcpy(this->deviceSignature_, pInquireMsg->deviceName, MAX_DEVICE_NAME_STRING);

}

MarqueeInquire::MarqueeInquire(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	// index into buffer
	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &mqInquireDisp_;
	for (unsigned int i = 0; i < sizeof(mqInquireDisp_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) deviceSignature_;
	for (unsigned int i = 0; i < MAX_DEVICE_NAME_STRING; i++)
	{
		*pChar++ = buffer[j++];
	}

}

MarqueeInquire::~MarqueeInquire()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *MarqueeInquire::serialize() const
{

	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &mqInquireDisp_;
	for (unsigned int i = 0; i < sizeof(mqInquireDisp_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) deviceSignature_;
	for (unsigned int i = 0; i < MAX_DEVICE_NAME_STRING; i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void MarqueeInquire::show()
{
	bool isTracing = Debug::isTracing(HobString("MarqueeMessages"));

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("MarqueeMessages") );
#else
		Debug::addTraceCategory("MarqueeMessages");
#endif
	}

	Debug::trace("MarqueeMessages", "MarqueeInquire: ");

	EvolMessage::show();

	Debug::trace("MarqueeMessages", "deviceSignature: ");
	Debug::trace("MarqueeMessages", Hex::toString((const char *)deviceSignature_, MAX_DEVICE_NAME_STRING));

	
	if (mqInquireDisp_ == MqValid)
	{
		Debug::trace("MarqueeMessages", "config status: valid");
	}
	else if (mqInquireDisp_ == MqError)
	{
		Debug::trace("MarqueeMessages", "config status: error");
	}
	else
	{
		Debug::trace("MarqueeMessages", HobString("config status: Unknown ") +
											 HobString(Integer::toString((UINT16) mqInquireDisp_)));
	}


	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::removeTraceCategory( std::string("MarqueeMessages") );
#else
		Debug::removeTraceCategory("MarqueeMessages");
#endif
	}
}

void MarqueeInquire::clear()
{
	mqInquireDisp_ = MqValid;
	memset(deviceSignature_, 0, MAX_DEVICE_NAME_STRING);

}

void MarqueeInquire::setMqDisposition(MqInquireDisp mqInquireDisp)
{
	mqInquireDisp_ = mqInquireDisp;
}

MqInquireDisp MarqueeInquire::getMqDisposition()
{
	return mqInquireDisp_;
}


void MarqueeInquire::setDeviceSignature(char * deviceSignature)
{
	memcpy(deviceSignature_, deviceSignature, MAX_DEVICE_NAME_STRING);
}


char * MarqueeInquire::getDeviceSignature()
{
	return deviceSignature_;
}

int MarqueeInquire::fillPeripheralMessage(MqInquireMsg& mqInquireMsg)
{

	mqInquireMsg.mqEvent = (UINT8)eventType;
	mqInquireMsg.pad0 = 0;
	mqInquireMsg.disposition = (UINT16)htons((u_short)mqInquireDisp_);

	memcpy(mqInquireMsg.deviceName, deviceSignature_, MAX_DEVICE_NAME_STRING);

	return sizeof(mqInquireMsg);

}


