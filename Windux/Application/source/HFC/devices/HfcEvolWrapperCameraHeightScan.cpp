/* Access Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcEvolWrapperCameraHeightScan.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperCameraHeightScan::_dataType_ = 0;
std::string HfcEvolWrapperCameraHeightScan::_dataTypeName_("HfcEvolWrapperCameraHeightScan"); 
#endif

HfcEvolWrapperCameraHeightScan::HfcEvolWrapperCameraHeightScan(UINT16 newScan) : EvolWrapperCameraHeightScan()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	Debug::trace("HfcEvolWrapperCameraHeightScan", "Constructor(newScan)");
	setScan(newScan);
}

HfcEvolWrapperCameraHeightScan::HfcEvolWrapperCameraHeightScan(const char *data) : EvolWrapperCameraHeightScan(data)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	Debug::trace("HfcEvolWrapperCameraHeightScan", "Constructor(data) - will now call show");
	show();
	Debug::trace("HfcEvolWrapperCameraHeightScan", "end of Constructor(data)");
}

HfcEvolWrapperCameraHeightScan::~HfcEvolWrapperCameraHeightScan()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcEvolWrapperCameraHeightScan::serialize(int format)const
{
	Debug::trace("HfcEvolWrapperCameraHeightScan", "serialize()");
	HfcObject::serializedData = "";
	switch (format)
	{
		case BrowserFormat:
		{
			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));
			HfcObject::serializedData.append(BrowserFormatSeparator);

			UINT16 scan = getScan();
			HfcObject::serializedData.append((scan & Bit_050_Mask) ? "1" : "0");
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append((scan & Bit_075_Mask) ? "1" : "0");
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append((scan & Bit_100_Mask) ? "1" : "0");
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append((scan & Bit_125_Mask) ? "1" : "0");
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append((scan & Bit_150_Mask) ? "1" : "0");
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append((scan & Bit_175_Mask) ? "1" : "0");
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append((scan & Bit_200_Mask) ? "1" : "0");
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append((scan & Bit_225_Mask) ? "1" : "0");
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append((scan & Bit_250_Mask) ? "1" : "0");
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append((scan & Bit_300_Mask) ? "1" : "0");
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append((scan & Bit_350_Mask) ? "1" : "0");
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append((scan & Bit_400_Mask) ? "1" : "0");
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append((scan & Bit_450_Mask) ? "1" : "0");
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append((scan & Bit_500_Mask) ? "1" : "0");
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append((scan & Bit_600_Mask) ? "1" : "0");
			break;
		}

		default:
			Debug::trace("HfcEvolWrapperCameraHeightScan", "serialize(Invalid format!!!)");
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
