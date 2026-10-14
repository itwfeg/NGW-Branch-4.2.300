#include <iostream>
using namespace std;

//#include "vxWorks.h"
#include "RendererEvent.h"
#include "RendererSetDestination.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int RendererSetDestination::dataType = 0;
std::string RendererSetDestination::dataTypeName("RendererSetDestination"); 
#endif

RendererSetDestination::RendererSetDestination(Hobart::CEvolMsgReceiver *pHobThread, UINT16 sessionId, const UINT8 *pDestination) : EvolMessage(Renderer, RdSetDest, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->sessionId = sessionId;
	this->pDestination = pDestination;
}

RendererSetDestination::RendererSetDestination(const char *buffer) : EvolMessage(buffer)
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

	pChar = (char *) &pDestination;
	for (unsigned int i = 0; i < sizeof(pDestination); i++)
	{
		*pChar++ = buffer[j++];
	}
}

RendererSetDestination::~RendererSetDestination()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *RendererSetDestination::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &sessionId;
	for (unsigned int i = 0; i < sizeof(sessionId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &pDestination;
	for (unsigned int i = 0; i < sizeof(pDestination); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void RendererSetDestination::show()
{
	EvolMessage::show();

	cout << "RendererSetDestination" << endl;
	cout << "Session ID: " << sessionId << endl;
	cout << "Destination Ptr: " << pDestination << endl;
}

UINT16 RendererSetDestination::getSessionId()
{
	return sessionId;
}

void RendererSetDestination::setSessionId(UINT16 sessionId)
{
	this->sessionId = sessionId;
}

const UINT8 *RendererSetDestination::getDestinationPointer()
{
	return (const UINT8 *) pDestination;
}

void RendererSetDestination::setDestinationPointer(const UINT8 *pDestination)
{
	this->pDestination = pDestination;
}


