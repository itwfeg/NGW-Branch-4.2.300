//#include "vxWorks.h"
#include "SystemEnable.h"

#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int SystemEnable::_dataType_ = 0;
std::string SystemEnable::_dataTypeName_("SystemEnable"); 
#endif

SystemEnable::SystemEnable(EvolDeviceType evolDevType, Hobart::CEvolMsgReceiver *pHobThread) : SystemGeneric(evolDevType, SysEnable, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

SystemEnable::SystemEnable(const char *buffer) : SystemGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

SystemEnable::~SystemEnable()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

