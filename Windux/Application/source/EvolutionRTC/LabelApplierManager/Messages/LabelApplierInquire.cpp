//#include "vxWorks.h"
#include "LabelApplierInquire.h"

#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierInquire::dataType = 0;
std::string LabelApplierInquire::dataTypeName("LabelApplierInquire"); 
#endif

LabelApplierInquire::LabelApplierInquire(Hobart::CEvolMsgReceiver *pHobThread) : LabelApplierConfiguration(pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	setEventType(LaInquire);
}

LabelApplierInquire::LabelApplierInquire(const char *buffer) : LabelApplierConfiguration(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierInquire::~LabelApplierInquire()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

void LabelApplierInquire::show()
{
#if defined(LINUX_FIX)
	Debug::addTraceCategory( std::string("LabelApplierInquire") );
#else
	Debug::addTraceCategory("LabelApplierInquire");
#endif
	Debug::trace("LabelApplierInquire", "LabelApplierInquire: ");

	LabelApplierConfiguration::show();

#if defined(LINUX_FIX)
	Debug::removeTraceCategory( std::string("LabelApplierInquire") );
#else
	Debug::removeTraceCategory("LabelApplierInquire");
#endif
}
