#include "HfcEvolWrapperPackageTimingsPointer.h"
#include "HfcEvolWrapperPackageTimings.h"

#include "util/e_string/utilstring.h"
#include "Hex.h"



#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperPackageTimingsPointer::_dataType_ = 0;
std::string HfcEvolWrapperPackageTimingsPointer::_dataTypeName_("HfcEvolWrapperPackageTimingsPointer"); 
#endif


HfcEvolWrapperPackageTimingsPointer::HfcEvolWrapperPackageTimingsPointer(const char* buffer)
	: EvolWrapperPackageTimingsPointer(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	if (Debug::isTracing("HfcEvolWrapperPackageTimingsPointer"))
	{
		Debug::trace("HfcEvolWrapperPackageTimingsPointer", "Constructor passing in a const char* buffer:  timingsPointer = " + HobString(Hex::toString((long long) getTimingsPointer())));
	}
}


HfcEvolWrapperPackageTimingsPointer::HfcEvolWrapperPackageTimingsPointer(WrEvent wrEvent, EvolWrapperPackageTimings *ewpt)
	: EvolWrapperPackageTimingsPointer(wrEvent, ewpt)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	if (Debug::isTracing("HfcEvolWrapperPackageTimingsPointer"))
	{
		HobString msgDebug;
		msgDebug.append("Constructor passing in a EvolWrapperPackageTimings *ewpt = ").append(Hex::toString((long long) ewpt));
		msgDebug.append(",  timingsPointer = ").append(Hex::toString((long long) getTimingsPointer()));
		Debug::trace("HfcEvolWrapperPackageTimingsPointer", msgDebug);
	}
}


HfcEvolWrapperPackageTimingsPointer::~HfcEvolWrapperPackageTimingsPointer()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


const char *HfcEvolWrapperPackageTimingsPointer::serialize(int format)const
{
	Debug::trace("HfcEvolWrapperPackageTimingsPointer", "serialize()");
	HfcObject::serializedData = "";
	switch (format)
	{
		case InternalFormat:
		{
			Debug::trace("HfcEvolWrapperPackageTimingsPointer", "serialize(InternalFormat)");
			EvolWrapperPackageTimingsPointer::serialize();
			EvolWrapperPackageTimingsPointer::getSerializedLength();
			HfcObject::serializedData = EvolWrapperPackageTimingsPointer::serializedData;
		}
		break;

		case EvolutionTaggedFormat:
		{
			Debug::trace("HfcEvolWrapperPackageTimingsPointer", "serialize(EvolutionTaggedFormat)");
			
			HfcEvolWrapperPackageTimings *wrPkgTimings = new HfcEvolWrapperPackageTimings((EvolWrapperPackageTimings *)getTimingsPointer());
			if (wrPkgTimings)
			{
				const char* serializedBase = wrPkgTimings->serialize(EvolutionTaggedFormat);
				HfcObject::serializedData = serializedBase;
				delete wrPkgTimings;
			}
			break;
		}

		default:
			Debug::trace("HfcEvolWrapperPackageTimingsPointer", "serialize(Invalid format!!!)");
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
