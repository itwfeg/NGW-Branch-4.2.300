/* Access Back-room Scale Project
 * COPYRIGHT (c) 2008.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "EvolWrapperCameraCalibration.h"

#include "HobString.h"
#include "Debug.h"
#include "Long.h"
#include "Integer.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperCameraCalibration::_dataType_ = 0;
std::string EvolWrapperCameraCalibration::_dataTypeName_("EvolWrapperCameraCalibration"); 
#endif

EvolWrapperCameraCalibration::EvolWrapperCameraCalibration(Hobart::CEvolMsgReceiver *pHobThread)
	: WrapperGeneric(WrCameraCalibration, pHobThread), camera_(RightCamera), result_(false), scaleFactor_(1), firstPixel_(0), lastPixel_(0)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

EvolWrapperCameraCalibration::EvolWrapperCameraCalibration(const char *buffer)
	: WrapperGeneric(buffer), camera_(RightCamera), result_(false), scaleFactor_(1), firstPixel_(0), lastPixel_(0)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	WrapperGeneric::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &camera_;
	for (unsigned int i = 0; i < sizeof(camera_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &result_;
	for (unsigned int i = 0; i < sizeof(result_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &scaleFactor_;
	for (unsigned int i = 0; i < sizeof(scaleFactor_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &firstPixel_;
	for (unsigned int i = 0; i < sizeof(firstPixel_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &lastPixel_;
	for (unsigned int i = 0; i < sizeof(lastPixel_); i++)
	{
		*pChar++ = buffer[j++];
	}
}

EvolWrapperCameraCalibration::EvolWrapperCameraCalibration(const EvolWrapperCameraCalibration &ewcc)
	: WrapperGeneric(WrCameraCalibration)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	camera_ = ewcc.getCamera();
	result_ = ewcc.getResult();
	scaleFactor_ = ewcc.getScaleFactor();
	firstPixel_ = ewcc.getFirstPixel();
	lastPixel_ = ewcc.getLastPixel();
}

EvolWrapperCameraCalibration::~EvolWrapperCameraCalibration()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *EvolWrapperCameraCalibration::serialize() const
{
	WrapperGeneric::serialize();

	Debug::trace("EvolWrapperCameraCalibration", "New EvolWrapperCameraCalibration serialize");

	char *pChar = (char *) &camera_;
	for (unsigned int i = 0; i < sizeof(camera_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &result_;
	for (unsigned int i = 0; i < sizeof(result_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &scaleFactor_;
	for (unsigned int i = 0; i < sizeof(scaleFactor_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &firstPixel_;
	for (unsigned int i = 0; i < sizeof(firstPixel_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &lastPixel_;
	for (unsigned int i = 0; i < sizeof(lastPixel_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void EvolWrapperCameraCalibration::show()
{
	if (Debug::isTracing("EvolWrapperCameraCalibration"))
	{
		HobString headers("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%");

		Debug::trace("EvolWrapperCameraCalibration", headers + "  START");
		HobString msgDebug("camera = ");
		if (camera_ == RightCamera)
			msgDebug.append("RightCamera");
		else if (camera_ == LeftCamera)
			msgDebug.append("LeftCamera");
		else
			msgDebug.append(Integer::toString(camera_)).append("! What is this?");
		Debug::trace("EvolWrapperCameraCalibration", msgDebug);

		msgDebug = "Calibration ";
		if (result_ == true)
			msgDebug.append("Passed.");
		else
			msgDebug.append("Failed.");
		Debug::trace("EvolWrapperCameraCalibration", msgDebug);

		msgDebug = "scaleFactor = ";
		msgDebug.append(Integer::toString(scaleFactor_));
		Debug::trace("EvolWrapperCameraCalibration", msgDebug);

		msgDebug = "firstPixel = ";
		msgDebug.append(Integer::toString(firstPixel_));
		Debug::trace("EvolWrapperCameraCalibration", msgDebug);

		msgDebug = "lastPixel = ";
		msgDebug.append(Integer::toString(lastPixel_));
		Debug::trace("EvolWrapperCameraCalibration", msgDebug);

		Debug::trace("EvolWrapperCameraCalibration", headers + "  END");
	}
}
