/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */



#include "HobTokenizerCSV.h"
#include "Long.h"
#include "Integer.h"
#include "HfcPrinterInquire.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPrinterInquire::dataType = 0;
std::string HfcPrinterInquire::dataTypeName("HfcPrinterInquire"); 
#endif

HfcPrinterInquire::HfcPrinterInquire()
	: HfcPrinterConfig()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setEventType(PrInquire);
}

HfcPrinterInquire::HfcPrinterInquire(const char *data)
	: HfcPrinterConfig(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setEventType(PrInquire);
}

HfcPrinterInquire::HfcPrinterInquire(const char *data, int len, int formatType)
	: HfcPrinterConfig(data, len, formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setEventType(PrInquire);
}

HfcPrinterInquire::~HfcPrinterInquire()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}
