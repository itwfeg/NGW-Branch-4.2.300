#include "WeigherCachedStatus.h"

#include "WeigherEvent.h"

#include "Debug.h"
#include "Bool.h"
#include "HobString.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WeigherCachedStatus::dataType = 0;
std::string WeigherCachedStatus::dataTypeName("WeigherCachedStatus"); 
#endif

WeigherCachedStatus::WeigherCachedStatus(Hobart::CEvolMsgReceiver *pHobThread)
		: WeigherStatus(pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	setEventType(WgCachedStatus);
	Debug::trace("WeigherManager.WeigherCachedStatus", HobString("constructor WeigherCachedStatus(HobThread *pHobThread)"));
}

WeigherCachedStatus::WeigherCachedStatus(const char* buffer)
	: WeigherStatus(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	setEventType(WgCachedStatus);
	Debug::trace("WeigherManager.WeigherCachedStatus", HobString("constructor WeigherCachedStatus(const char* buffer)"));
}

WeigherCachedStatus::~WeigherCachedStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

void WeigherCachedStatus::show()
{
	bool isTracing = Debug::isTracing("WeigherStatus");

	if ( isTracing == false )
		return;		//Debug::addTraceCategory("WeigherStatus");

	Debug::trace("WeigherStatus", "WeigherCachedStatus: ");

	EvolMessage::show();

	Debug::trace("WeigherStatus", HobString("isLargeMotion: ") +
								  HobString(Bool::toString(isLargeMotion())));
}
