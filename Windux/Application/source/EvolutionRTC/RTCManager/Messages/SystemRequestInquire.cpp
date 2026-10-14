//#include "vxWorks.h"
#include "SystemRequestInquire.h"

#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int SystemRequestInquire::_dataType_ = 0;
std::string SystemRequestInquire::_dataTypeName_("SystemRequestInquire"); 
#endif

SystemRequestInquire::SystemRequestInquire(EvolDeviceType evolDevType, Hobart::CEvolMsgReceiver *pHobThread) : SystemGeneric(evolDevType, SysReqInquire, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

SystemRequestInquire::SystemRequestInquire(const char *buffer) : SystemGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

SystemRequestInquire::~SystemRequestInquire()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

