/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "SystemEvent.h"
#include "HfcSystemInquire.h"
#include "HobTokenizerCSV.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcSystemInquire::dataType = 0;
std::string HfcSystemInquire::dataTypeName("HfcSystemInquire"); 
#endif

HfcSystemInquire::HfcSystemInquire()
	: HfcSystemConfig()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setEventType(SysInquire);
}

HfcSystemInquire::HfcSystemInquire(const char *data)
	: HfcSystemConfig(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setEventType(SysInquire);
}

HfcSystemInquire::HfcSystemInquire(const char *data, int len, int formatType)
	: HfcSystemConfig(data, len, formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setEventType(SysInquire);
}

HfcSystemInquire::~HfcSystemInquire()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}
