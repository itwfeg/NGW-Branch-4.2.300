/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcPrinterSystemConfig.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPrinterSystemConfig::dataType = 0;
std::string HfcPrinterSystemConfig::dataTypeName("HfcPrinterSystemConfig"); 
#endif

HfcPrinterSystemConfig::HfcPrinterSystemConfig()
	: PrinterSystemConfiguration()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcPrinterSystemConfig::HfcPrinterSystemConfig(const char *data)
	: PrinterSystemConfiguration(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcPrinterSystemConfig::HfcPrinterSystemConfig(const char *data, int UNUSED(len), int UNUSED(formatType))
	: PrinterSystemConfiguration(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcPrinterSystemConfig::~HfcPrinterSystemConfig()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcPrinterSystemConfig::serialize(int format) const
{
	switch (format)
	{
		case InternalFormat:
			PrinterSystemConfiguration::serialize();
			HfcObject::serializedData = PrinterSystemConfiguration::serializedData;
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
