#include "HfcEvolWrapperReqUIModel.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperReqUIModel::_dataType_ = 0;
std::string HfcEvolWrapperReqUIModel::_dataTypeName_("HfcEvolWrapperReqUIModel"); 
#endif

HfcEvolWrapperReqUIModel::HfcEvolWrapperReqUIModel(const char *data) : EvolWrapperReqUIModel(data)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	Debug::trace("HfcEvolWrapperReqUIModel", "HfcEvolWrapperReqUIModel(const char *data)");
}


HfcEvolWrapperReqUIModel::~HfcEvolWrapperReqUIModel()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcEvolWrapperReqUIModel::serialize(int format) const
{
	switch (format)
	{

		case BrowserFormat:
		{
			Debug::trace("HfcEvolWrapperReqUIModel", "serialize(BrowserFormat)");
			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));
		}
		break;
		
		default:
				Debug::trace("HfcEvolWrapperReqUIModel", "Invalid format");
		break;

	}

	return HfcObject::serializedData.data();
}