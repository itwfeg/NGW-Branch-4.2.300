#include <iostream>
using namespace std;

//#include "vxWorks.h"
#include "RendererEvent.h"
#include "RendererInitDotHeadWear.h"
#include "Debug.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int RendererInitDotHeadWear::dataType = 0;
std::string RendererInitDotHeadWear::dataTypeName("RendererInitDotHeadWear"); 
#endif

RendererInitDotHeadWear::RendererInitDotHeadWear(Hobart::CEvolMsgReceiver *pHobThread, UINT16 printheadSize) : EvolMessage(Renderer, RdInitDotHeadWear, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->printheadSize = printheadSize;
}

RendererInitDotHeadWear::RendererInitDotHeadWear(const char *buffer) : EvolMessage(buffer)
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
	char *pChar = (char *) &printheadSize;
	for (unsigned int i = 0; i < sizeof(printheadSize); i++)
	{
		*pChar++ = buffer[j++];
	}
}

RendererInitDotHeadWear::~RendererInitDotHeadWear()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *RendererInitDotHeadWear::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &printheadSize;
	for (unsigned int i = 0; i < sizeof(printheadSize); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void RendererInitDotHeadWear::show() const
{
	EvolMessage::show();

	Debug::trace("RendererInitDotHeadWear", "Number of dots: " + Integer::toString(printheadSize));
	
}

UINT16 RendererInitDotHeadWear::getPrintheadSize() const
{
	return printheadSize;
}

void RendererInitDotHeadWear::setPrintheadSize(const UINT16 printheadSize)
{
	this->printheadSize = printheadSize;
}


