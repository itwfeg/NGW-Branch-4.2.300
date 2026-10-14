//#include "vxWorks.h"
#include "RendererEvent.h"
#include "RendererGeneric.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int RendererGeneric::dataType = 0;
std::string RendererGeneric::dataTypeName("RendererGeneric"); 
#endif

RendererGeneric::RendererGeneric(RdEvent rdEvent, Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(Renderer, (UINT8)rdEvent, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

RendererGeneric::RendererGeneric(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

RendererGeneric::~RendererGeneric()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}
