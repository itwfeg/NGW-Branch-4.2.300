//#include "vxWorks.h"
#include "WeigherRequestWeigherKind.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WeigherRequestWeigherKind::dataType = 0;
std::string WeigherRequestWeigherKind::dataTypeName("WeigherRequestWeigherKind"); 
#endif

WeigherRequestWeigherKind::WeigherRequestWeigherKind(Hobart::CEvolMsgReceiver *pHobThread) : WeigherGeneric(WgReqKind, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WeigherRequestWeigherKind::WeigherRequestWeigherKind(const char *buffer) : WeigherGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WeigherRequestWeigherKind::~WeigherRequestWeigherKind()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
