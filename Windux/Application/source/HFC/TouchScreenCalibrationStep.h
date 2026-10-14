/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef TOUCHSCREENCALIBRATIONSTEP_H
#define TOUCHSCREENCALIBRATIONSTEP_H

#include "evolution.h"
#include "HfcObject.h"


typedef enum TouchScreenCalibrationStepFieldOffsets
{
	fieldOffsetTSCStepNum = 1,
	fieldOffsetTSCXCoordinate,
	fieldOffsetTSCYCoordinate,

	endTouchScreenCalibrationStepFieldOffsets
} TouchScreenCalibrationStepFieldOffsets_t;


class TouchScreenCalibrationStep : public HfcObject
{
	public:

		TouchScreenCalibrationStep(short stepNum = 0);
		TouchScreenCalibrationStep(const char *data, int len, int formatType);
		~TouchScreenCalibrationStep();

		virtual const char *serialize(int format) const;


		short getStep() const {return step;}
		void  setStep(short stepNum) {step = stepNum;}

		int  getX() const {return x;}
		void setX(int coordinate) {x = coordinate;}

		int  getY() const {return y;}
		void setY(int coordinate) {y = coordinate;}


	private:

		short step;
		int x;
		int y;
		
		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //TOUCHSCREENCALIBRATIONSTEP_H
