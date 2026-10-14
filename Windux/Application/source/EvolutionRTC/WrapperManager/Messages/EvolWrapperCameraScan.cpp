#include "EvolWrapperCameraScan.h"

#include "HobString.h"
#include "Debug.h"
#include "Long.h"
#include "Integer.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperCameraScan::_dataType_ = 0;
std::string EvolWrapperCameraScan::_dataTypeName_("EvolWrapperCameraScan"); 
#endif

//	Return to once they iron out the details of the camera messages.

EvolWrapperCameraScan::EvolWrapperCameraScan(Hobart::CEvolMsgReceiver *pHobThread)
	: WrapperGeneric(WrCameraScan, pHobThread), camera(RightCamera), imageType(AnalogImage)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	for ( unsigned int i( _CameraMaxImageSize_ ); i--; )
		cameraImage[i] = 0;
}

EvolWrapperCameraScan::EvolWrapperCameraScan(const char *buffer)
	: WrapperGeneric(buffer), camera(RightCamera), imageType(AnalogImage)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	WrapperGeneric::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &camera;
	for (unsigned int i = 0; i < sizeof(camera); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &imageType;
	for (unsigned int i = 0; i < sizeof(imageType); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &cameraImage;
	for (unsigned int i = 0; i < sizeof(cameraImage); i++)
	{
		*pChar++ = buffer[j++];
	}
}

EvolWrapperCameraScan::~EvolWrapperCameraScan()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *EvolWrapperCameraScan::serialize() const
{
	WrapperGeneric::serialize();

	Debug::trace("EvolWrapperCameraScan", "New EvolWrapperCameraScan serialize");

	char *pChar = (char *) &camera;
	for (unsigned int i = 0; i < sizeof(camera); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &imageType;
	for (unsigned int i = 0; i < sizeof(imageType); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &cameraImage;
	for (unsigned int i = 0; i < sizeof(cameraImage); i++)
	{
		serializedData.append(1, pChar[i]);
	}
//	Debug::trace("EvolWrapperCameraScan", "sizeof(cameraImage) = " + Integer::toString(sizeof(cameraImage)));

	return serializedData.data();
}

void EvolWrapperCameraScan::show()
{
	if (Debug::isTracing("EvolWrapperCameraScan"))
	{
		HobString headers("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%");

//		Debug::trace("EvolWrapperCameraScan", headers + "  START");
		HobString msgDebug("camera = ");
		if (camera == RightCamera)
			msgDebug.append("RightCamera");
		else if (camera == LeftCamera)
			msgDebug.append("LeftCamera");
		else
			msgDebug.append("UNKNOWN");
		Debug::trace("EvolWrapperCameraScan", msgDebug);

		msgDebug = "imageType = ";
		if (imageType == DigitalImage)
			msgDebug.append("DigitalImage");
		else if (imageType == AnalogImage)
			msgDebug.append("AnalogImage");
		else
			msgDebug.append("UNKNOWN");
		Debug::trace("EvolWrapperCameraScan", msgDebug);

		msgDebug = "cameraImage = [";
		msgDebug.append(Hex::toString((const char *) cameraImage, _CameraMaxImageSize_)).append("]");
		Debug::trace("EvolWrapperCameraScan", msgDebug);

		Debug::trace("EvolWrapperCameraScan", headers + "  END");

	}
}

WrapperCamera EvolWrapperCameraScan::getCamera() const
{
	return camera;
}

void EvolWrapperCameraScan::setCamera(WrapperCamera value)
{
	camera = value;
}

WrCameraImageType EvolWrapperCameraScan::getImageType()
{
	return imageType;
}

void EvolWrapperCameraScan::setImageType(WrCameraImageType value)
{
	imageType = value;
}

const UINT8 *EvolWrapperCameraScan::getCameraImage() const
{
	return cameraImage;
}
void EvolWrapperCameraScan::setCameraImage(const UINT8 *beginningOfImage)
{
	const UINT8 *imageOffset = beginningOfImage;

	for (unsigned int i=0; i< _CameraMaxImageSize_; i++)
		cameraImage[i] = *imageOffset++;

}

bool EvolWrapperCameraScan::equals(EvolWrapperCameraScan &wrScan)
{
	bool rv = true;

	if	(camera != wrScan.getCamera() || imageType != wrScan.getImageType())
		rv = false;
	else
	{
		const UINT8 *wrScanImage = wrScan.getCameraImage();
		for (unsigned int i=0; i<_CameraMaxImageSize_; i++)
		{
			if (cameraImage[i] != *wrScanImage++)
			{
				rv = false;
				break;
			}
		}
	}
	return rv;
}
