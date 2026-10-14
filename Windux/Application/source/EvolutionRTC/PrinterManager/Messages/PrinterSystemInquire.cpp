//#include "vxWorks.h"
#include "PrinterSystemInquire.h"
#include "SystemEvent.h"

#include "Debug.h"
#include "HobString.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterSystemInquire::dataType = 0;
std::string PrinterSystemInquire::dataTypeName("PrinterSystemInquire"); 
#endif

PrinterSystemInquire::PrinterSystemInquire(Hobart::CEvolMsgReceiver *pHobThread,
										   PrinterType prType,
										   PrinterKind prKind,
										   SysConfigType sysConfigType,
										   UINT8 *deviceSignature,
										   DeviceAddress stationAddress,
										   DeviceAddress controllerAddress,
										   UINT8 hardwareMajor,
										   UINT8 hardwareMinor) : PrinterSystemConfiguration(pHobThread,
										   													 prType,
																							 prKind,
																							 sysConfigType,
																							 deviceSignature,
																							 stationAddress,
																							 controllerAddress,
																							 hardwareMajor,
																							 hardwareMinor)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setEventType(SysInquire);
}

PrinterSystemInquire::PrinterSystemInquire(const char *buffer) : PrinterSystemConfiguration(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

PrinterSystemInquire::~PrinterSystemInquire()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

void PrinterSystemInquire::show()
{
	bool isTracing = Debug::isTracing("PrinterSystemInquire");

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("PrinterSystemInquire") );
#else
		Debug::addTraceCategory("PrinterSystemInquire");
#endif
	}

	Debug::trace("PrinterSystemInquire", "PrinterSystemInquire: ");

	PrinterSystemConfiguration::show();

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::removeTraceCategory( std::string("PrinterSystemInquire") );
#else
		Debug::removeTraceCategory("PrinterSystemInquire");
#endif
	}
}
