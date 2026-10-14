//#include "vxWorks.h"
#include "WrapperEvent.h"
#include "WrapperLoad.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperLoad::dataType = 0;
std::string WrapperLoad::dataTypeName("WrapperLoad"); 
#endif

WrapperLoad::WrapperLoad(Hobart::CEvolMsgReceiver *pHobThread, WrPackWidth filmWidth) : EvolMessage(Wrapper, WrLoad, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	this->filmWidth = filmWidth;
}

WrapperLoad::WrapperLoad(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &filmWidth;
	for (unsigned int i = 0; i < sizeof(filmWidth); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WrapperLoad::~WrapperLoad()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
 
const char *WrapperLoad::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &filmWidth;
	for (unsigned int i = 0; i < sizeof(filmWidth); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void WrapperLoad::show()
{
	Debug::addTraceCategory("WrapperLoad");
	Debug::trace("WrapperLoad", "WrapperLoad: ");

	EvolMessage::show();

	if (filmWidth == NarrowPackage)
	{
		Debug::trace("WrapperLoad", "filmWidth: NarrowPackage!");
	}
	else if (filmWidth == WidePackage)
	{
		Debug::trace("WrapperLoad", "filmWidth: WidePackage!");
	}
	else if (filmWidth == FilmWidthIdle)
	{
		Debug::trace("WrapperLoad", "filmWidth: FilmWidthIdle!");
	}
	else if (filmWidth == AutoWidthSelect)
	{
		Debug::trace("WrapperLoad", "filmWidth: AutoWidthSelect!");
	}
	else
	{
		Debug::trace("WrapperLoad", HobString("filmWidth: Unknown ") +
									HobString(Integer::toString((UINT16) filmWidth)));
	}

	Debug::removeTraceCategory("WrapperLoad");
}

WrPackWidth WrapperLoad::getWrPackWidth() const
{
	return filmWidth;
}

void WrapperLoad::setWrPackWidth(WrPackWidth filmWidth)
{
	this->filmWidth = filmWidth;
}
