//#include "vxWorks.h"
#include "ILAOff.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ILAOff::dataType = 0;
std::string ILAOff::dataTypeName("ILAOff"); 
#endif

ILAOff::ILAOff(Hobart::CEvolMsgReceiver *pHobThread) : LabelApplierGeneric(LaILAOff, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

ILAOff::ILAOff(const char *buffer) : LabelApplierGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

ILAOff::~ILAOff()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
