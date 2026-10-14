#include "EvolWrapperSystemInfoErase.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperSystemInfoErase::_dataType_ = 0;
std::string EvolWrapperSystemInfoErase::_dataTypeName_("EvolWrapperSystemInfoErase"); 
#endif

EvolWrapperSystemInfoErase::EvolWrapperSystemInfoErase(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrEvolSystemInfoErase, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


EvolWrapperSystemInfoErase::EvolWrapperSystemInfoErase(const char *buffer)
		: WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


EvolWrapperSystemInfoErase::~EvolWrapperSystemInfoErase()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}
