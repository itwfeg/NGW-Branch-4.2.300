//#include "vxWorks.h"
//#include <netinet/in.h>/* for htonl, ntohl etc */

#if defined(PAL_WIN32_RUN)
#include <winsock2.h>      /* for htons, ntohs, etc. */
#endif

#include "NumericEvent.h"
#include "NumericInquire.h"

#include "Debug.h"
#include "HobString.h"
#include "Hex.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int NumericInquire::dataType = 0;
std::string NumericInquire::dataTypeName("NumericInquire"); 
#endif

NumericInquire::NumericInquire(NmInquireDisp nmInquireDisp, char * deviceSignature, Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(Numeric, NmInquire, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->nmInquireDisp_ = nmInquireDisp;
	memcpy(this->deviceSignature_, deviceSignature, MAX_DEVICE_NAME_STRING);
}

NumericInquire::NumericInquire(NmInquireMsg *pInquireMsg, Hobart::CEvolMsgReceiver *pHobThread): EvolMessage(Numeric, NmInquire, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	this->nmInquireDisp_ = (NmInquireDisp)ntohs(pInquireMsg->disposition);

	memcpy(this->deviceSignature_, pInquireMsg->deviceName, MAX_DEVICE_NAME_STRING);

}

NumericInquire::NumericInquire(const char *buffer) : EvolMessage(buffer)
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
	char *pChar = (char *) &nmInquireDisp_;
	for (unsigned int i = 0; i < sizeof(nmInquireDisp_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) deviceSignature_;
	for (unsigned int i = 0; i < MAX_DEVICE_NAME_STRING; i++)
	{
		*pChar++ = buffer[j++];
	}

}

NumericInquire::~NumericInquire()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *NumericInquire::serialize() const
{

	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &nmInquireDisp_;
	for (unsigned int i = 0; i < sizeof(nmInquireDisp_); i++)
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

void NumericInquire::show()
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

	Debug::trace("NumericMessages", "NumericInquire: ");

	EvolMessage::show();

	Debug::trace("NumericMessages", "deviceSignature: ");
	Debug::trace("NumericMessages", Hex::toString((const char *)deviceSignature_, MAX_DEVICE_NAME_STRING));

	
	if (nmInquireDisp_ == NmValid)
	{
		Debug::trace("NumericMessages", "config status: valid");
	}
	else if (nmInquireDisp_ == NmError)
	{
		Debug::trace("NumericMessages", "config status: error");
	}
	else
	{
		Debug::trace("NumericMessages", HobString("config status: Unknown ") +
				HobString(Integer::toString((UINT16) nmInquireDisp_)));
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

void NumericInquire::clear()
{
	nmInquireDisp_ = NmValid;
	memset(deviceSignature_, 0, MAX_DEVICE_NAME_STRING);
}

void NumericInquire::setNmDisposition(NmInquireDisp nmInquireDisp)
{
	nmInquireDisp_ = nmInquireDisp;
}

NmInquireDisp NumericInquire::getNmDisposition()
{
	return nmInquireDisp_;
}


void NumericInquire::setDeviceSignature(char * deviceSignature)
{
	memcpy(deviceSignature_, deviceSignature, MAX_DEVICE_NAME_STRING);
}


char * NumericInquire::getDeviceSignature()
{
	return deviceSignature_;
}

int NumericInquire::fillPeripheralMessage(NmInquireMsg& nmInquireMsg)
{

	nmInquireMsg.nmEvent = eventType;
	nmInquireMsg.pad0 = 0;
	nmInquireMsg.disposition = (UINT16)htons((u_short)nmInquireDisp_);

	memcpy(nmInquireMsg.deviceName, deviceSignature_, MAX_DEVICE_NAME_STRING);

	return sizeof(nmInquireMsg);
}


