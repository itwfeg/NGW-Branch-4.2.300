//#include "vxWorks.h"
#include "PrinterRequestPrintheadPower.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterRequestPrintheadPower::dataType = 0;
std::string PrinterRequestPrintheadPower::dataTypeName("PrinterRequestPrintheadPower"); 
#endif

PrinterRequestPrintheadPower::PrinterRequestPrintheadPower(Hobart::CEvolMsgReceiver *pHobThread,
														   PrinterType printerType,
														   PrinterStateType powerState,
														   PrinterStateType shortCircuitProtection) : PrintheadPower(pHobThread,
														   															 printerType,
														   															 powerState,
														   															 shortCircuitProtection)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setEventType(PrReqPrintheadPower);
}

PrinterRequestPrintheadPower::PrinterRequestPrintheadPower(const char *buffer) : PrintheadPower(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

PrinterRequestPrintheadPower::~PrinterRequestPrintheadPower()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

void PrinterRequestPrintheadPower::show()
{
	bool isTracing = Debug::isTracing("PrinterRequestPrintheadPower");

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("PrinterRequestPrintheadPower") );
#else
		Debug::addTraceCategory("PrinterRequestPrintheadPower");
#endif
	}

	Debug::trace("PrinterRequestPrintheadPower", "PrinterRequestPrintheadPower: ");

	PrintheadPower::show();

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::removeTraceCategory( std::string("PrinterRequestPrintheadPower") );
#else
		Debug::removeTraceCategory("PrinterRequestPrintheadPower");
#endif
	}
}
