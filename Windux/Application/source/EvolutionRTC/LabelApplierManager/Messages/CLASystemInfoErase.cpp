#include "CLASystemInfoErase.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CLASystemInfoErase::dataType = 0;
std::string CLASystemInfoErase::dataTypeName("CLASystemInfoErase"); 
#endif

CLASystemInfoErase::CLASystemInfoErase(Hobart::CEvolMsgReceiver *pHobThread) : LabelApplierGeneric(LaSysInfoErase, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

CLASystemInfoErase::CLASystemInfoErase(const char *buffer) : LabelApplierGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

CLASystemInfoErase::~CLASystemInfoErase()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
