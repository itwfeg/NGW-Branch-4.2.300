/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWRAPPERPACKCODEINFO_H
#define HFCWRAPPERPACKCODEINFO_H

#include "HfcWrapperConfig.h"
#include "WrapperPackCodeInfo.h"

typedef enum HfcWrapperPackCodeFieldOffsets
{
	fieldOffsetWrPackCodeValue = 1,
	fieldOffsetWrPackCodeDescription,
	fieldOffsetWrPackCodeFilmWidth,
	fieldOffsetWrPackCodeFilmLength,
	fieldOffsetWrPackCodeWidthMin,
	fieldOffsetWrPackCodeWidthMax,
	fieldOffsetWrPackCodeLengthMin,
	fieldOffsetWrPackCodeLengthMax,
	fieldOffsetWrPackCodeSideClampOnMin,
	fieldOffsetWrPackCodeSideClampOnValue,
	fieldOffsetWrPackCodeSideClampOnMax,
	fieldOffsetWrPackCodeSideClampOffMin,
	fieldOffsetWrPackCodeSideClampOffValue,
	fieldOffsetWrPackCodeSideClampOffMax,
	fieldOffsetWrPackCodeGripperOnMin,
	fieldOffsetWrPackCodeGripperOnValue,
	fieldOffsetWrPackCodeGripperOnMax,
	fieldOffsetWrPackCodeGripperOffMin,
	fieldOffsetWrPackCodeGripperOffValue,
	fieldOffsetWrPackCodeGripperOffMax,
	fieldOffsetWrPackCodeKnifeOffMin,
	fieldOffsetWrPackCodeKnifeOffValue,
	fieldOffsetWrPackCodeKnifeOffMax,
	fieldOffsetWrPackCodeSlideBlock,
	fieldOffsetWrPackCodePackageBooster,

	endHfcWrapperPackCodeFieldOffsets	//	MUST be last!
} HfcWrapperPackCodeFieldOffsets_t;

class HfcWrapperPackCodeInfo : public WrapperPackCodeInfo, public HfcObject
{
	public:

		HfcWrapperPackCodeInfo(WrPackCode packCodeNum, HfcWrapperConfig *wrapConfig);
		HfcWrapperPackCodeInfo(const char *data, int len, int formatType);

		~HfcWrapperPackCodeInfo();

		const char *serialize(int format) const;

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCWRAPPERPACKCODEINFO_H
