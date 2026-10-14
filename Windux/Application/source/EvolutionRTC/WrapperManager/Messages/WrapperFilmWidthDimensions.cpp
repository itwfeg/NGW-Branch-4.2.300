#include "WrapperFilmWidthDimensions.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperFilmWidthDimensions::_dataType_ = 0;
std::string WrapperFilmWidthDimensions::_dataTypeName_("WrapperFilmWidthDimensions"); 
#endif

WrapperFilmWidthDimensions::WrapperFilmWidthDimensions()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	rollWidth = 0;
	rollDiameter = 0;
	remainingOnRoll = 0;
}

WrapperFilmWidthDimensions::WrapperFilmWidthDimensions(const char *buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	int j = 0;

	char *pChar = (char *) &rollWidth;
	for (unsigned int i = 0; i < sizeof(rollWidth); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &rollDiameter;
	for (unsigned int i = 0; i < sizeof(rollDiameter); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &remainingOnRoll;
	for (unsigned int i = 0; i < sizeof(remainingOnRoll); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WrapperFilmWidthDimensions::WrapperFilmWidthDimensions(const WrapperFilmWidthDimensions &wfwd)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	//Debug::trace("WrapperFilmWidthDimensions", "WrapperFilmWidthDimensions(const WrapperFilmWidthDimensions &wfwd) = " + Hex::toString((long) this));

	rollWidth = wfwd.getRollWidth();
	rollDiameter = wfwd.getRollDiameter();
	remainingOnRoll = wfwd.getRemainingOnRoll();
}

WrapperFilmWidthDimensions::~WrapperFilmWidthDimensions()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *WrapperFilmWidthDimensions::serialize() const
{
	serializedData = "";

	char *pChar = (char *) &rollWidth;
	for (unsigned int i = 0; i < sizeof(rollWidth); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &rollDiameter;
	for (unsigned int i = 0; i < sizeof(rollDiameter); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &remainingOnRoll;
	for (unsigned int i = 0; i < sizeof(remainingOnRoll); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

UINT32 WrapperFilmWidthDimensions::getSerializedLength() const
{
	return (UINT32)serializedData.length();
}

void WrapperFilmWidthDimensions::show() const
{
	if (Debug::isTracing("WrapperFilmWidthDimensions"))
	{
		Debug::trace("WrapperFilmWidthDimensions", HobString("rollWidth: ") + HobString(Integer::toString(rollWidth)));
		Debug::trace("WrapperFilmWidthDimensions", HobString("rollDiameter: ") + HobString(Integer::toString(rollDiameter)));
		Debug::trace("WrapperFilmWidthDimensions", HobString("remainingOnRoll: ") +	HobString(Integer::toString(remainingOnRoll)));
	}
}

UINT16 WrapperFilmWidthDimensions::getRollWidth() const
{
	return rollWidth;
}

void WrapperFilmWidthDimensions::setRollWidth(UINT16 width)
{
	rollWidth = width;
}

UINT16 WrapperFilmWidthDimensions::getRollDiameter() const
{
	return rollDiameter;
}

void WrapperFilmWidthDimensions::setRollDiameter(UINT16 width)
{
	rollDiameter = width;
}

UINT16 WrapperFilmWidthDimensions::getRemainingOnRoll() const
{
	return remainingOnRoll;
}

void WrapperFilmWidthDimensions::setRemainingOnRoll(UINT16 width)
{
	remainingOnRoll = width;
}

void WrapperFilmWidthDimensions::operator=(const WrapperFilmWidthDimensions &filmDim)
{
	rollWidth = filmDim.getRollWidth();
	rollDiameter = filmDim.getRollDiameter();
	remainingOnRoll = filmDim.getRemainingOnRoll();

	Debug::trace("WrapperFilmWidthDimensions", HobString("operator=(),  Show current object:"));
	show();
}
