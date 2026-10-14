/* Access Back-room Scale Project
 * COPYRIGHT (c) 2008.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCEVOLWRAPPERCAMERACALIBRATION_H
#define HFCEVOLWRAPPERCAMERACALIBRATION_H

#include "evolution.h"
#include "HfcObject.h"
#include "EvolWrapperCameraCalibration.h"


class HfcEvolWrapperCameraCalibration : public EvolWrapperCameraCalibration, public HfcObject
{
	public:
		HfcEvolWrapperCameraCalibration(const char *data);

		~HfcEvolWrapperCameraCalibration();

		virtual const char *serialize(int format = BrowserFormat) const;

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif	//	HFCEVOLWRAPPERCAMERACALIBRATION_H
