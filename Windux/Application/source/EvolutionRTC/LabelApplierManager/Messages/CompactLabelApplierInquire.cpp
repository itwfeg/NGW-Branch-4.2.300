//#include "vxWorks.h"
#include "CompactLabelApplierInquire.h"

#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CompactLabelApplierInquire::dataType = 0;
std::string CompactLabelApplierInquire::dataTypeName("CompactLabelApplierInquire"); 
#endif

CompactLabelApplierInquire::CompactLabelApplierInquire(Hobart::CEvolMsgReceiver *pHobThread) : CompactLabelApplierConfiguration(pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	setEventType(LaInquire);
}

CompactLabelApplierInquire::CompactLabelApplierInquire(const char *buffer) : CompactLabelApplierConfiguration(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

CompactLabelApplierInquire::~CompactLabelApplierInquire()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

void CompactLabelApplierInquire::show()
{
#if defined(LINUX_FIX)
	Debug::addTraceCategory( std::string("CompactLabelApplierInquire") );
#else
	Debug::addTraceCategory("CompactLabelApplierInquire");
#endif
	Debug::trace("CompactLabelApplierInquire", "CompactLabelApplierInquire: ");

	CompactLabelApplierConfiguration::show();

#if defined(LINUX_FIX)
	Debug::removeTraceCategory( std::string("CompactLabelApplierInquire") );
#else
	Debug::removeTraceCategory("CompactLabelApplierInquire");
#endif
}
