/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcPrinterSystemRequestStatus.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPrinterSystemRequestStatus::dataType = 0;
std::string HfcPrinterSystemRequestStatus::dataTypeName("HfcPrinterSystemRequestStatus"); 
#endif

HfcPrinterSystemRequestStatus::HfcPrinterSystemRequestStatus()
	: PrinterSystemRequestStatus()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcPrinterSystemRequestStatus::HfcPrinterSystemRequestStatus(const char *data)
	: PrinterSystemRequestStatus(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcPrinterSystemRequestStatus::HfcPrinterSystemRequestStatus(const char *data, int UNUSED(len), int formatType)
	: PrinterSystemRequestStatus()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	switch (formatType)
	{
		case InternalFormat:
			break;

		case BrowserFormat:
			setPrinterType((PrinterType) Integer::valueOf(data));
			break;

		case SMFormat:
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcPrinterSystemRequestStatus::~HfcPrinterSystemRequestStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcPrinterSystemRequestStatus::serialize(int format) const
{
	switch (format)
	{
		case InternalFormat:
		{
			PrinterType ptype = getPrinterType();
			HfcObject::serializedData.append((const char *)&ptype, sizeof(ptype) );

		}
		break;

		case BrowserFormat:
			HfcObject::serializedData = "Serialized Printer System Request Status : browser format";
		break;

		case SMFormat:
			HfcObject::serializedData = "Serialized Printer System Request Status : sm format";
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
