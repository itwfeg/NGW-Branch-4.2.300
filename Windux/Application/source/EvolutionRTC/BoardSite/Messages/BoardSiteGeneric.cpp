#include "BoardSiteGeneric.h"

BoardSiteGeneric::BoardSiteGeneric( BoardSiteEvent bsEvent, Hobart::CEvolMsgReceiver *pHobThread ) : EvolMessage( BoardSite, (UINT8)bsEvent, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

BoardSiteGeneric::BoardSiteGeneric( const char *buffer ) : EvolMessage( buffer )
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

BoardSiteGeneric::~BoardSiteGeneric()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
