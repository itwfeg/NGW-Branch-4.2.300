#include "EvolWrapperReqCycleCount.h"
//#include "WrapperEvent.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperReqCycleCount::_dataType_ = 0;
std::string EvolWrapperReqCycleCount::_dataTypeName_("EvolWrapperReqCycleCount"); 
#endif

EvolWrapperReqCycleCount::EvolWrapperReqCycleCount(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrEvolReqCycleCount, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


EvolWrapperReqCycleCount::EvolWrapperReqCycleCount(const char *buffer)
		: WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


EvolWrapperReqCycleCount::~EvolWrapperReqCycleCount()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}
