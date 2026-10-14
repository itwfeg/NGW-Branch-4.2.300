/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCILASPEED_H
#define HFCILASPEED_H

#include "evolution.h"
#include "HfcObject.h"
#include "ILASpeed.h"

typedef enum HfcILASpeedFieldOffsets
{
	fieldOffsetILASpeed = 1,
	fieldOffsetILACycleCount,
	endHfcILASpeedFieldOffsets		//	MUST be last!
} HfcILASpeedFieldOffsets_t;

class HfcILASpeed : public ILASpeed, public HfcObject
{
	public:

		HfcILASpeed();
		HfcILASpeed(const char *data);
		HfcILASpeed(const char *data, int len, int formatType);

		~HfcILASpeed();

		int getCycleCount() const;
		void setCycleCount(int cycleCount);
		void incrementCycleCount(int inc = 1);

		virtual const char *serialize(int format = InternalFormat) const;

	private:

		int cycleCount;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//	HFCILASPEED_H
