#include "EvolWrapperReqUIModel.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperReqUIModel::dataType = 0;
std::string EvolWrapperReqUIModel::dataTypeName("EvolWrapperReqUIModel"); 
#endif

EvolWrapperReqUIModel::EvolWrapperReqUIModel(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrReqUIWrapperModel, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

EvolWrapperReqUIModel::EvolWrapperReqUIModel(const char *buffer) : WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	WrapperGeneric::serialize();
}

const char *EvolWrapperReqUIModel::serialize() const
{
	WrapperGeneric::serialize();
	
	return serializedData.data();
}

EvolWrapperReqUIModel::~EvolWrapperReqUIModel()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}