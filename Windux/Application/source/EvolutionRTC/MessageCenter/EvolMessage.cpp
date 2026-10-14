#include "EvolMessage.h"


#include "MessageFormat.h"

#include "Debug.h"
#include "UInteger.h"
#include "Hex.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolMessage::dataType = 0;
std::string EvolMessage::dataTypeName("EvolMessage"); 
#endif

EvolMessage::EvolMessage(const EvolMessage& newEvol)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	deviceType = newEvol.deviceType;
	eventType = newEvol.eventType;
	sourceObject = newEvol.sourceObject;
	sequenceNumber = newEvol.sequenceNumber;
	serializedData = newEvol.serializedData;

}

EvolMessage::EvolMessage(EvolDeviceType deviceType, UINT8 eventType,
		Hobart::CEvolMsgReceiver * pSource, unsigned int seqNumber)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->deviceType = deviceType;
	this->eventType = eventType;
	sourceObject = pSource;
	sequenceNumber = seqNumber;
}


EvolMessage::EvolMessage(const char * buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	deserialize(buffer);
}


unsigned int EvolMessage::deserialize(const char * buffer)
{
	unsigned int j = 0;

	char * pChar = (char *) & deviceType;
	for (unsigned int i = 0; i < sizeof(deviceType); i++)
	{
		* pChar++ = buffer[j++];
	}

	pChar = (char *) & eventType;
	for (unsigned int i = 0; i < sizeof(eventType); i++)
	{
		* pChar++ = buffer[j++];
	}

	pChar = (char *) & sourceObject;
	for (unsigned int i = 0; i < sizeof(sourceObject); i++)
	{
		* pChar++ = buffer[j++];
	}

	pChar = (char *) & sequenceNumber;
	for (unsigned int i = 0; i < sizeof(sequenceNumber); i++)
	{
		* pChar++ = buffer[j++];
	}

	return j;
}


EvolMessage::~EvolMessage()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


const char *EvolMessage::serialize() const
{
	serializedData = "";

	char *pChar = (char *) & deviceType;
	for (unsigned int i = 0; i < sizeof(deviceType); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) & eventType;
	for (unsigned int i = 0; i < sizeof(eventType); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) & sourceObject;
	for (unsigned int i = 0; i < sizeof(sourceObject); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) & sequenceNumber;
	for (unsigned int i = 0; i < sizeof(sequenceNumber); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}


UINT32 EvolMessage::getSerializedLength() const
{
	return (UINT32)serializedData.length();
}


void EvolMessage::setDeviceType(EvolDeviceType deviceType)
{
	this->deviceType = deviceType;
}

EvolDeviceType EvolMessage::getDeviceType() const
{
	return deviceType;
}

void EvolMessage::setEventType(UINT8 eventType)
{
	this->eventType = eventType;
}

UINT8 EvolMessage::getEventType() const
{
	return eventType;
}

void EvolMessage::setSourceObject( Hobart::CEvolMsgReceiver * sourceObject)
{
	this->sourceObject = sourceObject;
}

Hobart::CEvolMsgReceiver * EvolMessage::getSourceObject()
{
	return sourceObject;
}

void EvolMessage::show() const
{

}


const char *EvolMessage::serialize(int format) const
{
	switch (format)
	{
		case BrowserFormat:
			serializedData = "Serialized EvolMessage : browser format";
			break;

		case SMFormat:
			serializedData = "Serialized EvolMessage : sm format";
			break;

		case InternalFormat:
		default:
			serialize();
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.data();
}


// static
UINT16 EvolMessage::getEvolMessageLengthInBuffer()
{
	EvolMessage e;
	e.serialize();
	return e.getSerializedLength();
}


// private
EvolMessage::EvolMessage()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	deviceType = UnknownDevice;
	eventType = 0;
	sourceObject = (Hobart::CEvolMsgReceiver *)NULL;
	sequenceNumber = 0;
}
