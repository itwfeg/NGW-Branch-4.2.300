#include "EvolWrapperMotorProfile.h"
#include "HobString.h"
#include "Debug.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperMotorProfile::_dataType_ = 0;
std::string EvolWrapperMotorProfile::_dataTypeName_("EvolWrapperMotorProfile"); 
#endif

EvolWrapperMotorProfile::EvolWrapperMotorProfile(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrEvolMotorProfile, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


EvolWrapperMotorProfile::EvolWrapperMotorProfile(const char *buffer) : WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	WrapperGeneric::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &bldcMechanism_;
	for (unsigned int i = 0; i < sizeof(bldcMechanism_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &bldcMotorStatus_;
	for (unsigned int i = 0; i < sizeof(bldcMotorStatus_); i++)
	{
		*pChar++ = buffer[j++];
	}
	
	pChar = (char *) &bldcMotorCapacityUsage_;
	for (unsigned int i = 0; i < sizeof(bldcMotorCapacityUsage_); i++)
	{
		*pChar++ = buffer[j++];
	}

}

EvolWrapperMotorProfile::~EvolWrapperMotorProfile()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


const char *EvolWrapperMotorProfile::serialize()const
{	
	WrapperGeneric::serialize();

	char *pChar = (char *) &bldcMechanism_;
	for (unsigned int i = 0; i < sizeof(bldcMechanism_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	
	pChar = (char *) &bldcMotorStatus_;
	for (unsigned int i = 0; i < sizeof(bldcMotorStatus_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	
	pChar = (char *) &bldcMotorCapacityUsage_;
	for (unsigned int i = 0; i < sizeof(bldcMotorCapacityUsage_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}


