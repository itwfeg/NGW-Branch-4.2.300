/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcWrapperIntakePkgSize.h"
#include "HobTokenizerCSV.h"
#include "WrapperPackageInfo.h"
#include "HfcWrapperConfig.h"
#include "Long.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWrapperIntakePkgSize::_dataType_ = 0;
std::string HfcWrapperIntakePkgSize::_dataTypeName_("HfcWrapperIntakePkgSize"); 
#endif

HfcWrapperIntakePkgSize::HfcWrapperIntakePkgSize(HfcWrapperStatus *wrapStatus)
	: WrapperPackageInfo(), packageFits(false)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	setIntakePkgSize(wrapStatus);
}

HfcWrapperIntakePkgSize::HfcWrapperIntakePkgSize(HfcWrapperStatus *wrapStatus, HfcWrapperConfig *wrapperConfig, WrPackCode packCode)
	: WrapperPackageInfo(), packageFits(false)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	setIntakePkgSize(wrapStatus);
	if (isPresent())
	{
		WrapperPackCodeInfo packCodeInfo = wrapperConfig->getWrapperPackCodeInfo(packCode);
		if ((getWidth() >= packCodeInfo.getPackSizeMinWidth()) && (getWidth() <= packCodeInfo.getPackSizeMaxWidth()) &&
			(getLength() >= packCodeInfo.getPackSizeMinLength()) && (getLength() <= packCodeInfo.getPackSizeMaxLength()))
			packageFits = true;
	}
}

HfcWrapperIntakePkgSize::~HfcWrapperIntakePkgSize()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

void HfcWrapperIntakePkgSize::setIntakePkgSize(HfcWrapperStatus *wrapStatus)
{
	WrapperPackageInfo intakePackageInfo = wrapStatus->getPackageInfo(IntakePosition);
	if (intakePackageInfo.isPresent() && (intakePackageInfo.getLength() > 0) && (intakePackageInfo.getWidth() > 0))
	{
		long length = intakePackageInfo.getLength();
		long width = intakePackageInfo.getWidth();

		setLength((UINT16)length);
		setWidth((UINT16)width);
		setPresent();
	}
	else
		clearPresent();
}

bool HfcWrapperIntakePkgSize::thePackageFits()
{
	return packageFits;
}

const char *HfcWrapperIntakePkgSize::serialize(int format) const
{
	HfcObject::serializedData = "";
	switch (format)
	{
		case BrowserFormat:
		{
			short loopCnt = 1;
			while(loopCnt < endHfcWrapperIntakePkgSizeFieldOffsets)
			{
				if (loopCnt > 1)
					HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
					case fieldOffsetWrIntakePkgWidth:
						HfcObject::serializedData.append(Integer::toString(getWidth()));
						break;

					case fieldOffsetWrIntakePkgLength:
						HfcObject::serializedData.append(Integer::toString(getLength()));
						break;

					default:
						break;
				}
				loopCnt++;
			}
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
