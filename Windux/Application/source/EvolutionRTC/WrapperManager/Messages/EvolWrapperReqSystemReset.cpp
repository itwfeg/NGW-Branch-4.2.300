#include "EvolWrapperReqSystemReset.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperReqSystemReset::_dataType_ = 0;
std::string EvolWrapperReqSystemReset::_dataTypeName_("EvolWrapperReqSystemReset"); 
#endif

EvolWrapperReqSystemReset::EvolWrapperReqSystemReset(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrEvolReqSystemReset, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


EvolWrapperReqSystemReset::EvolWrapperReqSystemReset(const char *buffer)
		: WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


EvolWrapperReqSystemReset::~EvolWrapperReqSystemReset()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}
