#include "WeigherEvent.h"
#include "WeigherColdReset.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WeigherColdReset::dataType = 0;
std::string WeigherColdReset::dataTypeName("WeigherColdReset"); 
#endif

WeigherColdReset::WeigherColdReset(Hobart::CEvolMsgReceiver *pHobThread) : WeigherGeneric(WgColdReset, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WeigherColdReset::WeigherColdReset(const char *buffer) : WeigherGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WeigherColdReset::~WeigherColdReset()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
