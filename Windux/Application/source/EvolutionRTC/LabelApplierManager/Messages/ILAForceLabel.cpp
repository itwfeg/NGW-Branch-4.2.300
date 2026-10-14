//#include "vxWorks.h"
#include "ILAForceLabel.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ILAForceLabel::dataType = 0;
std::string ILAForceLabel::dataTypeName("ILAForceLabel"); 
#endif

ILAForceLabel::ILAForceLabel(Hobart::CEvolMsgReceiver *pHobThread) : LabelApplierGeneric(LaILAForceLabel, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

ILAForceLabel::ILAForceLabel(const char *buffer) : LabelApplierGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

ILAForceLabel::~ILAForceLabel()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
