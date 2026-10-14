/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcPrinterRequestPrintheadPower.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPrinterRequestPrintheadPower::dataType = 0;
std::string HfcPrinterRequestPrintheadPower::dataTypeName("HfcPrinterRequestPrintheadPower"); 
#endif

HfcPrinterRequestPrintheadPower::HfcPrinterRequestPrintheadPower() : HfcPrintheadPower()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setEventType(PrReqPrintheadPower);
}

HfcPrinterRequestPrintheadPower::HfcPrinterRequestPrintheadPower(const char *data) : HfcPrintheadPower(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcPrinterRequestPrintheadPower::HfcPrinterRequestPrintheadPower(const char *data, int len, int formatType)	: HfcPrintheadPower(data, len, formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setEventType(PrReqPrintheadPower);
}

HfcPrinterRequestPrintheadPower::~HfcPrinterRequestPrintheadPower()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}
