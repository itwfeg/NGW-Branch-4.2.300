/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcWeigherSystemConfig.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWeigherSystemConfig::dataType = 0;
std::string HfcWeigherSystemConfig::dataTypeName("HfcWeigherSystemConfig"); 
#endif

HfcWeigherSystemConfig::HfcWeigherSystemConfig()
	: HfcSystemConfig()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcWeigherSystemConfig::HfcWeigherSystemConfig(const char *data)
	: HfcSystemConfig(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcWeigherSystemConfig::HfcWeigherSystemConfig(const char *data, int len, int formatType)
	: HfcSystemConfig(data, len, formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcWeigherSystemConfig::~HfcWeigherSystemConfig()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}
