//#include "vxWorks.h"
#include "WrapperEvent.h"
#include "WrapperJog.h"

#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperJog::dataType = 0;
std::string WrapperJog::dataTypeName("WrapperJog"); 
#endif

WrapperJog::WrapperJog(Hobart::CEvolMsgReceiver *pHobThread,
					   UINT8 packageId,
					   bool pullFilm,
					   WrPackCode packCode) : WrapperCycle(pHobThread, packageId, pullFilm, packCode)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setEventType(WrJog);
}

WrapperJog::WrapperJog(const char *buffer) : WrapperCycle(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

WrapperJog::~WrapperJog()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
 
void WrapperJog::show()
{
	Debug::addTraceCategory("WrapperJog");
	Debug::trace("WrapperJog", "WrapperJog: ");

	WrapperCycle::show();

	Debug::removeTraceCategory("WrapperJog");
}
