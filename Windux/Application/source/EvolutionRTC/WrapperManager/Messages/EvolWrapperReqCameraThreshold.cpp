#include "EvolWrapperReqCameraThreshold.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperReqCameraThreshold::_dataType_ = 0;
std::string EvolWrapperReqCameraThreshold::_dataTypeName_("EvolWrapperReqCameraThreshold"); 
#endif

EvolWrapperReqCameraThreshold::EvolWrapperReqCameraThreshold(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrEvolRequestCameraThreshold, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


EvolWrapperReqCameraThreshold::EvolWrapperReqCameraThreshold(const char *buffer)
		: WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


EvolWrapperReqCameraThreshold::~EvolWrapperReqCameraThreshold()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}
