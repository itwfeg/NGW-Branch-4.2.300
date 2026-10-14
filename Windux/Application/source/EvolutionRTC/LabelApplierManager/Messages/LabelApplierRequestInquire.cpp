//#include "vxWorks.h"
#include "LabelApplierRequestInquire.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierRequestInquire::dataType = 0;
std::string LabelApplierRequestInquire::dataTypeName("LabelApplierRequestInquire"); 
#endif

LabelApplierRequestInquire::LabelApplierRequestInquire(Hobart::CEvolMsgReceiver *pHobThread) : LabelApplierGeneric(LaReqInquire, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierRequestInquire::LabelApplierRequestInquire(const char *buffer) : LabelApplierGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierRequestInquire::~LabelApplierRequestInquire()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
