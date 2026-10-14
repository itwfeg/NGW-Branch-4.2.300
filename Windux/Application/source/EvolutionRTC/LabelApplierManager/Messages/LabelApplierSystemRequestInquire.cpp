//#include "vxWorks.h"
#include "LabelApplierSystemRequestInquire.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierSystemRequestInquire::dataType = 0;
std::string LabelApplierSystemRequestInquire::dataTypeName("LabelApplierSystemRequestInquire"); 
#endif

LabelApplierSystemRequestInquire::LabelApplierSystemRequestInquire(Hobart::CEvolMsgReceiver *pHobThread) : SystemRequestInquire(LabelApplierSystem, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierSystemRequestInquire::LabelApplierSystemRequestInquire(const char *buffer) : SystemRequestInquire(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierSystemRequestInquire::~LabelApplierSystemRequestInquire()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

