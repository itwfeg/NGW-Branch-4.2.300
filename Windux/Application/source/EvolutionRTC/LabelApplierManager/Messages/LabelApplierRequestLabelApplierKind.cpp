//#include "vxWorks.h"
#include "LabelApplierRequestLabelApplierKind.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierRequestLabelApplierKind::dataType = 0;
std::string LabelApplierRequestLabelApplierKind::dataTypeName("LabelApplierRequestLabelApplierKind"); 
#endif

LabelApplierRequestLabelApplierKind::LabelApplierRequestLabelApplierKind(Hobart::CEvolMsgReceiver *pHobThread) : LabelApplierGeneric(LaReqKind, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierRequestLabelApplierKind::LabelApplierRequestLabelApplierKind(const char *buffer) : LabelApplierGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierRequestLabelApplierKind::~LabelApplierRequestLabelApplierKind()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
