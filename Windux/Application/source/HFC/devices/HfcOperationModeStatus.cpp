/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcOperationModeStatus.h"
//#include "HobTokenizerCSV.h"
//#include "Integer.h"
//#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcOperationModeStatus::dataType = 0;
std::string HfcOperationModeStatus::dataTypeName("HfcOperationModeStatus"); 
#endif

HfcOperationModeStatus::HfcOperationModeStatus(const char *data)
	: OperationModeStatus(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcOperationModeStatus::~HfcOperationModeStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcOperationModeStatus::serialize(int format) const
{
	switch (format)
	{
		case BrowserFormat:
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
