/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWRAPPERPACKAGETOTALS_H
#define HFCWRAPPERPACKAGETOTALS_H

#include "HfcWrapperConfig.h"

class HobList;

typedef enum HfcWrapperPackageTotalsFieldOffsets
{
	fieldOffsetWrPkgTotPackages = 1,
	fieldOffsetWrPkgTotJams,
	fieldOffsetWrPkgTotCycles,
	fieldOffsetWrPkgTotResetDate,
	fieldOffsetWrPkgTotPCNameList,
	fieldOffsetWrPkgTotPCPackagesList,
	fieldOffsetWrPkgTotPCJamsList,

	endHfcWrapperPackageTotalsFieldOffsets	//	MUST be last!
} HfcWrapperPackageTotalsFieldOffsets_t;

class HfcWrapperPackageTotals : public HfcObject
{
	public:

		HfcWrapperPackageTotals(HfcWrapperConfig *wrapConfig);

		~HfcWrapperPackageTotals();

		const char *serialize(int format) const;

	private:
		HfcWrapperConfig *wrapperConfig;
		HobDateTime		*dateReset;
		HobList			*pcNameList;
		HobList			*pcPackageCounts;
		HobList			*pcJamCounts;
		long			totalPackages;
		long			totalJams;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCWRAPPERPACKAGETOTALS_H
