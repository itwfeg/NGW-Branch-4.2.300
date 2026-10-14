/* Access Back-room Scale Project
 * COPYRIGHT (c) 2008.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcEvolWrapperCameraCalibration.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperCameraCalibration::_dataType_ = 0;
std::string HfcEvolWrapperCameraCalibration::_dataTypeName_("HfcEvolWrapperCameraCalibration"); 
#endif

HfcEvolWrapperCameraCalibration::HfcEvolWrapperCameraCalibration(const char *data) : EvolWrapperCameraCalibration(data)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	Debug::trace("HfcEvolWrapperCameraCalibration", "Constructor(data) - will now call show");
	show();
	Debug::trace("HfcEvolWrapperCameraCalibration", "end of Constructor(data)");
}

HfcEvolWrapperCameraCalibration::~HfcEvolWrapperCameraCalibration()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcEvolWrapperCameraCalibration::serialize(int format) const
{
	Debug::trace("HfcEvolWrapperCameraCalibration", "serialize()");
	HfcObject::serializedData = "";
	switch (format)
	{
		case BrowserFormat:
		{
			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString(getCamera()));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Bool::toString(getResult()));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString(getScaleFactor()));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString(getFirstPixel()));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString(getLastPixel()));
			break;
		}

		default:
			Debug::trace("HfcEvolWrapperCameraCalibration", "serialize(Invalid format!!!)");
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
