/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcLabelApplierSystemRequestStatus.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcLabelApplierSystemRequestStatus::dataType = 0;
std::string HfcLabelApplierSystemRequestStatus::dataTypeName("HfcLabelApplierSystemRequestStatus"); 
#endif

HfcLabelApplierSystemRequestStatus::HfcLabelApplierSystemRequestStatus()
	: LabelApplierSystemRequestStatus()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcLabelApplierSystemRequestStatus::HfcLabelApplierSystemRequestStatus(const char *data)
	: LabelApplierSystemRequestStatus(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcLabelApplierSystemRequestStatus::~HfcLabelApplierSystemRequestStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}
