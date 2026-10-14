#include "EvolWrapperRequestPackageTimings.h"

#include "WrapperEvent.h"
#include "HobString.h"
#include "Debug.h"
#include "Hex.h"



#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperRequestPackageTimings::_dataType_ = 0;
std::string EvolWrapperRequestPackageTimings::_dataTypeName_("EvolWrapperRequestPackageTimings"); 
#endif


EvolWrapperRequestPackageTimings::EvolWrapperRequestPackageTimings(TimingsType type, Hobart::CEvolMsgReceiver *pHobThread)
		: WrapperGeneric(WrEvolRequestPackageTimings, pHobThread), timingsType_(type)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	show();
}


EvolWrapperRequestPackageTimings::EvolWrapperRequestPackageTimings(const char* buffer)
		: WrapperGeneric(buffer), timingsType_(InvalidTimings)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	EvolMessage::serialize();
	int bufCounter = EvolMessage::getSerializedLength();


	// Now, for the derived class items

	// Read the type of timings to request
	char* pChar = (char*)&timingsType_;
	for (unsigned int i = 0; i < sizeof(timingsType_); i++)
	{
		*pChar++ = buffer[bufCounter++];
	}
}


EvolWrapperRequestPackageTimings::~EvolWrapperRequestPackageTimings()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


const char* EvolWrapperRequestPackageTimings::serialize()const
{
	Debug::trace("EvolWrapperRequestPackageTimings", HobString("serialize()"));

	WrapperGeneric::serialize();

	// Now, for the derived class items

	// Write the type of timings to request
	const char* pChar = (const char*)&timingsType_;
	for ( unsigned int i = 0; i < sizeof(timingsType_); i++ )
		serializedData.append(1, pChar[i]);


	Debug::trace("EvolWrapperRequestPackageTimings", HobString("serialize: ") +
										HobString(Hex::toString(serializedData.data(), (int) serializedData.length())));

	return serializedData.data();
}


void EvolWrapperRequestPackageTimings::show() const
{
//	Debug::addTraceCategory("EvolWrapperRequestPackageTimings");
//	Debug::trace("EvolWrapperRequestPackageTimings", "******* EvolWrapperRequestPackageTimings: ");

	HobString typeStr;
	if ( timingsType_ == CurrentTimings )
		typeStr = "CurrentTimings";
	else if ( timingsType_ == TemporaryTimings )
		typeStr = "TemporaryTimings";
	else if ( timingsType_ == DefaultTimings )
		typeStr = "DefaultTimings";
	else
		typeStr = "InvalidTimings";

	Debug::trace("EvolWrapperRequestPackageTimings", "      TimingsType = " + typeStr);
	Debug::trace("EvolWrapperRequestPackageTimings", "******************************************");

//	Debug::removeTraceCategory("EvolWrapperRequestPackageTimings");
}
