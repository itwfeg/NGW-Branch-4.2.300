//#include "vxWorks.h"
#include "PrintheadPower.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrintheadPower::dataType = 0;
std::string PrintheadPower::dataTypeName("PrintheadPower"); 
#endif

PrintheadPower::PrintheadPower(Hobart::CEvolMsgReceiver *pHobThread,
							   PrinterType printerType,
							   PrinterStateType powerState,
							   PrinterStateType shortCircuitProtection) : PrinterGeneric(PrPrintheadPower, pHobThread, printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->powerState = powerState;
	this->shortCircuitProtection = shortCircuitProtection;
}

PrintheadPower::PrintheadPower(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	PrinterGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &powerState;
	for (unsigned int i = 0; i < sizeof(powerState); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &shortCircuitProtection;
	for (unsigned int i = 0; i < sizeof(shortCircuitProtection); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrintheadPower::~PrintheadPower()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *PrintheadPower::serialize() const
{
	PrinterGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &powerState;
	for (unsigned int i = 0; i < sizeof(powerState); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &shortCircuitProtection;
	for (unsigned int i = 0; i < sizeof(shortCircuitProtection); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PrintheadPower::show()
{
	bool isTracing = Debug::isTracing("PrintheadPower");

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("PrintheadPower") );
#else
		Debug::addTraceCategory("PrintheadPower");
#endif
	}

	Debug::trace("PrintheadPower", "PrintheadPower: ");

	PrinterGeneric::show();

	if (powerState == PrinterOff)
	{
		Debug::trace("PrintheadPower", HobString("powerState: PrinterOff"));
	}
	else if (powerState == PrinterOn)
	{
		Debug::trace("PrintheadPower", HobString("powerState: PrinterOn"));
	}
	else
	{
		Debug::trace("PrintheadPower", HobString("powerState: Unknown ") +
									   HobString(Integer::toString((UINT16) powerState)));
	}

	if (shortCircuitProtection == PrinterOff)
	{
		Debug::trace("PrintheadPower", HobString("shortCircuitProtection: PrinterOff"));
	}
	else if (shortCircuitProtection == PrinterOn)
	{
		Debug::trace("PrintheadPower", HobString("shortCircuitProtection: PrinterOn"));
	}
	else
	{
		Debug::trace("PrintheadPower", HobString("shortCircuitProtection: Unknown ") +
									   HobString(Integer::toString((UINT16) powerState)));
	}

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::removeTraceCategory( std::string("PrintheadPower") );
#else
		Debug::removeTraceCategory("PrintheadPower");
#endif
	}
}

PrinterStateType PrintheadPower::getPowerState() const
{
	return powerState;
}

void PrintheadPower::setPowerState()
{
	powerState = PrinterOn;
}

void PrintheadPower::clearPowerState()
{
	powerState = PrinterOff;
}

bool PrintheadPower::isPowerState() const
{
	return 	(powerState > 0);
}

PrinterStateType PrintheadPower::getShortCircuitProtection() const
{
	return shortCircuitProtection;
}

void PrintheadPower::setShortCircuitProtection()
{
	shortCircuitProtection = PrinterOn;
}

void PrintheadPower::clearShortCircuitProtection()
{
	shortCircuitProtection = PrinterOff;
}

bool PrintheadPower::isShortCircuitProtection() const
{
	return 	(shortCircuitProtection > 0);
}
