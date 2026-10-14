/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "SystemEvent.h"
#include "HfcPrinterSystemInquire.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPrinterSystemInquire::dataType = 0;
std::string HfcPrinterSystemInquire::dataTypeName("HfcPrinterSystemInquire"); 
#endif

HfcPrinterSystemInquire::HfcPrinterSystemInquire()
	: HfcPrinterSystemConfig()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setEventType(SysInquire);
}

HfcPrinterSystemInquire::HfcPrinterSystemInquire(const char *data)
	: HfcPrinterSystemConfig(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setEventType(SysInquire);
}

HfcPrinterSystemInquire::HfcPrinterSystemInquire(const char *data, int len, int formatType)
	: HfcPrinterSystemConfig(data, len, formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setEventType(SysInquire);
}

HfcPrinterSystemInquire::~HfcPrinterSystemInquire()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}
