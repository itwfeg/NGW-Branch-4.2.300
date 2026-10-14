//#include "vxWorks.h"
//#include <netinet/in.h>/* for htonl, ntohl etc */

#include <PAL_platformheader.h>
//#include <winsock2.h>      /* for htons, ntohs, etc. */ // replaced by PAL header


#include "MarqueeEvent.h"
#include "MarqueeConfiguration.h"

#include "Debug.h"
#include "HobString.h"
#include "Hex.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int MarqueeConfiguration::dataType = 0;
std::string MarqueeConfiguration::dataTypeName("MarqueeConfiguration"); 
#endif


MarqueeConfiguration::MarqueeConfiguration(MqConfigType mqConfigType, char * deviceSignature, Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(Marquee, MqConfig, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->mqConfigType_ = mqConfigType;
	memcpy(this->deviceSignature_, deviceSignature, MAX_DEVICE_NAME_STRING);
}

MarqueeConfiguration::MarqueeConfiguration(const char *buffer) : EvolMessage(buffer)
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
	char *pChar = (char *) &mqConfigType_;
	for (unsigned int i = 0; i < sizeof(mqConfigType_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) deviceSignature_;
	for (unsigned int i = 0; i < MAX_DEVICE_NAME_STRING; i++)
	{
		*pChar++ = buffer[j++];
	}
}

MarqueeConfiguration::~MarqueeConfiguration()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *MarqueeConfiguration::serialize() const
{

	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &mqConfigType_;
	for (unsigned int i = 0; i < sizeof(mqConfigType_); i++)
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

void MarqueeConfiguration::show()
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

	Debug::trace("MarqueeMessages", "MarqueeConfiguration: ");

	EvolMessage::show();

	Debug::trace("MarqueeMessages", "deviceSignature: ");
	Debug::trace("MarqueeMessages", Hex::toString( (const char *)deviceSignature_, MAX_DEVICE_NAME_STRING));

	
	if (mqConfigType_ == MqPerm)
	{
		Debug::trace("MarqueeMessages", "configType: MqPermanent");
	}
	else if (mqConfigType_ == MqTemporary)
	{
		Debug::trace("MarqueeMessages", "configType: MqTemporary");
	}
	else if (mqConfigType_ == MqDefault)
	{
		Debug::trace("MarqueeMessages", "configType: MqDefault");
	}
	else
	{
		Debug::trace("MarqueeMessages", HobString("configType: Unknown ") +
											 HobString(Integer::toString((UINT16) mqConfigType_)));
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

void MarqueeConfiguration::clear()
{
	mqConfigType_ = MqPerm;
	memset(deviceSignature_, 0, MAX_DEVICE_NAME_STRING);
}

void MarqueeConfiguration::setMqConfigType(MqConfigType mqConfigType)
{
	mqConfigType_ = mqConfigType;
}

MqConfigType MarqueeConfiguration::getMqConfigType()
{
	return mqConfigType_;
}


void MarqueeConfiguration::setDeviceSignature(char * deviceSignature)
{
	memcpy(deviceSignature_, deviceSignature, MAX_DEVICE_NAME_STRING);
}


char * MarqueeConfiguration::getDeviceSignature()
{
	return deviceSignature_;
}


int MarqueeConfiguration::fillPeripheralMessage(MqConfigMsg& MqConfigMsg)
{

	MqConfigMsg.mqEvent = (UINT8)eventType;
	MqConfigMsg.pad0 = 0;
	MqConfigMsg.disposition = (UINT16)htons((u_short)mqConfigType_);

	memcpy(MqConfigMsg.deviceName, deviceSignature_, MAX_DEVICE_NAME_STRING);

	return sizeof(MqConfigMsg);

}

