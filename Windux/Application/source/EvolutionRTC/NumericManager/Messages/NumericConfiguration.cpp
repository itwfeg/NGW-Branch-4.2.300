//#include "vxWorks.h"
//#include <netinet/in.h>/* for htonl, ntohl etc */

#include <PAL_platformheader.h>
//#include <winsock2.h>      /* for htons, ntohs, etc. */ // replaced by PAL header


#include "NumericEvent.h"
#include "NumericConfiguration.h"

#include "Debug.h"
#include "HobString.h"
#include "Hex.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int NumericConfiguration::dataType = 0;
std::string NumericConfiguration::dataTypeName("NumericConfiguration"); 
#endif

NumericConfiguration::NumericConfiguration(NmConfigType nmConfigType, char * deviceSignature, Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(Numeric, NmConfig, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->nmConfigType_ = nmConfigType;
	memcpy(this->deviceSignature_, deviceSignature, MAX_DEVICE_NAME_STRING);
}

NumericConfiguration::NumericConfiguration(const char *buffer) : EvolMessage(buffer)
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
	char *pChar = (char *) &nmConfigType_;
	for (unsigned int i = 0; i < sizeof(nmConfigType_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) deviceSignature_;
	for (unsigned int i = 0; i < MAX_DEVICE_NAME_STRING; i++)
	{
		*pChar++ = buffer[j++];
	}


}

NumericConfiguration::~NumericConfiguration()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *NumericConfiguration::serialize() const
{

	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &nmConfigType_;
	for (unsigned int i = 0; i < sizeof(nmConfigType_); i++)
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

void NumericConfiguration::show()
{
	bool isTracing = Debug::isTracing(HobString("NumericMessages"));

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("NumericMessages") );
#else
		Debug::addTraceCategory("NumericMessages");
#endif
	}

	Debug::trace("NumericMessages", "NumericConfiguration: ");

	EvolMessage::show();

	Debug::trace("NumericMessages", "deviceSignature: ");
	Debug::trace("NumericMessages", Hex::toString( (const char *)deviceSignature_, MAX_DEVICE_NAME_STRING));

	
	if (nmConfigType_ == NmPermanent)
	{
		Debug::trace("NumericMessages", "configType: MqPermanent");
	}
	else if (nmConfigType_ == NmTemporary)
	{
		Debug::trace("NumericMessages", "configType: MqTemporary");
	}
	else if (nmConfigType_ == NmDefault)
	{
		Debug::trace("NumericMessages", "configType: MqDefault");
	}
	else
	{
		Debug::trace("NumericMessages", HobString("configType: Unknown ") +
											 HobString(Integer::toString((UINT16) nmConfigType_)));
	}


	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::removeTraceCategory( std::string("NumericMessages") );
#else
		Debug::removeTraceCategory("NumericMessages");
#endif
	}
}

void NumericConfiguration::clear()
{
	nmConfigType_ = NmPermanent;
	memset(deviceSignature_, 0, MAX_DEVICE_NAME_STRING);
}

void NumericConfiguration::setNmConfigType(NmConfigType nmConfigType)
{
	nmConfigType_ = nmConfigType;
}

NmConfigType NumericConfiguration::getNmConfigType()
{
	return nmConfigType_;
}


void NumericConfiguration::setDeviceSignature(char * deviceSignature)
{
	memcpy(deviceSignature_, deviceSignature, MAX_DEVICE_NAME_STRING);
}


char * NumericConfiguration::getDeviceSignature()
{
	return deviceSignature_;
}


int NumericConfiguration::fillPeripheralMessage(NmConfigMsg& nmConfigMsg)
{

	nmConfigMsg.nmEvent = (UINT8)eventType;
	nmConfigMsg.pad0 = 0;
	nmConfigMsg.disposition = (UINT16)htons((u_short)nmConfigType_);

	memcpy(nmConfigMsg.deviceName, deviceSignature_, MAX_DEVICE_NAME_STRING);

	return sizeof(nmConfigMsg);
}
