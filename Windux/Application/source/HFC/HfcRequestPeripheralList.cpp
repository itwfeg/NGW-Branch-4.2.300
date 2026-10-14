//#include "vxWorks.h"
#include "SystemEvent.h"
#include "HfcRequestPeripheralList.h"

#include "Debug.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcRequestPeripheralList::dataType = 0;
std::string HfcRequestPeripheralList::dataTypeName("HfcRequestPeripheralList"); 
#endif

HfcRequestPeripheralList::HfcRequestPeripheralList(Hobart::CEvolMsgReceiver *pHobThread) : SystemGeneric(System, SysReqPeripheralList, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcRequestPeripheralList::HfcRequestPeripheralList(const char *buffer) : SystemGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcRequestPeripheralList::~HfcRequestPeripheralList()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

void HfcRequestPeripheralList::show()
{
	Debug::addTraceCategory("HfcRequestPeripheralList");
	Debug::trace("HfcRequestPeripheralList", "HfcRequestPeripheralList: ");

	SystemGeneric::show();

	Debug::removeTraceCategory("HfcRequestPeripheralList");
}
