//#include "vxWorks.h"
#include "RendererEvent.h"
#include "RendererRenderComplete.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int RendererRenderComplete::dataType = 0;
std::string RendererRenderComplete::dataTypeName("RendererRenderComplete"); 
#endif

RendererRenderComplete::RendererRenderComplete(Hobart::CEvolMsgReceiver *pHobThread, INT32 ticks) : EvolMessage(Renderer, RdRenderComplete, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->ticks = ticks;
}

RendererRenderComplete::RendererRenderComplete(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	// index into buffer
	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &ticks;
	for (unsigned int i = 0; i < sizeof(ticks); i++)
	{
		*pChar++ = buffer[j++];
	}
}

RendererRenderComplete::~RendererRenderComplete()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *RendererRenderComplete::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &ticks;
	for (unsigned int i = 0; i < sizeof(ticks); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void RendererRenderComplete::show()
{
	EvolMessage::show();

	//cout << "Starting Tick Count: " << ticks << endl;
}

INT32 RendererRenderComplete::getTicks()
{
	return ticks;
}

void RendererRenderComplete::setTicks(INT32 ticks)
{
	this->ticks = ticks;
}
