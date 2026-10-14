//#include "vxWorks.h"
#include "RendererEvent.h"
#include "RendererRenderStart.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int RendererRenderStart::dataType = 0;
std::string RendererRenderStart::dataTypeName("RendererRenderStart"); 
#endif

RendererRenderStart::RendererRenderStart(Hobart::CEvolMsgReceiver *pHobThread, INT32 ticks) : EvolMessage(Renderer, RdRenderStart, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->ticks = ticks;
}

RendererRenderStart::RendererRenderStart(const char *buffer) : EvolMessage(buffer)
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

RendererRenderStart::~RendererRenderStart()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *RendererRenderStart::serialize() const
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

void RendererRenderStart::show()
{
	EvolMessage::show();

	//cout << "Starting Tick Count: " << ticks << endl;
}

INT32 RendererRenderStart::getTicks()
{
	return ticks;
}

void RendererRenderStart::setTicks(INT32 ticks)
{
	this->ticks = ticks;
}
