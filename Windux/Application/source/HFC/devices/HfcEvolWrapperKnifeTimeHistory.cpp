#include "HfcEvolWrapperKnifeTimeHistory.h"
#include "KnifeTimeHistory.h"
#include "HobString.h"
#include "Integer.h"
#include "Debug.h"
#include "util/e_string/utilstring.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperKnifeTimeHistory::_dataType_ = 0;
std::string HfcEvolWrapperKnifeTimeHistory::_dataTypeName_("HfcEvolWrapperKnifeTimeHistory"); 
#endif


HfcEvolWrapperKnifeTimeHistory::HfcEvolWrapperKnifeTimeHistory(DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	Debug::trace("HfcEvolWrapperKnifeTimeHistory", "HfcEvolWrapperKnifeTimeHistory()");

	pKnifeUpTimeInstall_ = KnifeTimeHistory::findKnifeTimeHistory(KnifeTimeHistory::KnifeUpTimeInstall, access);
	if ( !pKnifeUpTimeInstall_ )
		pKnifeUpTimeInstall_ = new KnifeTimeHistory();

	pKnifeUpTimeMin_ = KnifeTimeHistory::findKnifeTimeHistory(KnifeTimeHistory::KnifeUpTimeMin, access);
	if ( !pKnifeUpTimeMin_ )
		pKnifeUpTimeMin_ = new KnifeTimeHistory();

	pKnifeUpTimeMax_ = KnifeTimeHistory::findKnifeTimeHistory(KnifeTimeHistory::KnifeUpTimeMax, access);
	if ( !pKnifeUpTimeMax_ )
		pKnifeUpTimeMax_ = new KnifeTimeHistory();

	pKnifeDownTimeInstall_ = KnifeTimeHistory::findKnifeTimeHistory(KnifeTimeHistory::KnifeDownTimeInstall, access);
	if ( !pKnifeDownTimeInstall_ )
		pKnifeDownTimeInstall_ = new KnifeTimeHistory();

	pKnifeDownTimeMin_ = KnifeTimeHistory::findKnifeTimeHistory(KnifeTimeHistory::KnifeDownTimeMin, access);
	if ( !pKnifeDownTimeMin_ )
		pKnifeDownTimeMin_ = new KnifeTimeHistory();

	pKnifeDownTimeMax_ = KnifeTimeHistory::findKnifeTimeHistory(KnifeTimeHistory::KnifeDownTimeMax, access);
	if ( !pKnifeDownTimeMax_ )
		pKnifeDownTimeMax_ = new KnifeTimeHistory();
}

HfcEvolWrapperKnifeTimeHistory::~HfcEvolWrapperKnifeTimeHistory()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
	Debug::trace("HfcEvolWrapperKnifeTimeHistory", "~HfcEvolWrapperKnifeTimeHistory()");

	if ( pKnifeUpTimeInstall_ )
		delete pKnifeUpTimeInstall_;
	if ( pKnifeUpTimeMin_ )
		delete pKnifeUpTimeMin_;
	if ( pKnifeUpTimeMax_ )
		delete pKnifeUpTimeMax_;

	if ( pKnifeDownTimeInstall_ )
		delete pKnifeDownTimeInstall_;
	if ( pKnifeDownTimeMin_ )
		delete pKnifeDownTimeMin_;
	if ( pKnifeDownTimeMax_ )
		delete pKnifeDownTimeMax_;
}


const char *HfcEvolWrapperKnifeTimeHistory::serialize(int format) const
{
	Debug::trace("HfcEvolWrapperKnifeTimeHistory", "serialize()");

	HfcObject::serializedData = "";
	if(format == BrowserFormat)
	{
		HfcObject::serializedData.append(pKnifeUpTimeInstall_->serialize(format));
		HfcObject::serializedData.append(BrowserFormatSeparator);
		HfcObject::serializedData.append(pKnifeUpTimeMin_->serialize(format));
		HfcObject::serializedData.append(BrowserFormatSeparator);
		HfcObject::serializedData.append(pKnifeUpTimeMax_->serialize(format));
		HfcObject::serializedData.append(BrowserFormatSeparator);
		HfcObject::serializedData.append(pKnifeDownTimeInstall_->serialize(format));
		HfcObject::serializedData.append(BrowserFormatSeparator);
		HfcObject::serializedData.append(pKnifeDownTimeMin_->serialize(format));
		HfcObject::serializedData.append(BrowserFormatSeparator);
		HfcObject::serializedData.append(pKnifeDownTimeMax_->serialize(format));
	}
	else
		Debug::trace("HfcEvolWrapperKnifeTimeHistory", "serialize():  No serializedData! format = " + util_string::inttostring(format));
	return 	HfcObject::serializedData.c_str();
}
