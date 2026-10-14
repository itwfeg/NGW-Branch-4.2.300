#include "EvolWrapperResetDefaultPackageTimings.h"



#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperResetDefaultPackageTimings::_dataType_ = 0;
std::string EvolWrapperResetDefaultPackageTimings::_dataTypeName_("EvolWrapperResetDefaultPackageTimings"); 
#endif


EvolWrapperResetDefaultPackageTimings::EvolWrapperResetDefaultPackageTimings(Hobart::CEvolMsgReceiver *pHobThread)
		: WrapperGeneric(WrEvolResetDefaultPackageTimings, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


EvolWrapperResetDefaultPackageTimings::EvolWrapperResetDefaultPackageTimings(const char* buffer)
		: WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


EvolWrapperResetDefaultPackageTimings::~EvolWrapperResetDefaultPackageTimings()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}
