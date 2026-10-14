//#include "vxWorks.h"

#include "LeapStatusWrite.h"

#include "Debug.h"
#include "HobString.h"
#include "NetworkEvent.h"
#include "Integer.h"

LeapStatusWrite::LeapStatusWrite(UINT16 code, Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(NetworkDev, cfgLEAPStatusWrite, pHobThread), code_(code)
{
}


LeapStatusWrite::~LeapStatusWrite()
{
}

const char *LeapStatusWrite::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items

	const char *pChar = (char *) & code_;
	for (unsigned int i = 0; i < sizeof(code_); i++)
	{
		serializedData.append(1, pChar[i]);
	}


	return serializedData.data();
}

void LeapStatusWrite::show()
{
	bool isTracing = Debug::isTracing("LeapStatusWrite");

	if (isTracing == false)
	{
    #if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("LeapStatusWrite") );
    #else
		Debug::addTraceCategory("LeapStatusWrite");
    #endif
	}
	
	Debug::trace("LeapStatusWrite", "LeapStatusWrite");

	EvolMessage::show();

#if defined(LINUX_FIX)
	Debug::trace( std::string("LeapStatusWrite"), HobString("code: ") + HobString(Integer::toString((UINT32) code_)));
#else
	Debug::trace("LeapStatusWrite", HobString("code: ") + HobString(Integer::toString((UINT32) code_)));
#endif

	if (isTracing == false)
	{
    #if defined(LINUX_FIX)
		Debug::removeTraceCategory( std::string("LeapStatusWrite") );
    #else
		Debug::removeTraceCategory("LeapStatusWrite");
    #endif
	}
}


