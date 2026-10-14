/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "WeigherEvent.h"
#include "HobTokenizerCSV.h"
#include "Long.h"
#include "Integer.h"
#include "HfcWeigherInquire.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWeigherInquire::dataType = 0;
std::string HfcWeigherInquire::dataTypeName("HfcWeigherInquire"); 
#endif

HfcWeigherInquire::HfcWeigherInquire()
	: HfcWeigherConfig()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setEventType(WgInquire);
}

HfcWeigherInquire::HfcWeigherInquire(const char *data)
	: HfcWeigherConfig(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setEventType(WgInquire);
}

HfcWeigherInquire::HfcWeigherInquire(const char *data, int len, int formatType)
	: HfcWeigherConfig(data, len, formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setEventType(WgInquire);
}

HfcWeigherInquire::~HfcWeigherInquire()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}
