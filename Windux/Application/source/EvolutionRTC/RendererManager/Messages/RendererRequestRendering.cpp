#include <iostream>
using namespace std;

//#include "vxWorks.h"
#include "RendererEvent.h"
#include "RendererRequestRendering.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int RendererRequestRendering::dataType = 0;
std::string RendererRequestRendering::dataTypeName("RendererRequestRendering"); 
#endif

RendererRequestRendering::RendererRequestRendering(Hobart::CEvolMsgReceiver *pHobThread, UINT16 sessionId) : EvolMessage(Renderer, RdReqRendering, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->sessionId = sessionId;
}

RendererRequestRendering::RendererRequestRendering(const char *buffer) : EvolMessage(buffer)
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
	char *pChar = (char *) &sessionId;
	for (unsigned int i = 0; i < sizeof(sessionId); i++)
	{
		*pChar++ = buffer[j++];
	}
}

RendererRequestRendering::~RendererRequestRendering()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *RendererRequestRendering::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &sessionId;
	for (unsigned int i = 0; i < sizeof(sessionId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void RendererRequestRendering::show()
{
	EvolMessage::show();

	cout << "Renderer Session ID: " << sessionId << endl;
}

UINT16 RendererRequestRendering::getSessionId()
{
	return sessionId;
}

void RendererRequestRendering::setSessionId(UINT16 sessionId)
{
	this->sessionId = sessionId;
}
