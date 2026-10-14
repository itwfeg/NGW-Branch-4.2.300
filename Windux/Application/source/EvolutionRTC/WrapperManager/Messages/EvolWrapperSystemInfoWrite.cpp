#include "EvolWrapperSystemInfoWrite.h"
#include "Integer.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperSystemInfoWrite::_dataType_ = 0;
std::string EvolWrapperSystemInfoWrite::_dataTypeName_("EvolWrapperSystemInfoWrite"); 
#endif

EvolWrapperSystemInfoWrite::EvolWrapperSystemInfoWrite(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrEvolSystemInfoWrite, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	awsSerialNumber_ = "";
	awsDateOfManufacture_ = "";
	awsDateOfInstall_ = "";
	
	modelNumber_ = EvolWrapperSystemInfo::NoModel;
	
	wrapperCycleCount_ = 0;
}


EvolWrapperSystemInfoWrite::EvolWrapperSystemInfoWrite(const char *buffer)
		: WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	WrapperGeneric::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &modelNumber_;
	for (unsigned int i = 0; i < sizeof(modelNumber_); i++)
	{
		*pChar++ = buffer[j++];
	}

	for (unsigned int i = 0; i < EvolWrapperSystemInfo::_SerialNumbSize_; i++)
	{
		if(((i == 2)  || (i == 7)) && buffer[j] != '-')
		{
			awsSerialNumber_.append(1, '-');
		}
		else
		{
			awsSerialNumber_.append(1, buffer[j++]);
		}	
	}
		
	for (unsigned int i = 0; i < EvolWrapperSystemInfo::_DateSize_; i++)
	{
		awsDateOfManufacture_.append(1, buffer[j++]);
	}
	
	for (unsigned int i = 0; i < EvolWrapperSystemInfo::_DateSize_; i++)
	{
		awsDateOfInstall_.append(1, buffer[j++]);
	}

	pChar = (char *) &wrapperCycleCount_;
	for (unsigned int i = 0; i < sizeof(wrapperCycleCount_); i++)
	{
		*pChar++ = buffer[j++];
	}
}


EvolWrapperSystemInfoWrite::~EvolWrapperSystemInfoWrite()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *EvolWrapperSystemInfoWrite::serialize()const
{
	WrapperGeneric::serialize();

	char *pChar = (char *) &modelNumber_;
	for (unsigned int i = 0; i < sizeof(modelNumber_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) awsSerialNumber_.data();
	for (unsigned int i = 0; i < EvolWrapperSystemInfo::_SerialNumbSize_; i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) awsDateOfManufacture_.data();
	for (unsigned int i = 0; i < awsDateOfManufacture_.size(); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) awsDateOfInstall_.data();
	for (unsigned int i = 0; i < awsDateOfInstall_.size(); i++)
	{
		serializedData.append(1, pChar[i]);
	}


	pChar = (char *) &wrapperCycleCount_;
	for (unsigned int i = 0; i < sizeof(wrapperCycleCount_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}
