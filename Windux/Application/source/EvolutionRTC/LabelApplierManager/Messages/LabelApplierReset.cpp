//#include "vxWorks.h"
#include "LabelApplierReset.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierReset::dataType = 0;
std::string LabelApplierReset::dataTypeName("LabelApplierReset"); 
#endif

LabelApplierReset::LabelApplierReset(Hobart::CEvolMsgReceiver *pHobThread) : LabelApplierGeneric(LaReset, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

LabelApplierReset::LabelApplierReset(const char *buffer) : LabelApplierGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierReset::~LabelApplierReset()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
