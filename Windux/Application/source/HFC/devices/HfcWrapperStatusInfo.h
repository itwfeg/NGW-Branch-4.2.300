/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWRAPPERSTATUS_H
#define HFCWRAPPERSTATUS_H

#include "evolution.h"
#include "HfcObject.h"
#include "WrapperStatusInfo.h"

typedef enum HfcWrapperStatusInfoFieldOffsets
{
	fieldOffsetWrSIRate = 1,
	fieldOffsetWrSIReportStatus,
	fieldOffsetWrSIPusherHome,
	fieldOffsetWrSIRateAnalysis,
	fieldOffsetWrSIOpenWindow,
	fieldOffsetWrSIMainPower,
	fieldOffsetWrSIMainMotor,
	fieldOffsetWrSILimpHome,
	fieldOffsetWrSIPackWidth,
	fieldOffsetWrSIFilmTension,
	fieldOffsetWrSIStateInfo,

	endHfcWrapperStatusInfoFieldOffsets		//	MUST be last!
} HfcWrapperStatusInfoFieldOffsets_t;

class HfcWrapperStatusInfo : public WrapperStatusInfo, public HfcObject
{
	public:

		HfcWrapperStatusInfo(const char *data);
		HfcWrapperStatusInfo(const char *data, int len, int formatType);

		~HfcWrapperStatusInfo();

		virtual const char *serialize(int format = InternalFormat) const;


	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//	HFCWRAPPERSTATUS_H
