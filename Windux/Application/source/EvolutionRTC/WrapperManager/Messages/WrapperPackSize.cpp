//#include "vxWorks.h"
#include "WrapperPackSize.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperPackSize::dataType = 0;
std::string WrapperPackSize::dataTypeName("WrapperPackSize"); 
#endif

WrapperPackSize::WrapperPackSize(UINT16 minLength, UINT16 maxLength,
								 UINT16 minWidth, UINT16 maxWidth)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->minLength = minLength;
	this->maxLength = maxLength;
	this->minWidth = minWidth;
	this->maxWidth = maxWidth;
}

WrapperPackSize::WrapperPackSize(const char *buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	int j = 0;

	char *pChar = (char *) &minLength;
	for (unsigned int i = 0; i < sizeof(minLength); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &maxLength;
	for (unsigned int i = 0; i < sizeof(maxLength); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &minWidth;
	for (unsigned int i = 0; i < sizeof(minWidth); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &maxWidth;
	for (unsigned int i = 0; i < sizeof(maxWidth); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WrapperPackSize::~WrapperPackSize()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *WrapperPackSize::serialize() const
{
	serializedData = "";

	char *pChar = (char *) &minLength;
	for (unsigned int i = 0; i < sizeof(minLength); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &maxLength;
	for (unsigned int i = 0; i < sizeof(maxLength); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &minWidth;
	for (unsigned int i = 0; i < sizeof(minWidth); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &maxWidth;
	for (unsigned int i = 0; i < sizeof(maxWidth); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

UINT32 WrapperPackSize::getSerializedLength() const
{
	return (UINT32)serializedData.length();
}

void WrapperPackSize::show()
{
	Debug::addTraceCategory("WrapperPackSize");
	Debug::trace("WrapperPackSize", "WrapperPackSize: ");

	Debug::trace("WrapperPackSize", HobString("Length: ") +
									HobString(Integer::toString(minLength)) +
									HobString(", ") +
									HobString(Integer::toString(maxLength)));

	Debug::trace("WrapperPackSize", HobString("Width: ") +
									HobString(Integer::toString(minWidth)) +
									HobString(", ") +
									HobString(Integer::toString(maxWidth)));

	Debug::removeTraceCategory("WrapperPackSize");
}

UINT16 WrapperPackSize::getMinLength() const
{
	return minLength;
}

void WrapperPackSize::setMinLength(UINT16 minLength)
{
	this->minLength = minLength;
}

UINT16 WrapperPackSize::getMaxLength() const
{
	return maxLength;
}

void WrapperPackSize::setMaxLength(UINT16 maxLength)
{
	this->maxLength = maxLength;
}

UINT16 WrapperPackSize::getMinWidth() const
{
	return minWidth;
}

void WrapperPackSize::setMinWidth(UINT16 minWidth)
{
	this->minWidth = minWidth;
}


UINT16 WrapperPackSize::getMaxWidth() const
{
	return maxWidth;
}

void WrapperPackSize::setMaxWidth(UINT16 maxWidth)
{
	this->maxWidth = maxWidth;
}
