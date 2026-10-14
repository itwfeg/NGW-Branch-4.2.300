#include "EvolWrapperPackageTimingsPointer.h"
#include "EvolWrapperPackageTimings.h"

#include "Debug.h"
#include "HobString.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperPackageTimingsPointer::_dataType_ = 0;
std::string EvolWrapperPackageTimingsPointer::_dataTypeName_("EvolWrapperPackageTimingsPointer"); 
#endif


EvolWrapperPackageTimingsPointer::EvolWrapperPackageTimingsPointer(WrEvent wrEvent, EvolWrapperPackageTimings *wptPointer)
	: WrapperGeneric(wrEvent), timingsPointer(wptPointer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	if (Debug::isTracing("EvolWrapperPackageTimingsPointer"))
	{
/*must port if used -- ats	
		Debug::trace("EvolWrapperPackageTimingsPointer", "Constructor passing in a EvolWrapperPackageTimings *wptPointer:  timingsPointer = " + Hex::toString((long) timingsPointer));
*/	
	}
}

EvolWrapperPackageTimingsPointer::EvolWrapperPackageTimingsPointer(const char *buffer) : WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	WrapperGeneric::serialize();
	int j = getSerializedLength();

	char* pChar = (char*) &timingsPointer;
	for (unsigned int i = 0; i < sizeof(timingsPointer); i++)
	{
		*pChar++ = buffer[j++];
	}

	if (Debug::isTracing("EvolWrapperPackageTimingsPointer"))
	{
/*must port if used -- ats	
		Debug::trace("EvolWrapperPackageTimingsPointer", "Constructor passing in a const char* buffer:  timingsPointer = " + Hex::toString((long) timingsPointer));
*/	
	}
}

EvolWrapperPackageTimingsPointer::~EvolWrapperPackageTimingsPointer()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
	//	We will not delete the timingsPointer here. It will be up to the recipient of this class to delete it.
}


void EvolWrapperPackageTimingsPointer::setTimingsPointer(EvolWrapperPackageTimings *wptPointer)
{
	this->timingsPointer = wptPointer;
	if (Debug::isTracing("EvolWrapperPackageTimingsPointer"))
	{
/*must port if used -- ats	
		Debug::trace("EvolWrapperPackageTimingsPointer", "setTimingsPointer(" + Hex::toString((long) timingsPointer) + ")");
*/	
	}
}
 EvolWrapperPackageTimings *EvolWrapperPackageTimingsPointer::getTimingsPointer()
{
	return timingsPointer;
}


const EvolWrapperPackageTimings *EvolWrapperPackageTimingsPointer::getTimingsPointer()const
{
	return timingsPointer;
}


const char* EvolWrapperPackageTimingsPointer::serialize()const
{
	Debug::trace("EvolWrapperPackageTimingsPointer", HobString("serialize()"));

	WrapperGeneric::serialize();

	const char* pChar = (const char*) &timingsPointer;
	for ( unsigned int i = 0; i < sizeof(timingsPointer); i++ )
		serializedData.append(1, pChar[i]);

	Debug::trace("EvolWrapperPackageTimingsPointer", HobString("serialize: ") +
										HobString(Hex::toString(serializedData.data(), (int)serializedData.length())));

	return serializedData.data();
}

UINT32 EvolWrapperPackageTimingsPointer::getSerializedLength()const
{
	return (UINT32)serializedData.length();
}

