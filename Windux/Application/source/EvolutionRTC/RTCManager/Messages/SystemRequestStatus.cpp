//#include "vxWorks.h"
#include "SystemRequestStatus.h"

#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int SystemRequestStatus::_dataType_ = 0;
std::string SystemRequestStatus::_dataTypeName_("SystemRequestStatus"); 
#endif

SystemRequestStatus::SystemRequestStatus(EvolDeviceType evolDevType, Hobart::CEvolMsgReceiver *pHobThread) : SystemGeneric(evolDevType, SysReqStatus, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

SystemRequestStatus::SystemRequestStatus(const char *buffer) : SystemGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

SystemRequestStatus::~SystemRequestStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

