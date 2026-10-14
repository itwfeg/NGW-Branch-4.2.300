//#include "vxWorks.h"
#include "SystemRequestWakeup.h"

#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int SystemRequestWakeup::_dataType_ = 0;
std::string SystemRequestWakeup::_dataTypeName_("SystemRequestWakeup"); 
#endif

SystemRequestWakeup::SystemRequestWakeup(EvolDeviceType evolDevType, Hobart::CEvolMsgReceiver *pHobThread) : SystemGeneric(evolDevType, SysReqWakeup, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

SystemRequestWakeup::SystemRequestWakeup(const char *buffer) : SystemGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

SystemRequestWakeup::~SystemRequestWakeup()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

