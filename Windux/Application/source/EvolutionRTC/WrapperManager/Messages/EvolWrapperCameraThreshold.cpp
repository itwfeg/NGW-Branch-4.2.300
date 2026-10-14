#include "EvolWrapperCameraThreshold.h"
#include "HobString.h"
#include "Debug.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperCameraThreshold::_dataType_ = 0;
std::string EvolWrapperCameraThreshold::_dataTypeName_("EvolWrapperCameraThreshold"); 
#endif

EvolWrapperCameraThreshold::EvolWrapperCameraThreshold(Hobart::CEvolMsgReceiver *pHobThread, UCHAR threshold) : WrapperGeneric(WrEvolCameraThreshold, pHobThread)
{
	threshold_	= threshold;
}


EvolWrapperCameraThreshold::EvolWrapperCameraThreshold(const char *buffer) : WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	WrapperGeneric::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &threshold_;
	for (unsigned int i = 0; i < sizeof(threshold_); i++)
	{
		*pChar++ = buffer[j++];
	}
}

EvolWrapperCameraThreshold::~EvolWrapperCameraThreshold()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


const char *EvolWrapperCameraThreshold::serialize()const
{	
	WrapperGeneric::serialize();

	char *pChar = (char *) &threshold_;
	for (unsigned int i = 0; i < sizeof(threshold_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void EvolWrapperCameraThreshold::show()
{	

}

