//#include "vxWorks.h"
#include "LabelApplierDisable.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierDisable::dataType = 0;
std::string LabelApplierDisable::dataTypeName("LabelApplierDisable"); 
#endif

LabelApplierDisable::LabelApplierDisable(Hobart::CEvolMsgReceiver *pHobThread) : LabelApplierGeneric(LaDisable, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierDisable::LabelApplierDisable(const char *buffer) : LabelApplierGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierDisable::~LabelApplierDisable()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
