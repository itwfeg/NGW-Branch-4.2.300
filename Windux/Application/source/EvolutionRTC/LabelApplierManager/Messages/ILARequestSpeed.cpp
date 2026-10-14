//#include "vxWorks.h"
#include "ILARequestSpeed.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ILARequestSpeed::dataType = 0;
std::string ILARequestSpeed::dataTypeName("ILARequestSpeed"); 
#endif

ILARequestSpeed::ILARequestSpeed(Hobart::CEvolMsgReceiver *pHobThread) : LabelApplierGeneric(LaILAReqSpeed, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

ILARequestSpeed::ILARequestSpeed(const char *buffer) : LabelApplierGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

ILARequestSpeed::~ILARequestSpeed()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
