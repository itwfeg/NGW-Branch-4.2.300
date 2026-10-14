//#include "vxWorks.h"
#include "WrapperEvent.h"
#include "EvolWrapperLoad.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperLoad::dataType = 0;
std::string EvolWrapperLoad::dataTypeName("EvolWrapperLoad"); 
#endif


EvolWrapperLoad::EvolWrapperLoad(Hobart::CEvolMsgReceiver *pHobThread, WrFilmWidthSelection filmWidthSelection) : EvolMessage(Wrapper, WrEvolLoad, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->filmWidthSelection = filmWidthSelection;
}

EvolWrapperLoad::EvolWrapperLoad(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &filmWidthSelection;
	for (unsigned int i = 0; i < sizeof(filmWidthSelection); i++)
	{
		*pChar++ = buffer[j++];
	}
}

EvolWrapperLoad::~EvolWrapperLoad()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
 
const char *EvolWrapperLoad::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &filmWidthSelection;
	for (unsigned int i = 0; i < sizeof(filmWidthSelection); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void EvolWrapperLoad::show()
{
	bool isTracing = Debug::isTracing("EvolWrapperLoad");

	if (isTracing == false)
	{
		Debug::addTraceCategory("EvolWrapperLoad");
	}

	Debug::trace("EvolWrapperLoad", "EvolWrapperLoad: ");

	EvolMessage::show();

	if (filmWidthSelection == UpperRoll)
	{
		Debug::trace("EvolWrapperLoad", "filmWidthSelection: UpperRoll!");
	}
	else if (filmWidthSelection == LowerRoll)
	{
		Debug::trace("EvolWrapperLoad", "filmWidthSelection: LowerRoll!");
	}
	else if (filmWidthSelection == Automatic)
	{
		Debug::trace("EvolWrapperLoad", "filmWidthSelection: Automatic!");
	}
	else if (filmWidthSelection == NoFilmWidthSelected)
	{
		Debug::trace("EvolWrapperLoad", "filmWidthSelection: NoFilmWidthSelected!");
	}
	else
	{
		Debug::trace("EvolWrapperLoad", HobString("filmWidthSelection: Unknown ") +
									HobString(Integer::toString((UINT16) filmWidthSelection)));
	}

	if (isTracing == false)
	{
		Debug::removeTraceCategory("EvolWrapperLoad");
	}

}

WrFilmWidthSelection EvolWrapperLoad::getFilmWidthSelection() const
{
	return filmWidthSelection;
}

void EvolWrapperLoad::setFilmWidthSelection(WrFilmWidthSelection filmWidthSelection)
{
	this->filmWidthSelection = filmWidthSelection;
}
