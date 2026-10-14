//#include "vxWorks.h"
#include "RendererEvent.h"
#include "RendererRequestSession.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int RendererRequestSession::dataType = 0;
std::string RendererRequestSession::dataTypeName("RendererRequestSession"); 
#endif

RendererRequestSession::RendererRequestSession(Hobart::CEvolMsgReceiver *pHobThread) :
	RendererGeneric(RdReqSession, pHobThread),
	printerType_(-1)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

RendererRequestSession::RendererRequestSession(const char *buffer) :
	RendererGeneric(buffer),
	printerType_(-1)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();

	int j = getSerializedLength();

    char * pChar = (char *) & printerType_;
	for (unsigned int i = 0; i < sizeof(printerType_); i++)
	{
		* pChar++ = buffer[j++];
	}
}

RendererRequestSession::~RendererRequestSession()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *RendererRequestSession::serialize() const
{
	EvolMessage::serialize();

    char *pChar = (char *) & printerType_;
	for (unsigned int i = 0; i < sizeof(printerType_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

