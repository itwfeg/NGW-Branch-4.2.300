#include <iostream>
using namespace std;

//#include "vxWorks.h"
#include "RendererEvent.h"
#include "RendererSessionId.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int RendererSessionId::dataType = 0;
std::string RendererSessionId::dataTypeName("RendererSessionId"); 
#endif

RendererSessionId::RendererSessionId(Hobart::CEvolMsgReceiver *pHobThread, UINT16 sessionId) : EvolMessage(Renderer, RdSessionId, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->sessionId = sessionId;
}

RendererSessionId::RendererSessionId(const char *buffer) : EvolMessage(buffer)
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

RendererSessionId::~RendererSessionId()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *RendererSessionId::serialize() const
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

void RendererSessionId::show()
{
	EvolMessage::show();

	cout << "Renderer Session ID: " << sessionId << endl;
}

UINT16 RendererSessionId::getSessionId()
{
	return sessionId;
}

void RendererSessionId::setSessionId(UINT16 sessionId)
{
	this->sessionId = sessionId;
}
