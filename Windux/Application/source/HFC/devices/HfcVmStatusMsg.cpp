/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

//#include "vxWorks.h"
#include "HfcVmStatusMsg.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcVmStatusMsg::dataType = 0;
std::string HfcVmStatusMsg::dataTypeName("HfcVmStatusMsg"); 
#endif

HfcVmStatusMsg::HfcVmStatusMsg() : VmStatusMsg()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcVmStatusMsg::HfcVmStatusMsg(const char *data) : VmStatusMsg(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcVmStatusMsg::~HfcVmStatusMsg()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


