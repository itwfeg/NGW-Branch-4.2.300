#include "EvolWrapperReqSystemInfo.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperReqSystemInfo::_dataType_ = 0;
std::string EvolWrapperReqSystemInfo::_dataTypeName_("EvolWrapperReqSystemInfo"); 
#endif

EvolWrapperReqSystemInfo::EvolWrapperReqSystemInfo(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrReqEvolSystemInfo, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


EvolWrapperReqSystemInfo::EvolWrapperReqSystemInfo(const char *buffer)
		: WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


EvolWrapperReqSystemInfo::~EvolWrapperReqSystemInfo()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}
