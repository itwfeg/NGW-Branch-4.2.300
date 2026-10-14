/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HobTokenizerCSV.h"
#include "Long.h"
#include "Integer.h"
#include "HfcWrapperInquire.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWrapperInquire::dataType = 0;
std::string HfcWrapperInquire::dataTypeName("HfcWrapperInquire"); 
#endif

HfcWrapperInquire::HfcWrapperInquire()
	: HfcWrapperConfig()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setEventType(WrInquire);
}

HfcWrapperInquire::HfcWrapperInquire(const char *data)
	: HfcWrapperConfig(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setEventType(WrInquire);
}

HfcWrapperInquire::HfcWrapperInquire(const char *data, int len, int formatType)
	: HfcWrapperConfig(data, len, formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setEventType(WrInquire);
}

HfcWrapperInquire::~HfcWrapperInquire()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}
