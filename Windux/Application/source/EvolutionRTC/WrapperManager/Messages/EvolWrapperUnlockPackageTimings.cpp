#include "EvolWrapperUnlockPackageTimings.h"



#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperrUnlockPackageTimings::_dataType_ = 0;
std::string EvolWrapperrUnlockPackageTimings::_dataTypeName_("EvolWrapperrUnlockPackageTimings");
#endif


EvolWrapperUnlockPackageTimings::EvolWrapperUnlockPackageTimings(Hobart::CEvolMsgReceiver *pHobThread)
		: WrapperGeneric(WrEvolUnlockPackageTimings, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


EvolWrapperUnlockPackageTimings::EvolWrapperUnlockPackageTimings(const char* buffer)
		: WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


EvolWrapperUnlockPackageTimings::~EvolWrapperUnlockPackageTimings()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}
