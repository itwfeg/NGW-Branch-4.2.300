/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcLabelApplierInquire.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcLabelApplierInquire::dataType = 0;
std::string HfcLabelApplierInquire::dataTypeName("HfcLabelApplierInquire"); 
#endif

HfcLabelApplierInquire::HfcLabelApplierInquire() : HfcLabelApplierConfig()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setEventType(LaInquire);
}

HfcLabelApplierInquire::HfcLabelApplierInquire(const char *data) : HfcLabelApplierConfig(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setEventType(LaInquire);
}

HfcLabelApplierInquire::HfcLabelApplierInquire(const char *data, int len, int formatType) : HfcLabelApplierConfig(data, len, formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setEventType(LaInquire);
}

HfcLabelApplierInquire::~HfcLabelApplierInquire()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}
