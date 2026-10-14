#include "HfcEvolWrapperSaveCycleCount.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperSaveCycleCount::_dataType_ = 0;
std::string HfcEvolWrapperSaveCycleCount::_dataTypeName_("HfcEvolWrapperSaveCycleCount"); 
#endif

HfcEvolWrapperSaveCycleCount::HfcEvolWrapperSaveCycleCount(const char *data) : EvolWrapperSaveCycleCount(data)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	Debug::trace("HfcEvolWrapperSaveCycleCount", "Constructor()");
}

HfcEvolWrapperSaveCycleCount::~HfcEvolWrapperSaveCycleCount()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}



const char *HfcEvolWrapperSaveCycleCount::serialize(int format)const
{
	HfcObject::serializedData = "";

	Debug::trace("HfcEvolWrapperSaveCycleCount", "serialize()");
	switch (format)
	{
		case BrowserFormat:
		{
				break;
		}
		case InternalFormat:
		{
			HfcObject::serializedData = EvolWrapperSaveCycleCount::serialize();
		}
		default:
			// todo - throw exception("invalid format");
			break;
	}
	Debug::trace("HfcEvolWrapperSaveCycleCount", "serialize():  " + 	HfcObject::serializedData);
	return 	HfcObject::serializedData.c_str();
}


void HfcEvolWrapperSaveCycleCount::show()
{
	if (Debug::isTracing("HfcEvolWrapperSaveCycleCount"))
	{
		Debug::trace("HfcEvolWrapperSaveCycleCount", "HfcEvolWrapperSaveCycleCount: ");
	}
}
