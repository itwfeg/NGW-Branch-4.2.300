/* Access Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcEvolLabelApplierConfig.h"
#include "EvolLabelApplierConfiguration.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolLabelApplierConfig::_dataType_ = 0;
std::string HfcEvolLabelApplierConfig::_dataTypeName_("HfcEvolLabelApplierConfig"); 
#endif

HfcEvolLabelApplierConfig::HfcEvolLabelApplierConfig(bool hasASecondPrinter, LaSideDischarge sideDischarge)
	: secondPrinter(hasASecondPrinter), sideDischargeType(sideDischarge)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	Debug::trace("HfcEvolLabelApplierConfig", "Constructor()");
	show();
}

HfcEvolLabelApplierConfig::~HfcEvolLabelApplierConfig()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcEvolLabelApplierConfig::serialize(int format)const
{
	Debug::trace("HfcEvolLabelApplierConfig", "serialize()");
	HfcObject::serializedData = "";
	switch (format)
	{
		case BrowserFormat:
			HfcObject::serializedData.append(Bool::toString(secondPrinter));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString(sideDischargeType));
			break;

		default:
			Debug::trace("HfcEvolLabelApplierConfig", "serialize(Invalid format!!!)");
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}

bool HfcEvolLabelApplierConfig::hasSecondPrinter()
{
	return secondPrinter;
}

void HfcEvolLabelApplierConfig::setSecondPrinter(bool value)
{
	secondPrinter = value;
}


LaSideDischarge HfcEvolLabelApplierConfig::getSideDischargeType() const
{
	return sideDischargeType;
}

void HfcEvolLabelApplierConfig::setSideDischargeType(LaSideDischarge type)
{
	sideDischargeType = type;
}

void HfcEvolLabelApplierConfig::show()
{
	if (Debug::isTracing("HfcEvolLabelApplierConfig"))
	{
		Debug::trace("HfcEvolLabelApplierConfig", HobString("hasSecondPrinter = ") + HobString(Bool::toString(secondPrinter)));
		Debug::trace("HfcEvolLabelApplierConfig", HobString("sideDischargeType = ") + HobString(Integer::toString(sideDischargeType)));
	}
}
