#ifndef EVOLWRAPPERCAMERASCAN_H
#define EVOLWRAPPERCAMERASCAN_H

#include <string>

#include "WrapperGeneric.h"
#include "Wrapper.h"


class EvolWrapperCameraScan : public WrapperGeneric
{
	public:
		static const unsigned int _CameraMaxImageSize_ = 128;
		static const unsigned int _MaxCameraImagePerTransaction_ = 64;

		EvolWrapperCameraScan(Hobart::CEvolMsgReceiver *pHobThread = 0);
		EvolWrapperCameraScan(const char *buffer);
		~EvolWrapperCameraScan();

		const char *serialize() const;
		void show();

		WrapperCamera getCamera()const;
		void setCamera(WrapperCamera value);

		WrCameraImageType getImageType();
		void setImageType(WrCameraImageType value);

		const UINT8 *getCameraImage() const;
		void setCameraImage(const UINT8 *beginningOfImage);

		bool equals(EvolWrapperCameraScan &wrScan);

	protected:

		WrapperCamera	 	camera;
		WrCameraImageType	imageType;
		UINT8 				cameraImage[_CameraMaxImageSize_];	

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif // EVOLWRAPPERCAMERASCAN_H
