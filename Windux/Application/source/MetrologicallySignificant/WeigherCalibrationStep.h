/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef WEIGHERCALIBRATIONSTEP_H
#define WEIGHERCALIBRATIONSTEP_H

#include "evolution.h"
#include "HfcObject.h"

// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
#include "Weigher.h"

typedef enum WeigherCalibrationStepFieldOffsets
{
	fieldOffsetStepNum = 1,
	fieldOffsetZeroedCounts,
	fieldOffsetDelta,
	fieldOffsetNonZeroedCounts,
	fieldOffsetZeroedCountsDisplayed,
	endWeigherCalibrationStepFieldOffsets
} WeigherCalibrationStepFieldOffsets_t;

class WeigherCalibrationStep : public HfcObject
{
	public:

		WeigherCalibrationStep(short stepNum, long desiredCounts, long maxError);
		WeigherCalibrationStep(const char *data, int len, int formatType);

		~WeigherCalibrationStep();

		short getStepNum() {return stepNum_;};
		void setStepNum(short value) {stepNum_ = value;}

		long getZeroedCounts() {return zeroedCounts_;}
		void setZeroedCounts(long value) {zeroedCounts_ = value;}

		long getZeroedCountsForDisplay() {return zeroedCountsForDisplay_;}
		void setZeroedCountsForDisplay(long value) {zeroedCountsForDisplay_ = value; zeroedCountsForDisplayValid_ = true;}

		long getNonZeroedCounts() {return nonZeroedCounts_;}
		void setNonZeroedCounts(long value) {nonZeroedCounts_ = value;}

		long getDelta() {return delta_;}
		void setDelta(long value) {delta_ = value;}

		long getDesiredCounts() {return desiredCounts_;}
		void setDesiredCounts(long value) {desiredCounts_ = value;}

		long getMaxError() {return maxError_;}
		void setMaxError(long value) {maxError_ = value;}

		long getError() {return error_;}
		long calculateError(long gainFactor, long baseCounts, WeigherKind WgKind);

		virtual const char *serialize(int format) const;

	private:

		short stepNum_;

		long zeroedCounts_;
		long zeroedCountsForDisplay_;
		long nonZeroedCounts_;
		long delta_;
		long desiredCounts_;
		long maxError_;

		long error_;

		bool zeroedCountsForDisplayValid_;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};

#endif //WEIGHERCALIBRATIONSTEP_H
