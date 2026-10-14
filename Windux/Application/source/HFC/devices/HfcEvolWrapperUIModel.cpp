#include "HfcEvolWrapperUIModel.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperUIModel::_dataType_ = 0;
std::string HfcEvolWrapperUIModel::_dataTypeName_("HfcEvolWrapperUIModel"); 
#endif


HfcEvolWrapperUIModel::HfcEvolWrapperUIModel(const char *data) : EvolWrapperUIModel()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	Debug::trace("HfcEvolWrapperUIModel", "HfcEvolWrapperUIModel(const char *data)");

	setModelSelection(Integer::valueOf(data));
}


HfcEvolWrapperUIModel::~HfcEvolWrapperUIModel()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcEvolWrapperUIModel::serialize(int format) const
{
	switch (format)
	{
		case InternalFormat:
		{
			EvolWrapperUIModel::serialize();
			HfcObject::serializedData = EvolWrapperUIModel::serializedData;
		}
		break;
		
		default:
				Debug::trace("HfcEvolWrapperUIModel", "Invalid format");
		break;
	}
	Debug::trace("HfcEvolWrapperUIModel", "serialize():  " + 	HfcObject::serializedData);
	return HfcObject::serializedData.data();
}