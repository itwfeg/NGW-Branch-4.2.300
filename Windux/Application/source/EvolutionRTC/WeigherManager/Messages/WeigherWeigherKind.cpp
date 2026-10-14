//#include "vxWorks.h"
#include "WeigherWeigherKind.h"

#include "WeigherEvent.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WeigherWeigherKind::dataType = 0;
std::string WeigherWeigherKind::dataTypeName("WeigherWeigherKind"); 
#endif

WeigherWeigherKind::WeigherWeigherKind(Hobart::CEvolMsgReceiver *pHobThread, WeigherKind weigherKind) : EvolMessage(Weigher, WgKind, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->weigherKind = weigherKind;
}

WeigherWeigherKind::WeigherWeigherKind(const char *buffer) : EvolMessage(buffer)
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
	char *pChar = (char *) &weigherKind;
	for (unsigned int i = 0; i < sizeof(weigherKind); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WeigherWeigherKind::~WeigherWeigherKind()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *WeigherWeigherKind::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &weigherKind;
	for (unsigned int i = 0; i < sizeof(weigherKind); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void WeigherWeigherKind::show()
{
	bool isTracing = Debug::isTracing("WeigherWeigherKind");

	if (isTracing == false)
	{
		Debug::addTraceCategory("WeigherWeigherKind");
	}

	Debug::trace("WeigherWeigherKind", "WeigherWeigherKind: ");

	EvolMessage::show();

	if (weigherKind == WgNoWeigher)
	{
		Debug::trace("WeigherWeigherKind", HobString("weigherKind: WgNoWeigher"));
	}
	else if (weigherKind == WgUltima)
	{
		Debug::trace("WeigherWeigherKind", HobString("weigherKind: WgUltima"));
	}
	else if (weigherKind == WgUsbEvol) // Ez-USB Weighing Device
	{
		Debug::trace("WeigherWeigherKind", HobString("weigherKind: WgUsbEvol"));
	}
	else if (weigherKind == WgUsbAWS) // AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
	{
		Debug::trace("WeigherWeigherKind", HobString("weigherKind: WgUsbAWS"));
	}
	else if (weigherKind == WgUsbApc)
	{
		Debug::trace("WeigherWeigherKind", HobString("weigherKind: WgUsbApc"));
	}
	else if (weigherKind == WgCanKpc)
	{
		Debug::trace("WeigherWeigherKind", HobString("weigherKind: WgCanKpc"));
	}
	else
	{
		Debug::trace("WeigherWeigherKind", HobString("weigherKind: Unknown ") +
											 HobString(Integer::toString((UINT16) weigherKind)));
	}

	if (isTracing == false)
	{
		Debug::removeTraceCategory("WeigherWeigherKind");
	}
}

void WeigherWeigherKind::setWeigherKind(WeigherKind weigherKind)
{
	this->weigherKind = weigherKind;
}

WeigherKind WeigherWeigherKind::getWeigherKind()
{
	return weigherKind;
}
