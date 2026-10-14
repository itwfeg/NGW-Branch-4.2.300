/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWRAPPERCYCLE_H
#define HFCWRAPPERCYCLE_H

#include "evolution.h"
#include "HfcObject.h"
#include "WrapperCycle.h"

#if 0
typedef enum HfcWrapperCycleFieldOffsets
{
	fieldOffsetWrCyPackageId = 1,
	fieldOffsetWrCyPullFilm,
	fieldOffsetWrCyPackCode,

	endHfcWrapperCycleFieldOffsets		//	MUST be last!
} HfcWrapperCycleFieldOffsets_t;
#endif

class HfcWrapperCycle : public WrapperCycle, public HfcObject
{
	public:

		HfcWrapperCycle(const char *data = NULL);
#if 0
		HfcWrapperCycle(const char *data, int len, int formatType);
#endif

		~HfcWrapperCycle();

#if 0
		virtual const char *serialize(int format = InternalFormat) const;
#endif

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//	HFCWRAPPERCYCLE_H
