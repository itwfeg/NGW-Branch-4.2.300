/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "SystemEvent.h"
#include "HfcWeigherSystemInquire.h"
//#include "HobTokenizerCSV.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWeigherSystemInquire::dataType = 0;
std::string HfcWeigherSystemInquire::dataTypeName("HfcWeigherSystemInquire"); 
#endif

HfcWeigherSystemInquire::HfcWeigherSystemInquire()
	: HfcWeigherSystemConfig()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setEventType(SysInquire);
}

HfcWeigherSystemInquire::HfcWeigherSystemInquire(const char *data)
	: HfcWeigherSystemConfig(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setEventType(SysInquire);
}

HfcWeigherSystemInquire::HfcWeigherSystemInquire(const char *data, int len, int formatType)
	: HfcWeigherSystemConfig(data, len, formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setEventType(SysInquire);
}

HfcWeigherSystemInquire::~HfcWeigherSystemInquire()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}
