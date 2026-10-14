#include <iostream>
using namespace std;

//#include "vxWorks.h"
#include "RendererEvent.h"
#include "RendererCloseSession.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int RendererCloseSession::dataType = 0;
std::string RendererCloseSession::dataTypeName("RendererCloseSession"); 
#endif

RendererCloseSession::RendererCloseSession(Hobart::CEvolMsgReceiver *pHobThread, UINT16 sessionId) : EvolMessage(Renderer, RdCloseSession, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->sessionId = sessionId;
}

RendererCloseSession::RendererCloseSession(const char *buffer) : EvolMessage(buffer)
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

RendererCloseSession::~RendererCloseSession()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *RendererCloseSession::serialize() const
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

void RendererCloseSession::show()
{
	EvolMessage::show();

	cout << "Renderer Close Session ID: " << sessionId << endl;
}

UINT16 RendererCloseSession::getSessionId()
{
	return sessionId;
}

void RendererCloseSession::setSessionId(UINT16 sessionId)
{
	this->sessionId = sessionId;
}
