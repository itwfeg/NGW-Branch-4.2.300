/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWRAPPERCYCLEMODE_H
#define HFCWRAPPERCYCLEMODE_H

#include "evolution.h"
#include "HfcObject.h"
#include "Wrapper.h"

typedef enum HfcWrapperCycleModeFieldOffsets
{
	fieldOffsetWrapperCycleMode = 1,
	fieldOffsetWrapperCyclePackCode,
	fieldOffsetWrapperCycleCount,

	endHfcWrapperCycleModeFieldOffsets	//	MUST be last!
} HfcWrapperCycleModeFieldOffsets_t;

class HfcWrapperCycleMode : public HfcObject
{
	public:

		static const int WRAPPERCYCLEINVALID		= 0;
		static const int WRAPPERCYCLEPACKAGESIZE	= 1;
		static const int WRAPPERCYCLEAUTODETECT		= 2;
		static const int WRAPPERCYCLEAUTOCONFIG		= 3;

		HfcWrapperCycleMode();
		HfcWrapperCycleMode(const char *data, int len, int formatType);

		~HfcWrapperCycleMode();

		int getMode();
		void setMode(int mode);
		WrPackCode getPackCode();
		void setPackCode(WrPackCode packCode);
		int getCycleCount();
		void setCycleCount(int count);

		bool isValidMode();
		bool isPackageSizeMode();
		bool isAutoDetectMode();
		bool isAutoConfigMode();


	private:
		int			mode;
		WrPackCode	packCode;
		int			cycleCount;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//	HFCWRAPPERCYCLEMODE_H
