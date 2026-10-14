#include "EvolWrapperCamerasTracking.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperCamerasTracking::_dataType_ = 0;
std::string EvolWrapperCamerasTracking::_dataTypeName_("EvolWrapperCamerasTracking"); 
#endif

EvolWrapperCamerasTracking::EvolWrapperCamerasTracking(Hobart::CEvolMsgReceiver *pHobThread,
											   bool tracking) : WrapperGeneric(WrEvolCamerasTracking, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	camerasTracking = tracking;
}

EvolWrapperCamerasTracking::EvolWrapperCamerasTracking(const char *buffer) : WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	WrapperGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &camerasTracking;
	for (unsigned int i = 0; i < sizeof(camerasTracking); i++)
	{
		*pChar++ = buffer[j++];
	}
}

EvolWrapperCamerasTracking::~EvolWrapperCamerasTracking()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *EvolWrapperCamerasTracking::serialize()const
{
	WrapperGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &camerasTracking;
	for (unsigned int i = 0; i < sizeof(camerasTracking); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void EvolWrapperCamerasTracking::setCamerasTracking()
{
	camerasTracking = true;
}

void EvolWrapperCamerasTracking::clearCamerasTracking()
{
	camerasTracking = false;
}

bool EvolWrapperCamerasTracking::isCamerasTracking()
{
	return (camerasTracking == true);
}

bool EvolWrapperCamerasTracking::getCamerasTracking()
{
	return camerasTracking;
}
