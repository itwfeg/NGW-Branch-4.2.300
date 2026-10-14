#include "CLAReqSystemInfo.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CLAReqSystemInfo::_dataType_ = 0;
std::string CLAReqSystemInfo::_dataTypeName_("CLAReqSystemInfo"); 
#endif

CLAReqSystemInfo::CLAReqSystemInfo(Hobart::CEvolMsgReceiver *pHobThread) : LabelApplierGeneric(LaReqSysInfo, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

CLAReqSystemInfo::CLAReqSystemInfo(const char *buffer) : LabelApplierGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

CLAReqSystemInfo::~CLAReqSystemInfo()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

