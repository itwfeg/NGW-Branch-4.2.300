#include "EvolWrapperErrorHistory.h"
#include "HobString.h"
#include "HobDateTime.h"
#include "Debug.h"
#include "Integer.h"
#include "Bool.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperErrorHistory::_dataType_ = 0;
std::string EvolWrapperErrorHistory::_dataTypeName_("EvolWrapperErrorHistory"); 
#endif

EvolWrapperErrorHistory::EvolWrapperErrorHistory(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrEvolErrorHistory, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	wrapperError_	= 0;

	packageLength_	= 0;
	packageWidth_	= 0;	
	packageHeight_	= 0;

	filmUsed_ = NoFilmWidthSelected;

	usedWrapBoxBestFit_ = false;
	modifiedNetStretch_ = false;

	trayName_ = "";	
	
	HobDateTime dt;
	//set the time stamp for this error
	dateTime_ = dt.convertToDBFormat();	
}


EvolWrapperErrorHistory::EvolWrapperErrorHistory(const char *buffer) : WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	char name[_MaxNameLength_ + 1];

	WrapperGeneric::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &wrapperError_;
	for (unsigned int i = 0; i < sizeof(wrapperError_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &packageLength_;
	for (unsigned int i = 0; i < sizeof(packageLength_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &packageWidth_;
	for (unsigned int i = 0; i < sizeof(packageWidth_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &packageHeight_;
	for (unsigned int i = 0; i < sizeof(packageHeight_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &filmUsed_;
	for (unsigned int i = 0; i < sizeof(filmUsed_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &usedWrapBoxBestFit_;
	for (unsigned int i = 0; i < sizeof(usedWrapBoxBestFit_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &modifiedNetStretch_;
	for (unsigned int i = 0; i < sizeof(modifiedNetStretch_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = name;
	for (unsigned int i = 0; i < _MaxNameLength_ + 1; i++)
	{
		*pChar++ = buffer[j++];
	}
	trayName_ = name;	

	HobDateTime dt;
	std::string tempStr = dt.convertToDBFormat();
	
	for (unsigned int i = 0; i < tempStr.size(); i++)
	{
		dateTime_.append(1, buffer[j++]);
	}

}

EvolWrapperErrorHistory::~EvolWrapperErrorHistory()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


const char *EvolWrapperErrorHistory::serialize()const
{
	char boxName[_MaxNameLength_ + 1];
	
	WrapperGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &wrapperError_;
	for (unsigned int i = 0; i < sizeof(wrapperError_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &packageLength_;
	for (unsigned int i = 0; i < sizeof(packageLength_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &packageWidth_;
	for (unsigned int i = 0; i < sizeof(packageWidth_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &packageHeight_;
	for (unsigned int i = 0; i < sizeof(packageHeight_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &filmUsed_;
	for (unsigned int i = 0; i < sizeof(filmUsed_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &usedWrapBoxBestFit_;
	for (unsigned int i = 0; i < sizeof(usedWrapBoxBestFit_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &modifiedNetStretch_;
	for (unsigned int i = 0; i < sizeof(modifiedNetStretch_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	strcpy(boxName, trayName_.c_str());
	pChar = boxName;
	for (unsigned int i = 0; i < _MaxNameLength_ + 1; i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *)dateTime_.data();
	for (unsigned int i = 0; i < dateTime_.size(); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void EvolWrapperErrorHistory::show()
{	
	Debug::trace("EvolWrapperErrorHistory", "EvolWrapperErrorHistory: ");
	Debug::trace("EvolWrapperErrorHistory", HobString("wrapperError_: ") + HobString(Integer::toString(wrapperError_)));
	Debug::trace("EvolWrapperErrorHistory", HobString("packageLength_: ") + HobString(Integer::toString(packageLength_)));
	Debug::trace("EvolWrapperErrorHistory", HobString("packageWidth_: ") + HobString(Integer::toString(packageWidth_)));
	Debug::trace("EvolWrapperErrorHistory", HobString("packageHeight_: ") + HobString(Integer::toString(packageHeight_)));
	Debug::trace("EvolWrapperErrorHistory", HobString("filmWidthUsedToWrap: ") + HobString(Integer::toString(filmUsed_)));
	Debug::trace("EvolWrapperErrorHistory", HobString("usedWrapBoxBestFit_: ") + HobString(Bool::toString(usedWrapBoxBestFit_)));
	Debug::trace("EvolWrapperErrorHistory", HobString("modifiedNetStretch_: ") + HobString(Bool::toString(modifiedNetStretch_)));
	Debug::trace("EvolWrapperErrorHistory", HobString("trayName_: ") + trayName_);

}

