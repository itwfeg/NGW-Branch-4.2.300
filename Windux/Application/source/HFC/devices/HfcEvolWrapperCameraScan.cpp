/* Access Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcEvolWrapperCameraScan.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperCameraScan::_dataType_ = 0;
std::string HfcEvolWrapperCameraScan::_dataTypeName_("EvolWrapperCameraHeightScan"); 
#endif

HfcEvolWrapperCameraScan::HfcEvolWrapperCameraScan(const char *data) : EvolWrapperCameraScan(data)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	Debug::trace("HfcEvolWrapperCameraScan", "Constructor(data) - will now call show");
	show();
	Debug::trace("HfcEvolWrapperCameraScan", "end of Constructor(data)");
}

HfcEvolWrapperCameraScan::~HfcEvolWrapperCameraScan()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcEvolWrapperCameraScan::serialize(int format) const
{
	Debug::trace("HfcEvolWrapperCameraScan", "serialize()");
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

			HobString image = "";
			for (UINT16 i=0; i<EvolWrapperCameraScan::_CameraMaxImageSize_; i++)
			{
				if (i > 0)
					image.append(BrowserFormatSeparator);
				image.append(Integer::toString(cameraImage[i]));
			}
			if (Debug::isTracing("HfcEvolWrapperCameraScan"))
				Debug::trace("HfcEvolWrapperCameraScan", HobString("serialized camera image: ") + image);
			image.quoteForCSV();
			HfcObject::serializedData.append(image);
			break;
		}

		default:
			Debug::trace("HfcEvolWrapperCameraScan", "serialize(Invalid format!!!)");
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
