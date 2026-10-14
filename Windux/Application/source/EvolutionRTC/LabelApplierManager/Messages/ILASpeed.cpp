//#include "vxWorks.h"
#include "LabelApplierEvent.h"
#include "ILASpeed.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ILASpeed::dataType = 0;
std::string ILASpeed::dataTypeName("ILASpeed"); 
#endif

ILASpeed::ILASpeed(Hobart::CEvolMsgReceiver *pHobThread, UINT16 ilaSpeed) : EvolMessage(LabelApplier, LaILASpeed, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	this->ilaSpeed = ilaSpeed;
}

ILASpeed::ILASpeed(const char *buffer) : EvolMessage(buffer)
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
	char *pChar = (char *) &ilaSpeed;
	for (unsigned int i = 0; i < sizeof(ilaSpeed); i++)
	{
		*pChar++ = buffer[j++];
	}
}

ILASpeed::~ILASpeed()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *ILASpeed::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &ilaSpeed;
	for (unsigned int i = 0; i < sizeof(ilaSpeed); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void ILASpeed::show()
{
#if defined(LINUX_FIX)
	Debug::addTraceCategory( std::string("ILASpeed") );
#else
	Debug::addTraceCategory("ILASpeed");
#endif
	Debug::trace("ILASpeed", "ILASpeed: ");

	EvolMessage::show();

	Debug::trace("ILASpeed", HobString("ilaSpeed: ") + HobString(Integer::toString(ilaSpeed)));

#if defined(LINUX_FIX)
	Debug::removeTraceCategory( std::string("ILASpeed") );
#else
	Debug::removeTraceCategory("ILASpeed");
#endif
}

void ILASpeed::setSpeed(UINT16 ilaSpeed)
{
	this->ilaSpeed = ilaSpeed;
}

UINT16 ILASpeed::getSpeed() const
{
	return ilaSpeed;
}
