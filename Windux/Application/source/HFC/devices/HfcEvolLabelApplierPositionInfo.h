/* Access Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCEVOLLABELAPPLIERPOSITIONINFO_H
#define HFCEVOLLABELAPPLIERPOSITIONINFO_H

#include "HfcObject.h"
#include "LabelApplierPositionInfo.h"

typedef enum HfcEvolLabelApplierPositionInfoFieldOffsets
{
	fieldOffsetTestLAPosInfoPriRotation = 1,
	fieldOffsetTestLAPosInfoPriLocation,
	fieldOffsetTestLAPosInfoSecRotation,
	fieldOffsetTestLAPosInfoPrinterStation,
	endHfcLabelTestApplierPositionInfoFieldOffsets	//	MUST be last!
} HfcEvolLabelApplierPositionInfoFieldOffsets_t;

class HfcEvolLabelApplierPositionInfo : public LabelApplierPositionInfo, public HfcObject
{
	public:

		HfcEvolLabelApplierPositionInfo(const char *data);
		HfcEvolLabelApplierPositionInfo(const char *data, int len, int formatType);

		~HfcEvolLabelApplierPositionInfo();

		virtual const char *serialize(int format = InternalFormat);

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif	// HFCEVOLLABELAPPLIERPOSITIONINFO_H
