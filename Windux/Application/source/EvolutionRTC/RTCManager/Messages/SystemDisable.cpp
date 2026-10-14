//#include "vxWorks.h"
#include "SystemDisable.h"

#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int SystemDisable::_dataType_ = 0;
std::string SystemDisable::_dataTypeName_("SystemDisable"); 
#endif

SystemDisable::SystemDisable(EvolDeviceType evolDevType, Hobart::CEvolMsgReceiver *pHobThread) : SystemGeneric(evolDevType, SysDisable, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

SystemDisable::SystemDisable(const char *buffer) : SystemGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

SystemDisable::~SystemDisable()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

