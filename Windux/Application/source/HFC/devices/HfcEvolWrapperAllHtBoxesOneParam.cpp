#include "HfcEvolWrapperAllHtBoxesOneParam.h"

#include "HfcEvolWrapperPackageTimings.h"

#include "Configuration.h"
#include "WrapBox.h"

#include "util/e_string/utilstring.h"
#include "Integer.h"
#include "Long.h"
#include "Bool.h"



#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"

	int HfcEvolWrapperAllHtBoxesOneParam::_dataType_ = 0;
	std::string HfcEvolWrapperAllHtBoxesOneParam::_dataTypeName_("HfcEvolWrapperAllHtBoxesOneParam"); 
#endif


HfcEvolWrapperAllHtBoxesOneParam::HfcEvolWrapperAllHtBoxesOneParam()
	: attribute_(-1), pluException_(0)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	Debug::trace("HfcEvolWrapperAllHtBoxesOneParam", "Constructor 1");
}


HfcEvolWrapperAllHtBoxesOneParam::HfcEvolWrapperAllHtBoxesOneParam(const HfcEvolWrapperPackageTimings& timings, int tray, int param)
	: attribute_(-1), pluException_(0)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	bool traceOn = Debug::isTracing("HfcEvolWrapperAllHtBoxesOneParam");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "Constructor 2:  ";
		Debug::trace("HfcEvolWrapperAllHtBoxesOneParam", msgDebug + "Passing in HfcEvolWrapperPackageTimings& timings, int tray, int param");
	}

	HobString result;
	const WrapBox* wrapBox = timings.getWrapBox(tray);
	int numHeightBoxes = 0;

	attribute_ = param;

	if ( wrapBox )
	{
		numHeightBoxes = wrapBox->getNumberHeightBoxes();
		pluException_ = wrapBox->getPLUException();
	}

	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("For tray [").append(Integer::toString(tray));
		msgDebug2.append("], pluException = ").append(Integer::toString(pluException_));
		msgDebug2.append(", numHeightBoxes = ").append(Integer::toString(numHeightBoxes));
		msgDebug2.append(",  attribute_ = ").append(Integer::toString(attribute_));
		Debug::trace("HfcEvolWrapperAllHtBoxesOneParam", msgDebug2);
	}

	if ( wrapBox )
	{
		int i = 0;
		WrapBox::const_iterator iter = wrapBox->begin();
		while ( iter != wrapBox->end() )
		{
			precedence_.push_back(Integer::toString(i));

			Configuration::getInstance()->formatDecimalValue((iter)->getHeight(), result, 1);
			height_.push_back(result);

			if ( attribute_ == WrapBox::GRIPPER_ADJUST_PARAMETER || attribute_ == WrapBox::FUF_ADJUST_PARAMETER || attribute_ == WrapBox::SUF_ADJUST_PARAMETER )
				attributeValue_.push_back(inttostring((iter)->getParameter(attribute_)));
			else
				attributeValue_.push_back(uinttostring((iter)->getParameter(attribute_)));

			++iter;
			++i;
		}
	}
	else
		Debug::trace("HfcEvolWrapperAllHtBoxesOneParam", "Could not get wrapBox, precedence " + Integer::toString(tray));
}


HfcEvolWrapperAllHtBoxesOneParam::~HfcEvolWrapperAllHtBoxesOneParam()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


void HfcEvolWrapperAllHtBoxesOneParam::show() const
{
	if (Debug::isTracing("HfcEvolWrapperAllHtBoxesOneParam.show"))
	{
		HobString precedenceStr, heightStr, attributeStr;
		int pReturnVal, hReturnVal, aReturnVal;

		std::string debugString = "Showing:  Tray precedence, Height Box, Attribute value for Attribute ";
		debugString.append(Integer::toString(attribute_));
		debugString.append("with PluException = ").append(Integer::toString(pluException_));
		Debug::trace("HfcEvolWrapperAllHtBoxesOneParam.show", debugString);

		pReturnVal = (const_cast<HobList&> (precedence_)).first(precedenceStr);
		hReturnVal = (const_cast<HobList&> (height_)).first(heightStr);
		aReturnVal = (const_cast<HobList&> (attributeValue_)).first(attributeStr);

		while ((pReturnVal != -1) && (hReturnVal != -1) && (aReturnVal != -1))
		{
			debugString = precedenceStr;
			debugString.append("  :  ").append(heightStr).append("  :  ").append(attributeStr);
			Debug::trace("HfcEvolWrapperAllHtBoxesOneParam.show", debugString);

			pReturnVal = (const_cast<HobList&> (precedence_)).next(precedenceStr);
			hReturnVal = (const_cast<HobList&> (height_)).next(heightStr);
			aReturnVal = (const_cast<HobList&> (attributeValue_)).next(attributeStr);
		}
	}
}


const char* HfcEvolWrapperAllHtBoxesOneParam::serialize(int format) const
{
	Debug::trace("HfcEvolWrapperAllHtBoxesOneParam", "serialize()");

	serializedData.erase();
	switch ( format )
	{
		case BrowserFormat:
		{
			serializedData.append(Integer::toString(attribute_));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(Long::toString(pluException_));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(precedence_.serialize(format));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(height_.serialize(format));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(attributeValue_.serialize(format));
		}
		default:
			// todo - throw exception("invalid format");
			break;
	}
	Debug::trace("HfcEvolWrapperAllHtBoxesOneParam", "serialize():  " + serializedData);

	return serializedData.c_str();
}
