#include <iostream>
using namespace std;

//#include "vxWorks.h"
#include "RendererEvent.h"
#include "RendererSetSource.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int RendererSetSource::dataType = 0;
std::string RendererSetSource::dataTypeName("RendererSetSource"); 
#endif

RendererSetSource::RendererSetSource(Hobart::CEvolMsgReceiver *pHobThread, UINT16 sessionId, const UINT8 *pSource) : EvolMessage(Renderer, RdSetSource, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->sessionId = sessionId;
	this->pSource = pSource;
}

RendererSetSource::RendererSetSource(const char *buffer) : EvolMessage(buffer)
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

	pChar = (char *) &pSource;
	for (unsigned int i = 0; i < sizeof(pSource); i++)
	{
		*pChar++ = buffer[j++];
	}
}

RendererSetSource::~RendererSetSource()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *RendererSetSource::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &sessionId;
	for (unsigned int i = 0; i < sizeof(sessionId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &pSource;
	for (unsigned int i = 0; i < sizeof(pSource); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void RendererSetSource::show()
{
	EvolMessage::show();

	cout << "RendererSetSource" << endl;
	cout << "Session ID: " << sessionId << endl;
	cout << "Source Ptr: " << pSource << endl;
}

UINT16 RendererSetSource::getSessionId()
{
	return sessionId;
}

void RendererSetSource::setSessionId(UINT16 sessionId)
{
	this->sessionId = sessionId;
}

const UINT8 *RendererSetSource::getSourcePointer()
{
	return (const UINT8 *) pSource;
}

void RendererSetSource::setSourcePointer(const UINT8 *pSource)
{
	this->pSource = pSource;
}


