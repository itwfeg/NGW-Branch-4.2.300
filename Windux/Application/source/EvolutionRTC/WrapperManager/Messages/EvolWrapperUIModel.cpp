#include "EvolWrapperUIModel.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperUIModel::_dataType_ = 0;
std::string EvolWrapperUIModel::_dataTypeName_("EvolWrapperUIModel"); 
#endif

EvolWrapperUIModel::EvolWrapperUIModel(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrUIWrapperModel, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

EvolWrapperUIModel::EvolWrapperUIModel(const char *buffer) : WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	WrapperGeneric::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &selection_;
	for (unsigned int i = 0; i < sizeof(selection_); i++)
	{
		*pChar++ = buffer[j++];
	}
}

const char *EvolWrapperUIModel::serialize() const
{
	WrapperGeneric::serialize();

	char *pChar = (char *) &selection_;
	for (unsigned int i = 0; i < sizeof(selection_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

EvolWrapperUIModel::~EvolWrapperUIModel()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}