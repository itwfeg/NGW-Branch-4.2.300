/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef WEIGHERCALIBRATIONEND_H
#define WEIGHERCALIBRATIONEND_H

#include "evolution.h"
#include "HfcObject.h"


typedef enum WeigherCalibrationEndFieldOffsets
{
	fieldOffsetEndState = 1,
	endWeigherCalibrationEndFieldOffsets
} WeigherCalibrationEndFieldOffsets_t;


class WeigherCalibrationEnd : public HfcObject
{
	public:

		static const short ENDSTATE_STEPSCOMPLETE = 0;
		static const short ENDSTATE_COMPLETE = 1;
		static const short ENDSTATE_CANCEL = 2;

		WeigherCalibrationEnd(short initialState = ENDSTATE_CANCEL);
		WeigherCalibrationEnd(const char *data, int len, int formatType);

		~WeigherCalibrationEnd();

		short getEndState() const;
		void setEndState(short endState);

		virtual const char *serialize(int format) const;

	private:

		short endState;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //WEIGHERCALIBRATIONEND_H
