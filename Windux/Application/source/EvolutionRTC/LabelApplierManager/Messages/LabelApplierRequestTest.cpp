//#include "vxWorks.h"
#include "LabelApplierRequestTest.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierRequestTest::dataType = 0;
std::string LabelApplierRequestTest::dataTypeName("LabelApplierRequestTest"); 
#endif

LabelApplierRequestTest::LabelApplierRequestTest(Hobart::CEvolMsgReceiver *pHobThread) : LabelApplierGeneric(LaReqTest, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierRequestTest::LabelApplierRequestTest(const char *buffer) : LabelApplierGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierRequestTest::~LabelApplierRequestTest()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

