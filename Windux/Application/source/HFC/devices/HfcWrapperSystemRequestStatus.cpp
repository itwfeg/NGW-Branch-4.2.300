/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcWrapperSystemRequestStatus.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWrapperSystemRequestStatus::dataType = 0;
std::string HfcWrapperSystemRequestStatus::dataTypeName("HfcWrapperSystemRequestStatus"); 
#endif

HfcWrapperSystemRequestStatus::HfcWrapperSystemRequestStatus()
	: WrapperSystemRequestStatus()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcWrapperSystemRequestStatus::HfcWrapperSystemRequestStatus(const char *data)
	: WrapperSystemRequestStatus(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcWrapperSystemRequestStatus::~HfcWrapperSystemRequestStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}
