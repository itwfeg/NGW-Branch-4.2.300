#include <iostream>
using namespace std;

//#include "vxWorks.h"
#include "RendererEvent.h"
#include "RendererNewDotHeadWear.h"
#include "Hex.h"
#include "Debug.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int RendererNewDotHeadWear::dataType = 0;
std::string RendererNewDotHeadWear::dataTypeName("RendererNewDotHeadWear"); 
#endif

RendererNewDotHeadWear::RendererNewDotHeadWear(Hobart::CEvolMsgReceiver *pHobThread, const std::vector<UINT8> pSource) : EvolMessage(Renderer, RdNewDotHeadWear, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->pSource = pSource;
}

RendererNewDotHeadWear::RendererNewDotHeadWear(Hobart::CEvolMsgReceiver *pHobThread, const UINT8 * pSource, int size) : EvolMessage(Renderer, RdNewDotHeadWear, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->pSource.assign(pSource,pSource + size);
}

RendererNewDotHeadWear::RendererNewDotHeadWear(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	// index into buffer
	EvolMessage::serialize();

	int j = getSerializedLength();

    //Nex should be size of vector
    int size = 0;
    char * pChar = (char *) & size;
	for (unsigned int i = 0; i < sizeof(size); i++)
	{
		* pChar++ = buffer[j++];
	}

	//get vector
    for( int i = 0; i < size; i++ ) {
		pSource.push_back( buffer[j++] );
	}
}

RendererNewDotHeadWear::~RendererNewDotHeadWear()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *RendererNewDotHeadWear::serialize() const
{
	EvolMessage::serialize();

    //Length of vector is first so that i can deserialize 

    const int size = (int) pSource.size();
    char *pChar = (char *) & size;
	for (unsigned int i = 0; i < sizeof(size); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	// Now, for the derived class items
    int i = 0;
	for(auto it = pSource.begin() ; it != pSource.end(); ++it) {
        i++;
        serializedData.append(1, *it);
	}
    Debug::trace("RendererNewDotHeadWear",Integer::toString(i) + " Bytes appended");
    Debug::trace("RendererNewDotHeadWear",Integer::toString(serializedData.length()) + " Searilized data length");
	return serializedData.data();
}

void RendererNewDotHeadWear::show()
{
	EvolMessage::show();

	for(std::vector<UINT8>::iterator it = pSource.begin() ; it != pSource.end(); ++it) {
		Debug::trace("RendererNewDotHeadWear", " Status: " + Hex::toString(*it));
	}
}



