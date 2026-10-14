//#include "vxWorks.h"
#include "ILACycle.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ILACycle::dataType = 0;
std::string ILACycle::dataTypeName("ILACycle"); 
#endif

ILACycle::ILACycle(Hobart::CEvolMsgReceiver *pHobThread) : LabelApplierGeneric(LaILACycle, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

ILACycle::ILACycle(const char *buffer) : LabelApplierGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

ILACycle::~ILACycle()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
