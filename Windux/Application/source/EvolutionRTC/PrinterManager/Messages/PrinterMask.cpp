#include "PrinterMask.h"

#include "Debug.h"
#include "HobString.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterMask::dataType = 0;
std::string PrinterMask::dataTypeName("PrinterMask"); 
#endif

PrinterMask::PrinterMask(Hobart::CEvolMsgReceiver *pHobThread,
						 PrinterType printerType,
						 UINT8 sensorMask,
						 UINT8 sensor2Mask,
						 UINT8 userMask) : PrinterGeneric(PrMask, pHobThread, printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->sensorMask = sensorMask;
	this->sensor2Mask = sensor2Mask;
	this->userMask = userMask;
}

PrinterMask::PrinterMask(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	PrinterGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &sensorMask;
	for (unsigned int i = 0; i < sizeof(sensorMask); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &sensor2Mask;
	for (unsigned int i = 0; i < sizeof(sensor2Mask); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &userMask;
	for (unsigned int i = 0; i < sizeof(userMask); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterMask::~PrinterMask()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *PrinterMask::serialize() const
{
	PrinterGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &sensorMask;
	for (unsigned int i = 0; i < sizeof(sensorMask); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &sensor2Mask;
	for (unsigned int i = 0; i < sizeof(sensor2Mask); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &userMask;
	for (unsigned int i = 0; i < sizeof(userMask); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PrinterMask::show()
{
	bool isTracing = Debug::isTracing("PrinterMask");

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("PrinterMask") );
#else
		Debug::addTraceCategory("PrinterMask");
#endif
	}

	Debug::trace("PrinterMask", "PrinterMask: ");
	
	EvolMessage::show();

	Debug::trace("PrinterMask", HobString("sensorMask: 0x") + Hex::toString((UINT32) sensorMask));
	Debug::trace("PrinterMask", HobString("sensor2Mask: 0x") + Hex::toString((UINT32) sensor2Mask));
	Debug::trace("PrinterMask", HobString("userMask: 0x") + Hex::toString((UINT32) userMask));

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::removeTraceCategory( std::string("PrinterMask") );
#else
		Debug::removeTraceCategory("PrinterMask");
#endif
	}
}

UINT8 PrinterMask::getSensorMask()
{
	return sensorMask;
}

void PrinterMask::setSensorMask(UINT8 sensorMask)
{
	this->sensorMask = sensorMask;
}

UINT8 PrinterMask::getSensor2Mask()
{
	return sensor2Mask;
}

void PrinterMask::setSensor2Mask(UINT8 sensor2Mask)
{
	this->sensor2Mask = sensor2Mask;
}

UINT8 PrinterMask::getUserMask()
{
	return userMask;
}

void PrinterMask::setUserMask(UINT8 userMask)
{
	this->userMask = userMask;
}
