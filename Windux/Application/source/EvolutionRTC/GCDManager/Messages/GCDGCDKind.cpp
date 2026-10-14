//#include "vxWorks.h"
#include "GCDGCDKind.h"

#include "GCDEvent.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int GCDGCDKind::dataType = 0;
std::string GCDGCDKind::dataTypeName("GCDGCDKind"); 
#endif

GCDGCDKind::GCDGCDKind(Hobart::CEvolMsgReceiver *pHobThread, GCDKind gcdKind) : EvolMessage(GCDSessionDev, GcdSessionKind, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	this->gcdKind_ = gcdKind;
}

GCDGCDKind::GCDGCDKind(const char *buffer) : EvolMessage(buffer)
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
	char *pChar = (char *) &gcdKind_;
	for (unsigned int i = 0; i < sizeof(gcdKind_); i++)
	{
		*pChar++ = buffer[j++];
	}
}

GCDGCDKind::~GCDGCDKind()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *GCDGCDKind::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &gcdKind_;
	for (unsigned int i = 0; i < sizeof(gcdKind_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void GCDGCDKind::show()
{
	bool isTracing = Debug::isTracing("GCDGCDKind");

	if (isTracing == false)
	{
		Debug::addTraceCategory("GCDGCDKind");
	}

	Debug::trace("GCDGCDKind", "GCDGCDKind: ");

	EvolMessage::show();

	if (gcdKind_ == GCDNoGCD)
	{
		Debug::trace("GCDGCDKind", HobString("gcdKind: GCDNoGCD"));
	}
	else if (gcdKind_ == GCDUsb)
	{
		Debug::trace("GCDGCDKind", HobString("gcdKind: GCDUsb"));
	}
	else if (gcdKind_ == GCDNative)
	{
		Debug::trace("GCDGCDKind", HobString("gcdKind: GCDNative"));
	}
	else
	{
		Debug::trace("GCDGCDKind", HobString("gcdKind: Unknown ") +
											 HobString(Integer::toString((UINT16) gcdKind_)));
	}

	if (isTracing == false)
	{
		Debug::removeTraceCategory("GCDGCDKind");
	}
}

void GCDGCDKind::setGCDKind(GCDKind gcdKind)
{
	this->gcdKind_ = gcdKind;
}

GCDKind GCDGCDKind::getGCDKind()
{
	return gcdKind_;
}
