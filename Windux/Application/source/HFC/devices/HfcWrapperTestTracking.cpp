/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcWrapperTestTracking.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWrapperTestTracking::dataType = 0;
std::string HfcWrapperTestTracking::dataTypeName("HfcWrapperTestTracking"); 
#endif

HfcWrapperTestTracking::HfcWrapperTestTracking(const char *data)
	: HfcWrapperSensorsTracking(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->eventType = WrTestTracking;
}

HfcWrapperTestTracking::HfcWrapperTestTracking(const char *data, int len, int formatType)
	: HfcWrapperSensorsTracking(data, len, formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->eventType = WrTestTracking;
}

HfcWrapperTestTracking::~HfcWrapperTestTracking()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}
