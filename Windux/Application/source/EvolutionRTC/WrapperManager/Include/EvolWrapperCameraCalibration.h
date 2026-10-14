/* Access Back-room Scale Project
 * COPYRIGHT (c) 2008.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef EVOLWRAPPERCAMERACALIBRATION_H
#define EVOLWRAPPERCAMERACALIBRATION_H

#include <string>

#include "WrapperGeneric.h"
#include "Wrapper.h"


class EvolWrapperCameraCalibration : public WrapperGeneric
{
	public:

		EvolWrapperCameraCalibration(Hobart::CEvolMsgReceiver *pHobThread = 0);
		EvolWrapperCameraCalibration(const char *buffer);
		EvolWrapperCameraCalibration(const EvolWrapperCameraCalibration &ewcc);
		~EvolWrapperCameraCalibration();

		const char *serialize() const;
		void show();

		WrapperCamera getCamera() const { return camera_; }
		void setCamera(WrapperCamera value) { camera_ = value; }

		bool getResult() const { return result_; }
		void setResult(bool value) { result_ = value; }

		UINT16 getScaleFactor() const { return scaleFactor_; }
		void setScaleFactor(UINT16 value) { scaleFactor_ = value; }

		UINT16 getFirstPixel() const { return firstPixel_; }
		void setFirstPixel(UINT16 value) { firstPixel_ = value; }

		UINT16 getLastPixel() const { return lastPixel_; }
		void setLastPixel(UINT16 value) { lastPixel_ = value; }

	protected:

		WrapperCamera	camera_;
		bool			result_;
		UINT16 			scaleFactor_;	
		UINT16 			firstPixel_;	
		UINT16 			lastPixel_;	

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif // EVOLWRAPPERCAMERACALIBRATION_H
