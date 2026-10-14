#include "EvolWrapperKnifeHistory.h"
#include "util/e_string/utilstring.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"

	int EvolWrapperKnifeHistory::_dataType_ = 0;
	std::string EvolWrapperKnifeHistory::_dataTypeName_("hfc::EvolWrapperKnifeHistory");
#endif

EvolWrapperKnifeHistory::EvolWrapperKnifeHistory()
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	knifeHistoryType_ = KnifeTimeNone;
	knifeTime_ = 0;
	knifeDateTime_ = "";	
}

EvolWrapperKnifeHistory::~EvolWrapperKnifeHistory()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	Debug::trace("Delete.EvolWrapperKnifeHistory", "    EvolWrapperKnifeHistory: " + util_string::ptrtostring(this) +
		"  KnifeHistoryType:" + util_string::inttostring(this->getType()));
}

short EvolWrapperKnifeHistory::getHobDateDay() const
{
	HobDateTime hobDT = getHobTimeStampDateTime();
	return hobDT.getDay();
}

short EvolWrapperKnifeHistory::getHobDateMonth() const
{
	HobDateTime hobDT = getHobTimeStampDateTime();
	return hobDT.getMonth();
}
short EvolWrapperKnifeHistory::getHobDateYear() const
{
	HobDateTime hobDT = getHobTimeStampDateTime();
	return hobDT.getYear();
}

const char *EvolWrapperKnifeHistory::serialize(int format) const
{
	HfcObject::serializedData = "";
	if(format == BrowserFormat)
	{
		HfcObject::serializedData.append(Integer::toString(knifeTime_));
		HfcObject::serializedData.append(BrowserFormatSeparator);
		HfcObject::serializedData.append(Integer::toString(getHobDateMonth()));			
		HfcObject::serializedData.append(BrowserFormatSeparator);
		HfcObject::serializedData.append(Integer::toString(getHobDateDay()));			
		HfcObject::serializedData.append(BrowserFormatSeparator);
		HfcObject::serializedData.append(Integer::toString(getHobDateYear()));			
	}
	return 	HfcObject::serializedData.c_str();
}







//}