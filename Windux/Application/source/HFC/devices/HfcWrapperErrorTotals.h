/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWRAPPERERRORTOTALS_H
#define HFCWRAPPERERRORTOTALS_H

#include "HfcWrapperConfig.h"

typedef enum HfcWrapperErrorTotalsFieldOffsets
{
	fieldOffsetWrErrTotElevatorCollapse = 1,
	fieldOffsetWrErrTotIntakeJam,
	fieldOffsetWrErrTotLengthJam,
	fieldOffsetWrErrTotWidthJam,
	fieldOffsetWrErrTotCoverOpen,
	fieldOffsetWrErrTotSealerBeltOpen,
	fieldOffsetWrErrTotPackageTooTall,
	fieldOffsetWrErrTotPusherHomeError,
	fieldOffsetWrErrTotFilmPullError,
	fieldOffsetWrErrTotResetDate,

	endHfcWrapperErrorTotalsFieldOffsets	//	MUST be last!
} HfcWrapperErrorTotalsFieldOffsets_t;

class HfcWrapperErrorTotals : public HfcObject
{
	public:

		HfcWrapperErrorTotals(HfcWrapperConfig *wrapConfig);

		~HfcWrapperErrorTotals();

		const char *serialize(int format) const;
		void resetTotals();

	private:
		HfcWrapperConfig *wrapperConfig;
		HobDateTime		*dateReset;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCWRAPPERERRORTOTALS_H
